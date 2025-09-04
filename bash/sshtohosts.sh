#!/bin/bash 

HLISTE=`cat listeler/stqaliste |  grep -v ^#`
#HLISTE=`cat  listeler/ipliste.txt | grep -v ^# | sed -e '/^$/d'`
SALSTE=`echo $HLISTE | wc -w`
N=1

for SATIR in $HLISTE; do
   echo  "[ $N / $SALSTE ]: $SATIR"
   #echo   "\n$SATIR    " >> kernels
   #ssh   username@$SATIR #'su - '  #'uname -r; uptime' >> QAhost.txt
   #ssh  root@$SATIR
   #ssh  -t  username$SATIR 'sudo su -' 
   #ssh admin@$SATIR  #  "yum list installed | grep bash; openssl version" >> at.txt
   ssh -t autodeploy@$SATIR -o ConnectTimeout=3  'sudo cat /etc/shadow ' >> $SATIR.shadow
   N=$(($N+1))
done

#"env X='() { :;}; echo busted' bash -c \"echo completed\""
