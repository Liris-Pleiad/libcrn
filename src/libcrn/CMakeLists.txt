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

# Build
add_library(crn ${LIBCRN_SRC} ${LIBCRN_HEADER_CFG} ${LIBCRN_HEADER})

# Dependencies
# Iconv needed
target_link_libraries(crn	${ICONV_LIBRARIES})

set(FINDLIBCRN_DEPS "# libcrn optional dependencies")

if(CRN_USING_GDIPLUS)
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_GDIPLUS on)")
	target_link_libraries(crn	${GDIPLUS_LIBRARIES})
endif()

# Gtk
if(CRN_USING_GLIB_INTL)
	if(GTK2_FOUND)
		target_link_libraries(crn GTK2::glib)
	else()
		target_link_libraries(crn GTK3::glib)
	endif()
	include_directories(${Intl_INCLUDE_DIRS})
	target_link_libraries(crn	${Intl_LIBRARIES})
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_INTL on)")
endif() 
if(CRN_USING_GDKPB)
	if(GTK2_FOUND)
		target_link_libraries(crn GTK2::gdk_pixbuf)
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_GTK2 on)")
	else()
		target_link_libraries(crn GTK3::gdk_pixbuf)
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_GTK3 on)")
	endif()
	if(CRN_USING_GLIB_INTL)
		include_directories(${Intl_INCLUDE_DIRS})
		target_link_libraries(crn	${Intl_LIBRARIES})
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_INTL on)")
	endif()
endif()

# libintl
if(CRN_USING_LIBINTL)
	include_directories(${Intl_INCLUDE_DIRS})
	target_link_libraries(crn	${Intl_LIBRARIES})
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_INTL on)")
endif()

# libpng
if(CRN_USING_LIBPNG)
	include_directories(${PNG_INCLUDE_DIRS})
	target_link_libraries(crn	${PNG_LIBRARIES})
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_PNG on)")
endif()

# jpeglib
if(CRN_USING_LIBJPEG)
	include_directories(${JPEG_INCLUDE_DIR})
	target_link_libraries(crn	${JPEG_LIBRARIES})
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_JPEG on)")
endif()

# zip
if(CRN_USING_LIBZIP)
	include_directories(${ZIP_INCLUDE_DIR})
	if (APPLE) # VERY BAD !!!
		include_directories(${ZIP_INCLUDE_DIR}/../lib/libzip/include)
	endif (APPLE)
	target_link_libraries(crn	${ZIP_LIBRARIES})
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_ZIP on)")
endif(CRN_USING_LIBZIP)

# haru
if(CRN_USING_HARU)
	include_directories(${HPDF_INCLUDE_DIR})
	target_link_libraries(crn	${HPDF_LIBRARIES})
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_HPDF on)")
endif(CRN_USING_HARU)

if(MSVC)
	add_dependencies(crn gendef)
	add_custom_command(TARGET crn PRE_LINK COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/gendef 
		${CMAKE_CURRENT_BINARY_DIR}/crn.def 
		$<TARGET_FILE_NAME:crn> 
		${CMAKE_CURRENT_BINARY_DIR}/crn.dir/${CMAKE_CFG_INTDIR}/*.obj)
# TODO s'assurer que gendef a été créé
	set_target_properties(crn PROPERTIES LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_BINARY_DIR}/crn.def\"")
endif()

# Install
# - binaries
install(TARGETS crn RUNTIME DESTINATION "${RUNTIME_INSTALL_PATH}" COMPONENT bin LIBRARY DESTINATION "${LIBRARY_INSTALL_PATH}" COMPONENT bin ARCHIVE DESTINATION "${ARCHIVE_INSTALL_PATH}" COMPONENT bin)

# - includes
set(LIBCRN_INCLUDE_INSTALL "include/libcrn-${CRN_PACKAGE_VERSION_MAJOR}.${CRN_PACKAGE_VERSION_MINOR}")

install(DIRECTORY . DESTINATION ${LIBCRN_INCLUDE_INSTALL} COMPONENT dev FILES_MATCHING PATTERN "*.h" PATTERN "3rdParty*" EXCLUDE PATTERN "po*" EXCLUDE)
install(DIRECTORY . DESTINATION ${LIBCRN_INCLUDE_INSTALL} COMPONENT dev FILES_MATCHING PATTERN "*.hpp" PATTERN "3rdParty*" EXCLUDE PATTERN "po*" EXCLUDE)
install(FILES "${PROJECT_BINARY_DIR}/include/libcrn/CRN.h" DESTINATION ${LIBCRN_INCLUDE_INSTALL} COMPONENT dev)

# - CMake file
configure_file(FindCRN.cmake.in ${PROJECT_BINARY_DIR}/share/FindCRN.cmake @ONLY)
install(FILES "${PROJECT_BINARY_DIR}/share/FindCRN.cmake" DESTINATION "share/cmake" COMPONENT dev)
install(FILES "${PROJECT_SOURCE_DIR}/cmake/FindIconv.cmake" DESTINATION "share/cmake" COMPONENT dev)
install(FILES "${PROJECT_SOURCE_DIR}/cmake/FindIntl.cmake" DESTINATION "share/cmake" COMPONENT dev)
install(FILES "${PROJECT_SOURCE_DIR}/cmake/FindGTK2.cmake" DESTINATION "share/cmake" COMPONENT dev)
install(FILES "${PROJECT_SOURCE_DIR}/cmake/FindGTK3.cmake" DESTINATION "share/cmake" COMPONENT dev)
install(FILES "${PROJECT_SOURCE_DIR}/cmake/FindZIP.cmake" DESTINATION "share/cmake" COMPONENT dev)
install(FILES "${PROJECT_SOURCE_DIR}/cmake/FindHPDF.cmake" DESTINATION "share/cmake" COMPONENT dev)

# Create Android package
#if(ANDROID)
	#include("cmake/android/Apk.cmake" REQUIRED)
	#android_create_apk(crn "${CMAKE_BINARY_DIR}/apk" "${APP_SHARED_LIBRARIES}" "" "Data")
#endif(ANDROID)

