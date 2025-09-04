
cchash.c: 
A small utility to create rainbow table for credit card number 
hashes. It uses an 8 digit prefix (line 99), and generates the 
first 10 numbers with their hashes. To generate all credit cards
uncomment the for loop at line 97. The last two digits, called
Luhn numbers are calculated in calc_luhn function and added to 
the number and then sent to hash function. The code is able to
generate md5, sha256 and sha512 hashes. 

The code is quite old, from 09.Aug.2016. The functions MD5_Init, 
MD5_Update and MD5_Final are deprecated since OpenSSL3.0 and 
subject to be removed in future (as of 04.09.25, they are 
still available) and the code needs to be compiled with 
-lcrypto -lssl parameters. 


aesdeneme.c & aesenc.c:
These examples are taken from:
https://stackoverflow.com/questions/9889492/how-to-do-encryption-using-aes-in-openssl

My intent was to add AES encryption feature to the cchash.c, to
create an encrypted version of the rainbow table, as well, in case
the AES key is known, i.e. exposed or captured etc. However, I 
couldn't find time for that later. 
