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

add_definitions("-DBUILD_LIBGTKCRNMM")

# Dependencies
set(LIBGTKCRNMM_LIB
	crn
	${GTK2_LIBRARIES}
)

# Build
add_library(gtkcrnmm ${LIBGTKCRNMM_SRC} ${LIBGTKCRNMM_HEADER})
target_link_libraries(gtkcrnmm ${LIBGTKCRNMM_LIB})

# Install
# - binaries
install(TARGETS gtkcrnmm RUNTIME DESTINATION "${RUNTIME_INSTALL_PATH}" COMPONENT bin  LIBRARY DESTINATION "${LIBRARY_INSTALL_PATH}" COMPONENT bin ARCHIVE DESTINATION "${ARCHIVE_INSTALL_PATH}" COMPONENT bin)
# - data
file(GLOB LIBGTKCRNMM_DATA data/*.png)
install(FILES ${LIBGTKCRNMM_DATA} DESTINATION "${CRN_DATA_PATH}" COMPONENT bin)
# - includes
set(LIBGTKCRNMM_INCLUDE_INSTALL "include/libgtkcrnmm-${CRN_PACKAGE_VERSION_MAJOR}.${CRN_PACKAGE_VERSION_MINOR}")
install(DIRECTORY . DESTINATION ${LIBGTKCRNMM_INCLUDE_INSTALL} COMPONENT dev FILES_MATCHING PATTERN "*.h")
# - CMake files
configure_file(FindGTKCRNMM2.cmake.in ${PROJECT_BINARY_DIR}/share/findGTKCRNMM2.cmake @ONLY)
install(FILES "${PROJECT_BINARY_DIR}/share/findGTKCRNMM2.cmake" DESTINATION "share/cmake" COMPONENT dev) # XXX TODO find where to install this!

