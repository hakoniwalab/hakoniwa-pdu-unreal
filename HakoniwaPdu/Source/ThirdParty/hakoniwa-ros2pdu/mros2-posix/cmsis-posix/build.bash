#!/bin/bash

if [ $# -ne 0 -a $# -ne 1  -a $# -ne 2 ]
then
	echo "Usage: $0 {all|test|clean} [test_config]"
	echo "test_config:fifo or rr or default"
	exit 1
fi


OPT=${1}
if [ $# -eq 0 ]
then
	OPT="all"
fi

if [ ${OPT} = "clean" ]
then
	rm -rf ./Third_Party/FreeRTOS
else
	bash Third_Party/download.bash
fi

cd cmake-build
if [ ${OPT} = "clean" ]
then
	rm -rf ./*
	exit 0
fi

if [ ${OPT} = "test" ]
then
	cmake -D test=true -D debug=true -D gcov=true ..
else
	cmake -D debug=true -D gcov=true ..
fi

function do_test()
{
	cd ..
	test_config=${1}
	echo "test_config=${test_config}"

	cp src/config/cmsis_config.h test/config/cmsis_config_org.h 

	if [ "${test_config}" = "fifo" ]
	then
		cp test/config/cmsis_config_sched_fifo.h src/config/cmsis_config.h
	elif [ "${test_config}" = "rr" ]
	then
		cp test/config/cmsis_config_sched_rr.h src/config/cmsis_config.h
	else
		cp test/config/cmsis_config_sched_default.h src/config/cmsis_config.h
	fi
	cd cmake-build

	make
	make test

	cd ..
	mv test/config/cmsis_config_org.h src/config/cmsis_config.h
	cd cmake-build
}

if [ ${OPT} = "test" ]
then
	if [ $# -eq 2 ]
	then
		TEST_CONFIG=${2}
		do_test ${TEST_CONFIG}
	else
		do_test "default"
	fi

	cd src/CMakeFiles/cmsis.dir/api
	lcov -c -d . -o lcov.info
	genhtml lcov.info -o ./info
	cd ../core
	lcov -c -d . -o lcov.info
	genhtml lcov.info -o ./info
else
	make
fi

