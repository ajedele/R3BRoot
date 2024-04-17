//
// Macro to modify the TOFD parameters by hand 
//
void remove_tofd_parameters(int fRunID)
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

  auto *parCont1 = (R3BTofDHitPar*)(rtdb->getContainer("tofdHitPar"));
  
  rtdb->initContainers(fRunID);
  
  // Here you can change the value of the parameters, example for plane 4 and paddle 44
  int counter = 0;
  for (int iPlane = 0; iPlane<4; iPlane++)
  {
  	for (int iBar = 12; iBar<32; iBar++)
	{

  		R3BTofDHitModulePar* par  = parCont1->GetModuleParAt(iPlane+1, iBar+1);
  		
  		par->SetOffset1(par->GetOffset1());
  		par->SetOffset2(par->GetOffset2());
  		par->SetToTOffset1(par->GetToTOffset1());
  		par->SetToTOffset2(par->GetToTOffset2());
  		par->SetSync(par->GetSync());
  		par->SetVeff(par->GetVeff());
  		par->SetLambda(par->GetLambda());
  		par->SetPar1a(par->GetPar1a());
  		par->SetPar1b(par->GetPar1b());
  		par->SetPar1c(par->GetPar1c());
  		par->SetPar1d(par->GetPar1d());
  		par->SetPar2a(par->GetPar2a());
  		par->SetPar2b(par->GetPar2b());
  		par->SetPar2c(par->GetPar2c());
  		par->SetPar2d(par->GetPar2d());
  		par->SetPola(par->GetPola());
  		par->SetPolb(par->GetPolb());
  		par->SetPolc(par->GetPolc());
  		par->SetPold(par->GetPold());
  		par->SetPar1za(par->GetPol1za());
  		par->SetPar1zb(par->GetPol1zb());
  		par->SetPar1zc(par->GetPol1zc());
  		par->SetPar1Walk(par->GetPar1Walk());
  		par->SetPar2Walk(par->GetPar2Walk());
  		par->SetPar3Walk(par->GetPar3Walk());
  		par->SetPar4Walk(par->GetPar4Walk());
  		par->SetPar5Walk(par->GetPar5Walk());

		rtdb->SaveOutput();
	}
  }
  printf("Total number to be fixed: %d\n", counter);
}
