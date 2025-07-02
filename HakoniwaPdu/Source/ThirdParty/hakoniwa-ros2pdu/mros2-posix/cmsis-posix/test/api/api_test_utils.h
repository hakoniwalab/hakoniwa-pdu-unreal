#ifndef _API_TEST_UTILS_H_
#define _API_TEST_UTILS_H_

#include "gtest/gtest.h"
#include <string.h>

#define TestAssertInRange(expect_value_mini, expect_value_max, actual_value)	\
	EXPECT_TRUE( ( ( (expect_value_mini) <= (actual_value)) && ((actual_value) <= (expect_value_max)) ) )

#endif /* _API_TEST_UTILS_H_ */