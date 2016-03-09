
set OPTIONS=^
NDK_PROJECT_PATH=. ^
NDK_APPLICATION_MK=jni/Application_x86.mk

call ndk-build clean %OPTIONS%
call ndk-build -j4 %OPTIONS%
