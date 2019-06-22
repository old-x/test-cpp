set(BINARY_NAME "ioc-test")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -fsanitize=address -fsanitize=leak")

aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/beans/src" SOURCE_FILES)

add_executable(${BINARY_NAME} ${SOURCE_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp")
add_test(NAME ${BINARY_NAME} COMMAND ${BINARY_NAME})

target_include_directories(${BINARY_NAME} PRIVATE ${IOC_INCLUDE_DIR})
target_include_directories(${BINARY_NAME} PRIVATE ${TEST_LIB_DIR})
target_include_directories(${BINARY_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/beans/include")

target_link_libraries(${BINARY_NAME} ${STATIC_STD_GCC_FLAGS})

install(TARGETS ${BINARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})
