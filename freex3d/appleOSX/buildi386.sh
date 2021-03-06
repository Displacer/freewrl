#!/bin/sh

# Make i386 library first
make distclean
cp -f vrml.conf.i386 vrml.conf.aqua
perl Makefile.PL.osx
make install
cp -f libFreeWRLFunc.dylib VRMLFunc_i386.dylib

# Glue the two libraries together and make sure the library has a good name
lipo -create VRMLFunc_i386.dylib -output libFreeWRLFunc.dylib
install_name_tool -id libFreeWRLFunc.dylib libFreeWRLFunc.dylib
cp libFreeWRLFunc.dylib /usr/local/lib/libFreeWRLFunc.dylib

# Clean up
rm -f VRMLFunc_i386.dylib
rm -f libFreeWRLjs.dylib
rm -f libFreeWRLFunc.dylib
rm -f libFreeWRLFunc_i386.so
