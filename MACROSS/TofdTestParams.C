//
// Macro to modify the TOFD parameters by hand 
//
#include "TH2.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TAttPad.h"

TH2D *Offset1[4][25];
TH2D *Offset2[4][25];
TH2D *ToTOffset1[4][25];
TH2D *ToTOffset2[4][25];
TH2D *Sync[4][25];
TH2D *Veff[4][25];
TH2D *Lambda[4][25];
TH2D *ParMeander1[4][25];
TH2D *ParMeander2[4][25];
TH2D *ParMeander3[4][25];
TH2D *ParMeander4[4][25];
TH2D *ParZ1[4][25];
TH2D *ParZ2[4][25];

TCanvas *CanOffset;
TCanvas *CanToTOffset;
TCanvas *CanSync;
TCanvas *CanVeff;
TCanvas *CanLambda;
TCanvas *CanParMeander1;
TCanvas *CanParMeander2;
TCanvas *CanParMeander3;
TCanvas *CanParMeander4;
TCanvas *CanParZ1;
TCanvas *CanParZ2;

TString nom;

void DrawHist()
{
	nom = Form("Offset");
	CanOffset = new TCanvas(nom.Data(),nom.Data(),800,800);
	gPad->SetLeftMargin(0.15);
	gPad->SetRightMargin(0.15);
	gPad->SetBottomMargin(0.15);
	CanOffset->Divide(2,2,0.01,0.01);
	nom = Form("ToTOffset");
	CanToTOffset = new TCanvas(nom.Data(),nom.Data(),800,800);
	gPad->SetLeftMargin(0.15);
	gPad->SetRightMargin(0.15);
	gPad->SetBottomMargin(0.15);
	CanToTOffset->Divide(2,2,0.01,0.01);
	nom = Form("Sync");
	CanSync = new TCanvas(nom.Data(),nom.Data(),800,800);
	gPad->SetLeftMargin(0.15);
	gPad->SetRightMargin(0.15);
	gPad->SetBottomMargin(0.15);
	CanSync->Divide(2,2,0.01,0.01);
	nom = Form("Veff");
	CanVeff = new TCanvas(nom.Data(),nom.Data(),800,800);
	gPad->SetLeftMargin(0.15);
	gPad->SetRightMargin(0.15);
	gPad->SetBottomMargin(0.15);
	CanVeff->Divide(2,2,0.01,0.01);
	nom = Form("Lambda");
	CanLambda = new TCanvas(nom.Data(),nom.Data(),800,800);
	gPad->SetLeftMargin(0.15);
	gPad->SetRightMargin(0.15);
	gPad->SetBottomMargin(0.15);
	CanLambda->Divide(2,2,0.01,0.01);
	nom = Form("ParMeander1");
	CanParMeander1 = new TCanvas(nom.Data(),nom.Data(),800,800);
	gPad->SetLeftMargin(0.15);
	gPad->SetRightMargin(0.15);
	gPad->SetBottomMargin(0.15);
	CanParMeander1->Divide(2,2,0.01,0.01);
	nom = Form("ParMeander2");
	CanParMeander2 = new TCanvas(nom.Data(),nom.Data(),800,800);
	gPad->SetLeftMargin(0.15);
	gPad->SetRightMargin(0.15);
	gPad->SetBottomMargin(0.15);
	CanParMeander2->Divide(2,2,0.01,0.01);
	nom = Form("ParMeander3");
	CanParMeander3 = new TCanvas(nom.Data(),nom.Data(),800,800);
	gPad->SetLeftMargin(0.15);
	gPad->SetRightMargin(0.15);
	gPad->SetBottomMargin(0.15);
	CanParMeander3->Divide(2,2,0.01,0.01);
	nom = Form("ParMeander4");
	CanParMeander4 = new TCanvas(nom.Data(),nom.Data(),800,800);
	gPad->SetLeftMargin(0.15);
	gPad->SetRightMargin(0.15);
	gPad->SetBottomMargin(0.15);
	CanParMeander4->Divide(2,2,0.01,0.01);
	nom = Form("ParZ1");
	CanParZ1 = new TCanvas(nom.Data(),nom.Data(),800,800);
	gPad->SetLeftMargin(0.15);
	gPad->SetRightMargin(0.15);
	gPad->SetBottomMargin(0.15);
	CanParZ1->Divide(2,2,0.01,0.01);
	nom = Form("ParZ2");
	CanParZ2 = new TCanvas(nom.Data(),nom.Data(),800,800);
	gPad->SetLeftMargin(0.15);
	gPad->SetRightMargin(0.15);
	gPad->SetBottomMargin(0.15);
	CanParZ2->Divide(2,2,0.01,0.01);
		
	for (int iPlane=0; iPlane<4; iPlane++)
	{
		for (int iBar=12; iBar<32; iBar++)
		{
			nom = Form("Offset1_Plane%d_Bar%d",iPlane+1,iBar+1);
			Offset1[iPlane][iBar-12] = new TH2D(nom.Data(),nom.Data(),21,12,32,100,-5,5); 
			nom = Form("Offset2_Plane%d_Bar%d",iPlane+1,iBar+1);
			Offset2[iPlane][iBar-12] = new TH2D(nom.Data(),nom.Data(),21,12,32,100,-5,5); 
			nom = Form("ToTOffset1_Plane%d_Bar%d",iPlane+1,iBar+1);
			ToTOffset1[iPlane][iBar-12] = new TH2D(nom.Data(),nom.Data(),21,12,32,100,0.9,1.1); 
			nom = Form("ToTOffset2_Plane%d_Bar%d",iPlane+1,iBar+1);
			ToTOffset2[iPlane][iBar-12] = new TH2D(nom.Data(),nom.Data(),21,12,32,100,0.9,1.1); 
			nom = Form("Sync_Plane%d_Bar%d",iPlane+1,iBar+1);
			Sync[iPlane][iBar-12] = new TH2D(nom.Data(),nom.Data(),21,12,32,100,-950,-900); 
			nom = Form("Veff_Plane%d_Bar%d",iPlane+1,iBar+1);
			Veff[iPlane][iBar-12] = new TH2D(nom.Data(),nom.Data(),21,12,32,80,-8,-4); 
			nom = Form("Lambda_Plane%d_Bar%d",iPlane+1,iBar+1);
			Lambda[iPlane][iBar-12] = new TH2D(nom.Data(),nom.Data(),21,12,32,200,-200,0); 
			nom = Form("ParMeander1_Plane%d_Bar%d",iPlane+1,iBar+1);
			ParMeander1[iPlane][iBar-12] = new TH2D(nom.Data(),nom.Data(),21,12,32,30,120,150); 
			nom = Form("ParMeander2_Plane%d_Bar%d",iPlane+1,iBar+1);
			ParMeander2[iPlane][iBar-12] = new TH2D(nom.Data(),nom.Data(),21,12,32,200,-0.2,0.2); 
			nom = Form("ParMeander3_Plane%d_Bar%d",iPlane+1,iBar+1);
			ParMeander3[iPlane][iBar-12] = new TH2D(nom.Data(),nom.Data(),21,12,32,100,-0.004,0.004); 
			nom = Form("ParMeander4_Plane%d_Bar%d",iPlane+1,iBar+1);
			ParMeander4[iPlane][iBar-12] = new TH2D(nom.Data(),nom.Data(),21,12,32,100,-0.0002,0.0002); 
			nom = Form("ParZ1_Plane%d_Bar%d",iPlane+1,iBar+1);
			ParZ1[iPlane][iBar-12] = new TH2D(nom.Data(),nom.Data(),21,12,32,300,-15,0); 
			nom = Form("ParZ2_Plane%d_Bar%d",iPlane+1,iBar+1);
			ParZ2[iPlane][iBar-12] = new TH2D(nom.Data(),nom.Data(),21,12,32,100,0.5,1.5); 
		}
	}
	return;
}

