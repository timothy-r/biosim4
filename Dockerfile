FROM docker.io/ubuntu:latest
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -yqq \
	build-essential \
	dumb-init \
	python3-pip \
	cmake \
	&& apt-get clean \
	&& rm -rf /var/lib/apt/lists/*

RUN pip install igraph

RUN apt-get update && apt-get install -yqq \
	cimg-dev \
	gnuplot \
	libopencv-dev \
	git \
	&& apt-get clean \
	&& rm -rf /var/lib/apt/lists/*

WORKDIR /app
RUN useradd -ms /bin/bash darwin
USER darwin

# Runs "/usr/bin/dumb-init -- /my/script --with --args"
ENTRYPOINT ["/usr/bin/dumb-init", "--"]
