#include "gtest/gtest.h"
#include "cmsis_os.h"
#include "api_test_utils.h"

class ThreadTest : public ::testing::Test
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
    thread_id = NULL;
  }

  virtual void TearDown()
  {
    osStatus_t ret = osThreadJoin(thread_id);
    EXPECT_EQ(osOK, ret);
  }

  osThreadDef_t thrdef;
  osThreadId thread_id;
};
static char* test_expect_string = NULL;

static void test_thread_task(void* argument)
{
  char* msg = (char*)argument;
  ASSERT_STREQ(test_expect_string, msg);
  osThreadTerminate(NULL);
  return;
}
static bool_t test_thread_task_terminate_check = false;
static void test_thread_task_terminate(void* argument)
{
  char* msg = (char*)argument;
  ASSERT_STREQ(test_expect_string, msg);
  (void)osThreadTerminate(NULL);
  test_thread_task_terminate_check = true;
  return;
}

TEST_F(ThreadTest, osThreadCreate_01)
{
  thrdef.pthread = (os_pthread)test_thread_task;
  test_expect_string = (char*)"osThreadCreate_Test01";

  thread_id = osThreadCreate(&thrdef, (void*)"osThreadCreate_Test01");
  EXPECT_TRUE(thread_id != NULL);
}

TEST_F(ThreadTest, osThreadNew_01)
{
  test_expect_string = (char*)"osThreadNew_Test01";
  thread_id = osThreadNew(test_thread_task, (void*)"osThreadNew_Test01", NULL);
  EXPECT_TRUE(thread_id != NULL);
}
TEST_F(ThreadTest, osThreadNew_02)
{
  osThreadAttr_t attr;
  test_expect_string = (char*)"osThreadNew_Test02";
  attr.priority = osPriorityIdle;
  thread_id = osThreadNew(test_thread_task, (void*)"osThreadNew_Test02", &attr);
  EXPECT_TRUE(thread_id != NULL);
}
TEST_F(ThreadTest, osThreadNew_03)
{
  osThreadAttr_t attr;
  test_expect_string = (char*)"osThreadNew_Test03";
  attr.priority = osPriorityRealtime7;
  thread_id = osThreadNew(test_thread_task, (void*)"osThreadNew_Test03", &attr);
  EXPECT_TRUE(thread_id != NULL);
}

TEST_F(ThreadTest, osThreadTerminate_01)
{
  test_expect_string = (char*)"osThreadTerminate_Test01";

  osThreadAttr_t attr;
  attr.priority = osPriorityNormal;
  thread_id = osThreadNew(test_thread_task_terminate, (void*)"osThreadTerminate_Test01", &attr);
  EXPECT_TRUE(thread_id != NULL);

  EXPECT_EQ(false, test_thread_task_terminate_check);
  return;
}