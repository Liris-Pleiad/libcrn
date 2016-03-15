# libcrn build script
set(LIBCRN_VERSION_API "14")
set(LIBCRN_VERSION_AGE "0")
set(LIBCRN_VERSION_REVISION "0")

add_definitions(-DBUILDING_CRN)
# build CRN.h
configure_file(
	"${PROJECT_SOURCE_DIR}/libcrn/CRN.h.in"
	"${PROJECT_BINARY_DIR}/libcrn/CRN.h"
	)

# add path to CRN.h
include_directories("${PROJECT_BINARY_DIR}/libcrn")
# add path to libcrn
include_directories("${PROJECT_SOURCE_DIR}/libcrn")
# add path to dependencies
if(CRN_USING_GTKMM)
	include_directories(${GTKMM_INCLUDE_DIRS})
	link_directories(${GTKMM_LIBRARY_DIRS})
endif(CRN_USING_GTKMM)
if(CRN_USING_GDKPB)
	include_directories(${GDKPB_INCLUDE_DIRS})
	link_directories(${GDKPB_LIBRARY_DIRS})
endif(CRN_USING_GDKPB)
if(CRN_USING_LIBPNG)
	include_directories(${PNG_INCLUDE_DIRS})
	link_directories(${PNG_LIBRARY_DIRS})
endif(CRN_USING_LIBPNG)
if (CRN_USING_LIBZIP)
	include_directories(${ZIP_INCLUDE_DIRS})
	link_directories(${ZIP_LIBRARY_DIRS})
endif (CRN_USING_LIBZIP)
if (CRN_USING_HARU)
	include_directories(${HARU_INCLUDE_DIRS})
	link_directories(${HARU_LIBRARY_DIRS})
endif (CRN_USING_HARU)

# Checks
INCLUDE(CheckFunctionExists)
INCLUDE(CheckSymbolExists)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckTypeSize)

# set sources
file(GLOB_RECURSE LIBCRN_HEADERS *.h *.hpp)
list(APPEND LIBCRN_HEADERS "${PROJECT_BINARY_DIR}/libcrn/CRN.h")
source_group("Headers" FILES ${LIBCRN_HEADERS})
file(GLOB_RECURSE LIBCRN_SOURCES *.cpp)
source_group("Sources" FILES ${LIBCRN_SOURCES})
file(GLOB_RECURSE LIBCRN_MISC *.in *.txt *.cmake)
source_group("Misc" FILES ${LIBCRN_MISC})

# create libcrn
add_library(crn ${library_link_mode} ${LIBCRN_SOURCES} ${LIBCRN_HEADERS})
set_target_properties(crn PROPERTIES
	COMPILE_FLAGS "${GTKMM_CFLAGS_OTHER}"
	LINK_FLAGS "${GTKMM_LDFLAGS_OTHER}"
	VERSION "${LIBCRN_VERSION_API}.${LIBCRN_VERSION_REVISION}.${LIBCRN_VERSION_AGE}"
	SOVERSION ${LIBCRN_VERSION_API}
	LT_VERSION_CURRENT ${LIBCRN_VERSION_API}
	LT_VERSION_AGE ${LIBCRN_VERSION_AGE}
	LT_VERSION_REVISION ${LIBCRN_VERSION_REVISION}
	LT_DEPENDENCY_LIBS "${LT_LDFLAGS}"
	OUTPUT_NAME "crn"
	)
# add dependencies
if (ANDROID)
	target_link_libraries(crn z)
	target_link_libraries(crn iconv)
endif (ANDROID)
if (APPLE)
	target_link_libraries(crn iconv)
endif (APPLE)
if (WIN32)
	target_link_libraries(crn iconv)
endif (WIN32)
target_link_libraries(crn stdc++)
if(CRN_USING_GTKMM)
	target_link_libraries(crn ${GTKMM_LIBRARIES})
else(CRN_USING_GTKMM)
	if(CRN_USING_GDKPB)
		target_link_libraries(crn ${GDKPB_LIBRARIES})
	endif(CRN_USING_GDKPB)
endif(CRN_USING_GTKMM)
if (CRN_USING_LIBZIP)
	target_link_libraries(crn ${ZIP_LIBRARIES})
