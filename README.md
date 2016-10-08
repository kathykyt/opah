# opah
Opah is a lightweight GUI designed for embedded system. It targets to run on top of an native Android system, just with native part (Without the Java implementation and run time). Opah targets to support SmartHome GUI control.



===========================================
  Build environment setup

===========================================
Opah depends on Android NDK to build. The environment shoud be under Ubuntu 14.04 64bit machine.

1. Install android-ndk-r10, http://dl.google.com/android/ndk/android-ndk32-r10b-linux-x86_64.tar.bz2 or http://dl.google.com/android/ndk/android-ndk32-r10-linux-x86_64.tar.bz2 (Need the directory platforms/android-19 as opah base on Android 4.2).
2. Under android-ndk-r10/samples, type git clone https://github.com/kathykyt/opah.git



==============================================
  How to build 
===============================================
1. cd (NDK top dir)/samples/opah/jni/project/sample1, type "./build_pic.sh"
2. cd (NDK top dir)/samples/opah, type "build_opah.sh"
3. There are two components build, (NDK top dir)/samples/opah/libs/armeabi/sample1_rgb565, and (NDK top dir)/samples/opah/libs/armeabi/libImageData.so

==============================================
  Setup Goldfish emulator (Moonfish) for Opah
===============================================









