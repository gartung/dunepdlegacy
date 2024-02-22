# project name
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.22.02) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible

art_make(BASENAME_ONLY
               LIBRARY_NAME dunepdlegacy_Overlays
               LIB_LIBRARIES
               ${ARTDAQ_CORE_ARTDAQ-CORE_DATA}
               ${ARTDAQ_CORE_ARTDAQ-CORE_UTILITIES}
  ${ARTDAQ_DAQDATA}
  ${CETLIB_EXCEPT}
  ${Boost_SYSTEM_LIBRARY}
  ${FHICLCPP}
  ${MF_MESSAGELOGGER}
  ${MF_MESSAGEUTILITIES}
  dunepdlegacy::rce_dataaccess
  ${GALLERY_LIB}
  gallery
  canvas
  ${ROOT_BASIC_LIB_LIST}
  ROOT::Core
  ${ART_LIB}
  art_Framework_Principal
  pthread
  z
  ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.22.02) - deprecated: use art_make_library(), art_dictonary(), and cet_build_plugin() with explicit source lists and plugin base types

install_headers()
install_source()
