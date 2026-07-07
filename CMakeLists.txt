cmake_minimum_required(VERSION 3.24)
project(stdx LANGUAGES CXX)
include(GNUInstallDirs)

# ---- Global Settings ----
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "-d") # postfix all libraries for debug builds

# ---- Options ----
option(ENABLE_UNIT_TESTS "Enable building unit tests" ON)
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" OFF)
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

# ---- Output Directories ----
set(COVERAGE_DIR ${CMAKE_SOURCE_DIR}/coverage)

message(STATUS "----General CMake Variables----")
message(STATUS "CMAKE_PROJECT_NAME              = ${CMAKE_PROJECT_NAME}")
message(STATUS "CMAKE_SOURCE_DIR                = ${CMAKE_SOURCE_DIR}") # This is the root of the source tree (where root CMakeLists.txt is located)
message(STATUS "CMAKE_CURRENT_SOURCE_DIR        = ${CMAKE_CURRENT_SOURCE_DIR}") # This is the directory of the current CMakeLists.txt
message(STATUS "CMAKE_BINARY_DIR                = ${CMAKE_BINARY_DIR}") # This is the root of the build tree (where you run cmake. e.g. build/)

message(STATUS "----Project Install Locations----")
message(STATUS "CMAKE_INSTALL_PREFIX            = ${CMAKE_INSTALL_PREFIX}") # where running cmake --install will install to
message(STATUS "CMAKE_INSTALL_BINDIR            = ${CMAKE_INSTALL_BINDIR}") # subdirectory where all executables should be installed to
message(STATUS "CMAKE_INSTALL_LIBDIR            = ${CMAKE_INSTALL_LIBDIR}") # subdirectory where all library files should be installed to
message(STATUS "CMAKE_INSTALL_INCLUDEDIR        = ${CMAKE_INSTALL_INCLUDEDIR}") # subdirectory where all header files should be installed to

message(STATUS "----Project Build Output Locations----")
message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY  = ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}") # where executables will be output after building
message(STATUS "CMAKE_LIBRARY_OUTPUT_DIRECTORY  = ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}") # where static library files will be output after building
message(STATUS "CMAKE_ARCHIVE_OUTPUT_DIRECTORY  = ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}") # where shared library files will be output after building

message(STATUS "----Architecture----")
message(STATUS "CMAKE_SYSTEM_NAME            = ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_VERSION         = ${CMAKE_SYSTEM_VERSION}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR       = ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_GENERATOR              = ${CMAKE_GENERATOR}")
message(STATUS "CMAKE_C_COMPILER             = ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER           = ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_C_COMPILER_ID          = ${CMAKE_C_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_ID        = ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_C_COMPILER_VERSION     = ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION   = ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_SIZEOF_VOID_P          = ${CMAKE_SIZEOF_VOID_P}")
message(STATUS "CMAKE_BUILD_TYPE             = ${CMAKE_BUILD_TYPE}")

message(STATUS "----Other----")
message(STATUS "COVERAGE_DIR                 = ${COVERAGE_DIR}")

# ---- compile_commands.json symlink ----
# Keeps a stable ${CMAKE_SOURCE_DIR}/compile_commands.json pointing at whichever
# preset was configured most recently, so clangd works regardless of build dir.
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    message(STATUS "Creating symbolic link to ${CMAKE_BINARY_DIR}/compile_commands.json in ${CMAKE_SOURCE_DIR}")
    file(CREATE_LINK
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        "${CMAKE_SOURCE_DIR}/compile_commands.json"
        SYMBOLIC
        RESULT _compile_commands_link_result
    )
    if(NOT _compile_commands_link_result EQUAL 0)
        message(WARNING
            "Failed to create symlink at ${CMAKE_SOURCE_DIR}/compile_commands.json (${_compile_commands_link_result})\n"
            "On Windows, this requires Developer Mode (or admin) and the likely fix is to turn on developer mode in settings.\n")
    endif()
    unset(_compile_commands_link_result)
endif()

