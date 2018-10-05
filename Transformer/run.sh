#!/bin/bash

set -e

if [ -z "$VETDIR" ]
then
	echo "Define VETDIR to point to root of VariableEncodingTool source code"
	exit 1
fi

if [ "$(which Transformer)" = "" ]
then
	echo Unable to run Transformer, please export PATH accordingly
	exit 1
fi

rfile=${1?'specify a rule file'}
ifile=${2?'specify an input file'}
ofile=${3}

bifile=$(basename ${ifile})

if [ "$ofile" = "" ]
then
	ofile=${bifile}
fi

tfile=/tmp/${bifile}.$$.cpp
tfile1=/tmp/${bifile}.inc.$$.cpp

# idir="$(dirname $(which Transformer))/res"
idir=${VETDIR}/Transformer/res

echo -e '#define wchar_t int\n#include "'${idir}'/Encoded.cpp"' | cat - ${ifile} > ${tfile1}

CPP=clang++

echo $CPP -E -I. -o ${tfile} ${tfile1}
$CPP -E -I. -o ${tfile} ${tfile1}

echo Transformer -n ${rfile} ${tfile}
Transformer -n ${rfile} ${tfile}

echo Transformer -t ${rfile} ${tfile}
Transformer -t ${rfile} ${tfile}

cp ${tfile} ${ofile}
# rm ${tfile1}
