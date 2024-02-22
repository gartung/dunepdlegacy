# ======================================================================
#  darkart main build file
#
#  cd .../path/to/build/directory
#  source ../path/to/dune-raw-data/ups/setup_for_development
#  buildtool
# ======================================================================


# use cmake 3.19 or later
cmake_minimum_required (VERSION 3.20 FATAL_ERROR) ### MIGRATE-CHANGED (migrate-3.22.02) - VERSION 3.19 -> 3.20
# cmake_policy(VERSION 3.19) ### MIGRATE-ACTION-COMPATIBILITY (migrate-3.22.02) - Uncomment to preserve compatibility with older CMake versions

find_package(cetmodules REQUIRED) ### MIGRATE-ADDED (migrate-3.22.02) - find_package(cetmodules) must precede project()
project(dunepdlegacy VERSION 1.01.00 LANGUAGES CXX)
set(dunepdlegacy_FW_DIR "files")
# cetbuildtools contains our cmake modules

list(APPEND CMAKE_MODULE_PATH $ENV{CANVAS_ROOT_IO_DIR}/Modules $ENV{ART_DIR}/Modules) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.22.02) - prefer find_package() to find external CMake modules

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT
#  WERROR
# NO_UNDEFINED
  EXTRA_FLAGS -pedantic
  EXTRA_DEFINITIONS -D_GLIBCXX_USE_NANOSLEEP #-march=native
)

cet_report_compiler_flags() ### MIGRATE-ACTION-RECOMMENDED (migrate-3.22.02) - add args: REPORT_THRESHOLD VERBOSE

# these are minimum required versions, not the actual product versions
find_package(Boost REQUIRED) # Needs to be first to cover for a missing transitive ### MIGRATE-CHANGED (migrate-3.22.02) - find_ups_product(...) -> find_package(REQUIRED ...)
                 # dependency from artdaq_core.
find_package( artdaq_core REQUIRED ) ### MIGRATE-CHANGED (migrate-3.22.02) - find_ups_product(artdaq_core ...) -> find_package(artdaq_core REQUIRED ...)
find_package( art REQUIRED ) ### MIGRATE-CHANGED (migrate-3.22.02) - find_ups_product(art ...) -> find_package(art REQUIRED ...)
find_package( cetlib REQUIRED ) ### MIGRATE-CHANGED (migrate-3.22.02) - find_ups_product(cetlib ...) -> find_package(cetlib REQUIRED ...)
find_package( gallery REQUIRED ) ### MIGRATE-CHANGED (migrate-3.22.02) - find_ups_product(gallery ...) -> find_package(gallery REQUIRED ...)
find_package( messagefacility REQUIRED ) ### MIGRATE-CHANGED (migrate-3.22.02) - find_ups_product(messagefacility ...) -> find_package(messagefacility REQUIRED ...)

# have to make sure cmake doesn't select the system libraries
# define libsigc
cet_find_library( SIGC    NAMES sigc-2.0  PATHS ENV LIBSIGCPP_LIB NO_DEFAULT_PATH ) ### MIGRATE-ACTION-REQUIRED (migrate-3.22.02) - avoid: use find_package() with custom Find<pkg>.cmake for Spack compatibility

# macros for art_dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)
include(CetTest)

# source
add_subdirectory(dunepdlegacy)

# test
add_subdirectory(test)

# run
add_subdirectory(run)

# ups - table and config files
# packaging utility
cet_cmake_config() ### MIGRATE-ADDED (migrate-3.22.02) - required cmd
