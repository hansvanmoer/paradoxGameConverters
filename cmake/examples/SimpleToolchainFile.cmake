#
# WARNING: this example file is not used for the build, do not modify this file nor commit your toolchain file(s) into the repository 
#
# This example toolchain file for the following situation:
#
# On a debian linux system, the default c++ compiler can be found at "/usr/bin/g++"
# Unfortunately this compiler does not support all required features and can't be used to compile the project
# Another, more recent, compiler was compiled from source and installed at "/opt/gcc/bin/g++"
# This toolchain file simply sets the compiler to the newly installed one
# When calling cmake to generate the build files, use the following command:
#
# cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain/file <other options>
#

set(CMAKE_CXX_COMPILER /opt/gcc/bin/g++)
