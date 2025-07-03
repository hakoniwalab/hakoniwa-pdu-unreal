#include "gtest/gtest.h"
#include "cmsis_os.h"
#include "api_test_utils.h"

class MessageQueueTest : public ::testing::Test
{
protected:
  static void SetUpTestCase()
  {
  }

  static void TearDownTestCase()
  {
  }

  virtual void SetUp()
  {
  }

  virtual void TearDown()
  {
  }

};
typedef struct {
  osMessageQueueId_t msgq_id;
  uint32_t timeout;
  int expect_value;
} ThreadArgType;

static void test_thread_task(void* argp)
{
  int msg_data;
  ThreadArgType* targp = (ThreadArgType*)argp;
  EXPECT_TRUE(targp->msgq_id != NULL);

  osStatus_t err = osMessageQueueGet(targp->msgq_id, (void*)&msg_data, 0, targp->timeout);
  EXPECT_EQ(osOK, err);
  EXPECT_EQ(targp->expect_value, msg_data);

  (void)osThreadTerminate(NULL);
  return;
}


TEST_F(MessageQueueTest, osMessageQueueNew_01)
{
  osMessageQueueId_t msgq_id = osMessageQueueNew(10, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);
  (void)osMessageQueueDelete(msgq_id);
}
TEST_F(MessageQueueTest, osMessageQueueDelete_01)
{
  osMessageQueueId_t msgq_id = osMessageQueueNew(10, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);
  osStatus_t err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);
  return;
}

TEST_F(MessageQueueTest, osMessageQueueDelete_02)
{
  osMessageQueueId_t msgq_id = osMessageQueueNew(10, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);
  osStatus_t err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);
  err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osErrorParameter, err);
  return;
}

TEST_F(MessageQueueTest, osMessageQueueGet_01)
{
  int msg_data = 10;
  ThreadArgType arg;
  osStatus_t err;
  osMessageQueueId_t msgq_id = osMessageQueueNew(1, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);
  {
    err = osMessageQueuePut(msgq_id, (void*)&msg_data, 0, osWaitForever);
    EXPECT_EQ(osOK, err);

    arg.msgq_id = msgq_id;
    arg.expect_value = msg_data;
    arg.timeout = 0;
    osThreadId_t id = osThreadNew(test_thread_task, &arg, NULL);
    EXPECT_TRUE(id != NULL);
    osStatus_t ret = osThreadJoin(id);
    EXPECT_EQ(osOK, ret);

    err = osMessageQueueGet(msgq_id, (void*)&msg_data, 0, 0);
    EXPECT_EQ(osErrorResource, err);
  }
  err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);
  return;
}

TEST_F(MessageQueueTest, osMessageQueueGet_07)
{
  osStatus_t err;
  int msg_data1 = 10;
  osMessageQueueId_t msgq_id = osMessageQueueNew(1, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);
  {
    err = osMessageQueueGet(msgq_id, (void*)&msg_data1, 0, 0);
    EXPECT_EQ(osErrorResource, err);
  }
  err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);
  return;
}
TEST_F(MessageQueueTest, osMessageQueueGet_09)
{
  osStatus_t err;
  int msg_data1 = 123;
  osMessageQueueId_t msgq_id = osMessageQueueNew(1, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);
  {
    uint32_t current_time = osKernelGetTickCount();
    err = osMessageQueueGet(msgq_id, (void*)&msg_data1, 0, 10);
    uint32_t wakeup_time = osKernelGetTickCount();
    TestAssertInRange(10, 11, (wakeup_time - current_time));
    EXPECT_EQ(osErrorTimeout, err);

  }
  err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);
  return;
}

