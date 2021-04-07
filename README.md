# BSides Canberra 2021 Badge

This is the badge software for the [BSides Canberra 2021 conference](https://bsidesau.com.au/index.html), which will happen in Canberra Australia on 9-10
April 2021. The badge  was developed by [Penten](https://www.penten.com/) in collaboration with community volunteers. It's designed for hacking, featuring
two microcontrollers and a range of components.

Details of the badge including schematics and compiled firmware are available
[here](https://bsidesau.com.au/badge2021.html).

## Hardware

The following is an overview of the hardware for the badge. During the
conference some components are available for purchase as part of an expansion
pack (as marked).

### ATSAMD21G18A MCU
- 8 x Capacitive Touch Buttons
- 3 x User LEDs
- 8 x RGB LEDs
- Shitty Add-On Header
- Infra-Red Transceiver (expansion)

### ESP32-PICO-D4 MCU
- eInk Display
- 5 x User Buttons
- Audio Codec (expansion)
- Inertial Measurement Unit (expansion)
- microSD Card
- Flash (expansion)
- RAM (expansion)

### Power
Power can be delivered by three different sources:
- LIPO battery
- AAA battery pack (not included)
- USB 2.0

## Getting Started

There are two supported methods for building the BSides Badge 2021 software:

1. Docker
2. Linux (Ubuntu- or Debian-based)

### Download the Software

The main repository and its submodules must be downloaded. Note you will need git-LFS for some files!

```
# Install git-lfs (on Ubuntu-/Debian-based systems, this is):
$ sudo apt-get install -y git git-lfs
$ git clone git@gitlab.com:bsides-2020/src/2020-badge.git
$ cd 2020-badge
$ git submodule update --init --recursive
```

### Setup Build Environment (Docker)

The following instructions outline how to configure the build environment in Docker.
It is configured to share the source directory between your host machine and the
container. This allows you to use an IDE and git on your host while producing the
builds in the container.

#### Install Docker

1. Install [Docker](https://docs.docker.com/get-docker/).
2. Install [Docker Compose](https://docs.docker.com/compose/install/).

#### Start the Docker Container

The following will open a `/bin/bash` shell to the container. This may take a few
minutes the first time it is run because it will need to build the image.

```
$ docker-compose run bsidesbadge2021
```

At this stage it is ideal to run the `build.sh` script to download the software
dependencies, build the binary and flash the badge.

To exit the container shell, type the following command:

```
$ exit
```

Because the source directory is shared between the source and the container previous
downloads and builds will persist through subsequent `docker-compose run` iterations.

### Setup Build Environment (Ubuntu)

The following instructions outline how to configure the build environment natively in Ubuntu 18.04.

### Environment

The following commands are required prior to running the `build` script.

#### Linux (Ubuntu-based)

If using an Ubuntu- or Debian-based operating system, the software dependencies can be
downloaded using the following command:

```
$ sudo apt-get install -y cmake gcc git git-lfs virtualenv wget make libncurses-dev libx11-dev flex bison gperf python python-pip python-setuptools python-serial python-cryptography python-future python-pyparsing python3-pip python3-serial
```

#### Python:

A number of Python modules are required and can be downloaded using the
following commands:

```
$ sudo python -m pip install -r requirements/build-py.txt
$ sudo python3 -m pip install -r requirements/build-py.txt
```

#### Python Virtualenv

A python virtual environment is _strongly_ recommended when developing on this -- especially given the older ESP toolchain python requirements.
```
# In the BSides badge source root directory:
$ mkdir venv
$ virtualenv ./venv/
$ source ./venv/bin/activate
$ python -m pip install -r requirements/build-py.txt
```

## Building the Software

### Dependencies

Build tools can be downloaded automatically using the following command:

```
$ ./build.sh --download
```

### Help

For detailed help information for the `build` script, use the following command:

```
$ ./build.sh --help
```

### Build

The following commands can be used to build the badge software.

#### Build:

```
$ ./build.sh --build
```

#### Rebuild:

```
$ ./build.sh --clean --build
```

#### Makefile

Alternatively there is a Makefile wrapper around the commonly used `build.sh` steps, which speeds them up with multiple jobs.

### Flash

The following commands can be used to flash the software onto the badge.

>  **Note:** To prepare the badge for flashing, you must hold in the SW3 to
>  select boot mode, and then press SW5 to restart the ESP32.

#### Flash:

To flash the software onto the badge use the following command:

```
$ ./build.sh --flash
```

The target and port can be specified when flashing the badge. For example:

```
$ ./build/sh --target esp32 --esp32-port /dev/ttyUSB0 --flash
```

#### Flash and Monitor:

It is possible to monitor the serial output when flashing the badge:

```
$ ./build.sh --flash --monitor
```

# License

Code is released under the MIT License, Copyright 2020-2021 Penten and BSides badge 2020-2021 volunteers.

The "Penten" logo is copyright Penten.  All rights reserved unless explicitly stated otherwise.

The "BSides" logo is copyright BSides Canberra. All rights reserved unless explicitly stated otherwise.
