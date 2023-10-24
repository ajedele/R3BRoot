//
// Macro to modify the TOFD parameters by hand 
//
void change_tofd_parameters(int fRunID)
{

  // Runtime data base ------------------------------------
  auto *rtdb = FairRuntimeDb::instance();

  // Open an OUTPUT file to save the new parameters
  Bool_t kParameterMerged = false;
  FairParRootFileIo* parOut2 = new FairParRootFileIo(kParameterMerged);
  parOut2->open("tofdhitpar_v5.root", "RECREATE");
  rtdb->setOutput(parOut2);
  rtdb->addRun(fRunID);
   
   
  // Open the INPUT file
  FairParRootFileIo* parIo1 = new FairParRootFileIo(kParameterMerged);
  parIo1->open(Form("/u/ajedele/R3BRoot_fork/MACROSS/parameter/tofd_HitPar_s473_Z%04d.root",fRunID), "in");
  rtdb->setFirstInput(parIo1);

  //auto *parCont1 = (R3BTCalPar*)(rtdb->getContainer("LosTCalPar"));
  //
  //auto *parCont2 = (R3BTCalPar*)(rtdb->getContainer("TofdTCalPar"));
  
  auto *parCont3 = (R3BTofDHitPar*)(rtdb->getContainer("tofdHitPar"));
  
  rtdb->initContainers(fRunID);
  
  // Here you can change the value of the parameters, example for plane 4 and paddle 44
 
  int counter = 0;
  for (int iPlane = 0; iPlane<4; iPlane++)
  {
  	for (int iBar = 12; iBar<32; iBar++)
	{

  		R3BTofDHitModulePar* par = parCont3->GetModuleParAt(iPlane+1, iBar+1);
  		if (par->GetPar1zc()>0.009 || par->GetPar1za()>15 || par->GetPar1za()==0) 
		{
			printf("Plane: %d, Bar: %d \n Para: %lf, Parb: %lf, Parc: %lf\n\n", iPlane+1, iBar+1, par->GetPar1za(), par->GetPar1zb(), par->GetPar1zc());
			counter++;
		}
  		
  		//par->SetPar1Walk(2.);
  		//par->printParams();
  		
  		//rtdb->saveOutput();
	}
  }
  printf("Total number to be fixed: %d\n", counter);
}
