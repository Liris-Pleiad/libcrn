# titus build script

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
	TITUS_SRC
	*.cpp
)
file(
	GLOB_RECURSE
	TITUS_HEADER
	*.h
	*.hpp
)
if (MSVC)
	GroupSources(src/titus)
	add_definitions("/wd4250")
endif(MSVC)

set(TITUS_LIB
	gtkcrnmm
)

add_executable(titus ${TITUS_SRC} ${TITUS_HEADER})
target_link_libraries(titus ${TITUS_LIB})

install(TARGETS titus DESTINATION bin COMPONENT app)
install(FILES "titus.png" DESTINATION ${CRN_DATA_PATH} COMPONENT app)