TEST_F(MessageQueueTest, osMessageQueueGet_10)
{
  ThreadArgType arg;
  int msg_data = 10;
  osStatus_t err;
  osMessageQueueId_t msgq_id = osMessageQueueNew(1, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);
  {
    arg.msgq_id = msgq_id;
    arg.expect_value = msg_data;
    arg.timeout = osWaitForever;
    osThreadId_t id = osThreadNew(test_thread_task, &arg, NULL);
    EXPECT_TRUE(id != NULL);

    osDelay(1000);

    err = osMessageQueuePut(msgq_id, (void*)&msg_data, 0, osWaitForever);
    EXPECT_EQ(osOK, err);

    osStatus_t ret = osThreadJoin(id);
    EXPECT_EQ(osOK, ret);

    err = osMessageQueueGet(msgq_id, (void*)&msg_data, 0, 0);
    EXPECT_EQ(osErrorResource, err);
  }
  err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);
  return;
}
TEST_F(MessageQueueTest, osMessageQueueGet_11)
{
  osThreadId_t ids[2];
  ThreadArgType arg[2];
  int msg_data1 = 1024;
  int msg_data2 = 8192;
  osStatus_t err;
  osMessageQueueId_t msgq_id = osMessageQueueNew(1, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);
  {
    arg[0].msgq_id = msgq_id;
    arg[0].expect_value = msg_data1;
    arg[0].timeout = osWaitForever;
    ids[0] = osThreadNew(test_thread_task, &arg[0], NULL);
    osDelay(100);

    arg[1].msgq_id = msgq_id;
    arg[1].expect_value = msg_data2;
    arg[1].timeout = osWaitForever;
    ids[1] = osThreadNew(test_thread_task, &arg[1], NULL);
    osDelay(100);

    err = osMessageQueuePut(msgq_id, (void*)&msg_data1, 0, osWaitForever);
    EXPECT_EQ(osOK, err);
    err = osMessageQueuePut(msgq_id, (void*)&msg_data2, 0, osWaitForever);
    EXPECT_EQ(osOK, err);

    osThreadJoin(ids[0]);
    osThreadJoin(ids[1]);

    err = osMessageQueueGet(msgq_id, (void*)&msg_data1, 0, 0);
    EXPECT_EQ(osErrorResource, err);

  }
  err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);
  return;
}

TEST_F(MessageQueueTest, osMessageQueueGet_12)
{
  osThreadId_t ids[3];
  ThreadArgType arg[3];
  int msg_data1 = 1024;
  int msg_data2 = 8192;
  int msg_data3 = 4096;
  osStatus_t err;
  osMessageQueueId_t msgq_id = osMessageQueueNew(1, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);
  {
    arg[0].msgq_id = msgq_id;
    arg[0].expect_value = msg_data1;
    arg[0].timeout = osWaitForever;
    ids[0] = osThreadNew(test_thread_task, &arg[0], NULL);
    osDelay(100);

    arg[1].msgq_id = msgq_id;
    arg[1].expect_value = msg_data2;
    arg[1].timeout = osWaitForever;
    ids[1] = osThreadNew(test_thread_task, &arg[1], NULL);
    osDelay(100);

    arg[2].msgq_id = msgq_id;
    arg[2].expect_value = msg_data3;
    arg[2].timeout = osWaitForever;
    ids[2] = osThreadNew(test_thread_task, &arg[2], NULL);
    osDelay(100);

    err = osMessageQueuePut(msgq_id, (void*)&msg_data1, 0, osWaitForever);
    EXPECT_EQ(osOK, err);
    err = osMessageQueuePut(msgq_id, (void*)&msg_data2, 0, osWaitForever);
    EXPECT_EQ(osOK, err);
    err = osMessageQueuePut(msgq_id, (void*)&msg_data3, 0, osWaitForever);
    EXPECT_EQ(osOK, err);

    osThreadJoin(ids[0]);
    osThreadJoin(ids[1]);
    osThreadJoin(ids[2]);

    err = osMessageQueueGet(msgq_id, (void*)&msg_data1, 0, 0);
    EXPECT_EQ(osErrorResource, err);

  }
  err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);
  return;
}

TEST_F(MessageQueueTest, osMessageQueueGet_13)
{
  int msg_data1 = 1024;
  osStatus_t err;
  osMessageQueueId_t msgq_id = osMessageQueueNew(1, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);
  err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);

  err = osMessageQueueGet(msgq_id, (void*)&msg_data1, 0, 0);
  EXPECT_EQ(osErrorParameter, err);
  return;
}

