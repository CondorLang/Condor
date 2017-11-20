FROM ubuntu:latest

# Create a well-known place for all this to happen
RUN mkdir /Condor
WORKDIR /Condor

# Update/upgrade all packages
RUN apt-get update

# Install Git, Python, make and build tools
RUN apt-get install -y build-essential git python cmake make gcc-multilib g++-multilib clang curl jq gdb

ADD ./ /Condor