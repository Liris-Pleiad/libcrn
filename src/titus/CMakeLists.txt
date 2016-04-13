# titus build script

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
	add_executable(titus2 ${TITUS_SRC} ${TITUS_HEADER})
	target_include_directories(titus2 PRIVATE "${PROJECT_BINARY_DIR}/include/libgtkcrnmm2")
	target_include_directories(titus2 PRIVATE "${GTK2_INCLUDE_DIRS}")
	target_link_libraries(titus2 gtkcrnmm2)
	if(CRN_USING_LIBINTL OR CRN_USING_GLIB_INTL)
		include_directories(${Intl_INCLUDE_DIRS})
		target_link_libraries(titus2	${Intl_LIBRARIES})
	endif()
	install(TARGETS titus2 DESTINATION bin COMPONENT app)
endif()

if(GTK3_FOUND)
	add_executable(titus3 ${TITUS_SRC} ${TITUS_HEADER})
	target_include_directories(titus3 PRIVATE "${PROJECT_BINARY_DIR}/include/libgtkcrnmm3")
	target_include_directories(titus3 PRIVATE "${GTK3_INCLUDE_DIRS}")
	target_link_libraries(titus3 gtkcrnmm3)
	if(CRN_USING_LIBINTL OR CRN_USING_GLIB_INTL)
		include_directories(${Intl_INCLUDE_DIRS})
		target_link_libraries(titus3	${Intl_LIBRARIES})
	endif()
	install(TARGETS titus3 DESTINATION bin COMPONENT app)
endif()

install(FILES "titus.png" DESTINATION ${CRN_DATA_PATH} COMPONENT app)

