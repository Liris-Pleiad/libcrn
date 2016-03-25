# libgtkcrnmm build script

#############################
# add path to CRN.h
include_directories("${PROJECT_BINARY_DIR}/include/libcrn")
# add path to libcrn
include_directories("${PROJECT_SOURCE_DIR}/src/libcrn")
# add path to libgtkcrnmm
include_directories("${PROJECT_SOURCE_DIR}/src/libgtkcrnmm")
# add path to Gtkmm
include_directories(${GTK2_INCLUDE_DIRS})
#############################

# sources
file(
	GLOB_RECURSE
	LIBGTKCRNMM_SRC
	*.cpp
)
file(
	GLOB_RECURSE
	LIBGTKCRNMM_HEADER
	*.h
	*.hpp
)
if (MSVC)
	GroupSources(src/libgtkcrnmm)
	add_definitions("/wd4250")
endif(MSVC)

set(LIBGTKCRNMM_LIB
	libcrn
	${GTK2_LIBRARIES}
)

add_library(libgtkcrnmm ${LIBGTKCRNMM_SRC} ${LIBGTKCRNMM_HEADER})
target_link_libraries(libgtkcrnmm ${LIBGTKCRNMM_LIB})
