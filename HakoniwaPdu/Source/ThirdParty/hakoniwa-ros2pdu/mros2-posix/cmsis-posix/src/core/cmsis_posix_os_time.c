#include "cmsis_posix_os_time.h"

static uint64_t posix_os_activated_time64;

static uint64_t posix_os_get_current_time(void)
{
  struct timespec tmo;
  clock_gettime(CLOCK_REALTIME, &tmo);
  uint64_t sec = tmo.tv_sec;
  uint64_t msec = (tmo.tv_nsec / (TIMESPEC_MSEC * TIMESPEC_MSEC));
  uint64_t ticks = (sec * TIMESPEC_MSEC) + msec;
  return ticks;
}

osStatus_t PosixOsTimerInit(void)
{
  if (posix_os_activated_time64 != 0) {
    return osError;
  }
  posix_os_activated_time64 = posix_os_get_current_time();
  return osOK;
}

uint32_t PosixOsTimeGetTickCount(void)
{
  uint64_t ctime64 = posix_os_get_current_time();
  uint64_t ctime64_relative = ctime64 - posix_os_activated_time64;
  return ((uint32_t)ctime64_relative);
}
