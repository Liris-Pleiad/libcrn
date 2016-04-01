# libcrn build script

#############################
# add path to CRN.h
include_directories("${PROJECT_BINARY_DIR}/include/libcrn")
# add path to libcrn
include_directories("${PROJECT_SOURCE_DIR}/src/libcrn")
#############################

# sources
configure_file(CRN.h.in ${PROJECT_BINARY_DIR}/include/libcrn/CRN.h)

file(
	GLOB_RECURSE
	LIBCRN_SRC
	*.cpp
)
file(
	GLOB_RECURSE
	LIBCRN_HEADER_CFG
	${PROJECT_BINARY_DIR}/include/libcrn/*.h
)
file(
	GLOB_RECURSE
	LIBCRN_HEADER
	*.h
	*.hpp
)
if (MSVC)
	set(LIBCRN_HEADER ${LIBCRN_HEADER} 3rdParty/_msvc_only_/dirent.hh)
	GroupSources(src/libcrn)
endif(MSVC)

add_definitions("-DBUILD_LIBCRN")

# Dependencies
set(LIBCRN_LIB
	${ICONV_LIBRARIES}
	${GDIPLUS_LIBRARIES}
)

set(FINDLIBCRN_DEPS "# libcrn optional dependencies")

# Gtk
if(CRN_USING_GLIB_INTL OR CRN_USING_GDKPB)
	include_directories(${GTK2_INCLUDE_DIRS})
	list(APPEND LIBCRN_LIB
		${GTK2_LIBRARIES}
	)
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_GTK2 on)")
	if(CRN_USING_GLIB_INTL)
		include_directories(${Intl_INCLUDE_DIRS})
		list(APPEND LIBCRN_LIB
			${Intl_LIBRARIES}
		)
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_INTL on)")
	endif()
endif()

# libintl
if(CRN_USING_LIBINTL)
	include_directories(${Intl_INCLUDE_DIRS})
	list(APPEND LIBCRN_LIB
		${Intl_LIBRARIES}
	)
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_INTL on)")
endif()

# libpng
if(CRN_USING_LIBPNG)
	include_directories(${PNG_INCLUDE_DIRS})
	list(APPEND LIBCRN_LIB
		${PNG_LIBRARIES}
	)
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_PNG on)")
endif()

# jpeglib
if(CRN_USING_LIBJPEG)
	include_directories(${JPEG_INCLUDE_DIRS})
	list(APPEND LIBCRN_LIB
		${JPEG_LIBRARIES}
	)
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_JPEG on)")
endif()

# Build
add_library(crn ${LIBCRN_SRC} ${LIBCRN_HEADER_CFG} ${LIBCRN_HEADER})
target_link_libraries(crn ${LIBCRN_LIB})

# Install
# - binaries
install(TARGETS crn RUNTIME DESTINATION "${RUNTIME_INSTALL_PATH}" COMPONENT bin  LIBRARY DESTINATION "${LIBRARY_INSTALL_PATH}" COMPONENT bin ARCHIVE DESTINATION "${ARCHIVE_INSTALL_PATH}" COMPONENT bin)

# - includes
set(LIBCRN_INCLUDE_INSTALL "include/libcrn-${CRN_PACKAGE_VERSION_MAJOR}.${CRN_PACKAGE_VERSION_MINOR}")

install(DIRECTORY . DESTINATION ${LIBCRN_INCLUDE_INSTALL} COMPONENT dev FILES_MATCHING PATTERN "*.h" PATTERN "3rdParty*" EXCLUDE PATTERN "po*" EXCLUDE)
install(DIRECTORY . DESTINATION ${LIBCRN_INCLUDE_INSTALL} COMPONENT dev FILES_MATCHING PATTERN "*.hpp" PATTERN "3rdParty*" EXCLUDE PATTERN "po*" EXCLUDE)
install(FILES "${PROJECT_BINARY_DIR}/include/libcrn/CRN.h" DESTINATION ${LIBCRN_INCLUDE_INSTALL} COMPONENT dev)

# - CMake file
configure_file(FindCRN.cmake.in ${PROJECT_BINARY_DIR}/share/FindCRN.cmake @ONLY)
install(FILES "${PROJECT_BINARY_DIR}/share/FindCRN.cmake" DESTINATION "share/cmake" COMPONENT dev) # XXX TODO find where to install this!
install(FILES "${PROJECT_SOURCE_DIR}/cmake/FindIconv.cmake" DESTINATION "share/cmake" COMPONENT dev) # XXX TODO find where to install this!
install(FILES "${PROJECT_SOURCE_DIR}/cmake/FindIntl.cmake" DESTINATION "share/cmake" COMPONENT dev) # XXX TODO find where to install this!
install(FILES "${PROJECT_SOURCE_DIR}/cmake/FindGTK2.cmake" DESTINATION "share/cmake" COMPONENT dev) # XXX TODO find where to install this!
install(FILES "${PROJECT_SOURCE_DIR}/cmake/FindGTK3.cmake" DESTINATION "share/cmake" COMPONENT dev) # XXX TODO find where to install this!

# Create Android package
#if(ANDROID)
	#include("cmake/android/Apk.cmake" REQUIRED)
	#android_create_apk(crn "${CMAKE_BINARY_DIR}/apk" "${APP_SHARED_LIBRARIES}" "" "Data")
#endif(ANDROID)

#add_subdirectory(po)

