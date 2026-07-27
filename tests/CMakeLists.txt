include(FetchContent)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE) # Do not install gtest
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/85087857ad10bd407cd6ed2f52f7ea9752db621f.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable(${UNIT_TEST_TARGET_NAME}
    test_array.cpp
    test_array_list.cpp
    test_ctype.cpp
    test_fixed_array_list.cpp
    test_iterator.cpp
    test_math.cpp
    test_memory.cpp
    test_num_utils.cpp
    test_span.cpp
    test_stdxlib.cpp
    test_stopwatch.cpp
    test_string.cpp
)

set_target_properties(${UNIT_TEST_TARGET_NAME} PROPERTIES
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

target_link_libraries(${UNIT_TEST_TARGET_NAME}
    PRIVATE
        ${CMAKE_PROJECT_NAME}
        GTest::gtest
        GTest::gtest_main
)

enable_compiler_warnings(${UNIT_TEST_TARGET_NAME})
check_enable_sanitizers(${UNIT_TEST_TARGET_NAME})
check_enable_coverage(${UNIT_TEST_TARGET_NAME})

add_test(NAME ${UNIT_TEST_TARGET_NAME} COMMAND ${UNIT_TEST_TARGET_NAME})
