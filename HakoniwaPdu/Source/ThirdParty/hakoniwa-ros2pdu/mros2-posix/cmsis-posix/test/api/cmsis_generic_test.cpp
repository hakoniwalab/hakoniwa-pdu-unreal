#include "gtest/gtest.h"
#include "cmsis_os.h"
#include "api_test_utils.h"

class GenericTest : public ::testing::Test
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

TEST_F(GenericTest, osDelay_01)
{
  uint32_t slp_time;
  osStatus_t ret;

  before_time = osKernelGetTickCount();
  ret = osDelay(300);
  after_time = osKernelGetTickCount();

  slp_time = after_time - before_time;
  TestAssertInRange(300, 301, slp_time);
  EXPECT_EQ(osOK, ret);

}