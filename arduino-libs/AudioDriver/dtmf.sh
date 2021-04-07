#!/usr/bin/env bash
set -o nounset
set -o pipefail
set -o errexit

OUTPUT_FNAME="dtmf.wav"
# Standard machine to machine DTMF usage is 0.1s tones and pauses but we allow
# this to be tuned to reduce the size of generated audio
TONE_LEN="0.05"

SOX="$(command -v sox)"
PLAY="$(command -v play)"
if [ -z "${SOX}" ] || [ -z "${PLAY}" ]; then
    echo "Please install sox to use this script" >&2
    exit 1
fi

# DTMF keypad frequencies
declare -ar LOs=(697 770 852 941)
declare -ar HIs=(1209 1336 1477 1633)

declare -r PAUSE="trim 0 ${PAUSE_LEN:-${TONE_LEN}}"
declare -a PARTS=()

if [ -t 0 ]; then
    echo "^D to escape input loop"
    stty -echo
fi
while read -rn 1 KEY; do
    [ "$(printf "${KEY}" | xxd -p)" == "04" ] && break
    case "${KEY^}" in
        [123A]) LO=${LOs[0]} ;;
        [456B]) LO=${LOs[1]} ;;
        [789C]) LO=${LOs[2]} ;;
        [*0#D]) LO=${LOs[3]} ;;
        *) continue ;;
    esac
    case "${KEY^}" in
        [147*]) HI=${HIs[0]} ;;
        [2580]) HI=${HIs[1]} ;;
        [369#]) HI=${HIs[2]} ;;
        [ABCD]) HI=${HIs[3]} ;;
        *) continue ;;
    esac
    echo "${KEY^} ${LO} ${HI}"
    part="synth ${TONE_LEN} sin ${LO} sin ${HI}"
    "${PLAY}" -q -n ${part} remix -
    PARTS+=( "${part}" )
done

echo "Concatenating tones"
declare SOX_PARTS=()
for part in "${PARTS[@]}"; do
    SOX_PARTS+=( "|sox -n -p ${part} remix -" )
    SOX_PARTS+=( "|sox -n -p ${PAUSE} remix -" )
done
# Remove the trailing PAUSE
unset SOX_PARTS[-1]
"${SOX}" "${SOX_PARTS[@]}" -c 2 -r 48000 -b 16 "${OUTPUT_FNAME}"
echo "Playing it back"
"${PLAY}" "${OUTPUT_FNAME}"
