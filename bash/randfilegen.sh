#!/bin/bash -x

# Random bir directory tree olustur ve icine yine rastgele buyuklukte
# rastgele adlarda dosyalar üret

DIZINS=$((RANDOM%10+1))

for (( I=1; I<=$DIZINS; I++ )); do
  DIZINA=$RANDOM
  mkdir $DIZINA
  cd $DIZINA
  DOSYAS=$((RANDOM%10+1))
  for (( J=1; J<=$DOSYAS; J++ )); do
    DOSYAB=$((RANDOM%100+1))
    DOSYAA=$RANDOM
    dd if=/dev/urandom of=$DOSYAA count=$DOSYAB
  done
  cd ..
done


for (( J=1; J<=$DOSYAS; J++ )); do
  DOSYAB=$((RANDOM%100+1))
  DOSYAA=$RANDOM
  dd if=/dev/urandom of=$DOSYAA count=$DOSYAB
done

