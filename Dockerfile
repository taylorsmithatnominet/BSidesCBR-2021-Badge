FROM ubuntu:18.04

WORKDIR /usr/src/bsidesbadge2021

# Install package dependencies
RUN apt-get update && \
    apt-get install -y cmake gcc git wget make libncurses-dev libx11-dev flex bison gperf python python-pip python-setuptools python-serial python-cryptography python-future python-pyparsing python3-pip python3-serial && \
    apt-get clean

# Install a more up to date `cmake` from the kitware APT repo
RUN apt-get update  \
 && apt-get install -y  \
        apt-transport-https software-properties-common  \
        ca-certificates gnupg wget                      \
 && wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc       \
        | gpg --dearmor - >/etc/apt/trusted.gpg.d/kitware.gpg               \
 && apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'    \
 && apt-get update  \
 && apt-get install -y  \
        cmake   \
 && rm -rf -- /var/lib/apt/*

# Import python dependencies
COPY requirements/build-py.txt requirements.txt

# Install python dependencies
RUN python -m pip install -r requirements.txt && \
    python3 -m pip install -r requirements.txt

# Ensure container doesn't exit
CMD [ "/bin/bash" ]