#!/bin/bash

# verilen kullanicinin sifresi Deneme123 mu degil mi?

echo $1
FROM=$(grep :\\$  $1  | cut -d: -f1); 

for ACTUSR in $FROM; do 
	FIELD=$(grep $ACTUSR  $1  | cut -d':' -f 2)
	SALT=$(echo $FIELD | cut -d'$' -f 3)
	HASHED=$(echo $FIELD | cut -d'$' -f 4)
	WEAKPASS=$(perl -e 'print crypt("Deneme123","\$6\$'${SALT}'\$") . "\n"' )
	if [[ "$FIELD" == "$WEAKPASS" ]]; then
		echo "Bingo!" $ACTUSR "in sifresi Deneme123"
	fi
done

