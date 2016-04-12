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

