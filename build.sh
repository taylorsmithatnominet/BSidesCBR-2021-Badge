#!/usr/bin/env bash

declare -r SCRIPT=$(realpath ${0})
declare -r BUILD_PY=$(dirname ${SCRIPT})/build.py

python3 ${BUILD_PY} $@
