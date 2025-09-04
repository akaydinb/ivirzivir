#!/bin/bash

DAKIKA=0
SANIYE=0

while true; do
  echo -ne "Input the value [mm:ss]: "
  read ZAMAN
  if [ x$ZAMAN == 'x' ]; then
    break;
  fi
  TD=$(echo $ZAMAN | cut -d':' -f 1)
  TS=$(echo $ZAMAN | cut -d':' -f 2)
  DAKIKA=$((10#$DAKIKA + 10#$TD))
  SANIYE=$((10#$SANIYE + 10#$TS))
  if [[ $SANIYE -gt 60 ]]; then
    SANIYE=$((10#$SANIYE-60))
    DAKIKA=$((10#$DAKIKA+1))
  fi
  echo $DAKIKA:$SANIYE
done

