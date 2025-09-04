#!/bin/bash 

# Grub'daki aktif boot entry'i goster
SIRA=$(grep default /boot/grub/grub.conf  | cut -d'=' -f 2)

echo $SIRA
grep title /boot/grub/grub.conf | grep -v ^# | sed "${SIRA+1}q;d" 
