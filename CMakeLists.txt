project(themis)
cmake_minimum_required(VERSION 3.8)
include_directories(src)
#add_definitions(-DOPENSSL "-DCRYPTO_ENGINE_PATH=openssl")

file(GLOB SOTER_SOURCE_FILES src/soter/*.c src/soter/openssl/*.c src/soter/ed25519/*)
add_library(soter ${SOTER_SOURCE_FILES})
add_library(soter_shared SHARED ${SOTER_SOURCE_FILES})
set_target_properties(soter_shared PROPERTIES OUTPUT_NAME soter)

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
