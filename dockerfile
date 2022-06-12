FROM ubuntu:22.04

RUN apt update

RUN apt install gcc make openssl libssl-dev -y