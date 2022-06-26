FROM ubuntu:20.04

RUN apt update

RUN apt install gcc make openssl libssl-dev python3 -y