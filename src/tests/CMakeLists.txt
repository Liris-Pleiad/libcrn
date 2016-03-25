# tests build script

#############################
# add path to CRN.h
include_directories("${PROJECT_BINARY_DIR}/include/libcrn")
# add path to libcrn
include_directories("${PROJECT_SOURCE_DIR}/src/libcrn")
# add path to tests
include_directories("${PROJECT_SOURCE_DIR}/src/tests")
#############################

# sources
file(
	GLOB_RECURSE
	TESTS_SRC
	*.cpp
)
file(
	GLOB_RECURSE
	TESTS_HEADER
	*.h
	*.hpp
)
if (MSVC)
	GroupSources(src/tests)
	add_definitions("/wd4250")
endif(MSVC)

set(TESTS_LIB
	libcrn
)

add_executable(tests ${TESTS_SRC} ${TESTS_HEADER})
target_link_libraries(tests ${TESTS_LIB})
add_test(all tests)

