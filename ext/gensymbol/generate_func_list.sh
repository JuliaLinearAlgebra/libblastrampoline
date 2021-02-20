#!/bin/bash

LBT_DIR="$(dirname $(dirname "$(cd "$(dirname "${BASH_SOURCE[0]}")" > /dev/null && pwd)"))"

SYS=osx
ARCH=x86_64
BU=_
EXPRECISION=1
NO_CBLAS=0
NO_LAPACK=0
NO_LAPACKE=0
NEED2UNDERSCORES=0
ONLY_CBLAS=0
SYMBOLPREFIX=""
SYMBOLSUFFIX=""
BUILD_LAPACK_DEPRECATED=1
BUILD_BFLOAT16=1
BUILD_SINGLE=1
BUILD_DOUBLE=1
BUILD_COMPLEX=1
BUILD_COMPLEX16=1

perl ./gensymbol ${SYS} ${ARCH} ${BU} ${EXPRECISION} \
                 ${NO_CBLAS} ${NO_LAPACK} ${NO_LAPACKE} ${NEED2UNDERSCORES} \
                 ${ONLY_CBLAS} "" "" \
                 ${BUILD_LAPACK_DEPRECATED} ${BUILD_BFLOAT16} ${BUILD_SINGLE} ${BUILD_DOUBLE} ${BUILD_COMPLEX} ${BUILD_COMPLEX16} | LC_COLLATE=C sort > tempsymbols.def

OUTPUT_FILE="${LBT_DIR}/src/exported_funcs.inc"
echo -n "Outputting to ${OUTPUT_FILE}....."
echo "// This file generated by 'ext/gensymbol/generate_func_list.sh'" > "${OUTPUT_FILE}"
echo "// Do not edit manually; your changes will be overwritten" >> "${OUTPUT_FILE}"

# These are all the symbols we will export
# Note that we filter out all openblas-specific stuff.
EXPORTED_FUNCS=$(cut -b2-100 tempsymbols.def | grep -v openblas_ | grep -v goto_)
echo "#ifndef EXPORTED_FUNCS" >> "${OUTPUT_FILE}"
echo "#define EXPORTED_FUNCS(XX) \\" >> "${OUTPUT_FILE}"
NUM_EXPORTED=0
for s in ${EXPORTED_FUNCS}; do
    echo "    XX(${s}) \\" >> "${OUTPUT_FILE}"
    NUM_EXPORTED=$((${NUM_EXPORTED} + 1))
    [ $((${NUM_EXPORTED} % 100)) == 0 ] && echo -n "."
done
echo >> "${OUTPUT_FILE}"
echo "#endif" >> "${OUTPUT_FILE}"


# Helper function that finds the index of a word in a list of words
word_idx() {
    idx=0
    for word in $1; do
        if [[ "$word" == "$2" ]]; then
            echo $idx
            break
        fi
        idx=$((idx+1))
    done
}

# Also generate a list of functions that need an f2c wrapper

NUM_F2C=0

function output_func() {
    # Skip floatret functions that don't actually exist in our exported funcs
    widx=$(word_idx "${EXPORTED_FUNCS}" ${1})
    if [[ -n "${widx}" ]]; then
        echo "    XX(${1}, ${widx}) \\" >> "${OUTPUT_FILE}"
        NUM_F2C=$((${NUM_F2C} + 1))
        [ $((${NUM_F2C} % 10)) == 0 ] && echo -n "."
    fi
}

FLOAT32_FUNCS="sdot_ sdsdot_ sasum_ scasum_ ssum_ scsum_ samax_ scamax_ samin_ scamin_ smax_ scmax_ smin_ scmin_ snrm2_ scnrm2_ slamch_ slamc3_ "
echo >> "${OUTPUT_FILE}"
echo "#ifndef FLOAT32_FUNCS" >> "${OUTPUT_FILE}"
echo "#define FLOAT32_FUNCS(XX) \\" >> "${OUTPUT_FILE}"
for func_name in ${FLOAT32_FUNCS}; do
    output_func "${func_name}"
done
echo >> "${OUTPUT_FILE}"
echo "#endif" >> "${OUTPUT_FILE}"

COMPLEX64_FUNCS="cdotu_ cdotc_"
echo >> "${OUTPUT_FILE}"
echo "#ifndef COMPLEX64_FUNCS" >> "${OUTPUT_FILE}"
echo "#define COMPLEX64_FUNCS(XX) \\" >> "${OUTPUT_FILE}"
for func_name in ${COMPLEX64_FUNCS}; do
    output_func "${func_name}"
done
echo >> "${OUTPUT_FILE}"
echo "#endif" >> "${OUTPUT_FILE}"

COMPLEX128_FUNCS="zdotu_ zdotc_"
echo >> "${OUTPUT_FILE}"
echo "#ifndef COMPLEX128_FUNCS" >> "${OUTPUT_FILE}"
echo "#define COMPLEX128_FUNCS(XX) \\" >> "${OUTPUT_FILE}"
for func_name in ${COMPLEX128_FUNCS}; do
    output_func "${func_name}"
done
echo >> "${OUTPUT_FILE}"
echo "#endif" >> "${OUTPUT_FILE}"

# Report to the user and cleanup
echo "Done, with ${NUM_EXPORTED} symbols generated (${NUM_F2C} f2c functions noted)."
rm -f tempsymbols.def
