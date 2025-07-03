#include <stdio.h>
#include "cmsis_os.h"
#include "gtest/gtest.h"

static int result = -1;
void TestThread(void* arg)
{
  if (arg == NULL) {
    result = RUN_ALL_TESTS();
    osThreadTerminate(NULL);
  }
}


int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  osKernelStart();
  osThreadAttr_t attr;
  attr.priority = osPriorityNormal;
  osThreadId_t thr = osThreadNew(TestThread, NULL, &attr);
  osThreadJoin(thr);
  return result;
}
