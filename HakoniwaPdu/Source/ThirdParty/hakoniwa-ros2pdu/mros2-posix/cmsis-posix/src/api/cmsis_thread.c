#include "cmsis_posix_os_memory.h"

typedef struct {
  osThreadFunc_t func;
  void* argument;
} PosixOsThreadFuncType;

static void* posix_os_thread_func(void* argp)
{
  osThreadFunc_t func = ((PosixOsThreadFuncType*)argp)->func;
  void* argument = ((PosixOsThreadFuncType*)argp)->argument;

  PosixOsMemoryFree(argp);
  func(argument);
  return NULL;
}
#ifdef CMSIS_PTHREAD_SCHED_REALTIME
static int get_pthread_priority(osPriority_t prio)
{
  if (prio > osPriorityRealtime7) {
    prio = osPriorityRealtime7;
  }
  return CMSIS_PTHREAD_GET_PRIORITY(prio);
}
static int set_pthread_schedule_policy(pthread_attr_t* pattr, const osThreadAttr_t* attr)
{
  int err;
  struct sched_param param;
  err = pthread_attr_setinheritsched(pattr, PTHREAD_EXPLICIT_SCHED);
  if (err != 0) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d pthread_attr_setinheritsched() error=%d\n", __FILE__, __FUNCTION__, __LINE__, err);
    return err;
  }
  err = pthread_attr_setschedpolicy(pattr, CMSIS_PTHREAD_SCHED_POLICY);
  if (err != 0) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d pthread_attr_setschedpolicy() error=%d\n", __FILE__, __FUNCTION__, __LINE__, err);
    return err;
  }
  if (attr != NULL) {
    param.sched_priority = get_pthread_priority(attr->priority);
  } else {
    param.sched_priority = get_pthread_priority(osPriorityNormal);
  }
  err = pthread_attr_setschedparam(pattr, &param);
  if (err != 0) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d pthread_attr_setschedparam() error=%d\n", __FILE__, __FUNCTION__, __LINE__, err);
    return err;
  }
#if 0 /* not supported on linux */
  err = pthread_attr_setscope(pattr, PTHREAD_SCOPE_PROCESS);
  if (err != 0) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d pthread_attr_setscope() error=%d\n", __FILE__, __FUNCTION__, __LINE__, err);
    return err;
  }
#endif
  return 0;
}
#endif /* CMSIS_PTHREAD_SCHED_REALTIME */

osThreadId_t osThreadNew(osThreadFunc_t	func, void* argument, const osThreadAttr_t* attr)
{
  int err;
  osThreadId_t ret = (osThreadId_t)NULL;
  pthread_t thread_id;
  PosixOsThreadFuncType *argp;

  if (CurrentContextIsISR()) {
    return NULL;
  }
  argp = (PosixOsThreadFuncType*)PosixOsMemoryAlloc(sizeof(PosixOsThreadFuncType));
  if (argp == NULL) {
    return NULL;
  }
  argp->func = func;
  argp->argument = argument;

  pthread_attr_t pattr;
  err = pthread_attr_init(&pattr);
  if (err != 0) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d pthread_attr_init() error=%d\n", __FILE__, __FUNCTION__, __LINE__, err);
    return NULL;
  }
#ifdef CMSIS_PTHREAD_SCHED_REALTIME
  if (set_pthread_schedule_policy(&pattr, attr) != 0) {
    pthread_attr_destroy(&pattr);
    return NULL;
  }
#endif /* CMSIS_PTHREAD_SCHED_REALTIME */
  err = pthread_create(&thread_id, &pattr, posix_os_thread_func, argp);
  if (err == 0) {
    ret = (osThreadId_t)thread_id;
  } else {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d pthread_create() error=%d\n", __FILE__, __FUNCTION__, __LINE__, err);
  }
  pthread_attr_destroy(&pattr);
  return (osThreadId_t)ret;
}

osStatus_t osThreadTerminate(osThreadId_t thread_id)
{
  if (CurrentContextIsISR()) {
    return osErrorISR;
  }
  pthread_exit(NULL);
  return osOK;
}
osStatus_t osThreadJoin(osThreadId_t thread_id)
{
  if (CurrentContextIsISR()) {
    return osErrorISR;
  }
  if (thread_id == NULL) {
    return osErrorParameter;
  }
  int ret = pthread_join((pthread_t)thread_id, NULL);
  if (ret != 0) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d pthread_join() error=%d\n", __FILE__, __FUNCTION__, __LINE__, errno);
    return osErrorResource;
  }
  return osOK;
}
/*
 * Version 1
 */
osThreadId osThreadCreate(const osThreadDef_t* thread_def, void* argument)
{
  if (thread_def == NULL) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d thread_def should not be NULL\n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
  } else if (thread_def->pthread == NULL) {
    CMSIS_IMPL_ERROR("ERROR:%s %s() %d thread_def->pthread should not be NULL\n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
  }
  return osThreadNew((osThreadFunc_t)thread_def->pthread, argument, &thread_def->attr);
}
