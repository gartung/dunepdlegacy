add_executable(FELIXdecode FELIXdecode.cc) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.22.02) - prefer cet_make_exec() for transitivity
target_link_libraries(FELIXdecode
  dunepdlegacy_Overlays
  ${ARTDAQ-CORE_DATA}
  pthread)
