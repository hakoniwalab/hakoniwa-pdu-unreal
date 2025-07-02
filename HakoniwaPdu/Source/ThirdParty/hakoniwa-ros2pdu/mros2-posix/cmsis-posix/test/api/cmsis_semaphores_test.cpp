#include "gtest/gtest.h"
#include "cmsis_os.h"
#include "api_test_utils.h"

class SemaphoresTest : public ::testing::Test
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
  osSemaphoreId_t id;
  bool_t need_aquire;
  uint32_t slp_time;
  uint32_t wait_timeout;
} TestAcquireInfoType;

static void test_acquire_arg_init(TestAcquireInfoType* info, osSemaphoreId_t id)
{
  info->id = id;
  info->need_aquire = true;
  info->slp_time = 100;
  info->wait_timeout = 0;
  return;
}

static void test_acquire_task(void* argp)
{
  osStatus_t err;
  TestAcquireInfoType* info;

  info = (TestAcquireInfoType*)argp;
  if (info->need_aquire) {
    err = osSemaphoreAcquire(info->id, info->wait_timeout);
    EXPECT_EQ(osOK, err);
  }

  if (info->slp_time > 0) {
    (void)osDelay(info->slp_time);
  }

  err = osSemaphoreRelease(info->id);
  EXPECT_EQ(osOK, err);

  return;
}

TEST_F(SemaphoresTest, osSemaphoreNew_01)
{
  osSemaphoreId_t id;
  osStatus_t err;
  {
    //do
    id = osSemaphoreNew(2, 2, NULL);
    EXPECT_TRUE(id != NULL);

    //done
    err = osSemaphoreDelete(id);
    EXPECT_EQ(osOK, err);
  }
}

TEST_F(SemaphoresTest, osSemaphoreAcquire_01)
{
  osSemaphoreId_t id;
  osStatus_t err;

  {
    //pre
    id = osSemaphoreNew(2, 2, NULL);
    EXPECT_TRUE(id != NULL);

    //do
    err = osSemaphoreAcquire(id, 0);
    EXPECT_EQ(osOK, err);

    //done
    err = osSemaphoreRelease(id);
    EXPECT_EQ(osOK, err);
    err = osSemaphoreDelete(id);
    EXPECT_EQ(osOK, err);

  }

  return;
}


TEST_F(SemaphoresTest, osSemaphoreAcquire_02)
{
  osSemaphoreId_t id;
  osStatus_t err;
  TestAcquireInfoType info;
  osThreadId_t thr;

  {
    //pre
    id = osSemaphoreNew(2, 2, NULL);
    EXPECT_TRUE(id != NULL);

    test_acquire_arg_init(&info, id);
    thr = osThreadNew(test_acquire_task, (void*)&info, NULL);

    //do
    err = osSemaphoreAcquire(id, 0);
    EXPECT_EQ(osOK, err);

    osThreadJoin(thr);

    //done
    err = osSemaphoreRelease(id);
    EXPECT_EQ(osOK, err);
    err = osSemaphoreDelete(id);
    EXPECT_EQ(osOK, err);

  }

  return;
}

TEST_F(SemaphoresTest, osSemaphoreAcquire_03)
{
  osSemaphoreId_t id;
  osStatus_t err;
  osThreadId_t thr[2];
  TestAcquireInfoType info[2];

  {
    //pre
    id = osSemaphoreNew(2, 2, NULL);
    EXPECT_TRUE(id != NULL);

    test_acquire_arg_init(&info[0], id);
    info[0].slp_time = 1000;
    info[1] = info[0];

    thr[0] = osThreadNew(test_acquire_task, (void*)&info[0], NULL);
    thr[1] = osThreadNew(test_acquire_task, (void*)&info[1], NULL);

    (void)osDelay(300);

    //do
    err = osSemaphoreAcquire(id, 0);
    EXPECT_EQ(osErrorResource, err);

    osThreadJoin(thr[0]);
    osThreadJoin(thr[1]);

    //done
    err = osSemaphoreDelete(id);
    EXPECT_EQ(osOK, err);

  }

  return;
}

