#include "gtest/gtest.h"
#include "cmsis_os.h"
#include "api_test_utils.h"

class KernelTest : public ::testing::Test
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
  uint32_t before_time;
  uint32_t after_time;
};

TEST_F(KernelTest, osKernelGetTickCount_01)
{
  uint32_t slp_time;

  before_time = osKernelGetTickCount();
  (void)osDelay(100);
  after_time = osKernelGetTickCount();

  slp_time = after_time - before_time;
  TestAssertInRange(100, 101, slp_time);

}