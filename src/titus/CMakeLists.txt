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

if(GTK2_FOUND)
	include_directories("${PROJECT_BINARY_DIR}/include/libgtkcrnmm2")
	
	set(TITUS2_LIB
		gtkcrnmm2
	)

	add_executable(titus2 ${TITUS_SRC} ${TITUS_HEADER})
	target_link_libraries(titus2 ${TITUS2_LIB})

	install(TARGETS titus2 DESTINATION bin COMPONENT app)
endif()

if(GTK3_FOUND)
	include_directories("${PROJECT_BINARY_DIR}/include/libgtkcrnmm3")

	set(TITUS3_LIB
		gtkcrnmm3
	)

	add_executable(titus3 ${TITUS_SRC} ${TITUS_HEADER})
	target_link_libraries(titus3 ${TITUS3_LIB})

	install(TARGETS titus3 DESTINATION bin COMPONENT app)
endif()

install(FILES "titus.png" DESTINATION ${CRN_DATA_PATH} COMPONENT app)
