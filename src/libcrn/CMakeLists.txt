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

set(LIBCRN_LIB
	${ICONV_LIBRARIES}
	${GDIPLUS_LIBRARIES}
)
if(CRN_USING_GLIB_INTL OR CRN_USING_GDKPB)
	include_directories(${GTK2_INCLUDE_DIRS})
	list(APPEND LIBCRN_LIB
		${GTK2_LIBRARIES}
	)
endif()

add_library(${PRJ_NAME} ${LIBCRN_SRC} ${LIBCRN_HEADER_CFG} ${LIBCRN_HEADER})
target_link_libraries(${PRJ_NAME} ${LIBCRN_LIB})

# Create Android package
#if(ANDROID)
	#include("cmake/android/Apk.cmake" REQUIRED)
	#android_create_apk(${PRJ_NAME} "${CMAKE_BINARY_DIR}/apk" "${APP_SHARED_LIBRARIES}" "" "Data")
#endif(ANDROID)
