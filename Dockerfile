FROM ubuntu
MAINTAINER Chase Willden <chase.willden@gmail.com>

WORKDIR index.docker.io/chasewillden/cobralang
RUN make