TEST_F(MessageQueueTest, osMessageQueueGetCount_01)
{
  int msg_data = 10;

  osStatus_t err;
  osMessageQueueId_t msgq_id = osMessageQueueNew(10, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);

  err = osMessageQueuePut(msgq_id, (void*)&msg_data, 0, osWaitForever);
  EXPECT_EQ(osOK, err);

  uint32_t count = osMessageQueueGetCount(msgq_id);
  EXPECT_EQ(1, count);

  err = osMessageQueueGet(msgq_id, (void*)&msg_data, 0, 0);
  EXPECT_EQ(osOK, err);

  err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);
  return;
}

TEST_F(MessageQueueTest, osMessageQueueGetCount_02)
{
  int msg_data = 10;
  int i;

  osStatus_t err;
  osMessageQueueId_t msgq_id = osMessageQueueNew(10, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);

  for (i = 0; i < 10; i++) {
    err = osMessageQueuePut(msgq_id, (void*)&msg_data, 0, osWaitForever);
    EXPECT_EQ(osOK, err);
  }

  uint32_t count = osMessageQueueGetCount(msgq_id);
  EXPECT_EQ(10, count);

  for (i = 0; i < 10; i++) {
    err = osMessageQueueGet(msgq_id, (void*)&msg_data, 0, 0);
    EXPECT_EQ(osOK, err);
  }

  err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);
  return;
}

TEST_F(MessageQueueTest, osMessageQueueGetCount_03)
{
  osStatus_t err;
  osMessageQueueId_t msgq_id = osMessageQueueNew(10, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);

  uint32_t count = osMessageQueueGetCount(msgq_id);
  EXPECT_EQ(0, count);

  err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);
  return;
}

TEST_F(MessageQueueTest, osMessageQueueGetCount_04)
{
  osStatus_t err;
  osMessageQueueId_t msgq_id = osMessageQueueNew(1, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);

  err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);

  uint32_t count = osMessageQueueGetCount(msgq_id);
  EXPECT_EQ(0, count);
  return;
}

TEST_F(MessageQueueTest, osMessageQueuePut_01)
{
  int msg_data = 10;
  osStatus_t err;
  osMessageQueueId_t msgq_id = osMessageQueueNew(1, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);
  {
    err = osMessageQueuePut(msgq_id, (void*)&msg_data, 0, 0);
    EXPECT_EQ(osOK, err);

    err = osMessageQueueGet(msgq_id, (void*)&msg_data, 0, 0);
    EXPECT_EQ(osOK, err);
  }
  err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);
  return;
}

static void test_put_task(void* argp)
{
  ThreadArgType* targp = (ThreadArgType*)argp;
  osMessageQueueId_t msgq_id = (osMessageQueueId_t)targp->msgq_id;
  EXPECT_TRUE(targp->msgq_id != NULL);

  osStatus err = osMessageQueuePut(msgq_id, (void*)&targp->expect_value, 0, targp->timeout);
  EXPECT_EQ(osOK, err);
  return;
}

TEST_F(MessageQueueTest, osMessageQueuePut_11)
{
  ThreadArgType arg;
  osThreadId_t id;
  int msg_data = 10;
  osStatus_t err;
  osMessageQueueId_t msgq_id = osMessageQueueNew(1, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);
  {
    osStatus err = osMessageQueuePut(msgq_id, (void*)&msg_data, 0, osWaitForever);
    EXPECT_EQ(osOK, err);

    arg.msgq_id = msgq_id;
    arg.expect_value = 11;
    arg.timeout = osWaitForever;
    id = osThreadNew(test_put_task, &arg, NULL);

    osDelay(100);

    err = osMessageQueueGet(msgq_id, (void*)&msg_data, 0, osWaitForever);
    EXPECT_EQ(osOK, err);
    EXPECT_EQ(10, msg_data);

    err = osMessageQueueGet(msgq_id, (void*)&msg_data, 0, osWaitForever);
    EXPECT_EQ(osOK, err);
    EXPECT_EQ(11, msg_data);

    err = osMessageQueueGet(msgq_id, (void*)&msg_data, 0, 0);
    EXPECT_EQ(osErrorResource, err);

    osThreadJoin(id);
  }
  err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);
  return;
}

