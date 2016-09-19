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
set(FINDLIBCRN_DEPS "# libcrn optional dependencies")

target_link_libraries(crn	${ICONV_LIBRARIES})
if(MSVC)
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(ICONV_DIR \$ENV{LIBCRN_DEPS_PATH}/libiconv-1.14)")
endif(MSVC)

if(CRN_USING_GDIPLUS)
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_GDIPLUS on)")
	target_link_libraries(crn	${GDIPLUS_LIBRARIES})
endif()

# Gtk
if(CRN_USING_GLIB_INTL)
	if(GTK2_FOUND)
		target_link_libraries(crn GTK2::glib)
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_GTK2 on)")
		if(MSVC)
			set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(Intl_INCLUDE_DIR \$ENV{LIBCRN_DEPS_PATH}/gtkmm2/include)")
			set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(Intl_LIBRARY \$ENV{LIBCRN_DEPS_PATH}/gtkmm2/lib/intl.lib)")
			set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(GTK2_HINTS \$ENV{LIBCRN_DEPS_PATH}/gtkmm2)")
		endif(MSVC)
	else()
		target_link_libraries(crn GTK3::glib)
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_GTK3 on)")
		if(MSVC)
			set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(Intl_INCLUDE_DIR \$ENV{LIBCRN_DEPS_PATH}/gtkmm3_release/include)")
			set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(Intl_LIBRARY \$ENV{LIBCRN_DEPS_PATH}/gtkmm3_release/lib/intl.dll.lib)")
			set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(GTK3_HINTS \$ENV{LIBCRN_DEPS_PATH}/gtkmm3_release)")
		endif(MSVC)
	endif()
	include_directories(${Intl_INCLUDE_DIRS})
	target_link_libraries(crn	${Intl_LIBRARIES})
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_INTL on)")
endif() 
if(CRN_USING_GDKPB)
	if(GTK2_FOUND)
		target_link_libraries(crn GTK2::gdk_pixbuf)
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_GTK2 on)")
		if(MSVC)
			set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(GTK2_HINTS \$ENV{LIBCRN_DEPS_PATH}/gtkmm2)")
		endif(MSVC)
	else()
		target_link_libraries(crn GTK3::gdk_pixbuf)
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_GTK3 on)")
		if(MSVC)
			set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(GTK3_HINTS \$ENV{LIBCRN_DEPS_PATH}/gtkmm3_release)")
		endif(MSVC)
	endif()
	#if(CRN_USING_GLIB_INTL)
	#include_directories(${Intl_INCLUDE_DIRS})
	#target_link_libraries(crn	${Intl_LIBRARIES})
	#set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_INTL on)")
	#endif()
endif()

# libintl
if(CRN_USING_LIBINTL)
	include_directories(${Intl_INCLUDE_DIRS})
	target_link_libraries(crn	${Intl_LIBRARIES})
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_INTL on)")
	if(MSVC)
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(Intl_INCLUDE_DIR \$ENV{LIBCRN_DEPS_PATH}/gtkmm2/include)")
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(Intl_LIBRARY \$ENV{LIBCRN_DEPS_PATH}/gtkmm2/lib/intl.lib)")
	endif(MSVC)
endif()

# libpng
if(CRN_USING_LIBPNG)
	include_directories(${PNG_INCLUDE_DIRS})
	target_link_libraries(crn	${PNG_LIBRARIES})
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_PNG on)")
	if(MSVC)
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(PNG_PNG_INCLUDE_DIR \$ENV{LIBCRN_DEPS_PATH}/3rdparty/libpng)")
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(PNG_LIBRARY \$ENV{LIBCRN_DEPS_PATH}/3rdparty/libpng/lib/Release/libpng.lib)")
	endif(MSVC)
endif()

# jpeglib
if(CRN_USING_LIBJPEG)
	include_directories(${JPEG_INCLUDE_DIR})
	target_link_libraries(crn	${JPEG_LIBRARIES})
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_JPEG on)")
	if(MSVC)
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(JPEG_INCLUDE_DIR \$ENV{LIBCRN_DEPS_PATH}/3rdparty/libjpeg)")
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(JPEG_LIBRARY \$ENV{LIBCRN_DEPS_PATH}/3rdparty/libjpeg/lib/Release/jpeg.lib)")
	endif(MSVC)
endif()

# zip
if(CRN_USING_LIBZIP)
	include_directories(${ZIP_INCLUDE_DIR})
	if (APPLE) # VERY BAD !!!
		include_directories(${ZIP_INCLUDE_DIR}/../lib/libzip/include)
	endif (APPLE)
	target_link_libraries(crn	${ZIP_LIBRARIES})
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_ZIP on)")
	if(MSVC)
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(ZIP_INCLUDE_DIR \$ENV{LIBCRN_DEPS_PATH}/libzip-1.1.2/include)")
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(ZIP_LIBRARY \$ENV{LIBCRN_DEPS_PATH}/libzip-1.1.2/lib/Release/zip.lib)")
	endif(MSVC)
endif(CRN_USING_LIBZIP)

