#!/bin/bash

# -r parametresiyle, verilen IP listesini reverse nslookup yapar
# parametresiz verilen host listesini nslookup yapar.

if [[ $1 == "-r" ]]; then
    HOSTLIST=" 172.18.178.192 172.18.182.125 172.18.182.193 172.18.182.76 172.18.182.84 172.18.182.98 172.18.183.185 172.18.183.186 172.18.183.40 172.18.183.61 172.18.183.81 172.18.183.82 172.18.183.85 172.18.183.86 172.18.183.87 172.18.185.27 172.18.185.29 172.18.186.172 172.18.186.210   "
    #HOSTLIST=$(cat listeler/stqaliste)
    for HO in $HOSTLIST; do
        echo -ne "\n$HO   "
        nslookup $HO 172.18.186.199 | grep name | awk '{printf $4}'
    done
    echo -ne "\n"

else
HOSTLIST=" dprhvcfm01p dprhvcfm02p dprhvcfm03p dprhvcfm04p dprhvcfm05p dprhvcfm06p dprhvcfm07p dprhvcfm08p dprhvcfm09p dprhvcfm10p"
for HO in $HOSTLIST; do 
        echo -n "$HO   "
        nslookup $HO 172.18.186.200 |grep Address | tail -n1 | awk '{printf $2}' 
    done
fi

