#ifndef _CMSIS_POSIX_OS_MESSAGE_QUEUE_H_
#define _CMSIS_POSIX_OS_MESSAGE_QUEUE_H_

#include "cmsis_posix_os_queue.h"

/***********************************************
 *
 * MESSAGE QUEUE
 *
 ***********************************************/
#define MESSAGE_QUEUE_ENTRY_ALIGNMENT_SIZE	4U
#define MESSAGE_QUEUE_ENTRY_ALIGNED_SIZE(size)	\
	(	\
			(	\
				(	\
						(size) + (MESSAGE_QUEUE_ENTRY_ALIGNMENT_SIZE - 1)	\
				)	\
				/ MESSAGE_QUEUE_ENTRY_ALIGNMENT_SIZE	\
			)	\
			* MESSAGE_QUEUE_ENTRY_ALIGNMENT_SIZE	\
	)


#define POSIX_OSMESSAGE_QUEUE_HEAD_MAGICNO		0xDEAFBEAD
typedef struct {
  uint16_t			prealloc_num;
  uint16_t			entry_size;
  uint32_t			magicno;
  PosixOsQueueHeadType	used;
  PosixOsQueueHeadType	free;
  PosixOsQueueHeadType	getter_waiting;
  PosixOsQueueHeadType	putter_waiting;
} PosixOsMessageQueueType;


typedef struct {
  CMSIS_IMPL_QUEUE						queue;
  /*
   * Priority
   *
   *  Highest  UINT16_MAX
   *  Lowest   0
   */
  uint16_t					prio;
  void* data;
} PosixOsMessageQueueEntryType;


typedef struct {
  uint16_t 					prealloc_num;
  uint16_t 					entry_size;
  PosixOsMessageQueueEntryType* control_datap;
  void* entries_datap;
} PosixOsMessageQueueConfigType;
extern PosixOsMessageQueueType* PosixOsMessageQueueCreate(PosixOsMessageQueueConfigType* config);
extern osStatus_t PosixOsMessageQueueDelete(PosixOsMessageQueueType* qh);

extern osStatus_t PosixOsMessageQueueGet(PosixOsMessageQueueType* qh, void* msg_ptr, uint8_t* msg_prio, uint32_t timeout);
extern osStatus_t PosixOsMessageQueuePut(PosixOsMessageQueueType* qh, const void* msg_ptr, uint8_t msg_prio, uint32_t timeout);
extern bool_t PosixOsMessageQueueIsValid(PosixOsMessageQueueType* qh);

#endif /* _CMSIS_POSIX_OS_MESSAGE_QUEUE_H_ */
