nmap-6.47/nmap -v -sn -n 172.18.178.0/24 -oG - | awk '/Status: Down/{print $2}'
