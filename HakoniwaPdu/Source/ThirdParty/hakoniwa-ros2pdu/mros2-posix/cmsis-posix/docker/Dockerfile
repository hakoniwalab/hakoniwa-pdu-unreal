FROM ubuntu:20.04 as cmsis-posix

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
	git	\
	build-essential	\
	wget	\
	gcc	\
	g++	\
	vim	\
	libssl-dev libreadline-dev zlib1g-dev \
	make	\
	autoconf \
	automake \
	pkg-config \
	curl \
	net-tools \
	netcat \
	&& apt-get clean

RUN apt update
RUN apt upgrade

RUN apt-get install -y cmake
RUN apt-get install -y libgtest-dev lcov

RUN git clone git@github.com:mROS-base/cmsis-posix.git
