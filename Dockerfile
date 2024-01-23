FROM ubuntu:22.04

COPY ./entrypoint.sh ./

ADD examples/ examples/

RUN apt update && apt install lsb-core -y

RUN apt update && apt install build-essential -y

RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]