# ============================================================
# Compiler Flags / properties functions for Targets
# ============================================================
function(enable_compiler_warnings target)
    if(MSVC)
        # Covers cl.exe and clang-cl (Clang targeting the MSVC ABI)
        target_compile_options(${target} PRIVATE
            $<$<COMPILE_LANGUAGE:C>:/W4>
            $<$<COMPILE_LANGUAGE:CXX>:/W4>
        )
        message(STATUS "Compiler warnings `/W4` enabled for target: ${target}")
    else()
        target_compile_options(${target} PRIVATE
            $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
            $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
        )
        message(STATUS "Compiler warnings `-Wall -Wextra -Wpedantic` enabled for target: ${target}")
    endif()
endfunction()

function(check_enable_clang_tidy target)
    if(ENABLE_CLANG_TIDY)
        find_program(CLANG_TIDY_EXE NAMES clang-tidy)

        if(CLANG_TIDY_EXE)
            message(STATUS "Found clang-tidy found at: ${CLANG_TIDY_EXE}")

            set_target_properties(${target} PROPERTIES
                CXX_CLANG_TIDY "${CLANG_TIDY_EXE};--use-color"
            )
        else()
            message(WARNING "clang-tidy enabled but not found! Ensure clang-tidy is installed!")
        endif()
    endif()
endfunction()

