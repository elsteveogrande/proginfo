# DOCKER_BUILDKIT=1 docker build --tag linux images/linux/
# docker run -v $(realpath ~/code/proginfo):/code/proginfo -it linux make -C /code/proginfo test_linux

FROM ubuntu:latest

RUN export TZ=America/New_York                 \
    && export DEBIAN_FRONTEND=noninteractive   \
    && apt -y update

RUN apt -y install g++

RUN apt -y install clang
RUN apt -y install lld

RUN apt -y install make file xxd
