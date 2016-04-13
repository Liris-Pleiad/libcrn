if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "19")
	if(NOT DEFINED ENV{APPVEYOR})
		message("--> VS2015 KIT used")
		include("${CMAKE_SOURCE_DIR}/cmake/msvc/kit_vs2015-64_v01.cmake") # vs2015 (actual kit)
	else()
		message("--> VS2015 KIT used (AppVeyor packages)")
		include("${CMAKE_SOURCE_DIR}/cmake/msvc/kit_vs2015-64_v01-AppVeyor.cmake") # vs2015 (actual kit)
	endif()
elseif (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "18")
#	message("--> NO VS2013 KIT !") # vs2013 (no kit...)
elseif (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "17")
#	message("--> NO VS2012 KIT !") # vs2012 (no kit...)
endif()