#ifndef _CMSIS_POSIX_OS_MEMORY_H_
#define _CMSIS_POSIX_OS_MEMORY_H_

#include "cmsis_posix_os_common.h"

extern void *PosixOsMemoryAlloc(uint32_t size);
extern void PosixOsMemoryFree(void *addrp);

#endif /* _CMSIS_POSIX_OS_MEMORY_H_ */
