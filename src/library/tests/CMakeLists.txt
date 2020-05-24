set(BINARY_NAME "library-test")
set(CMAKE_CXX_FLAGS ${TEST_CXX_FLAGS})

add_executable(${BINARY_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp")
add_test(NAME ${BINARY_NAME} COMMAND ${BINARY_NAME})

add_library(shared MODULE test.c)
add_dependencies(${BINARY_NAME} shared)

target_include_directories(${BINARY_NAME} PRIVATE ${LIBRARY_INCLUDE_DIR})

target_link_libraries(${BINARY_NAME} ${STATIC_STD_GCC_FLAGS} -ldl)

install(TARGETS ${BINARY_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/tests")