TEST_F(SemaphoresTest, osSemaphoreAcquire_04)
{
  osSemaphoreId_t id;
  osStatus_t err;
  osThreadId_t thr[2];
  TestAcquireInfoType info[2];

  {
    //pre
    id = osSemaphoreNew(2, 2, NULL);
    EXPECT_TRUE(id != NULL);

    test_acquire_arg_init(&info[0], id);
    info[0].slp_time = 1000;
    info[1] = info[0];

    thr[0] = osThreadNew(test_acquire_task, (void*)&info[0], NULL);
    thr[1] = osThreadNew(test_acquire_task, (void*)&info[1], NULL);

    (void)osDelay(300);

    //do
    err = osSemaphoreAcquire(id, 10);
    EXPECT_EQ(osErrorTimeoutResource, err);

    osThreadJoin(thr[0]);
    osThreadJoin(thr[1]);
    //done
    err = osSemaphoreDelete(id);
    EXPECT_EQ(osOK, err);

  }

  return;
}

TEST_F(SemaphoresTest, osSemaphoreAcquire_05)
{
  osSemaphoreId_t id;
  osStatus_t err;
  osThreadId_t thr;
  TestAcquireInfoType info;

  {
    //pre
    id = osSemaphoreNew(1, 1, NULL);
    EXPECT_TRUE(id != NULL);

    err = osSemaphoreAcquire(id, 0);
    EXPECT_EQ(osOK, err);

    //do
    test_acquire_arg_init(&info, id);
    info.slp_time = 0;
    info.wait_timeout = osWaitForever;
    thr = osThreadNew(test_acquire_task, (void*)&info, NULL);

    osDelay(100);

    //done
    err = osSemaphoreRelease(id);
    EXPECT_EQ(osOK, err);

    osThreadJoin(thr);

    err = osSemaphoreDelete(id);
    EXPECT_EQ(osOK, err);
  }

  return;
}

TEST_F(SemaphoresTest, osSemaphoreAcquire_06)
{
  osSemaphoreId_t id;
  osStatus_t err;
  osThreadId_t thr[2];
  TestAcquireInfoType info[2];

  {
    //pre
    id = osSemaphoreNew(1, 1, NULL);
    EXPECT_TRUE(id != NULL);

    test_acquire_arg_init(&info[0], id);
    info[0].slp_time = 0;
    info[0].wait_timeout = osWaitForever;
    info[1] = info[0];

    err = osSemaphoreAcquire(id, 0);
    EXPECT_EQ(osOK, err);

    //do
    thr[0] = osThreadNew(test_acquire_task, (void*)&info[0], NULL);
    thr[1] = osThreadNew(test_acquire_task, (void*)&info[1], NULL);

    (void)osDelay(100);

    //done
    err = osSemaphoreRelease(id);
    EXPECT_EQ(osOK, err);

    osThreadJoin(thr[0]);
    osThreadJoin(thr[1]);

    err = osSemaphoreDelete(id);
    EXPECT_EQ(osOK, err);
  }

  return;
}

TEST_F(SemaphoresTest, osSemaphoreAcquire_07)
{
  osSemaphoreId_t id;
  osStatus_t err;
  osThreadId_t thr[3];
  TestAcquireInfoType info[3];

  {
    //pre
    id = osSemaphoreNew(1, 1, NULL);
    EXPECT_TRUE(id != NULL);

    test_acquire_arg_init(&info[0], id);
    info[0].slp_time = 0;
    info[0].wait_timeout = osWaitForever;
    info[1] = info[0];
    info[2] = info[0];

    err = osSemaphoreAcquire(id, 0);
    EXPECT_EQ(osOK, err);

    //do
    thr[0] = osThreadNew(test_acquire_task, (void*)&info[0], NULL);
    thr[1] = osThreadNew(test_acquire_task, (void*)&info[1], NULL);
    thr[2] = osThreadNew(test_acquire_task, (void*)&info[2], NULL);

    (void)osDelay(100);

    //done
    err = osSemaphoreRelease(id);
    EXPECT_EQ(osOK, err);

    osThreadJoin(thr[0]);
    osThreadJoin(thr[1]);
    osThreadJoin(thr[2]);

    err = osSemaphoreDelete(id);
    EXPECT_EQ(osOK, err);
  }

  return;
}

