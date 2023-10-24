#include "mapping_tofd_trig.hh" // Copy this file from your upexps/202205_s522 folder

void make_tofd_trig_mapping(){

  tofd_trig_map_setup();

  auto rtdb = FairRuntimeDb::instance();
  
  auto parOut = new FairParAsciiFileIo();
  parOut->open("tofd_mapping_v1.par", "out");
  rtdb->setOutput(parOut);

  auto par3 = (R3BTofDMappingPar*)(rtdb->getContainer("tofdMappingPar"));
  
  rtdb->addRun(1);

  for(Int_t i = 0; i < 4; i++)
  for(Int_t j = 0; j < 2; j++)
  for(Int_t k = 0; k < 44; k++)
  {  
    par3->SetTrigMap(g_tofd_trig_map[i][j][k],i+1,k+1,j+1);
  }
  
  par3->setChanged();
  
  rtdb->writeContainers();

  rtdb->saveOutput();

  rtdb->print();
}
