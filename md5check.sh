#!/bin/bash

# Compare local md5 hash and remote S3 object ETag
# akaydinb, 10.Dec.2024

read -p  "Enter backup path: "  BACKUPPATH
read -p  "Enter bucket name: "  BUCKETNAME

OUTPUTFILE="output_$(date "+%Y-%m-%d-%H").txt"

for bifile in $BACKUPPATH/*; do
    S3APIOUT=$(aws s3api  head-object --bucket $BUCKETNAME  --key "image_backup/"${bifile}  --part-number 1)
    #PARTS=$(echo $S3APIOUT | grep PartsCount | awk '{print  $2}')
    PARTS=$(echo $S3APIOUT | jq -r '.PartsCount')
    ETAG=$(echo $S3APIOUT | jq -r '.ETag' | sed 's/\"//g')
    CHUNKSIZE=$(echo $S3APIOUT | jq -r '.ContentLength')
    if [[ $PARTS == "null" ]]; then
        # if there is no PartsCount key in JSON, it's then single parted
        PARTS=1
    fi

    echo "$bifile consists of $PARTS part(s). Hashing ..."

    if [[ $PARTS -eq 1 ]]; then
        # for single part files ETAG = MD5
        MD5=$(md5sum $bifile | awk '{print $1}')
    else
        # for multipart uploads there is a block size. 16M or 8M or whatever
        # you have to get the blocks of the file, hash them with md5 one
        # by one and add them together. This is done in a temp file...
        OUT="$(mktemp)"
        echo "-> Block: "
        for block in $(seq 1 $PARTS); do 
            echo -ne "$block  ";  # just like a progressbar
            #dd if=$bifile  bs=16777216 count=1 skip=$(($block-1)) 2> /dev/null | md5sum | awk '{print $1}' >> $OUT; 
            dd if=$bifile  bs=${CHUNKSIZE} count=1 skip=$(($block-1)) 2> /dev/null | md5sum | awk '{print $1}' >> $OUT; 
        done

        # finally concat'd file is hashed once again to find the final ETAG value. 
        # The value is suffixed with the number of Parts separated by a dash
        MD5=$(xxd -r -p $OUT | md5sum | awk '{print $1}')
        rm $OUT
        echo
    fi

    # below part compares the hash with local file hash and writes the output
    if [[ $PARTS -eq 1 ]]; then
        XETAG="$ETAG-1"
    else
        XETAG=$ETAG
    fi

    if [ "$MD5"-"$PARTS" == "$XETAG" ]; then
        echo -ne "[ OK ]    "
        echo -ne "[ OK ]    "  >> $OUTPUTFILE
    else
        echo -ne "[FAIL]    "
        echo -ne "[FAIL]    "  >> $OUTPUTFILE
    fi

    echo -ne "$MD5    $ETAG    $bifile\n"
    echo -ne "$MD5    $ETAG    $bifile\n" >> $OUTPUTFILE
    echo

done
