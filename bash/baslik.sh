#!/bin/bash

# parametre olarak verilen stringin basina ve sonuna --- karakterlerini
# veya verilen baska bir karakteri ekleyerek txt dosyalar icin baslik
# haline getir.

if [[ x$2 == 'x' ]]; then
  DOLDUR="-"
else
  DOLDUR="$2"
fi

# karakter sayisini hesapla
KARAKTER=$(echo $1 | wc -c)
# karakter sayisini 72'den cikart
EKSILER=$(echo \(72 - $KARAKTER\) / 2 | bc)

for EEE in $(seq -s ' ' 1 $EKSILER); do 
   echo -ne $DOLDUR
done

echo -ne $1

for EEE in $(seq -s ' ' 1 $EKSILER); do
   echo -ne "$DOLDUR"
done

if [[ $(($EKSILER + $KARAKTER + $EKSILER)) != 72 ]]; then
   echo "$DOLDUR"
else
   echo
fi

