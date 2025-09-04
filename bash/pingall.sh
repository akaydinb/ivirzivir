#!/bin/bash

# normalde nmap ile de yapilabilecek basit bir pingsweep
# ping cevabi gelen makinaya ssh ile baglanmaya calisiyor
# tek farki bu

#SUBNETS="181 182 183 184 185 186 187"
SUBNETS="187 188"

for O3 in $SUBNETS; do
    for O4 in {10..254}; do 
        IPADDR="172.18."$O3.$O4
        #echo "172.18."$O3.$O4
        ping -c 2  $IPADDR  &>  /dev/null 
        if [[ $? == 0 ]]; then
            #ssh -o ConnectTimeout=3 -t root@$IPADDR  "cat /etc/shadow "
            echo $IPADDR ping cevabi alindi.
        fi
    done
done
