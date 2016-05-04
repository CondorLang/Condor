FROM ubuntu
MAINTAINER Chase Willden <chase.willden@gmail.com>

RUN apt-get update && \
    apt-get install -y build_essential && \
    apt-get clean
    
CMD ["make"]
