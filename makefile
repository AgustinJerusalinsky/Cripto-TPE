PWD = $(shell pwd)

all:
	gcc -o stegobmp main.c args.c steg.c -g -Wall -pedantic -lcrypto

docker:
	docker build -t cripto .
	docker run -v ${PWD}:/root/cripto -w /root/cripto --name cripto_container -it cripto

start:
	docker start cripto_container
	docker exec -it cripto_container /bin/bash

stop:
	docker stop cripto_container

EMLSB1:
	./stegobmp --embed -p ejemplo2022/lado.bmp --in ejemplo2022/itba.png --out lsb1_embed.bmp --steg LSB1

EXLSB1:
	./stegobmp --extract -p lsb1_embed.bmp --out lsb1_extract.bmp --steg LSB1 

LSB1:
	./stegobmp --extract -p ejemplo2022/ladoLSB1.bmp --out lsb1 --steg LSB1 

LSB4:
	./stegobmp --extract -p ejemplo2022/ladoLSB4.bmp --out lsb4 --steg LSB4

