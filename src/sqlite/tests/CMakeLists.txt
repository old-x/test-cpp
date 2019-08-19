set(BINARY_NAME "sqlite-test")
set(CMAKE_CXX_FLAGS ${TEST_CXX_FLAGS})

add_executable(${BINARY_NAME} $<TARGET_OBJECTS:libsqlite> "${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp")
add_test(NAME ${BINARY_NAME} COMMAND ${BINARY_NAME})

target_include_directories(${BINARY_NAME} PRIVATE ${SQLITE_INCLUDE_DIR})

target_link_libraries(${BINARY_NAME} sqlite ${STATIC_STD_GCC_FLAGS} dl pthread stdc++fs)

install(TARGETS ${BINARY_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/tests")
