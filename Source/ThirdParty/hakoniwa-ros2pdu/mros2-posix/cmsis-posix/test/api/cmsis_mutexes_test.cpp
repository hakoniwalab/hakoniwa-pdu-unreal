#include "gtest/gtest.h"
#include "cmsis_os.h"
#include "api_test_utils.h"

class MutexesTest : public ::testing::Test
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
  osMutexId_t id;
  uint32_t slp_time;
  uint32_t wait_timeout;
} TestAcquireInfoType;

static void test_acquire_arg_init(TestAcquireInfoType* info, osMutexId_t id)
{
  info->id = id;
  info->slp_time = 100;
  info->wait_timeout = 0;
  return;
}

static void test_acquire_task(void* argp)
{
  osStatus_t err;
  TestAcquireInfoType* info;

  info = (TestAcquireInfoType*)argp;
  err = osMutexAcquire(info->id, info->wait_timeout);
  EXPECT_EQ(osOK, err);

  if (info->slp_time > 0) {
    (void)osDelay(info->slp_time);
  }

  err = osMutexRelease(info->id);
  EXPECT_EQ(osOK, err);

  return;
}

TEST_F(MutexesTest, osMutexNew_01)
{
  osMutexId_t id;
  osStatus_t err;

  id = osMutexNew(NULL);
  EXPECT_TRUE(id != NULL);

  err = osMutexDelete(id);
  EXPECT_EQ(osOK, err);
}
TEST_F(MutexesTest, osMutexDelete_01)
{
  osMutexId_t id;
  osStatus_t err;

  //pre
  id = osMutexNew(NULL);
  EXPECT_TRUE(id != NULL);

  //do
  err = osMutexDelete(id);
  EXPECT_EQ(osOK, err);
  return;
}
TEST_F(MutexesTest, osMutexDelete_02)
{
  osMutexId_t id;
  osStatus_t err;
  TestAcquireInfoType info;

  {
    //pre
    id = osMutexNew(NULL);
    EXPECT_TRUE(id != NULL);
    err = osMutexAcquire(id, 0);
    EXPECT_EQ(osOK, err);

    test_acquire_arg_init(&info, id);
    info.wait_timeout = osWaitForever;
    info.slp_time = 0;
    osThreadId_t thread_id = osThreadNew(test_acquire_task, (void*)&info, NULL);

    osDelay(100);

    //do
    err = osMutexDelete(id);
    EXPECT_EQ(osErrorResource, err);

    //done
    err = osMutexRelease(id);
    EXPECT_EQ(osOK, err);

    osThreadJoin(thread_id);

    err = osMutexDelete(id);
    EXPECT_EQ(osOK, err);
  }
}

TEST_F(MutexesTest, osMutexDelete_03)
{
  osMutexId_t id;
  osStatus_t err;

  {
    //pre
    id = osMutexNew(NULL);
    EXPECT_TRUE(id != NULL);
    err = osMutexDelete(id);
    EXPECT_EQ(osOK, err);

    //do
    err = osMutexDelete(id);
    EXPECT_EQ(osErrorParameter, err);

  }
  return;
}

TEST_F(MutexesTest, osMutexAcquire_01)
{
  osMutexId_t id;
  osStatus_t err;

  {
    //pre
    id = osMutexNew(NULL);
    EXPECT_TRUE(id != NULL);

    //do
    err = osMutexAcquire(id, 0);
    EXPECT_EQ(osOK, err);

    //done

    err = osMutexRelease(id);
    EXPECT_EQ(osOK, err);

    err = osMutexDelete(id);
    EXPECT_EQ(osOK, err);
  }

  return;
}

TEST_F(MutexesTest, osMutexAcquire_02)
{
  osMutexId_t id;
  osStatus_t err;
  osThreadId_t thr[2];
  TestAcquireInfoType info[2];

  {
    //pre
    id = osMutexNew(NULL);
    EXPECT_TRUE(id != NULL);

    test_acquire_arg_init(&info[0], id);
    test_acquire_arg_init(&info[1], id);
    info[0].wait_timeout = osWaitForever;
    info[1].wait_timeout = osWaitForever;

    thr[0] = osThreadNew(test_acquire_task, (void*)&info[0], NULL);
    osDelay(10);
    thr[1] = osThreadNew(test_acquire_task, (void*)&info[1], NULL);
    osDelay(10);

    //do
    err = osMutexAcquire(id, 0);
    EXPECT_EQ(osErrorResource, err);

    osThreadJoin(thr[0]);
    osThreadJoin(thr[1]);

    //done
    err = osMutexDelete(id);
    EXPECT_EQ(osOK, err);
  }

  return;
}

TEST_F(MutexesTest, osMutexAcquire_03)
{

  osMutexId_t id;
  osStatus_t err;
  uint32_t start_time, end_time;
  osThreadId_t thr[2];
  TestAcquireInfoType info[2];

  {
    //pre
    id = osMutexNew(NULL);
    EXPECT_TRUE(id != NULL);

    test_acquire_arg_init(&info[0], id);
    test_acquire_arg_init(&info[1], id);
    info[1].wait_timeout = osWaitForever;

    thr[0] = osThreadNew(test_acquire_task, (void*)&info[0], NULL);
    osDelay(10);
    thr[1] = osThreadNew(test_acquire_task, (void*)&info[1], NULL);
    osDelay(10);

    //do
    start_time = osKernelGetTickCount();
    err = osMutexAcquire(id, 10);
    end_time = osKernelGetTickCount();
    TestAssertInRange(10, 11, (end_time - start_time));
    EXPECT_EQ(osErrorTimeoutResource, err);

    //done
    osThreadJoin(thr[0]);
    osThreadJoin(thr[1]);

    err = osMutexDelete(id);
    EXPECT_EQ(osOK, err);
  }

  return;
}

