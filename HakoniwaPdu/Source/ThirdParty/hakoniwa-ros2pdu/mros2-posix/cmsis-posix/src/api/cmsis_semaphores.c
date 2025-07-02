#include "cmsis_semaphores_private.h"
#include "cmsis_posix_os_thread_sync.h"
#include "cmsis_posix_os_memory.h"

osSemaphoreId_t osSemaphoreNew(uint32_t max_count, uint32_t initial_count, const osSemaphoreAttr_t* attr)
{
  CmsisSemType* semp = NULL;

  if (CurrentContextIsISR()) {
    return NULL;
  } else if (attr != NULL) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d attr must be null\n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
  }
  semp = (CmsisSemType*)PosixOsMemoryAlloc(sizeof(CmsisSemType));
  if (semp == NULL) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d cannot allocate memory size=%ld\n", __FILE__, __FUNCTION__, __LINE__, sizeof(CmsisSemType));
    return NULL;
  }
  semp->count = initial_count;
  semp->max_count = max_count;
  semp->magicno = POSIX_OSSEM_HEAD_MAGICNO;
  PosixOsQueueHeadInit(&semp->waiting);
  return (osSemaphoreId_t)semp;
}


osStatus_t osSemaphoreAcquire(osSemaphoreId_t semaphore_id, uint32_t timeout)
{
  CmsisSemType* semp;
  osStatus_t err;
  bool_t is_ctx_isr = CurrentContextIsISR();

  if (semaphore_id == NULL) {
    return osErrorParameter;
  }
  semp = (CmsisSemType*)semaphore_id;
  PosixOsThreadSyncLock();
  if (semp->magicno != POSIX_OSSEM_HEAD_MAGICNO) {
    PosixOsThreadSyncUnlock();
    return osErrorParameter;
  }
  if (is_ctx_isr) {
    if (timeout != 0) {
      PosixOsThreadSyncUnlock();
      return osErrorResource;
    }
  }
  err = osSemaphoreAcquire_nolock(semp, timeout);
  PosixOsThreadSyncUnlock();
  return err;
}

osStatus_t osSemaphoreRelease(osSemaphoreId_t semaphore_id)
{
  CmsisSemType* semp;
  osStatus_t err = osOK;

  if (semaphore_id == NULL) {
    return osErrorParameter;
  }
  semp = (CmsisSemType*)semaphore_id;
  PosixOsThreadSyncLock();
  if (semp->magicno != POSIX_OSSEM_HEAD_MAGICNO) {
    PosixOsThreadSyncUnlock();
    return osErrorParameter;
  }
  err = osSemaphoreRelease_nolock(semp);
  PosixOsThreadSyncUnlock();
  return err;
}
osStatus_t osSemaphoreDelete(osSemaphoreId_t semaphore_id)
{
  CmsisSemType* semp;
  osStatus_t err = osOK;

  if (CurrentContextIsISR()) {
    return osErrorISR;
  } else if (semaphore_id == NULL) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d semaphore_id is invalid value(%p)\n", __FILE__, __FUNCTION__, __LINE__, semaphore_id);
    return osErrorParameter;
  }
  semp = (CmsisSemType*)semaphore_id;
  PosixOsThreadSyncLock();
  if (semp->magicno != POSIX_OSSEM_HEAD_MAGICNO) {
    PosixOsThreadSyncUnlock();
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d invalid magicno(0x%x)\n", __FILE__, __FUNCTION__, __LINE__, semp->magicno);
    return osErrorParameter;
  }
  if (semp->waiting.count == 0) {
    semp->magicno = 0;
    PosixOsMemoryFree(semp);
  } else {
    err = osErrorResource;
  }
  PosixOsThreadSyncUnlock();
  return err;
}

osStatus_t osSemaphoreAcquire_nolock(CmsisSemType* semp, uint32_t timeout)
{
  osStatus_t err = osOK;
  osStatus_t ercd;

  if (semp->count > 0) {
    semp->count--;
  } else {
    if (timeout == 0) {
      err = osErrorResource;
    } else {
      if (timeout == osWaitForever) {
        timeout = POSIX_OS_THREAD_SYNC_WAIT_FOREVER;
      }
      (void)PosixOsThreadSyncWait(&semp->waiting, timeout, &ercd);
      if (ercd != osOK) {
        err = osErrorTimeout;
      }
    }
  }
  return err;
}

osStatus_t osSemaphoreRelease_nolock(CmsisSemType* semp)
{
  osStatus_t err = osOK;
  if (semp->waiting.count > 0) {
    (void)PosixOsThreadSyncWakeupFirstEntry(&semp->waiting, NULL, osOK);
  } else if (semp->count < semp->max_count) {
    semp->count++;
  } else {
    err = osErrorResource;
  }
  return err;
}


/*
 * Version 1
 */
osSemaphoreId osSemaphoreCreate(const osSemaphoreDef_t* semaphore_def, int32_t count)
{
  if (semaphore_def != NULL) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d semaphore_def should not be null\n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
  }
  return (osSemaphoreId)osSemaphoreNew(count, count, NULL);
}

int32_t osSemaphoreWait(osSemaphoreId semaphore_id, uint32_t millisec)
{
  CmsisSemType* semp = (CmsisSemType*)semaphore_id;
  osStatus_t err;
  bool_t is_ctx_isr = CurrentContextIsISR();

  if (is_ctx_isr) {
    return -1;
  }
  if (semaphore_id == NULL) {
    return -1;
  }
  semp = (CmsisSemType*)semaphore_id;
  PosixOsThreadSyncLock();
  if (semp->magicno != POSIX_OSSEM_HEAD_MAGICNO) {
    PosixOsThreadSyncUnlock();
    return -1;
  }
  err = osSemaphoreAcquire_nolock(semp, millisec);
  PosixOsThreadSyncUnlock();
  if (err != osOK) {
    return 0;
  }
  /*
   * Specification says:
   *  return value number of available tokens, and 0 means no semaphore was available.
   *
   * Available tokens must be the value before decrementing,
   * because user can not determine acquiring the token when previous value was 1.
   *
   * see following situation:
   *  previous value          : 1 , 0
   *  before decrementing case: 1 , 0
   *  after decrementing case : 0 , 0 <--- same value(user can not determine acquiring)
   *
   *  ref:
   *   https://github.com/ARMmbed/mbed-os/issues/4456
   */
  return (int32_t)(semp->count + 1);
}
