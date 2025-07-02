cmake_minimum_required(VERSION ${MY_CMAKE_VERSION})

project(cmsis-posix-project
	LANGUAGES C CXX
)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-long-long -pedantic")

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(cmsis-test 
	api/main.cpp
	api/cmsis_kernel_test.cpp
	api/cmsis_generic_test.cpp
	api/cmsis_thread_test.cpp
	api/cmsis_message_queue_test.cpp
	api/cmsis_mutexes_test.cpp
	api/cmsis_semaphores_test.cpp
)

target_include_directories(
	cmsis-test 
	PRIVATE ${CMSIS_INTERFACE_DIR}
	PRIVATE ${CMSIS_IMPL_INTERFACE_DIR}
	PRIVATE ${PROJECT_SOURCE_DIR}/api_test
	PRIVATE ${PROJECT_SOURCE_DIR}/fw
	PRIVATE ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(cmsis-test cmsis pthread GTest::GTest)

gtest_add_tests(TARGET cmsis-test)