TEST_F(MutexesTest, osMutexAcquire_04)
{

  osMutexId_t id;
  osStatus_t err;
  osThreadId_t thr[2];
  TestAcquireInfoType info[2];

  {
    //pre
    id = osMutexNew(NULL);
    EXPECT_TRUE(id != NULL);

    test_acquire_arg_init(&info[0], id);
    info[0].slp_time = 0;
    info[0].wait_timeout = osWaitForever;
    test_acquire_arg_init(&info[1], id);
    info[1].slp_time = 0;
    info[1].wait_timeout = osWaitForever;

    err = osMutexAcquire(id, 0);
    EXPECT_EQ(osOK, err);

    thr[0] = osThreadNew(test_acquire_task, (void*)&info[0], NULL);
    osDelay(10);

    thr[1] = osThreadNew(test_acquire_task, (void*)&info[1], NULL);
    osDelay(10);

    osDelay(1000);

    //done
    err = osMutexRelease(id);
    EXPECT_EQ(osOK, err);

    osThreadJoin(thr[0]);
    osThreadJoin(thr[1]);

    err = osMutexDelete(id);
    EXPECT_EQ(osOK, err);
  }

  return;
}

static void test_acquire_task1(void* argp)
{
  TestAcquireInfoType* info;
  osThreadId_t thr;
  osStatus_t err;

  info = (TestAcquireInfoType*)argp;
  thr = osThreadNew(test_acquire_task, (void*)info, NULL);

  err = osMutexAcquire(info->id, info->wait_timeout);
  EXPECT_EQ(osOK, err);

  if (info->slp_time > 0) {
    osDelay(info->slp_time);
  }

  err = osMutexRelease(info->id);
  EXPECT_EQ(osOK, err);

  osThreadJoin(thr);
  return;
}


TEST_F(MutexesTest, osMutexAcquire_05)
{
  osMutexId_t id;
  osStatus_t err;
  TestAcquireInfoType info;
  osThreadId_t thr;

  {
    //pre
    id = osMutexNew(NULL);
    EXPECT_TRUE(id != NULL);

    test_acquire_arg_init(&info, id);
    info.wait_timeout = osWaitForever;

    err = osMutexAcquire(id, 0);
    EXPECT_EQ(osOK, err);
    thr = osThreadNew(test_acquire_task1, (void*)&info, NULL);

    osDelay(300);

    err = osMutexRelease(id);
    EXPECT_EQ(osOK, err);

    osThreadJoin(thr);

    //done
    err = osMutexDelete(id);
    EXPECT_EQ(osOK, err);
  }

  return;
}


TEST_F(MutexesTest, osMutexAcquire_06)
{
  osMutexId_t id;
  osStatus_t err;

  {
    //pre
    id = osMutexNew(NULL);
    EXPECT_TRUE(id != NULL);
    err = osMutexDelete(id);
    EXPECT_EQ(osOK, err);

    //do
    err = osMutexAcquire(id, 0);
    EXPECT_EQ(osErrorParameter, err);

  }

  return;
}

TEST_F(MutexesTest, osMutexRelease_01)
{
  osMutexId_t id;
  osStatus_t err;
  osThreadId_t thr;
  TestAcquireInfoType info;

  {
    //pre
    id = osMutexNew(NULL);
    EXPECT_TRUE(id != NULL);

    err = osMutexAcquire(id, 0);
    EXPECT_EQ(osOK, err);

    test_acquire_arg_init(&info, id);
    info.slp_time = 0;
    info.wait_timeout = osWaitForever;

    thr = osThreadNew(test_acquire_task, (void*)&info, NULL);
    osDelay(100);

    //do
    err = osMutexRelease(id);
    EXPECT_EQ(osOK, err);

    //done
    osThreadJoin(thr);

    err = osMutexDelete(id);
    EXPECT_EQ(osOK, err);
  }

  return;
}

TEST_F(MutexesTest, osMutexRelease_02)
{
  osMutexId_t id;
  osStatus_t err;

  {
    //pre
    id = osMutexNew(NULL);
    EXPECT_TRUE(id != NULL);

    err = osMutexAcquire(id, 0);
    EXPECT_EQ(osOK, err);
    err = osMutexRelease(id);
    EXPECT_EQ(osOK, err);

    //do
    err = osMutexRelease(id);
    EXPECT_EQ(osErrorResource, err);

    //done
    err = osMutexDelete(id);
    EXPECT_EQ(osOK, err);
  }

  return;
}

TEST_F(MutexesTest, osMutexRelease_03)
{
  osMutexId_t id;
  osStatus_t err;

  {
    //pre
    id = osMutexNew(NULL);
    EXPECT_TRUE(id != NULL);

    err = osMutexDelete(id);
    EXPECT_EQ(osOK, err);

    //do
    err = osMutexRelease(id);
    EXPECT_EQ(osErrorParameter, err);

  }

  return;
}
