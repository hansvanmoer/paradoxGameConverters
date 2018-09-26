#
# Contains compiler feature check macros
#

include(CheckCXXSourceCompiles)

macro(check_compiler_feature featureName message code)

check_cxx_source_compiles("${code}" ${featureName})

if(NOT ${featureName})
  message(FATAL_ERROR "\nMissing compiler or standard library feature:\n\t${message}\nPlease update your compiler (currently '${CMAKE_CXX_COMPILER}') or specify another toolchain by running the cmake command with -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain/file\nExample toolchain files can be found in the cmake/examples folder in the paradoxGameConverters source\nNote that this test is only performed once and the result is cached: delete the CMakeCache.txt before running cmake again.\n")
endif()

endmacro()

macro(check_compiler_feature_std_optional)

check_compiler_feature("std_optional" "template std::optional was not recognized" "

  #include <optional>

  int main(int arg_count, const char **args){
    std::optional<int> opt{1};
    return 0;
  }

")

endmacro()

macro(check_compiler_feature_std_regex)

check_compiler_feature("std_regex" "template std::regex was not recognized" "

  #include <regex>

  int main(int arg_count, const char **args){
    std::regex reg{};
    return 0;
  }

")

endmacro()