endif (CRN_USING_LIBZIP)
if (CRN_USING_HARU)
	target_link_libraries(crn ${HARU_LIBRARIES})
endif (CRN_USING_HARU)

if(CRN_USING_LIBPNG)
	target_link_libraries(crn ${PNG_LIBRARIES})
endif(CRN_USING_LIBPNG)
if(CRN_USING_LIBJPEG)
	target_link_libraries(crn jpeg)
endif(CRN_USING_LIBJPEG)

# installation
# object
install(TARGETS crn RUNTIME DESTINATION "${RUNTIME_INSTALL_PATH}" COMPONENT bin  LIBRARY DESTINATION "${LIBRARY_INSTALL_PATH}" COMPONENT bin ARCHIVE DESTINATION "${ARCHIVE_INSTALL_PATH}" COMPONENT bin)
# project directory (not working)
#set(createproj "install(FILES .dummy DESTINATION \"${CRN_PROJECT_FULL_PATH}/documents\" PERMISSIONS WORLD_READ WORLD_WRITE)")
#install(CODE "${createproj}" COMPONENT bin)
# includes
set(LIBCRN_INCLUDE_INSTALL "include/libcrn-${CRN_PACKAGE_VERSION_MAJOR}.${CRN_PACKAGE_VERSION_MINOR}")
install(DIRECTORY . DESTINATION ${LIBCRN_INCLUDE_INSTALL} COMPONENT dev FILES_MATCHING PATTERN "*.h" PATTERN "props*" EXCLUDE PATTERN "3rdParty*" EXCLUDE REGEX ".*svn.*" EXCLUDE)
install(DIRECTORY . DESTINATION ${LIBCRN_INCLUDE_INSTALL} COMPONENT dev FILES_MATCHING PATTERN "*.hpp" PATTERN "props*" EXCLUDE PATTERN "3rdParty*" EXCLUDE REGEX ".*svn.*" EXCLUDE)
install(FILES "${PROJECT_BINARY_DIR}/libcrn/CRN.h" DESTINATION ${LIBCRN_INCLUDE_INSTALL} COMPONENT dev)
# devel libs
if(CMAKE_COMPILER_IS_GNUCXX)
	# libtool
	create_libtool_file(crn lib dev)
	# pkg-config
	if(CRN_USING_GTKMM)
		set(PKGC_LIBCRN_DEPS "${PKGC_LIBCRN_DEPS} gtkmm-2.4 gthread-2.0")
	else(CRN_USING_GTKMM)
		if(CRN_USING_GDKPB)
			set(PKGC_LIBCRN_DEPS "${PKGC_LIBCRN_DEPS} gdk-pixbuf-2.0")
		endif(CRN_USING_GDKPB)
	endif(CRN_USING_GTKMM)
	if(CRN_USING_LIBPNG)
		set(PKGC_LIBCRN_DEPS "${PKGC_LIBCRN_DEPS} libpng")
	endif(CRN_USING_LIBPNG)
	if(CRN_USING_LIBJPEG)
		set(PKGC_LIBCRN_LDFLAGS "${PKGC_LIBCRN_LDFLAGS} -ljpeg")
	endif(CRN_USING_LIBJPEG)
	if(CRN_USING_LIBZIP)
		set(PKGC_LIBCRN_DEPS "${PKGC_LIBCRN_DEPS} libzip")
	endif(CRN_USING_LIBZIP)
	if (CRN_USING_HARU)
		#set(PKGC_LIBCRN_DEPS "${PKGC_LIBCRN_DEPS} libhpdf")
		set(PKGC_LIBCRN_LDFLAGS "${PKGC_LIBCRN_LDFLAGS} -lhpdf")
	endif (CRN_USING_HARU)

	configure_file(
		"${PROJECT_SOURCE_DIR}/libcrn/libcrn.pc.in"
		"${PROJECT_BINARY_DIR}/libcrn/libcrn.pc"
		@ONLY)
	install(FILES "${PROJECT_BINARY_DIR}/libcrn/libcrn.pc" DESTINATION "lib/pkgconfig" COMPONENT dev)
endif(CMAKE_COMPILER_IS_GNUCXX)