void TofdTestParams(int fRunID)
{
	DrawHist();
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
	parIo1->open(Form("/u/ajedele/R3BRoot_fork/MACROSS/parameter/tofd_HitPar_s473_Z%03d.root",fRunID), "in");
	rtdb->setFirstInput(parIo1);
	
	auto *parCont3 = (R3BTofDHitPar*)(rtdb->getContainer("tofdHitPar"));
	rtdb->initContainers(fRunID);
	
	// Here you can change the value of the parameters, example for plane 4 and paddle 44
	int counter = 0;
	int counterOffset1=0, counterOffset2=0, counterToTOffset1=0, counterToTOffset2=0, counterSync=0, counterVeff=0, counterLambda=0, counterPola=0, counterPolb=0, counterPolc=0, counterPold=0, counterPar1za=0, counterPar1zb=0;
	double avgOffset1=0, avgOffset2=0, avgToTOffset1=0, avgToTOffset2=0, avgSync=0, avgVeff=0, avgLambda=0, avgPola=0, avgPolb=0, avgPolc=0, avgPold=0, avgPar1za=0, avgPar1zb=0;
	for (int iPlane = 0; iPlane<4; iPlane++)
	{
		for (int iBar = 13; iBar<32; iBar++)
		{
			R3BTofDHitModulePar* par = parCont3->GetModuleParAt(iPlane+1,iBar+1);
			if (par->GetOffset1()!=0) {avgOffset1 = par->GetOffset1() + avgOffset1; counterOffset1++;}
			if (par->GetOffset2()!=0) {avgOffset2 = par->GetOffset2() + avgOffset2; counterOffset2++;}
			if (par->GetToTOffset1()!=0) {avgToTOffset1 = par->GetToTOffset1() + avgToTOffset1; counterToTOffset1++;}
			if (par->GetToTOffset2()!=0) {avgToTOffset2 = par->GetToTOffset2() + avgToTOffset2; counterToTOffset2++;}
			if (par->GetSync()!=0) {avgSync = par->GetSync() + avgSync; counterSync++;}
			if (par->GetVeff()!=0) {avgVeff = par->GetVeff() + avgVeff; counterVeff++;}
			if (par->GetLambda()!=0) {avgLambda = par->GetLambda() + avgLambda; counterLambda++;}
			if (par->GetPola()!=0) {avgPola = par->GetPola() + avgPola; counterPola++;}
			if (par->GetPolb()!=0) {avgPolb = par->GetPolb() + avgPolb; counterPolb++;}
			if (par->GetPolc()!=0) {avgPolc = par->GetPolc() + avgPolc; counterPolc++;}
			if (par->GetPold()!=0) {avgPold = par->GetPold()*1000 + avgPold; counterPold++;}
			if (par->GetPar1za()!=0) {avgPar1za = par->GetPar1za() + avgPar1za; counterPar1za++;}
			if (par->GetPar1zb()!=0) {avgPar1zb = par->GetPar1zb() + avgPar1zb; counterPar1zb++;}
		}
	}
	avgOffset1 = avgOffset1/counterOffset1;
	avgOffset2 = avgOffset2/counterOffset2;
	avgToTOffset1 = avgToTOffset1/counterToTOffset1;
	avgToTOffset2 = avgToTOffset2/counterToTOffset2;
	avgSync = avgSync/counterSync;
	avgVeff = avgVeff/counterVeff;
	avgLambda = avgLambda/counterLambda;
	avgPola = avgPola/counterPola;
	avgPolb = avgPolb/counterPolb;
	avgPolc = avgPolc/counterPolc;
	avgPold = avgPold/counterPold;
	avgPar1za = avgPar1za/counterPar1za;
	avgPar1zb = avgPar1zb/counterPar1zb;

	printf("avgOffset1: %lf, avgOffset2: %lf, avgToTOffset1: %lf, avgToTOffset2: %lf, avgSync: %lf, avgVeff: %lf, avgLambda:%lf, avgPola: %lf, avgPolb: %lf, avgPolc: %lf, avgPold: %lf, avgPar1za: %lf, avgPar1zb: %lf\n",avgOffset1,avgOffset2,avgToTOffset1,avgToTOffset2,avgSync,avgVeff,avgLambda,avgPola,avgPolb,avgPolc,avgPold,avgPar1za,avgPar1zb);

	for (int iPlane = 0; iPlane<4; iPlane++)
	{
		for (int iBar = 12; iBar<32; iBar++)
		{
			R3BTofDHitModulePar* par = parCont3->GetModuleParAt(iPlane+1,iBar+1);
	      		Offset1[iPlane][iBar-12]->Fill(iBar+1,par->GetOffset1());
	      		Offset2[iPlane][iBar-12]->Fill(iBar+1,par->GetOffset2());
			ToTOffset1[iPlane][iBar-12]->Fill(iBar+1,par->GetToTOffset1());
			ToTOffset2[iPlane][iBar-12]->Fill(iBar+1,par->GetToTOffset2());
	      		Sync[iPlane][iBar-12]->Fill(iBar+1,par->GetSync());
	      		Veff[iPlane][iBar-12]->Fill(iBar+1,par->GetVeff());
	      		Lambda[iPlane][iBar-12]->Fill(iBar+1,par->GetLambda());
	      		ParMeander1[iPlane][iBar-12]->Fill(iBar+1,par->GetPola());
	      		ParMeander2[iPlane][iBar-12]->Fill(iBar+1,par->GetPolb());
	      		ParMeander3[iPlane][iBar-12]->Fill(iBar+1,par->GetPolc());
	      		ParMeander4[iPlane][iBar-12]->Fill(iBar+1,par->GetPold());
	      		ParZ1[iPlane][iBar-12]->Fill(iBar+1,par->GetPar1za());
	      		ParZ2[iPlane][iBar-12]->Fill(iBar+1,par->GetPar1zb());
	
			CanOffset->cd(iPlane+1);
			if (counter==0) Offset1[iPlane][iBar-12]->Draw();
			else Offset1[iPlane][iBar-12]->Draw("same");
			Offset2[iPlane][iBar-12]->Draw("same");
			Offset1[iPlane][iBar-12]->SetMarkerStyle(20);
			Offset1[iPlane][iBar-12]->SetStats(0);
			Offset2[iPlane][iBar-12]->SetMarkerStyle(20);
			Offset2[iPlane][iBar-12]->SetMarkerColor(2);
			Offset2[iPlane][iBar-12]->SetStats(0);
			Offset1[iPlane][iBar-12]->SetTitle(Form("Plane %d",iPlane+1));
			Offset1[iPlane][iBar-12]->GetXaxis()->SetTitle("Bar Number");
			Offset1[iPlane][iBar-12]->GetYaxis()->SetTitle("Time Diff Offset (ns)");
			Offset1[iPlane][iBar-12]->GetXaxis()->CenterTitle();
			Offset1[iPlane][iBar-12]->GetYaxis()->CenterTitle();
			CanToTOffset->cd(iPlane+1);
			if (counter==0) ToTOffset1[iPlane][iBar-12]->Draw();
			else ToTOffset1[iPlane][iBar-12]->Draw("same");
			ToTOffset2[iPlane][iBar-12]->Draw("same");
			ToTOffset1[iPlane][iBar-12]->SetMarkerStyle(20);
			ToTOffset1[iPlane][iBar-12]->SetStats(0);
			ToTOffset2[iPlane][iBar-12]->SetMarkerStyle(20);
			ToTOffset2[iPlane][iBar-12]->SetMarkerColor(2);
			ToTOffset2[iPlane][iBar-12]->SetStats(0);
			ToTOffset2[iPlane][iBar-12]->SetTitle(Form("Plane %d",iPlane+1));
			ToTOffset1[iPlane][iBar-12]->GetXaxis()->SetTitle("Bar Number");
			ToTOffset1[iPlane][iBar-12]->GetYaxis()->SetTitle("ToT Offset (ns)");
			ToTOffset1[iPlane][iBar-12]->GetXaxis()->CenterTitle();
			ToTOffset1[iPlane][iBar-12]->GetYaxis()->CenterTitle();
			CanSync->cd(iPlane+1);
			if (counter==0) Sync[iPlane][iBar-12]->Draw();
			else Sync[iPlane][iBar-12]->Draw("same");
			Sync[iPlane][iBar-12]->SetMarkerStyle(20);
			Sync[iPlane][iBar-12]->SetTitle(Form("Plane %d",iPlane+1));
			Sync[iPlane][iBar-12]->SetStats(0);
			Sync[iPlane][iBar-12]->GetXaxis()->SetTitle("Bar Number");
			Sync[iPlane][iBar-12]->GetYaxis()->SetTitle("Sync (ns)");
			Sync[iPlane][iBar-12]->GetXaxis()->CenterTitle();
			Sync[iPlane][iBar-12]->GetYaxis()->CenterTitle();
			CanVeff->cd(iPlane+1);
			if (counter==0) Veff[iPlane][iBar-12]->Draw();
			else Veff[iPlane][iBar-12]->Draw("same");
			Veff[iPlane][iBar-12]->SetMarkerStyle(20);
			Veff[iPlane][iBar-12]->SetStats(0);
			Veff[iPlane][iBar-12]->SetTitle(Form("Plane %d",iPlane+1));
			Veff[iPlane][iBar-12]->GetXaxis()->SetTitle("Bar Number");
			Veff[iPlane][iBar-12]->GetYaxis()->SetTitle("v_eff/2 (cm/ns)");
			Veff[iPlane][iBar-12]->GetXaxis()->CenterTitle();
			Veff[iPlane][iBar-12]->GetYaxis()->CenterTitle();
			CanLambda->cd(iPlane+1);
			if (counter==0) Lambda[iPlane][iBar-12]->Draw();
			else Lambda[iPlane][iBar-12]->Draw("same");
			Lambda[iPlane][iBar-12]->SetMarkerStyle(20);
			Lambda[iPlane][iBar-12]->SetStats(0);
			Lambda[iPlane][iBar-12]->SetTitle(Form("Plane %d",iPlane+1));
			Lambda[iPlane][iBar-12]->GetXaxis()->SetTitle("Bar Number");
			Lambda[iPlane][iBar-12]->GetYaxis()->SetTitle("Lambda (ns)");
			Lambda[iPlane][iBar-12]->GetXaxis()->CenterTitle();
			Lambda[iPlane][iBar-12]->GetYaxis()->CenterTitle();
			CanParMeander1->cd(iPlane+1);
			if (counter==0) ParMeander1[iPlane][iBar-12]->Draw();
			else ParMeander1[iPlane][iBar-12]->Draw("same");
			ParMeander1[iPlane][iBar-12]->SetMarkerStyle(20);
			ParMeander1[iPlane][iBar-12]->SetStats(0);
			ParMeander1[iPlane][iBar-12]->SetTitle(Form("Plane %d",iPlane+1));
			ParMeander1[iPlane][iBar-12]->GetXaxis()->SetTitle("Bar Number");
			ParMeander1[iPlane][iBar-12]->GetYaxis()->SetTitle("Par 1 ToT (ns)");
			ParMeander1[iPlane][iBar-12]->GetXaxis()->CenterTitle();
			ParMeander1[iPlane][iBar-12]->GetYaxis()->CenterTitle();
			CanParMeander2->cd(iPlane+1);
			if (counter==0) ParMeander2[iPlane][iBar-12]->Draw();
			else ParMeander2[iPlane][iBar-12]->Draw("same");
			ParMeander2[iPlane][iBar-12]->SetMarkerStyle(20);
			ParMeander2[iPlane][iBar-12]->SetStats(0);
			ParMeander2[iPlane][iBar-12]->SetTitle(Form("Plane %d",iPlane+1));
			ParMeander2[iPlane][iBar-12]->GetXaxis()->SetTitle("Bar Number");
			ParMeander2[iPlane][iBar-12]->GetYaxis()->SetTitle("Par 2 ToT (ns)");
			ParMeander2[iPlane][iBar-12]->GetXaxis()->CenterTitle();
			ParMeander2[iPlane][iBar-12]->GetYaxis()->CenterTitle();
			CanParMeander3->cd(iPlane+1);
			if (counter==0) ParMeander3[iPlane][iBar-12]->Draw();
			else ParMeander3[iPlane][iBar-12]->Draw("same");
			ParMeander3[iPlane][iBar-12]->SetMarkerStyle(20);
			ParMeander3[iPlane][iBar-12]->SetStats(0);
			ParMeander3[iPlane][iBar-12]->SetTitle(Form("Plane %d",iPlane+1));
			ParMeander3[iPlane][iBar-12]->GetXaxis()->SetTitle("Bar Number");
			ParMeander3[iPlane][iBar-12]->GetYaxis()->SetTitle("Par 3 ToT (ns)");
			ParMeander3[iPlane][iBar-12]->GetXaxis()->CenterTitle();
			ParMeander3[iPlane][iBar-12]->GetYaxis()->CenterTitle();
			CanParMeander4->cd(iPlane+1);
			if (counter==0) ParMeander4[iPlane][iBar-12]->Draw();
			else ParMeander4[iPlane][iBar-12]->Draw("same");
			ParMeander4[iPlane][iBar-12]->SetMarkerStyle(20);
			ParMeander4[iPlane][iBar-12]->SetStats(0);
			ParMeander4[iPlane][iBar-12]->SetTitle(Form("Plane %d",iPlane+1));
			ParMeander4[iPlane][iBar-12]->GetXaxis()->SetTitle("Bar Number");
			ParMeander4[iPlane][iBar-12]->GetYaxis()->SetTitle("Par 4 ToT (ns)");
			ParMeander4[iPlane][iBar-12]->GetXaxis()->CenterTitle();
			ParMeander4[iPlane][iBar-12]->GetYaxis()->CenterTitle();
			CanParZ1->cd(iPlane+1);
			if (counter==0) ParZ1[iPlane][iBar-12]->Draw();
			else ParZ1[iPlane][iBar-12]->Draw("same");
			ParZ1[iPlane][iBar-12]->SetMarkerStyle(20);
			ParZ1[iPlane][iBar-12]->SetStats(0);
			ParZ1[iPlane][iBar-12]->SetTitle(Form("Plane %d",iPlane+1));
			ParZ1[iPlane][iBar-12]->GetXaxis()->SetTitle("Bar Number");
			ParZ1[iPlane][iBar-12]->GetYaxis()->SetTitle("Par 2 (charge number)");
			ParZ1[iPlane][iBar-12]->GetXaxis()->CenterTitle();
			ParZ1[iPlane][iBar-12]->GetYaxis()->CenterTitle();
			CanParZ2->cd(iPlane+1);
			if (counter==0) ParZ2[iPlane][iBar-12]->Draw();
			else ParZ2[iPlane][iBar-12]->Draw("same");
			ParZ2[iPlane][iBar-12]->SetMarkerStyle(20);
			ParZ2[iPlane][iBar-12]->SetStats(0);
			ParZ2[iPlane][iBar-12]->SetTitle(Form("Plane %d",iPlane+1));
			ParZ2[iPlane][iBar-12]->GetXaxis()->SetTitle("Bar Number");
			ParZ2[iPlane][iBar-12]->GetYaxis()->SetTitle("Par 2 (charge number)");
			ParZ2[iPlane][iBar-12]->GetXaxis()->CenterTitle();
			ParZ2[iPlane][iBar-12]->GetYaxis()->CenterTitle();
			counter++;
		}
	}
	CanOffset->SaveAs(".png");
	CanToTOffset->SaveAs(".png");
	CanSync->SaveAs(".png");
	CanVeff->SaveAs(".png");
	CanLambda->SaveAs(".png");
	CanParMeander1->SaveAs(".png");
	CanParMeander2->SaveAs(".png");
	CanParMeander3->SaveAs(".png");
	CanParMeander4->SaveAs(".png");
	CanParZ1->SaveAs(".png");
	CanParZ2->SaveAs(".png");
	counter=0;
	return;
}
