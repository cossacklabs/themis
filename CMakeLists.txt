project(themis)
cmake_minimum_required(VERSION 3.8)
include_directories(src)
add_definitions(-DOPENSSL "-DCRYPTO_ENGINE_PATH=openssl")
file(GLOB_RECURSE THEMIS_SOURCE_FILES src/themis/*.c)
file(GLOB_RECURSE THEMIS_HEADER_FILES src/themis/*.h)

file(GLOB SOTER_SOURCE_FILES src/soter/*.c src/soter/openssl/*.c)
file(GLOB_RECURSE SOTER_HEADER_FILES src/soter/*.h)
#link_libraries(openssl)
add_library(themis SHARED ${THEMIS_SOURCE_FILES})

add_library(soter SHARED ${SOTER_SOURCE_FILES})
include_directories(/usr/include)
link_directories(/usr/lib)


file(GLOB SOTER_TEST_SOURCE tests/soter/*.c tests/common/*.c tests/common/*.h tests/soter/*.h src/soter/*.c src/soter/openssl/*.c)

add_executable(soter_test ${SOTER_TEST_SOURCE} )
target_link_libraries(soter_test soter crypto)