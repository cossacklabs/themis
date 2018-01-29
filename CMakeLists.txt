project(themis)
cmake_minimum_required(VERSION 3.8)
include_directories(src)


file(GLOB SOTER_SOURCE_FILES src/soter/*.c src/soter/openssl/*.c src/soter/ed25519/*)
add_library(soter ${SOTER_SOURCE_FILES})
add_library(soter_shared SHARED ${SOTER_SOURCE_FILES})
set_target_properties(soter_shared PROPERTIES OUTPUT_NAME soter)
target_compile_definitions(soter PRIVATE "DNIST_STS_EXE_PATH=tests/soter/nist-sts")
target_compile_options(soter PRIVATE -std=gnu99)

target_compile_definitions(soter_shared PRIVATE "DNIST_STS_EXE_PATH=tests/soter/nist-sts")
target_compile_options(soter_shared PRIVATE -std=gnu99)

file(GLOB_RECURSE THEMIS_SOURCE_FILES src/themis/*.c)
add_library(themis ${THEMIS_SOURCE_FILES})
add_library(themis_shared SHARED ${THEMIS_SOURCE_FILES})
set_target_properties(themis_shared PROPERTIES OUTPUT_NAME themis)

file(GLOB SOTER_TEST_SOURCE tests/soter/*.c tests/common/*.c )
file(GLOB THEMIS_TEST_SOURCE tests/themis/*.c tests/common/*.c)

add_executable(soter_test ${SOTER_TEST_SOURCE} )
target_link_libraries(soter_test soter crypto)

add_executable(themis_test ${THEMIS_TEST_SOURCE} ${SOTER_SOURCE_FILES})
target_include_directories(themis_test PRIVATE tests)
target_link_libraries(themis_test soter crypto themis)

file(GLOB THEMISPP_SOURCE src/wrappers/themis/themispp/*.hpp)
add_library(themispp ${THEMISPP_SOURCE})
target_link_libraries(themispp soter themis)
set_target_properties(themispp PROPERTIES LINKER_LANGUAGE CXX)

file(GLOB THEMISPP_TEST_SOURCE tests/themispp/*.hpp tests/themispp/*.cpp tests/common/*)
add_executable(themispp_test ${THEMISPP_TEST_SOURCE})
target_include_directories(themispp_test PUBLIC tests src/wrappers/themis)
target_link_libraries(themispp_test themispp themis soter crypto)
set_target_properties(themispp_test PROPERTIES LINKER_LANGUAGE CXX)