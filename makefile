PWD = $(shell pwd)

all:
	gcc -o stegobmp main.c -Wall -pedantic -lcrypto

docker:
	docker build -t cripto .
	docker run -v ${PWD}:/root/cripto -w /root/cripto --name cripto_container -it cripto

start:
	docker start cripto_container
	docker exec -it cripto_container /bin/bash

stop:
	docker stop cripto_container