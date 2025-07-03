#include "cmsis_posix_os_thread_sync.h"

osStatus_t osDelay(uint32_t ticks)
{
  if (CurrentContextIsISR() == true) {
    return osErrorISR;
  }
  if (ticks == 0) {
    return osErrorParameter;
  }
  return PosixOsThreadSyncSleep(ticks);
}
