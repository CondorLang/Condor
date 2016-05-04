FROM ubuntu
MAINTAINER Chase Willden <chase.willden@gmail.com>

RUN apt-get update
RUN apt-get install -y build_essential
    
CMD ["make"]
