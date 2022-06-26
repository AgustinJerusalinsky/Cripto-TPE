PWD = $(shell pwd)
CARRIER = "ejemplo2022/lado.bmp"
SECRET = "ejemplo2022/itba.png"

COMPARISON_CARRIER = "bibloteca.bmp"
COMPARISON_SECRET = "secret.pdf"

all:
	gcc -o stegobmp main.c args.c steg.c encrypt.c -g -Wall -pedantic -fsanitize=address -lcrypto
	gcc -o analyzer analyzer.c -Wall -pedantic

docker:
	docker build -t cripto .
	docker run -v ${PWD}:/root/cripto -w /root/cripto --name cripto_container -it cripto

start:
	docker start cripto_container
	docker exec -it cripto_container /bin/bash

stop:
	docker stop cripto_container

EMLSB1:
	./stegobmp --embed -p ${CARRIER} --in ${SECRET} --out lsb1_embed.bmp --steg LSB1

EXLSB1:
	./stegobmp --extract -p lsb1_embed.bmp --out lsb1_extract --steg LSB1 

EMLSB4:
	./stegobmp --embed -p ${CARRIER} --in ${SECRET} --out lsb4_embed.bmp --steg LSB4

EXLSB4:
	./stegobmp --extract -p lsb4_embed.bmp --out lsb4_extract --steg LSB4

EMLSBI:
	./stegobmp --embed -p ${CARRIER} --in ${SECRET} --out lsbi_embed.bmp --steg LSBI

EXLSBI:
	./stegobmp --extract -p lsbi_embed.bmp --out lsbi_extract --steg LSBI


LSB1:
	./stegobmp --extract -p ejemplo2022/ladoLSB1.bmp --out lsb1 --steg LSB1 

LSB4:
	./stegobmp --extract -p ejemplo2022/ladoLSB4.bmp --out lsb4 --steg LSB4

LSBI:
	./stegobmp --extract -p ejemplo2022/ladoLSBI.bmp --out lsbi --steg LSBI

DECLSB1192CBC:
	./stegobmp --extract -p ejemplo2022/ladoLSB1aes192cbc.bmp --out lsb1 --steg LSB1 -a AES192 -m CBC --pass escondite


DECLSBI256OFB:
	./stegobmp --extract -p ejemplo2022/ladoLSBIaes256ofb.bmp --out lsbi --steg LSBI -a AES256 -m OFB --pass secreto

ENCLSB1:
	./stegobmp --embed -p ${CARRIER} --in ${SECRET} --out lsb1_enc.bmp --steg LSB1 -a AES192 -m CBC --pass escondite

DECLSB1:
	./stegobmp --extract -p lsb1_enc.bmp --out lsb1_dec --steg LSB1 -a AES192 -m CBC --pass escondite

ENCLSB4:
	./stegobmp --embed -p ${CARRIER} --in ${SECRET} --out lsb4_enc.bmp --steg LSB4 -a AES192 -m CBC --pass escondite

DECLSB4:
	./stegobmp --extract -p lsb4_enc.bmp --out lsb4_dec --steg LSB4 -a AES192 -m CBC --pass escondite

ENCLSBI:
	./stegobmp --embed -p ${CARRIER} --in ${SECRET} --out lsbi_enc.bmp --steg LSBI -a AES192 -m CBC --pass escondite

DECLSBI:
	./stegobmp --extract -p lsbi_enc.bmp --out lsbi_dec --steg LSBI -a AES192 -m CBC --pass escondite

MEDIANOCHE:
	./stegobmp --extract -p grupo21/medianoche1.bmp --out medianoche_out --steg LSB1

KINGS:
	./stegobmp --extract -p grupo21/kings.bmp --out kings_out --steg LSBI

KINGS1:
	./stegobmp --extract -p grupo21/kings1.bmp --out kings1_out --steg LSB4 -a AES256 -m OFB --pass escondido

SILENCE:
	./stegobmp --extract -p grupo21/silence.bmp --out silence_out --steg LSB4 -a AES128 -m OFB --pass escondido

COMPARACION1:
	./stegobmp --embed -p ${COMPARISON_CARRIER} --in ${COMPARISON_SECRET} --out comparacion1.bmp --steg LSB1


COMPARACION4:
	./stegobmp --embed -p ${COMPARISON_CARRIER} --in ${COMPARISON_SECRET} --out comparacion4.bmp --steg LSB4


COMPARACIONI:
	./stegobmp --embed -p ${COMPARISON_CARRIER} --in ${COMPARISON_SECRET} --out comparacioni.bmp --steg LSBI

EXCOMPARACION1:
	./stegobmp --extract -p comparacion1.bmp --out comparacion1_out --steg LSB1

EXCOMPARACION4:
	./stegobmp --extract -p comparacion4.bmp --out comparacion4_out --steg LSB4

EXCOMPARACIONI:
	./stegobmp --extract -p comparacioni.bmp --out comparacioni_out --steg LSBI

COMPARACION: 
	make COMPARACION1 COMPARACION4 COMPARACIONI
	./analyzer ${COMPARISON_CARRIER} comparacion1.bmp 
	./analyzer ${COMPARISON_CARRIER} comparacion4.bmp
	./analyzer ${COMPARISON_CARRIER} comparacioni.bmp

clean:
	rm -rf lsb1_embed.bmp lsb4_embed.bmp lsbi_embed.bmp
	rm -rf lsb1_extract.png lsb4_extract.png lsbi_extract.png
	rm -rf lsb1_enc.bmp lsb4_enc.bmp lsbi_enc.bmp
	rm -rf lsb1_dec.png lsb4_dec.png lsbi_dec.png
	rm -rf comparacion1.bmp comparacion4.bmp comparacioni.bmp
	rm -rf comparacion1_out.* comparacion4_out.* comparacioni_out.*