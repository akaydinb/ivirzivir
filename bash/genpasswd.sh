#!/bin/bash

# openssl rand ile sifre uretmek icin bir script. Sifrede okunmasi sorunlu
# (o,O,0) veya (1,I,l) gibi karakterler varsa bunlari kabul etme. 

if [[ $# -eq 0 ]]; then
  COMPLEXITY=9
else
  COMPLEXITY=$1
fi

while :
do
    PASSWD=`openssl rand $COMPLEXITY -base64 | grep -v 0 | grep -v o | grep -v O | grep -v 1 | grep -v I | grep -v i | grep -v l | grep -v + | grep -v /`
    if [[ ! -z $PASSWD ]]; then
        echo $PASSWD
        break;
    fi
done


