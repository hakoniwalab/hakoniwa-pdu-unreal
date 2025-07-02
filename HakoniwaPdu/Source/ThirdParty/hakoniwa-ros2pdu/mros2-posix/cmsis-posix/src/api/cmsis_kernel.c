#include "cmsis_posix_os_time.h"
#include "cmsis_posix_os_thread_sync.h"

osStatus_t osKernelStart(void)
{
  PosixOsThreadSyncInit();
  (void)PosixOsTimerInit();
  CMSIS_IMPL_INFO("osKernelStart");
  return osOK;
}

uint32_t osKernelGetTickCount(void)
{
  return PosixOsTimeGetTickCount();
}
