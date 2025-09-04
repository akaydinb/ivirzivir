#!/bin/bash 

trap "echo SIGTERM received." TERM

while true 
do 
  sleep 1
done