# haru
if(CRN_USING_HARU)
	include_directories(${HPDF_INCLUDE_DIR})
	target_link_libraries(crn	${HPDF_LIBRARIES})
	set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(FINDLIBCRN_HPDF on)")
	if(MSVC)
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(HPDF_INCLUDE_DIR \$ENV{LIBCRN_DEPS_PATH}/libharu-2.3.0/include)")
		set(FINDLIBCRN_DEPS "${FINDLIBCRN_DEPS}\nset(HPDF_LIBRARY \$ENV{LIBCRN_DEPS_PATH}/libharu-2.3.0/lib/Release/libhpdf.lib)")
	endif(MSVC)
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
install(TARGETS crn RUNTIME DESTINATION "${RUNTIME_INSTALL_PATH}" COMPONENT app LIBRARY DESTINATION "${LIBRARY_INSTALL_PATH}" COMPONENT bin ARCHIVE DESTINATION "${ARCHIVE_INSTALL_PATH}" COMPONENT bin)

if (NOT MSVC) # error when install destination is absolute...
	install(DIRECTORY crn DESTINATION ${CRN_PROJECT_BASE}
		DIRECTORY_PERMISSIONS 
			OWNER_WRITE OWNER_READ OWNER_EXECUTE 
			GROUP_WRITE GROUP_READ GROUP_EXECUTE 
			WORLD_WRITE WORLD_READ WORLD_EXECUTE
		 COMPONENT bin)
endif()

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

# Dependencies
if(MSVC)
	install(FILES "$ENV{LIBCRN_DEPS_PATH}/libiconv-1.14/lib/libiconv.dll" DESTINATION deps COMPONENT bin)
	set(INSTALL_GTK2 false)
	set(INSTALL_GTK3 false)
	set(INSTALL_INTL false)
	set(INSTALL_ZLIB false)
	set(INSTALL_LIBPNG false)
	if(CRN_USING_GLIB_INTL)
		if(GTK2_FOUND)
			set(INSTALL_GTK2 true)
		else()
			set(INSTALL_GTK3 true)
		endif()
		set(INSTALL_INTL true)
	endif()
	if(CRN_USING_GDKPB)
		if(GTK2_FOUND)
			set(INSTALL_GTK2 true)
		else()
			set(INSTALL_GTK3 true)
		endif()
	endif()
	if(CRN_USING_LIBPNG)
		set(INSTALL_ZLIB true)
		set(INSTALL_LIBPNG true)
	endif()
	if(CRN_USING_LIBJPEG)
		# TODO (right now the lib is static)
	endif()
	if(CRN_USING_LIBZIP)
		set(INSTALL_ZLIB true)
		install(FILES "$ENV{LIBCRN_DEPS_PATH}/libzip-1.1.2/lib/Release/zip.dll" DESTINATION deps COMPONENT bin)
	endif()
	if(CRN_USING_HARU)
		set(INSTALL_ZLIB true)
		set(INSTALL_LIBPNG true)
		install(FILES "$ENV{LIBCRN_DEPS_PATH}/libharu-2.3.0/lib/Release/libhpdf.dll" DESTINATION deps COMPONENT bin)
	endif()

	if(INSTALL_ZLIB)
		install(FILES "$ENV{LIBCRN_DEPS_PATH}/3rdparty/zlib/lib/Release/zlib.dll" DESTINATION deps COMPONENT bin)
	endif()
	if(INSTALL_LIBPNG)
		install(FILES "$ENV{LIBCRN_DEPS_PATH}/3rdparty/libpng/lib/Release/libpng.dll" DESTINATION deps COMPONENT bin)
	endif()
	if(INSTALL_GTK2)
		file(GLOB GTK2DLL "$ENV{LIBCRN_DEPS_PATH}/gtkmm2/redist/*.dll")
		install(FILES ${GTK2DLL} DESTINATION deps COMPONENT bin)
		install(DIRECTORY "$ENV{LIBCRN_DEPS_PATH}/gtkmm2/redist/etc" DESTINATION deps COMPONENT bin)
		install(DIRECTORY "$ENV{LIBCRN_DEPS_PATH}/gtkmm2/redist/lib" DESTINATION deps COMPONENT bin)
		install(DIRECTORY "$ENV{LIBCRN_DEPS_PATH}/gtkmm2/redist/share" DESTINATION ${CRN_DATA_PATH} COMPONENT app)
	endif()
	if(INSTALL_GTK3)
		file(GLOB GTK3DLL "$ENV{LIBCRN_DEPS_PATH}/gtkmm3_release/bin/*.dll")
		install(FILES ${GTK3DLL} DESTINATION deps COMPONENT bin)
		install(DIRECTORY "$ENV{LIBCRN_DEPS_PATH}/gtkmm3_release/etc" DESTINATION deps COMPONENT bin)
		install(DIRECTORY "$ENV{LIBCRN_DEPS_PATH}/gtkmm3_release/share" DESTINATION ${CRN_DATA_PATH} COMPONENT app)
	endif()
	if(INSTALL_INTL)
		install(FILES ${Intl_LIBRARIES} DESTINATION "${RUNTIME_INSTALL_PATH}" COMPONENT bin)
	endif()
endif()

# Create Android package
#if(ANDROID)
#include("cmake/android/Apk.cmake" REQUIRED)
#android_create_apk(crn "${CMAKE_BINARY_DIR}/apk" "${APP_SHARED_LIBRARIES}" "" "Data")
#endif(ANDROID)

