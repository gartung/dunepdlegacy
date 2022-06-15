# project name
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

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
  ${ART_LIB}
  art_Framework_Principal
  pthread
  z
  )

install_headers()
install_source()
