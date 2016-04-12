# MSVC - KIT VS 2015 x64 - V01

if( DEFINED ENV{MSVC_KIT_ROOT} )
	set( MSVC_KIT_ROOT $ENV{MSVC_KIT_ROOT} )
endif()
if( NOT DEFINED MSVC_KIT_ROOT )
	message(FATAL_ERROR "MSVC_KIT_ROOT not set.  Please set MSVC_KIT_ROOT.")
endif()

###

set(ICONV_DIR							${MSVC_KIT_ROOT}/libiconv-1.14)

###

#set(GTK2_HINTS							${MSVC_KIT_ROOT}/gtkmm2)
#
#set(FREETYPE_INCLUDE_DIR_ft2build		${GTK2_HINTS}/include)
#set(FREETYPE_INCLUDE_DIR_freetype2		${GTK2_HINTS}/include/freetype2)
#
#set(Intl_INCLUDE_DIR					${GTK2_HINTS}/include)
#set(Intl_LIBRARY						${GTK2_HINTS}/lib/intl.lib)
#
#set(GETTEXT_DIR						${GTK2_HINTS}/bin)

###

set(GTK3_HINTS							${MSVC_KIT_ROOT}/gtkmm3)
#
set(FREETYPE_INCLUDE_DIR_ft2build		${GTK3_HINTS}/include/freetype)
set(FREETYPE_INCLUDE_DIR_freetype2		${GTK3_HINTS}/include/freetype/freetype)

###