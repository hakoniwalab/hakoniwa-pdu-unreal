#include "cmsis_posix_os_message_queue.h"
#include "cmsis_posix_os_memory.h"
#include "cmsis_posix_os_thread_sync.h"
#include <string.h>

PosixOsMessageQueueType* PosixOsMessageQueueCreate(PosixOsMessageQueueConfigType* config)
{
  uint32_t i;
  uint32_t entry_total_size;
  uint32_t msg_data_total_size;
  char* top;
  char* entry_top;
  char* msg_top;
  PosixOsMessageQueueType* qh;

  entry_total_size = sizeof(PosixOsMessageQueueEntryType) * config->prealloc_num;
  msg_data_total_size = config->entry_size * config->prealloc_num;

  top = PosixOsMemoryAlloc(sizeof(PosixOsMessageQueueType) + entry_total_size + msg_data_total_size);
  if (top == NULL) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d cannot allocate memory size=%ld\n", __FILE__, __FUNCTION__, __LINE__, sizeof(PosixOsMessageQueueType) + entry_total_size + msg_data_total_size);
    return NULL;
  }
  qh = (PosixOsMessageQueueType*)top;
  qh->entry_size = config->entry_size;
  qh->prealloc_num = config->prealloc_num;
  qh->magicno = POSIX_OSMESSAGE_QUEUE_HEAD_MAGICNO;
  PosixOsQueueHeadInit(&qh->free);
  PosixOsQueueHeadInit(&qh->used);
  PosixOsQueueHeadInit(&qh->getter_waiting);
  PosixOsQueueHeadInit(&qh->putter_waiting);

  entry_top = &top[sizeof(PosixOsMessageQueueType)];
  msg_top = &entry_top[entry_total_size];
  /*
   *  [ queue memory image ]
   *
   *     qh     entry0 entry1   .................  entryN    msg0     msg1      ...................     msgN
   *   |------|------|------|------|------|------|------|----------|----------|----------|----------|----------|
   */
  PosixOsMessageQueueEntryType* entry = (PosixOsMessageQueueEntryType*)entry_top;
  for (i = 0; i < qh->prealloc_num; i++) {
    entry->prio = 0;
    entry->data = &msg_top[i * qh->entry_size];
    PosixOsQueueHeadAddTail(&qh->free, &entry->queue);
    entry++;
  }
  return qh;
}

osStatus_t PosixOsMessageQueueDelete(PosixOsMessageQueueType* qh)
{
  if (qh == NULL) {
    return osErrorParameter;
  } else if (qh->magicno != POSIX_OSMESSAGE_QUEUE_HEAD_MAGICNO) {
    return osErrorParameter;
  }
  qh->magicno = 0;
  PosixOsMemoryFree(qh);
  return osOK;
}

osStatus_t PosixOsMessageQueueGet(PosixOsMessageQueueType* qh, void* msg_ptr, uint8_t* msg_prio, uint32_t timeout)
{
  osStatus_t ercd = osOK;
  PosixOsMessageQueueEntryType* entry = NULL;

  PosixOsThreadSyncLock();
  if (qh->magicno != POSIX_OSMESSAGE_QUEUE_HEAD_MAGICNO) {
    PosixOsThreadSyncUnlock();
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d invalid magicno=%d\n", __FILE__, __FUNCTION__, __LINE__, qh->magicno);
    return osErrorParameter;
  }
  entry = (PosixOsMessageQueueEntryType*)PosixOsQueueHeadRemoveFirst(&qh->used);
  if (entry == NULL) {
    if (timeout != 0) {
      entry = (PosixOsMessageQueueEntryType*)PosixOsThreadSyncWait(&qh->getter_waiting, timeout, NULL);
    }
  }
  if (entry != NULL) {
    memcpy(msg_ptr, entry->data, qh->entry_size);
    if (!PosixOsThreadSyncWakeupFirstEntry(&qh->putter_waiting, entry, osOK)) {
      PosixOsQueueHeadAddTail(&qh->free, &entry->queue);
    } else {
      /* nothing to do */
    }
  } else {
    ercd = osErrorResource;
  }
  PosixOsThreadSyncUnlock();
  return ercd;
}
bool_t PosixOsMessageQueueIsValid(PosixOsMessageQueueType* qh)
{
  return (qh->magicno == POSIX_OSMESSAGE_QUEUE_HEAD_MAGICNO);
}

osStatus_t PosixOsMessageQueuePut(PosixOsMessageQueueType* qh, const void* msg_ptr, uint8_t msg_prio, uint32_t timeout)
{
  osStatus_t ercd = osOK;
  PosixOsMessageQueueEntryType* entry = NULL;

  PosixOsThreadSyncLock();
  if (qh->magicno != POSIX_OSMESSAGE_QUEUE_HEAD_MAGICNO) {
    PosixOsThreadSyncUnlock();
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d invalid magicno=%d\n", __FILE__, __FUNCTION__, __LINE__, qh->magicno);
    return osErrorParameter;
  }
  entry = (PosixOsMessageQueueEntryType*)PosixOsQueueHeadRemoveFirst(&qh->free);
  if (entry == NULL) {
    if (timeout != 0) {
      entry = (PosixOsMessageQueueEntryType*)PosixOsThreadSyncWait(&qh->putter_waiting, timeout, NULL);
    }
  }
  if (entry != NULL) {
    memcpy(entry->data, msg_ptr, qh->entry_size);
    if (!PosixOsThreadSyncWakeupFirstEntry(&qh->getter_waiting, entry, osOK)) {
      (void)PosixOsQueueHeadAddTail(&qh->used, &entry->queue);
    } else {
      /* nothing to do */
    }
  } else {
    ercd = osErrorResource;
  }
  PosixOsThreadSyncUnlock();
  return ercd;
}
