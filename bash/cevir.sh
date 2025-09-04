#!/bin/bash 

# ex1hosts.txt dosyasindaki IP listesini alip hosts dosyasiyla karsilastirir
# ve bulduklarini ekrana yazar. Bir bakima /etc/hosts ile calisan rev. nslookup

HLISTE=`cat ex1hosts.txt | grep -v ^# `
#echo $HLISTE

for SATIR in $HLISTE; do
   #grep $SATIR /etc/hosts | grep -v local ## liste
   HN=`grep $SATIR /etc/hosts | awk '{ print $2 }' | grep -v local | head -n 1` ## liste
   if [ ! -z $HN ]; then 
      echo -ne $SATIR '->' $HN "\n"
   else
      echo $SATIR
   fi

done

#for SATIR in $HLISTE; do
#   echo $SATIR | cut -d'.' -f 1 >> hostlistewodom
#done

#cat IPliste | sort > temp; rm -f IPliste; mv temp IPliste

