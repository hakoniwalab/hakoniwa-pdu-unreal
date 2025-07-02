set(MY_CMAKE_VERSION 3.5.1)
cmake_minimum_required(VERSION ${MY_CMAKE_VERSION})

project(cmsis-posix-project
    VERSION 1.0.0
    DESCRIPTION "cmsis-posix"
    LANGUAGES C CXX
)
enable_testing()
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON) 

add_compile_definitions(OS_POSIX)
set(CMSIS_INTERFACE_DIR "${PROJECT_SOURCE_DIR}/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2")
set(CMSIS_IMPL_INTERFACE_DIR "${PROJECT_SOURCE_DIR}/src/include")

if (DEFINED CMSIS_POSIX_OPTION_FILEPATH)
	include(${CMSIS_POSIX_OPTION_FILEPATH})
else()
	message(STATUS "CMSIS_POSIX_OPTION_FILEPATH is not defined")
	include(default-cmake-options.cmake)
endif()

message(STATUS "BUILD_TYPE " ${BUILD_TYPE})
message(STATUS "GCOV " ${GCOV})

add_subdirectory(src)

if (test)
    add_subdirectory(test)
endif()

set(INSTALL_CMAKE_DIR ${PROJECT_SOURCE_DIR}/public)

install(
	DIRECTORY ${PROJECT_SOURCE_DIR}/src/include/
	DESTINATION ${INSTALL_CMAKE_DIR}/include
)
install(
	DIRECTORY ${CMSIS_INTERFACE_DIR}/
	DESTINATION ${INSTALL_CMAKE_DIR}/include
)

install(
	TARGETS cmsis 
	DESTINATION     ${INSTALL_CMAKE_DIR} 
	EXPORT cmsis-export
    LIBRARY         DESTINATION lib
    INCLUDES        DESTINATION include
    PUBLIC_HEADER   DESTINATION include
)
install(
	EXPORT cmsis-export 
    FILE    cmsis-config.cmake
	DESTINATION ${INSTALL_CMAKE_DIR}
    EXPORT_LINK_INTERFACE_LIBRARIES
)
