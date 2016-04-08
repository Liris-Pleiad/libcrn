# libgtkcrnmm build script

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

add_definitions("-DBUILD_LIBGTKCRNMM")

set(LIBGTKCRNMM_INCLUDE_INSTALL "include/libgtkcrnmm-${CRN_PACKAGE_VERSION_MAJOR}.${CRN_PACKAGE_VERSION_MINOR}")

# Gtkmm2
if(GTK2_FOUND)
	# sources
	configure_file(libgtkcrnmm_config2.h.in "${PROJECT_BINARY_DIR}/include/libgtkcrnmm2/libgtkcrnmm_config.h")
	# Build
	add_library(gtkcrnmm2 ${LIBGTKCRNMM_SRC} ${LIBGTKCRNMM_HEADER})
	target_include_directories(gtkcrnmm2 PRIVATE "${PROJECT_BINARY_DIR}/include/libgtkcrnmm2")

	if(MSVC)
		add_dependencies(gtkcrnmm2 gendef)
		add_custom_command(TARGET gtkcrnmm2 PRE_LINK COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/gendef
			${CMAKE_CURRENT_BINARY_DIR}/gtkcrnmm2.def 
			$<TARGET_FILE_NAME:gtkcrnmm2> 
			${CMAKE_CURRENT_BINARY_DIR}/gtkcrnmm2.dir/${CMAKE_CFG_INTDIR}/*.obj)
		set_target_properties(gtkcrnmm2 PROPERTIES LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_BINARY_DIR}/gtkcrnmm2.def\"")
	endif()

	# Dependencies
	target_link_libraries(gtkcrnmm2 crn)
	target_link_libraries(gtkcrnmm2 GTK2::gtkmm)

	# Install
	# - binaries
	install(TARGETS gtkcrnmm2 RUNTIME DESTINATION "${RUNTIME_INSTALL_PATH}" COMPONENT bin  LIBRARY DESTINATION "${LIBRARY_INSTALL_PATH}" COMPONENT bin ARCHIVE DESTINATION "${ARCHIVE_INSTALL_PATH}" COMPONENT bin)
	# - includes
	set(LIBGTKCRNMM2_INCLUDE_INSTALL "include/libgtkcrnmm2-${CRN_PACKAGE_VERSION_MAJOR}.${CRN_PACKAGE_VERSION_MINOR}")
	install(FILES "${PROJECT_BINARY_DIR}/include/libgtkcrnmm2/libgtkcrnmm_config.h" DESTINATION "${LIBGTKCRNMM2_INCLUDE_INSTALL}" COMPONENT dev)
	# - CMake files
	configure_file(FindGTKCRNMM2.cmake.in ${PROJECT_BINARY_DIR}/share/FindGTKCRNMM2.cmake @ONLY)
	install(FILES "${PROJECT_BINARY_DIR}/share/FindGTKCRNMM2.cmake" DESTINATION "share/cmake" COMPONENT dev) # XXX TODO find where to install this!
endif()

# Gtkmm3
if(GTK3_FOUND)
	#set(CRN_USING_GTKMM3 on)
	# sources
	configure_file(libgtkcrnmm_config3.h.in ${PROJECT_BINARY_DIR}/include/libgtkcrnmm3/libgtkcrnmm_config.h)

	# Build
	add_library(gtkcrnmm3 ${LIBGTKCRNMM_SRC} ${LIBGTKCRNMM_HEADER})
	target_include_directories(gtkcrnmm3 PRIVATE "${PROJECT_BINARY_DIR}/include/libgtkcrnmm3")

	if(MSVC)
		add_dependencies(gtkcrnmm3 gendef)
		add_custom_command(TARGET gtkcrnmm3 PRE_LINK COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/gendef
			${CMAKE_CURRENT_BINARY_DIR}/gtkcrnmm3.def 
			$<TARGET_FILE_NAME:gtkcrnmm3> 
			${CMAKE_CURRENT_BINARY_DIR}/gtkcrnmm3.dir/${CMAKE_CFG_INTDIR}/*.obj)
		set_target_properties(gtkcrnmm3 PROPERTIES LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_BINARY_DIR}/gtkcrnmm3.def\"")
	endif()
	# Dependencies
	target_link_libraries(gtkcrnmm3 GTK3::gtkmm)
	target_link_libraries(gtkcrnmm3 crn)
	# Install
	# - binaries
	install(TARGETS gtkcrnmm3 RUNTIME DESTINATION "${RUNTIME_INSTALL_PATH}" COMPONENT bin  LIBRARY DESTINATION "${LIBRARY_INSTALL_PATH}" COMPONENT bin ARCHIVE DESTINATION "${ARCHIVE_INSTALL_PATH}" COMPONENT bin)
	# - includes
	set(LIBGTKCRNMM3_INCLUDE_INSTALL "include/libgtkcrnmm3-${CRN_PACKAGE_VERSION_MAJOR}.${CRN_PACKAGE_VERSION_MINOR}")
	install(FILES "${PROJECT_BINARY_DIR}/include/libgtkcrnmm3/libgtkcrnmm_config.h" DESTINATION "${LIBGTKCRNMM3_INCLUDE_INSTALL}" COMPONENT dev)
	# - CMake files
	configure_file(FindGTKCRNMM3.cmake.in ${PROJECT_BINARY_DIR}/share/FindGTKCRNMM3.cmake @ONLY)
	install(FILES "${PROJECT_BINARY_DIR}/share/FindGTKCRNMM3.cmake" DESTINATION "share/cmake" COMPONENT dev) # XXX TODO find where to install this!
endif()

# Install
# - data
file(GLOB LIBGTKCRNMM_DATA data/*.png)
install(FILES ${LIBGTKCRNMM_DATA} DESTINATION "${CRN_DATA_PATH}" COMPONENT bin)
# - includes
install(DIRECTORY . DESTINATION ${LIBGTKCRNMM_INCLUDE_INSTALL} COMPONENT dev FILES_MATCHING PATTERN "*.h" PATTERN "po" EXCLUDE PATTERN "data" EXCLUDE)

