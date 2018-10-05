#!/bin/bash

source regression.properties

OUT="out"
HOMEDIR=`pwd`

TIME_CLEAR="time_clear.txt"
TIME_OBF="time_obf.txt"
TIME_CSV="time.txt"

ERROR_DIR=0
ERROR_MESSAGE=""

ERROR_ORACLE=0
ERROR_TEST=0
PASS_TEST=0
FAIL_TEST=0

rm -r ${OUT}
mkdir ${OUT}


for DIR in ${REGRESSION_DIRS}
do	
	echo "*** TEST CASE: ${DIR}"
	cd ${DIR}

	echo "* Loading test case properties [${DIR}]"
	source testcase.properties

	echo "* Compiling original [${DIR}]"
	echo "${TEST_BUILD}..."
	${TEST_BUILD}
	if [ $? -ne 0 ]
	then
		echo "ERROR"
		ERROR_DIR=`expr ${ERROR_DIR} + 1`
		ERROR_MESSAGE="${ERROR_MESSAGE} ${DIR}"
		continue;
	else
		echo "...ok"
	fi

	echo "* Running the original [${DIR}]"
	rm output.txt
	while read -r LINE
	do
		ts=$(date +%s%N)		
		./${TEST_PROGRAM} ${LINE} >> output.txt
		te=$((($(date +%s%N) - $ts)/1000000))
		echo "$te" >> time_clear.txt		

		if [ $? -ne 0 ]
			then
			echo "ERROR on running original code for input: ${LINE}"
			ERROR_ORACLE=`expr ${ERROR_ORACLE} + 1`
		fi
	done <${TEST_DATA}


	echo "* Transforming [${DIR}]"
	cd "${HOMEDIR}"
	echo "cp -r ${DIR} ${OUT}..."
	cp -r ${DIR} ${OUT}
	cd ${OUT}/${DIR} 
	THIS_ERROR=0
	for FILE in ${TEST_SOURCES}
	do
		echo "${REGRESSION_TRANSFORM} ${FILE}.rule ${FILE}..."
		${REGRESSION_TRANSFORM} ${FILE}.rule ${FILE}
		if [ $? -ne 0 ]
		then
			echo "ERROR while transforming ${DIR}/${FILE}"
			THIS_ERROR=1
		fi
	done
	if [ ${THIS_ERROR} -ne 0 ]
	then
		echo "ERROR in applying the transformation ${REGRESSION_TRANSFORM}"
		ERROR_DIR=`expr ${ERROR_DIR} + 1`
		ERROR_MESSAGE="${ERROR_MESSAGE} ${DIR}"
		continue;
	else
		echo "...ok"
	fi


	echo "* Compiling transformed [${DIR}]"
	cd ${HOMEDIR}/${OUT}/${DIR}
	echo "${TEST_BUILD}..."
	${TEST_BUILD}
	if [ $? -ne 0 ]
	then
		echo "ERROR when compiling transformed code"
		ERROR_DIR=`expr ${ERROR_DIR} + 1`
		ERROR_MESSAGE="${ERROR_MESSAGE} ${DIR}"
		continue;
	else
		echo "...ok"
	fi

	echo "* Running the transformed [${DIR}]"
	rm output.txt
	while read -r LINE
	do
		ts=$(date +%s%N)		
		./${TEST_PROGRAM} ${LINE} >> output.txt
		te=$((($(date +%s%N) - $ts)/1000000))
		echo "$te" >> time_obf.txt		
		if [ $? -ne 0 ]
			then
			echo "ERROR on running transformed code for input ${LINE}"
			ERROR_TEST=`expr ${ERROR_TEST} + 1`
		fi
	done < ${TEST_DATA}

	echo "* Comparing output [${DIR}]"
	cd ${HOMEDIR}
	echo "diff --minimal ${HOMEDIR}/${DIR}/output.txt ${HOMEDIR}/${OUT}/${DIR}/output.txt > ${DIR}.diff..."
	diff --minimal ${HOMEDIR}/${DIR}/output.txt ${HOMEDIR}/${OUT}/${DIR}/output.txt > ${DIR}.diff
	if [ `cat ${DIR}.diff | wc -l` -ne 0 ]
	then
		echo "Test failure in ${DIR}"
		N=`cat ${DIR}.diff | wc -l`
		FAIL_TEST=`expr ${FAIL_TEST} + ${N}`
	else
		echo "Test pass in '${DIR}'"
		N=`cat ${HOMEDIR}/${DIR}/${TEST_DATA} | wc -l`
		PASS_TEST=`expr ${PASS_TEST} + ${N}`
	fi

	cd ${HOMEDIR}

	exec 5< ${HOMEDIR}/${OUT}/${DIR}/${TIME_OBF}
	exec 6< ${HOMEDIR}/${DIR}/${TIME_CLEAR}
	while read line1 <&5 && read line2 <&6
	do
	echo  "$line1,$line2" >> time.txt
	done

	rm ${HOMEDIR}/${OUT}/${DIR}/${TIME_OBF}
	rm ${HOMEDIR}/${DIR}/${TIME_CLEAR}
echo
echo

done

echo
echo "*** REGRESSION TESTING REPORT"
N=`echo ${DIR} | wc -w `
echo "* test programs: ${N}"
echo "* pass tests: ${PASS_TEST}"
echo "* fail tests: ${FAIL_TEST}"
echo "* errors test: ${ERROR_TEST}"
echo "* errors in oracle: ${ERROR_ORACLE}"
if [ ${ERROR_DIR} -ne 0 ]
then
	echo "* ERROR ON ${ERROR_DIR} programs: ${ERROR_MESSAGE}"
fi

ALL=`expr ${ERROR_DIR} + ${FAIL_TEST} + ${ERROR_TEST} + ${ERROR_ORACLE}`
if [ ${ALL} -ne 0 ]
then
	echo "<<TEST FAIL!!>>"
else
	echo "<<Test pass>>"
fi