TEST_F(SemaphoresTest, osSemaphoreAcquire_08)
{
  osSemaphoreId_t id;
  osStatus_t err;

  {
    //pre
    id = osSemaphoreNew(2, 2, NULL);
    EXPECT_TRUE(id != NULL);

    err = osSemaphoreDelete(id);
    EXPECT_EQ(osOK, err);

    //do
    err = osSemaphoreAcquire(id, 0);
    EXPECT_EQ(osErrorParameter, err);

  }

  return;
}

TEST_F(SemaphoresTest, osSemaphoreRelease_01)
{
  osSemaphoreId_t id;
  TestAcquireInfoType info;
  osThreadId_t thr;
  osStatus_t err;

  {
    //pre
    id = osSemaphoreNew(2, 2, NULL);
    EXPECT_TRUE(id != NULL);

    err = osSemaphoreAcquire(id, 0);
    EXPECT_EQ(osOK, err);

    test_acquire_arg_init(&info, id);

    thr = osThreadNew(test_acquire_task, (void*)&info, NULL);
    osDelay(10);

    //do
    err = osSemaphoreRelease(id);
    EXPECT_EQ(osOK, err);

    osThreadJoin(thr);

    //done
    err = osSemaphoreDelete(id);
    EXPECT_EQ(osOK, err);

  }

  return;
}

TEST_F(SemaphoresTest, osSemaphoreRelease_02)
{
  osSemaphoreId_t id;
  osThreadId_t thr;
  TestAcquireInfoType info;
  osStatus_t err;

  {
    //pre
    id = osSemaphoreNew(2, 2, NULL);
    EXPECT_TRUE(id != NULL);

    err = osSemaphoreAcquire(id, 0);
    EXPECT_EQ(osOK, err);

    test_acquire_arg_init(&info, id);
    info.slp_time = 10;

    thr = osThreadNew(test_acquire_task, (void*)&info, NULL);

    osThreadJoin(thr);

    //do
    err = osSemaphoreRelease(id);
    EXPECT_EQ(osOK, err);

    //done
    err = osSemaphoreDelete(id);
    EXPECT_EQ(osOK, err);

  }

  return;
}

TEST_F(SemaphoresTest, osSemaphoreRelease_03)
{
  osSemaphoreId_t id;
  osThreadId_t thr;
  TestAcquireInfoType info;
  osStatus_t err;

  {
    //pre
    id = osSemaphoreNew(2, 2, NULL);
    EXPECT_TRUE(id != NULL);

    err = osSemaphoreAcquire(id, 0);
    EXPECT_EQ(osOK, err);

    test_acquire_arg_init(&info, id);
    info.slp_time = 10;

    thr = osThreadNew(test_acquire_task, (void*)&info, NULL);

    osThreadJoin(thr);

    err = osSemaphoreRelease(id);
    EXPECT_EQ(osOK, err);

    //do
    err = osSemaphoreRelease(id);
    EXPECT_EQ(osErrorResource, err);

    //done
    err = osSemaphoreDelete(id);
    EXPECT_EQ(osOK, err);

  }

  return;
}

TEST_F(SemaphoresTest, osSemaphoreRelease_05)
{
  osSemaphoreId_t id;
  osStatus_t err;

  {
    //pre
    id = osSemaphoreNew(1, 1, NULL);
    EXPECT_TRUE(id != NULL);

    err = osSemaphoreDelete(id);
    EXPECT_EQ(osOK, err);

    //do
    err = osSemaphoreRelease(id);
    EXPECT_EQ(osErrorParameter, err);

  }

  return;
}

TEST_F(SemaphoresTest, osSemaphoreDelete_01)
{
  osSemaphoreId_t id;
  osStatus_t err;

  {
    //pre
    id = osSemaphoreNew(1, 1, NULL);
    EXPECT_TRUE(id != NULL);

    //do
    err = osSemaphoreDelete(id);
    EXPECT_EQ(osOK, err);

    //done

  }

  return;
}

TEST_F(SemaphoresTest, osSemaphoreDelete_02)
{
  osSemaphoreId_t id;
  osStatus_t err;

  {
    //pre
    id = osSemaphoreNew(1, 1, NULL);
    EXPECT_TRUE(id != NULL);
    err = osSemaphoreDelete(id);
    EXPECT_EQ(osOK, err);

    //do
    err = osSemaphoreDelete(id);
    EXPECT_EQ(osErrorParameter, err);

    //done

  }

  return;
}
