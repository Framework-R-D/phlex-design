cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

########################################################################
# Ensure we have access to a suitable version of Cetmodules
include(FetchContent)
FetchContent_Declare(
  cetmodules
  GIT_REPOSITORY https://github.com/FNALssi/cetmodules
  GIT_TAG v4-branch
#  GIT_TAG fd6ebf45
  FIND_PACKAGE_ARGS 3.99.00
)
# ... and Microsoft's C++ Guideline Support Library
FetchContent_Declare(GSL
  GIT_REPOSITORY https://github.com/microsoft/GSL
  GIT_TAG 2828399820ef4928cc89b65605dca5dc68efca6e # v4.2.0
  GIT_SHALLOW ON
  FIND_PACKAGE_ARGS NAMES Microsoft.GSL
)
# ... and Catch2
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG 914aeecfe23b1e16af6ea675a4fb5dbd5a5b8d0a # v3.8.0
  GIT_SHALLOW ON
  FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(cetmodules)
########################################################################

find_package(cetmodules 3.99.00 REQUIRED)
project(f1cd VERSION 0.1 LANGUAGES CXX)

# These activations are delayed to avoid invocation of language-specific
# features by CMake Config files before they have been initialized by
# `project()`.
FetchContent_MakeAvailable(GSL Catch2)

include(CetCMakeEnv)
include(SetCompilerFlags)
include(CetCMakeConfig)
include(CTest)

cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic
  )

if(DEFINED ENV{CET_SANITIZE_ADDRESS} AND DEFINED ENV{GCC_FQ_DIR})
  find_file(ASAN_LIB libasan.so NO_DEFAULT_PATH PATHS $ENV{GCC_FQ_DIR}/lib64)
  cet_add_compiler_flags(CXX -fsanitize=address)
  cet_add_compiler_flags(C -fsanitize=address)
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address")
  set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=address")
  set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=address")
endif()

if(DEFINED ENV{CET_SANITIZE_THREAD} AND DEFINED ENV{GCC_FQ_DIR})
  find_file(TSAN_LIB libtsan.so NO_DEFAULT_PATH PATHS $ENV{GCC_FQ_DIR}/lib64)
  cet_add_compiler_flags(CXX -fsanitize=thread -D__SANITIZE_THREAD__)
  cet_add_compiler_flags(C -fsanitize=thread -D__SANITIZE_THREAD__)
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=thread -D__SANITIZE_THREAD__")
  set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -fsanitize=thread -D__SANITIZE_THREAD__")
  set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=thread -D__SANITIZE_THREAD__")
endif()

cet_report_compiler_flags(REPORT_THRESHOLD VERBOSE)

########################################################################
# Build code and tests
find_package(fhiclpy 4.04 REQUIRED)
find_package(fhiclcpp 4.19 REQUIRED)
find_package(cetlib REQUIRED)
find_package(Jsonnet 0.21 REQUIRED)
find_package(Python 3.9 COMPONENTS Development REQUIRED)
find_package(Microsoft.GSL REQUIRED)
find_package(yaml-cpp 0.8.0 REQUIRED)

include(CetMake)
include(CetMakeLibrary)
include(CetTest)

cet_make_exec(NAME fhicl_t NO_EXPORT NO_EXPORT_ALL_SYMBOLS
  LIBRARIES PRIVATE fhiclcpp::fhiclcpp cetlib::cetlib
)
target_compile_features(fhicl_t PRIVATE cxx_std_20)

cet_test(fhicl_t HANDBUILT
  TEST_EXEC fhicl_t
  DATAFILES FHiCL.fcl
)

cet_make_library(LIBRARY_NAME f1cd SOURCE make_parameter_set_from_YAML.cc
  LIBRARIES PUBLIC fhiclcpp::fhiclcpp PRIVATE yaml-cpp::yaml-cpp
)
target_compile_features(f1cd PRIVATE cxx_std_20)

cet_make_exec(NAME json_t
  SOURCE json_t.cc
  NO_EXPORT NO_EXPORT_ALL_SYMBOLS
  LIBRARIES PRIVATE f1cd::f1cd fhiclcpp::fhiclcpp Jsonnet::libjsonnet++
)
target_compile_features(json_t PRIVATE cxx_std_20)

cet_test(json_t HANDBUILT
  TEST_EXEC json_t
  DATAFILES JSON.json
)

cet_make_exec(NAME yaml_t
  SOURCE yaml_t.cc
  NO_EXPORT NO_EXPORT_ALL_SYMBOLS
  LIBRARIES PRIVATE f1cd::f1cd fhiclcpp::fhiclcpp
)
target_compile_features(yaml_t PRIVATE cxx_std_20)

cet_test(yaml_t HANDBUILT
  TEST_EXEC yaml_t
  DATAFILES YAML.yaml
)

########################################################################
# End matter
cet_cmake_config()
########################################################################
