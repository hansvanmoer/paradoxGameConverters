#
# Contains compiler feature check macros
#

include(CheckCXXSourceCompiles)

macro(compiler_feature_check_optional)

check_cxx_source_compiles("

  #include <optional>

  int main(int arg_count, const char **args){
    std::optional<int> opt{1};
    return 0;
  }

" featureCheckOptional)

if(NOT featureCheckOptional)
  message(FATAL_ERROR "missing compiler feature: template std::optional was not recognized by the specified compiler '${CMAKE_CXX_COMPILER}':\nplease update your compiler or specify another toolchain by running the cmake command with -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain/file\nExample toolchain files can be found in the cmake/examples folder in the paradoxGameConverters source\nNote that this test is only performed once and the result is cached: delete the CMakeCache.txt before running cmake again.")
endif()

endmacro()

macro(compiler_feature_check_regex)

check_cxx_source_compiles("

  #include <regex>

  int main(int arg_count, const char **args){
    std::regex reg{};
    return 0;
  }

" featureCheckRegex)

if(NOT featureCheckRegex)
  message(FATAL_ERROR "missing compiler feature: template std::regex was not recognized by the specified compiler '${CMAKE_CXX_COMPILER}':\nplease update your compiler or specify another toolchain by running the cmake command with -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain/file\nExample toolchain files can be found in the cmake/examples folder in the paradoxGameConverters source\nNote that this test is only performed once and the result is cached: delete the CMakeCache.txt before running cmake again.")
endif()

endmacro()
