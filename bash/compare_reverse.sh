#!/bin/bash

# verilen host listesindeki hostname'lerin PTR kayitlarinin A kayitlariyla
# uyumlu oldugunu kontrol eden bir script. 

HLISTE=$(cat hosts.txt)
N=1

for SATIR in $HLISTE; do
    IPADDR=$(nslookup  $SATIR   172.18.186.199 | grep -A 1 Name | grep Address | awk '{print $2}')
    #echo $IPADDR
    REVERSE=$(nslookup  $IPADDR  172.18.186.199 | grep name | awk '{print $4}' | cut -d'.' -f 1)
    if [[ "$SATIR" != "$REVERSE" ]]; then
        echo $SATIR ve $REVERSE esit degil.
    fi
done
