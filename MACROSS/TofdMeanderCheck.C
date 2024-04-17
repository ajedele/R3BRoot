//
// Macro to modify the TOFD parameters by hand 
//
#include "TH2.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TAttPad.h"

TH2D *Meander[4][25];

TCanvas *CanMeander[25];

TString nom;

void DrawCanvas()
{
	for (int iBar=12; iBar<32; iBar++)
	{
		nom = Form("Meander_Bar%d",iBar+1);
		CanMeander[iBar-12] = new TCanvas(nom.Data(),nom.Data(),800,800);
		gPad->SetLeftMargin(0.15);
		gPad->SetRightMargin(0.15);
		gPad->SetBottomMargin(0.15);
		CanMeander[iBar-12]->Divide(2,2,0.01,0.01);
	}
	return;
}

void TofdMeanderCheck(int fRunID)
{
	DrawCanvas();
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
	parIo1->open("/u/ajedele/R3BRoot_fork/MACROSS/parameter/tofd_HitPar_s473_meander197.root", "in");
	rtdb->setFirstInput(parIo1);
	
	auto *parCont3 = (R3BTofDHitPar*)(rtdb->getContainer("tofdHitPar"));
	rtdb->initContainers(fRunID);
	
	TString path = Form("/lustre/land/ajedele/s473/rootfiles/TOFD");
	TFile *file1 = new TFile(Form("%s/TofdHitPar%04d_Histo.root",path.Data(),fRunID));

	// Here you can change the value of the parameters, example for plane 4 and paddle 44
	int counter = 0;
	TString histName;
	for (int iBar = 12; iBar<32; iBar++)
	{
		for (int iPlane = 0; iPlane<4; iPlane++)
		{
			R3BTofDHitModulePar* par = parCont3->GetModuleParAt(iPlane+1,iBar+1);
	      		TGraphErrors *tg1 = new TGraphErrors();
			for (int pos= -40; pos <= 40; pos++)
			{
				double ToT = par->GetPola() + par->GetPolb()*(double)pos + par->GetPolc()*(double)pos*(double)pos + par->GetPold()*(double)pos*(double)pos*(double)pos;
				tg1->SetPoint(counter, pos, ToT);
				counter++;
			}
			counter = 0;
			
			histName = Form("SqrtQ_vs_PosToT_Plane_%d_Bar_%d",iPlane, iBar);
			printf("Name of hist: %s\n",histName.Data());	
			Meander[iPlane][iBar-12] = (TH2D*)(file1->FindObjectAny(histName));

			CanMeander[iBar-12]->cd(iPlane+1);
			Meander[iPlane][iBar-12]->Draw("colz");
			Meander[iPlane][iBar-12]->SetStats(0);
			Meander[iPlane][iBar-12]->SetTitle(Form("Plane %d",iPlane+1));
			Meander[iPlane][iBar-12]->GetXaxis()->SetTitle("PosToT");
			Meander[iPlane][iBar-12]->GetYaxis()->SetTitle("ToT (ns)");
			Meander[iPlane][iBar-12]->GetXaxis()->CenterTitle();
			Meander[iPlane][iBar-12]->GetYaxis()->CenterTitle();
			Meander[iPlane][iBar-12]->GetXaxis()->SetRangeUser(-40,40);
			Meander[iPlane][iBar-12]->GetYaxis()->SetRangeUser(100,160);
			gPad->SetLogz();
			tg1->Draw("L");
		}
		//CanMeander[iBar-12]->SaveAs(".png");
	}
	counter=0;
	return;
}
