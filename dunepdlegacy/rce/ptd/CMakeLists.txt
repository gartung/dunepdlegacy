
cet_make_exec(PdEntropy
  SOURCE
    PdEntropy.cc
  LIBRARIES
    dunepdlegacy::rce_dataaccess
)

cet_make_exec(PdReaderTest
  SOURCE
    PdReaderTest.cc
  LIBRARIES
    dunepdlegacy::rce_dataaccess
)

cet_make_exec(PdWibFrameTest
  SOURCE
    PdWibFrameTest.cc
  LIBRARIES
    dunepdlegacy::rce_dataaccess
)

cet_make_exec(PdWibFrameExtract
  SOURCE
    PdWibFrameExtract.cc
  LIBRARIES
    dunepdlegacy::rce_dataaccess
)

