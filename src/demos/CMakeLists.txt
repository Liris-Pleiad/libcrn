# libcrn demos build script

# create estiamteStrokes
add_executable(estimateStrokes estimateStrokes.cpp)
# add dependencies
target_link_libraries(estimateStrokes libcrn)
