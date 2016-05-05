FROM ubuntu
<<<<<<< HEAD
MAINTAINER Chase Willde <chase.willden@gmail.com>
=======
MAINTAINER Chase Willden <chase.willden@gmail.com>
>>>>>>> 9024d1ee2da3a5aa7dbfaa31ef69b4295368824d

RUN apt-get update && apt-get install -y \
	gcc \
	libc6-dev \
	--no-install-recommends \
	&& rm -rf /var/lib/apt/lists/*
    
CMD ["make"]