function(check_enable_sanitizers target)
    if(ENABLE_ASAN)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            message(STATUS "AddressSanitizer (Asan) enabled for target: ${target}")
            target_compile_options(${target} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
            target_link_options(${target} PRIVATE -fsanitize=address)
        else()
            message(WARNING "AddressSanitizer (Asan) only supported with GCC/Clang. ASan not enabled for target: ${target}")
        endif()
    endif()
endfunction()

function(check_enable_coverage target)
    if(ENABLE_COVERAGE)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            message(STATUS "Coverage (gcov) enabled for target: ${target}")
            target_compile_options(${target} PRIVATE --coverage)
            target_link_options(${target} PRIVATE --coverage)
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
            message(STATUS "Coverage (llvm-cov) enabled for target: ${target}")
            target_compile_options(${target} PRIVATE -fprofile-instr-generate -fcoverage-mapping -fcoverage-mcdc
            )
            target_link_options(${target} PRIVATE -fprofile-instr-generate)
        else()
            message(WARNING "Coverage only supported with GCC/Clang/AppleClang. Coverage not enabled for target: ${target}")
        endif()
    endif()
endfunction()


# ============================================================
# Library
# ============================================================
add_subdirectory(src)


# ============================================================
# Tests
# ============================================================
set(UNIT_TEST_TARGET_NAME "${CMAKE_PROJECT_NAME}_test-unit")

if(ENABLE_UNIT_TESTS)
    enable_testing()
    add_subdirectory(tests)
else()
    message(STATUS "Building for unit tests disabled")
endif()


# ============================================================
# Custom Targets
# ============================================================
if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        find_program(GCOVR_PATH gcovr REQUIRED)

        add_custom_target(coverage
            COMMAND ${CMAKE_CTEST_COMMAND} --test-dir ${CMAKE_BINARY_DIR} --output-on-failure
            COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_DIR}
            COMMAND ${GCOVR_PATH}
                -r ${CMAKE_SOURCE_DIR}
                --filter ${CMAKE_SOURCE_DIR}/src/
                --filter ${CMAKE_SOURCE_DIR}/include/
                --exclude-throw-branches
                --exclude-unreachable-branches
                --html
                --html-details
                --html-self-contained
                --html-title "${CMAKE_PROJECT_NAME} Coverage Report"
                --html-details=${COVERAGE_DIR}/index.html
                --txt=${COVERAGE_DIR}/coverage.txt
                --print-summary
            COMMENT "Running tests and generating GCC coverage report..."
            USES_TERMINAL
        )
        add_dependencies(coverage ${UNIT_TEST_TARGET_NAME})

    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
        string(REGEX MATCH "^[0-9]+" _CLANG_MAJOR "${CMAKE_CXX_COMPILER_VERSION}")
        find_program(LLVM_PROFDATA_PATH NAMES llvm-profdata llvm-profdata-${_CLANG_MAJOR} REQUIRED)
        find_program(LLVM_COV_PATH NAMES llvm-cov llvm-cov-${_CLANG_MAJOR} REQUIRED)

        set(PROFRAW_FILE ${CMAKE_BINARY_DIR}/default.profraw)
        set(PROFDATA_FILE ${CMAKE_BINARY_DIR}/coverage.profdata)

        set(_LLVM_COV_BASE_ARGS
            $<TARGET_FILE:${UNIT_TEST_TARGET_NAME}>
            -instr-profile=${PROFDATA_FILE}
        )
        set(_LLVM_COV_SOURCES
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_SOURCE_DIR}/include
        )
        set(_LLVM_COV_GENERAL_FLAGS
            -show-instantiation-summary # Show instantiation summary in summary table
            -show-mcdc-summary          # add MC/DC summary in the summary table
        )
        set(_LLVM_COV_VIEWING_FLAGS
            -show-line-counts           # annotate each line with its execution count
            -show-regions               # highlight individual coverage regions within a line
            -show-expansions            # expand macros inline to show their internal coverage
            -show-instantiations        # break out coverage per template instantiation
            -show-branches=count        # show taken/not-taken counts at each branch point
            -show-mcdc                  # highlight MC/DC condition coverage inline
        )

        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_DIR}
            # Run tests — LLVM_PROFILE_FILE directs raw profile data to a known path
            COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=${PROFRAW_FILE}
                ${CMAKE_CTEST_COMMAND} --test-dir ${CMAKE_BINARY_DIR} --output-on-failure
            # Merge .profraw -> .profdata (required before llvm-cov can read it)
            COMMAND ${LLVM_PROFDATA_PATH} merge -sparse ${PROFRAW_FILE} -o ${PROFDATA_FILE}
            # Generate HTML report (browseable annotated source)
            COMMAND ${LLVM_COV_PATH} show
                ${_LLVM_COV_BASE_ARGS}
                -format=html                # write HTML instead of text
                -output-dir=${COVERAGE_DIR} # directory for generated HTML files
                ${_LLVM_COV_GENERAL_FLAGS}
                ${_LLVM_COV_VIEWING_FLAGS}
                ${_LLVM_COV_SOURCES}
            # Generate text report (.txt version of the browseable HTML report)
            COMMAND ${LLVM_COV_PATH} show
                ${_LLVM_COV_BASE_ARGS}
                -format=text
                -output-dir=${COVERAGE_DIR}
                ${_LLVM_COV_GENERAL_FLAGS}
                ${_LLVM_COV_VIEWING_FLAGS}
                ${_LLVM_COV_SOURCES}
            # Generate LCOV report (machine-readable .lcov for upload to Coveralls/Codecov)
            COMMAND ${LLVM_COV_PATH} export
                ${_LLVM_COV_BASE_ARGS}
                -format=lcov
                ${_LLVM_COV_GENERAL_FLAGS}
                ${_LLVM_COV_SOURCES}
                > ${COVERAGE_DIR}/coverage.lcov
            # Generate JSON report (machine-readable .json)
            COMMAND ${LLVM_COV_PATH} export
                ${_LLVM_COV_BASE_ARGS}
                -format=text
                ${_LLVM_COV_GENERAL_FLAGS}
                ${_LLVM_COV_SOURCES}
                > ${COVERAGE_DIR}/coverage.json
            # Print text summary to the console
            COMMAND ${LLVM_COV_PATH} report
                ${_LLVM_COV_BASE_ARGS}
                ${_LLVM_COV_GENERAL_FLAGS}
                -use-color
                ${_LLVM_COV_SOURCES}
            COMMENT "Running unit tests and generating LLVM coverage report..."
            USES_TERMINAL
        )
        add_dependencies(coverage ${UNIT_TEST_TARGET_NAME})

    else()
        message(WARNING "Coverage target requires GCC or Clang/AppleClang. Skipping coverage target.")
    endif()
endif()

find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile # Documentation will be generated to the docs\html directory
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating Doxygen documentation..."
    )
else()
    message(STATUS "Doxygen not found. Doxygen target (--target doxygen) will not be available. Install Doxygen to enable documentation generation.")
endif()
