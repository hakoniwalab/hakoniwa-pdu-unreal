#!/bin/bash


CMSIS_OS_BRANCH="mros2"
CMSIS_OS_HDIR="Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2"
CMSIS_OS_REPO=https://raw.githubusercontent.com/mROS-base/STM32CubeF7/${CMSIS_OS_BRANCH}/Middlewares/${CMSIS_OS_HDIR}

function download_cmsis_osheader()
{
	if [ -d ${CMSIS_OS_HDIR} ]
	then
		:
	else
		wget "${CMSIS_OS_REPO}/cmsis_os.h"
		wget "${CMSIS_OS_REPO}/cmsis_os2.h"
		mkdir -p ${CMSIS_OS_HDIR}
		mv cmsis_os.h ${CMSIS_OS_HDIR}/
		mv cmsis_os2.h ${CMSIS_OS_HDIR}/
	fi
}

download_cmsis_osheader
