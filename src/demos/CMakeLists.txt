# libcrn demos build script

#############################
# add path to CRN.h
include_directories("${PROJECT_BINARY_DIR}/include/libcrn")
# add path to libcrn
include_directories("${PROJECT_SOURCE_DIR}/src/libcrn")
#############################

# create estiamteStrokes
add_executable(estimateStrokes estimateStrokes.cpp)
# add dependencies
target_link_libraries(estimateStrokes crn)

# create ocr4dummies
add_executable(ocr4dummies ocr4dummies.cpp)
# add dependencies
target_link_libraries(ocr4dummies crn)

#############################

if(CRN_DEPENDENCY_OPENCV)
	find_package( OpenCV REQUIRED )
	if(OpenCV_FOUND)
	  message(STATUS "Found OpenCV libs: ${OpenCV_LIBS}")
	  message(STATUS "Found OpenCV includes: ${OpenCV_INCLUDE_DIRS}")
	else(OpenCV_FOUND) 
	  message(STATUS "OpenCV (needed for 'crn2opencv2crn' demo) not found. Please set OpenCV_DIR or uncheck CRN_DEPENDENCY_OPENCV.")
	endif(OpenCV_FOUND)
endif(CRN_DEPENDENCY_OPENCV)

if(OpenCV_FOUND)
	include_directories(${OpenCV_INCLUDE_DIRS})

	# create crn2opencv2crn
	add_executable(crn2opencv2crn crn2opencv2crn.cpp)
	# add dependencies
	target_link_libraries(crn2opencv2crn crn ${OpenCV_LIBS})
endif()
