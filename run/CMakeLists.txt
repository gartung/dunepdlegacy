add_executable(FELIXdecode FELIXdecode.cc)
target_link_libraries(FELIXdecode
  dunepdlegacy_Overlays
  ${ARTDAQ-CORE_DATA}
  pthread)
