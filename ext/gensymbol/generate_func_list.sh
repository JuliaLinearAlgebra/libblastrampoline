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
NUM_LINES=0
echo -n "Outputting to ${OUTPUT_FILE}....."
echo "// This file generated by 'ext/gensymbol/generate_func_list.sh'" > "${OUTPUT_FILE}"
echo "#ifndef EXPORTED_FUNCS" >> "${OUTPUT_FILE}"
echo "#define EXPORTED_FUNCS(XX) \\" >> "${OUTPUT_FILE}"
for s in `cut -b2-100 tempsymbols.def`; do
    echo "    XX(${s}) \\" >> "${OUTPUT_FILE}"
    NUM_LINES=$((${NUM_LINES} + 1))
    [ $((${NUM_LINES} % 100)) == 0 ] && echo -n "."
done
echo >> "${OUTPUT_FILE}"
echo "#endif" >> "${OUTPUT_FILE}"
echo "Done, with ${NUM_LINES} symbols generated."

rm -f tempsymbols.def