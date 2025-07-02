#include "cmsis_posix_os_memory.h"
#include "cmsis_semaphores_private.h"
#include "cmsis_posix_os_thread_sync.h"

#define AUTOSAR_OSMUTEX_HEAD_MAGICNO		0xDEADEEEB
typedef struct {
  pthread_t			owner;
  uint32_t			count;
  uint32_t			magicno;
  osSemaphoreId_t* sem;
  bool_t				is_recursive;
} CmsisMutexType;

osMutexId_t osMutexNew(const osMutexAttr_t* attr)
{
  CmsisMutexType* mutex = NULL;

  if (CurrentContextIsISR()) {
    return NULL;
  }
  mutex = (CmsisMutexType*)PosixOsMemoryAlloc(sizeof(CmsisMutexType));
  if (mutex == NULL) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d cannot allocate memory size=%ld\n", __FILE__, __FUNCTION__, __LINE__, sizeof(CmsisMutexType));
    return NULL;
  }
  mutex->sem = osSemaphoreNew(1, 1, NULL);
  if (mutex->sem == NULL) {
    PosixOsMemoryFree(mutex);
    return NULL;
  }
  mutex->magicno = AUTOSAR_OSMUTEX_HEAD_MAGICNO;
  mutex->owner = 0;
  mutex->count = 0;
  mutex->is_recursive = false;
  if (attr != NULL) {
    if ((attr->attr_bits & osMutexRecursive) != 0) {
      mutex->is_recursive = true;
    }
  }
  return (osMutexId_t)mutex;
}

osStatus_t osMutexAcquire(osMutexId_t mutex_id, uint32_t timeout)
{
  CmsisMutexType* mutex;
  pthread_t thread_id;
  osStatus_t err = osOK;
  bool_t is_ctx_isr = CurrentContextIsISR();

  if (is_ctx_isr) {
    return osErrorISR;
  } else if (mutex_id == NULL) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d invalid mutex_id(NULL)\n", __FILE__, __FUNCTION__, __LINE__);
    return osErrorParameter;
  }
  thread_id = pthread_self();
  mutex = (CmsisMutexType*)mutex_id;
  PosixOsThreadSyncLock();
  if (mutex->magicno != AUTOSAR_OSMUTEX_HEAD_MAGICNO) {
    PosixOsThreadSyncUnlock();
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d invalid magicno(0x%x)\n", __FILE__, __FUNCTION__, __LINE__, mutex->magicno);
    return osErrorParameter;
  }
  if ((mutex->is_recursive) && (mutex->count > 0) && (mutex->owner == thread_id)) {
    mutex->count++;
  } else {
    err = osSemaphoreAcquire_nolock((CmsisSemType*)mutex->sem, timeout);
    if (err == osOK) {
      mutex->owner = thread_id;
      mutex->count = 1;
    }
  }
  PosixOsThreadSyncUnlock();
  return err;
}

osStatus_t osMutexRelease(osMutexId_t mutex_id)
{
  CmsisMutexType* mutex;
  pthread_t thread_id;
  osStatus_t err = osOK;
  bool_t is_ctx_isr = CurrentContextIsISR();

  if (is_ctx_isr) {
    return osErrorISR;
  } else if (mutex_id == NULL) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d invalid mutex_id(NULL)\n", __FILE__, __FUNCTION__, __LINE__);
    return osErrorParameter;
  }
  thread_id = pthread_self();
  mutex = (CmsisMutexType*)mutex_id;
  PosixOsThreadSyncLock();
  if (mutex->magicno != AUTOSAR_OSMUTEX_HEAD_MAGICNO) {
    PosixOsThreadSyncUnlock();
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d invalid magicno(0x%x)\n", __FILE__, __FUNCTION__, __LINE__, mutex->magicno);
    return osErrorParameter;
  }
  if (mutex->owner == thread_id) {
    if ((mutex->is_recursive) && (mutex->count > 1)) {
      mutex->count--;
    } else {
      err = osSemaphoreRelease_nolock((CmsisSemType*)mutex->sem);
      if (err == osOK) {
        mutex->owner = 0;
        mutex->count = 0;
      }
    }
  } else {
    err = osErrorResource;
  }
  PosixOsThreadSyncUnlock();
  return err;
}
osStatus_t osMutexDelete(osMutexId_t mutex_id)
{
  CmsisMutexType* mutex;
  osStatus_t err = osOK;

  if (CurrentContextIsISR()) {
    return osErrorISR;
  } else if (mutex_id == NULL) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d invalid mutex_id(NULL)\n", __FILE__, __FUNCTION__, __LINE__);
    return osErrorParameter;
  }
  PosixOsThreadSyncLock();
  mutex = (CmsisMutexType*)mutex_id;
  if (mutex->magicno != AUTOSAR_OSMUTEX_HEAD_MAGICNO) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d invalid magicno(0x%x)\n", __FILE__, __FUNCTION__, __LINE__, mutex->magicno);
    err = osErrorParameter;
  } else if (mutex->count == 0) {
    mutex->magicno = 0;
  } else {
    err = osErrorResource;
  }
  PosixOsThreadSyncUnlock();

  if (err == osOK) {
    (void)osSemaphoreDelete(mutex->sem);
    PosixOsMemoryFree(mutex);
  }
  return err;
}

/*
 * Version 1
 */
osMutexId osMutexCreate(const osMutexDef_t* mutex_def)
{
  if (mutex_def != NULL) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d mutex_def should not be null\n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
  }
  return (osMutexId)osMutexNew(NULL);
}
