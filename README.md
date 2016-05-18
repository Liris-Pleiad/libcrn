# libcrn

libcrn is a document image processing library written in C++11 for Linux, Windows, Mac OS X and Google Android. It is a toolbox that allows to create easily software such as OCRs and layout analysis tools.

( --> Initial commit from http://sourceforge.net/projects/libcrn/ - Tree [r1364] : http://sourceforge.net/p/libcrn/code/1364/ <-- )

## Windows
Visual Studio 2015 needed.
Download dependencies at: http://TODO .
For now, only one option between Gtkmm2, Gtkmm3-Release and Gtkmm3-Debug can be chosen at the same time.

## Linux
Builds with g++ 4.8 to 6.1 and clang++ 3.6.

## MaxOS
Dependencies available in homebrew.

## Android
### Build
```
mkdir buildand
cd buildand
ANDROID_NDK=absolute_path_to_ndk cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/android/android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=9 ..
```
### Run tests
Launch an emulator.
```
adb push src/libcrn/libcrn.so /sdcard/
adb push src/tests/tests /sdcard/
adb shell
cd /data/local
cp /sdcard/libcrn.so .
chmod 751 libcrn.so
cp /sdcard/tests
chmod 751 tests
LD_LIBRARY_PATH=. ./tests
```
