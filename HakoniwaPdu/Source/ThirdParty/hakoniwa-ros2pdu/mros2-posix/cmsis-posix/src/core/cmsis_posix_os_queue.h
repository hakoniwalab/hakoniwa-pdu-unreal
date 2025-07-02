#ifndef _CMSIS_POSIX_OS_QUEUE_H_
#define _CMSIS_POSIX_OS_QUEUE_H_

#include "cmsis_posix_os_common.h"

typedef struct {
  uint16_t	count;
  CMSIS_IMPL_QUEUE		*entries;
} PosixOsQueueHeadType;

#define PosixOsQueueHeadInitializer(var)	\
		PosixOsQueueHeadType var = { .count = 0, .entries = NULL }
#define PosixOsQueueHeadInit(qheadp)	\
do {	\
	(qheadp)->count = 0;	\
	(qheadp)->entries = NULL;	\
} while (0)

extern void PosixOsQueueHeadAddTail(PosixOsQueueHeadType *headp, CMSIS_IMPL_QUEUE *entry);
extern CMSIS_IMPL_QUEUE* PosixOsQueueHeadRemoveFirst(PosixOsQueueHeadType *headp);
extern void PosixOsQueueHeadRemoveEntry(PosixOsQueueHeadType *headp, CMSIS_IMPL_QUEUE *entry);

#endif /* _CMSIS_POSIX_OS_QUEUE_H_ */
