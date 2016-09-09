# nimrod build script

#############################
# add path to CRN.h
include_directories("${PROJECT_BINARY_DIR}/include/libcrn")
# add path to libcrn
include_directories("${PROJECT_SOURCE_DIR}/src/libcrn")
# add path to libgtkcrnmm
include_directories("${PROJECT_SOURCE_DIR}/src/libgtkcrnmm")
#############################

# sources
file(
	GLOB_RECURSE
	NIMROD_SRC
	*.cpp
)
file(
	GLOB_RECURSE
	NIMROD_HEADER
	*.h
	*.hpp
)
if (MSVC)
	GroupSources(src/nimrod)
	add_definitions("/wd4250")
endif(MSVC)

add_executable(nimrod ${NIMROD_SRC} ${NIMROD_HEADER})
target_include_directories(nimrod PRIVATE "${PROJECT_BINARY_DIR}/include/libgtkcrnmm2")
target_include_directories(nimrod PRIVATE "${GTK2_INCLUDE_DIRS}")
target_link_libraries(nimrod gtkcrnmm2)
if(CRN_USING_LIBINTL OR CRN_USING_GLIB_INTL)
	include_directories(${Intl_INCLUDE_DIRS})
	target_link_libraries(nimrod	${Intl_LIBRARIES})
endif()
install(TARGETS nimrod DESTINATION bin COMPONENT app)

#install(FILES "nimrod.png" DESTINATION ${CRN_DATA_PATH} COMPONENT app)

