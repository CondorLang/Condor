FROM ubuntu
MAINTAINER Chase Willden <chase.willden@gmail.com>

RUN apt-get update && apt-get install -y \
	gcc \
	libc6-dev \
	--no-install-recommends \
	&& rm -rf /var/lib/apt/lists/*
    
CMD ["make"]