TEST_F(MessageQueueTest, osMessageQueuePut_12)
{
  ThreadArgType arg[2];
  osThreadId_t ids[2];
  int msg_data = 10;
  osStatus_t err;
  osMessageQueueId_t msgq_id = osMessageQueueNew(1, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);
  {
    osStatus err = osMessageQueuePut(msgq_id, (void*)&msg_data, 0, osWaitForever);
    EXPECT_EQ(osOK, err);

    arg[0].msgq_id = msgq_id;
    arg[0].expect_value = 11;
    arg[0].timeout = osWaitForever;
    ids[0] = osThreadNew(test_put_task, &arg[0], NULL);
    osDelay(100);

    arg[1].msgq_id = msgq_id;
    arg[1].expect_value = 12;
    arg[1].timeout = osWaitForever;
    ids[1] = osThreadNew(test_put_task, &arg[1], NULL);
    osDelay(100);

    err = osMessageQueueGet(msgq_id, (void*)&msg_data, 0, 0);
    EXPECT_EQ(osOK, err);
    EXPECT_EQ(10, msg_data);
    osThreadJoin(ids[0]);

    err = osMessageQueueGet(msgq_id, (void*)&msg_data, 0, osWaitForever);
    EXPECT_EQ(osOK, err);
    EXPECT_EQ(11, msg_data);
    osThreadJoin(ids[1]);

    err = osMessageQueueGet(msgq_id, (void*)&msg_data, 0, osWaitForever);
    EXPECT_EQ(osOK, err);
    EXPECT_EQ(12, msg_data);

    err = osMessageQueueGet(msgq_id, (void*)&msg_data, 0, 0);
    EXPECT_EQ(osErrorResource, err);
  }
  err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);
  return;
}

TEST_F(MessageQueueTest, osMessageQueuePut_13)
{
  ThreadArgType arg[3];
  osThreadId_t ids[3];
  int msg_data = 10;
  osStatus_t err;
  osMessageQueueId_t msgq_id = osMessageQueueNew(1, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);
  {
    osStatus err = osMessageQueuePut(msgq_id, (void*)&msg_data, 0, osWaitForever);
    EXPECT_EQ(osOK, err);

    arg[0].msgq_id = msgq_id;
    arg[0].expect_value = 11;
    arg[0].timeout = osWaitForever;
    ids[0] = osThreadNew(test_put_task, &arg[0], NULL);
    osDelay(100);

    arg[1].msgq_id = msgq_id;
    arg[1].expect_value = 12;
    arg[1].timeout = osWaitForever;
    ids[1] = osThreadNew(test_put_task, &arg[1], NULL);
    osDelay(100);

    arg[2].msgq_id = msgq_id;
    arg[2].expect_value = 13;
    arg[2].timeout = osWaitForever;
    ids[2] = osThreadNew(test_put_task, &arg[2], NULL);
    osDelay(100);

    err = osMessageQueueGet(msgq_id, (void*)&msg_data, 0, 0);
    EXPECT_EQ(osOK, err);
    EXPECT_EQ(10, msg_data);
    osThreadJoin(ids[0]);

    err = osMessageQueueGet(msgq_id, (void*)&msg_data, 0, osWaitForever);
    EXPECT_EQ(osOK, err);
    EXPECT_EQ(11, msg_data);
    osThreadJoin(ids[1]);

    err = osMessageQueueGet(msgq_id, (void*)&msg_data, 0, osWaitForever);
    EXPECT_EQ(osOK, err);
    EXPECT_EQ(12, msg_data);
    osThreadJoin(ids[2]);

    err = osMessageQueueGet(msgq_id, (void*)&msg_data, 0, osWaitForever);
    EXPECT_EQ(osOK, err);
    EXPECT_EQ(13, msg_data);

    err = osMessageQueueGet(msgq_id, (void*)&msg_data, 0, 0);
    EXPECT_EQ(osErrorResource, err);
  }
  err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);
  return;
}

TEST_F(MessageQueueTest, osMessageQueuePut_14)
{
  int msg_data = 1024;
  osStatus_t err;
  osMessageQueueId_t msgq_id = osMessageQueueNew(1, sizeof(int), NULL);
  EXPECT_TRUE(msgq_id != NULL);
  err = osMessageQueueDelete(msgq_id);
  EXPECT_EQ(osOK, err);

  err = osMessageQueuePut(msgq_id, (void*)&msg_data, 0, 0);
  EXPECT_EQ(osErrorParameter, err);
  return;
}