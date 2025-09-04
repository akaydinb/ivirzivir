#/bin/bash

if [[ $# == 0 ]]; then
    X=$(cat /dev/stdin)
else if [[ $# == 1 ]]; then
    X=$1
fi
fi


# gzip veya cat gibi eger parametre verilmisse, parametrede verilen
# dosyayi ac, parametresiz calistirilirsa girdiyi stdin'den al
