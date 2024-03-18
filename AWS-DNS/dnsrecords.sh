#!/bin/bash

LISTOFDOMAINS="CLOUD-1671.csv"


while IFS= read -r line
do
  ZONE=$(echo "$line" | cut -d"," -f 1)
  ZONEID=$(grep $ZONE  HostedZones.txt | cut -d"," -f 1 | cut -d"/" -f 3)

  #echo "Line: $line"
  echo "Zone: $ZONE"
  #echo "ZoneID: $ZONEID"

  TMPFILE=$(mktemp)
  sed -e "s/CHANGEMEEE/$ZONE/" sablon.json > $TMPFILE
  aws route53 change-resource-record-sets --hosted-zone-id $ZONEID --change-batch file://$TMPFILE

  sed -e "s/CHANGEMEEE/www.$ZONE/" sablon.json > $TMPFILE
  aws route53 change-resource-record-sets --hosted-zone-id $ZONEID --change-batch file://$TMPFILE

  rm -f $TMPFILE

done < "$LISTOFDOMAINS"
