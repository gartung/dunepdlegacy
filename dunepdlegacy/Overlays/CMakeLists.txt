# project name
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.22.02) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible

art_make(BASENAME_ONLY
               LIBRARY_NAME dunepdlegacy_Overlays
               LIB_LIBRARIES
               artdaq_core::artdaq-core_Data
               artdaq_core::artdaq-core_Utilities
  cetlib_except::cetlib_except
  fhiclcpp::fhiclcpp
  messagefacility::MF_MessageLogger
  messagefacility::headers
  dunepdlegacy::rce_dataaccess
  gallery::gallery
  canvas::canvas
  ROOT::Core
  art::Framework_Principal
  pthread
  z
  ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.22.02) - deprecated: use art_make_library(), art_dictonary(), and cet_build_plugin() with explicit source lists and plugin base types

install_headers()
install_source()
