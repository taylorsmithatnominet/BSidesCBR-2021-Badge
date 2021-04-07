#!/usr/bin/env bash
SCRIPT_DIR="$(realpath -e "${0%/*}")"
REPO_ROOT="$(git rev-parse --show-toplevel)"
BUNDLE_DIR="${REPO_ROOT}/bundle"
BUNDLE_ZIP="${REPO_ROOT##*/}.zip"

rsync -avv --files-from="${SCRIPT_DIR}/bundle.rsync.lst" --delete           \
    "${REPO_ROOT}" "${BUNDLE_DIR}/"
ln -srf "${BUNDLE_DIR}/scripts/daemon.py" "${BUNDLE_DIR}/daemon.py"
ln -srf "${BUNDLE_DIR}/requirements/daemon.txt" "${BUNDLE_DIR}/requirements.txt"
(cd "${REPO_ROOT}" && zip -r --symlinks \
    "${REPO_ROOT}/${BUNDLE_ZIP}" "${BUNDLE_DIR#${REPO_ROOT}/}"
)
