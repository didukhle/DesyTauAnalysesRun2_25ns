///----------------------Version 1.0-------------------------//
///Plotting Macro for E -> Tau Fake Rates study
///Author: Yiwen Wen
///DESY
///-----------------------------------------------------------//
#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/format.hpp"
#include "boost/program_options.hpp"
#include "boost/range/algorithm.hpp"
#include "boost/range/algorithm_ext.hpp"
#include "DesyTauAnalyses/NTupleMaker/test/Plotting.h"
#include "DesyTauAnalyses/NTupleMaker/test/Plotting_Style.h"
#include "DesyTauAnalyses/NTupleMaker/test/HttStylesNew.cc"
#include "TFile.h"
#include "TH1.h"
#include "TROOT.h"
#include "TColor.h"
#include "TEfficiency.h"
#include "TMath.h"
void PlotShapes(
                                 TString varName= "m_vis",
                                 TString xtitle = "m_{vis} [GeV]",
                                 TString ytitle = "dN/dm_{vis}[1/GeV]",
                                 TString eta = "Lt1p460",
                                 TString wp = "VLoose",
                                 float xmin = 60,
                                 float xmax = 120,
                                 int numberofbins = 12,
                                 bool posfit = false,
                                 bool passProbe = true,
                                 bool logY = false,
                                 bool legLeft = false
                                 )
{
  //SetStyle();
        float lumi= 59970;

	//	float lumi = 41860;
	//Deal with bins and binning
	float xMin = xmin;
	float xMax = xmax;
	int nBins = numberofbins;
	float bins[100];
	float binWidth = (xMax-xMin)/float(nBins);
        for (int iB=0; iB<=nBins; ++iB)
        	bins[iB] = xMin + float(iB)*binWidth;
    
    TString suffix;

    if(posfit)
        suffix = "_postfit";
    else
        suffix = "_prefit";
    
    TString suffixPassOrFail;
    
    if(passProbe)
        suffixPassOrFail = "_pass";
    else
        suffixPassOrFail = "_fail";
    


    TFile * file = new TFile("./ETauFR"+wp+eta+"_PostFitShape.root");
    TH1D * data_obs = (TH1D*)file->Get("/ETauFR"+suffixPassOrFail+suffix+"/data_obs");
    TH1D * ZEE = (TH1D*)file->Get("/ETauFR"+suffixPassOrFail+suffix+"/ZEE");
    TH1D * ZJ = (TH1D*)file->Get("/ETauFR"+suffixPassOrFail+suffix+"/ZJ");
    //if(wp !="VTight" && eta!="Gt1p558")
    TH1D * ZTT_el = (TH1D*)file->Get("/ETauFR"+suffixPassOrFail+suffix+"/ZTT_el");
    
    TH1D * ZTT_et = (TH1D*)file->Get("/ETauFR"+suffixPassOrFail+suffix+"/ZTT_et");

    TH1D * TT = (TH1D*)file->Get("/ETauFR"+suffixPassOrFail+suffix+"/TT");
    TH1D * VV = (TH1D*)file->Get("/ETauFR"+suffixPassOrFail+suffix+"/VV");
    TH1D * W = (TH1D*)file->Get("/ETauFR"+suffixPassOrFail+suffix+"/W");
    TH1D * QCD = (TH1D*)file->Get("/ETauFR"+suffixPassOrFail+suffix+"/QCD");
    //if(eta =="0p8to1p2" && !passProbe) TH1D * W = (TH1D*)VV->Clone("W");//brute force no W in this region
	TH1D * dummy = (TH1D*)ZEE->Clone("dummy");

    float errLumi = 0.062;
    float errQCD = 0.3;
    float errDY=0.03;
    float errVV = 0.1;
    float errW = 0.2;
    float errTT = 0.1;
    float errTauID = 0.03;
    float errEleID = 0.05;
    
    for (int iB=1; iB<=nBins; ++iB)
    {
      float eQCD = 0;
      if(QCD)errQCD*(QCD->GetBinContent(iB));
      float eVV = errVV*(VV->GetBinContent(iB));
      float eDYEE = errDY*2*(ZEE->GetBinContent(iB));
      float eDYZJ = errDY*(ZJ->GetBinContent(iB));
        //float eDYZTT_el = errDY*ZTT_el->GetBinContent(iB);
      float eDYZTT_et = errDY*(ZTT_et->GetBinContent(iB));

        float eW;
        //if(!passProbe && eta=="0p8to1p2") eW = 0;
        eW = errW*(W->GetBinContent(iB));
        float eTT = errTT*(TT->GetBinContent(iB));
        float err2 = eQCD*eQCD + eVV*eVV + eW*eW + eTT*eTT+eDYEE*eDYEE+eDYZJ*eDYZJ+eDYZTT_et*eDYZTT_et;
        float errTot = TMath::Sqrt(err2);
        dummy->SetBinError(iB,errTot);
    }
    float numberZEE = ZEE->GetSumOfWeights();
    
    W->Add(W,QCD);
    TT->Add(TT,W);
	VV->Add(VV,TT);
	ZTT_et->Add(ZTT_et,VV);
    if(wp !="VTight" && eta!="Gt1p558")
    {
        ZTT_el->Add(ZTT_el,ZTT_et);
        ZJ->Add(ZJ,ZTT_el);
    }
    else
        ZJ->Add(ZJ,ZTT_et);
	ZEE->Add(ZEE,ZJ);
    
    float totData = data_obs->GetSumOfWeights();
    float totMC = ZEE->GetSumOfWeights();
	
	TH1D * bkgdErr = (TH1D*)ZEE->Clone("bkgdErr");
  	bkgdErr->SetFillStyle(3013);
  	bkgdErr->SetFillColor(1);
  	bkgdErr->SetMarkerStyle(21);
  	bkgdErr->SetMarkerSize(0);
    
  	for (int iB=1; iB<=nBins; ++iB)
    {  
        W->SetBinError(iB,0);
        if(QCD)QCD->SetBinError(iB,0);
        VV->SetBinError(iB,0);
        TT->SetBinError(iB,0);
        ZTT_et->SetBinError(iB,0);

        ZJ->SetBinError(iB,0);
        ZEE->SetBinError(iB,0);
        float eStat =  bkgdErr->GetBinError(iB);
        float X = bkgdErr->GetBinContent(iB);
        float eLumi = errLumi * X;
        float eBkg = dummy->GetBinError(iB);
	float Err=0; 
	if(posfit)
        {
        Err = TMath::Sqrt(eStat*eStat);
        }
        else
        {
        Err = TMath::Sqrt(eLumi*eLumi+eBkg*eBkg+errEleID*errEleID+errTauID*errTauID+eStat*eStat);
        //float Err = TMath::Sqrt(eStat*eStat);
        }
        //float normalErr = TMath::Sqrt(eLumi*eLumi+eBkg*eBkg+errEleID*errEleID);
        //errZEE = normalErr+errZEE;//for calcluating Fake rate error
        bkgdErr->SetBinError(iB,Err);
    }
    
   	//Colors
 	Int_t colorZEE = TColor::GetColor("#ffcc66");
	Int_t colorZJ = TColor::GetColor("#4496C8");
	Int_t colorTT = TColor::GetColor("#9999CC");
	Int_t colorVV = TColor::GetColor("#6F2D35");
	Int_t colorW = TColor::GetColor("#DE5A6A");
	Int_t colorQCD = TColor::GetColor("#FFCCFF");

	InitData(data_obs);
	if(QCD)InitHist(QCD,TColor::GetColor("#FFCCFF"));
	InitHist(ZEE,TColor::GetColor("#DE5A6A"));
	InitHist(TT,TColor::GetColor("#9999CC"));
	InitHist(VV,TColor::GetColor("#6F2D35"));
	InitHist(ZJ,TColor::GetColor("#FFCC66"));
	InitHist(W,TColor::GetColor("#4496C8"));
	data_obs->GetXaxis()->SetTitle(xtitle);
 	data_obs->GetYaxis()->SetTitle(ytitle);
	data_obs->GetYaxis()->SetTitleOffset(1.5);
	data_obs->GetYaxis()->SetTitleSize(0.06);
	data_obs->GetXaxis()->SetRangeUser(xmin,xmax);
	float ymax = data_obs->GetMaximum();
	if (logY)
	data_obs->GetYaxis()->SetRangeUser(0.5,2*ymax);
	else
	data_obs->GetYaxis()->SetRangeUser(0,1.3*ymax);
	data_obs->SetMarkerSize(1.5);
	data_obs->GetXaxis()->SetLabelSize(0);
	data_obs->GetYaxis()->SetLabelSize(0.06);
	
	TCanvas * canv1 = new TCanvas("canv1", "", 1000, 800);
	
	TPad *upper = new TPad("upper", "pad",0,0.31,1,1);
	upper->Draw();
	upper->cd();
	upper->SetFillColor(0);
	upper->SetBorderMode(0);
	upper->SetBorderSize(10);
	upper->SetTickx(1);
	upper->SetTicky(1);
	upper->SetLeftMargin(0.17);
	upper->SetRightMargin(0.05);
	upper->SetBottomMargin(0.02);
 	upper->SetFrameFillStyle(0);
	upper->SetFrameLineStyle(0);
  	upper->SetFrameLineWidth(2);
  	upper->SetFrameBorderMode(0);
  	upper->SetFrameBorderSize(10);
  	upper->SetFrameFillStyle(0);
  	upper->SetFrameLineStyle(0);
  	upper->SetFrameLineWidth(2);
  	upper->SetFrameBorderMode(0);
  	upper->SetFrameBorderSize(10);

	//Drawing histogram
  	data_obs->Draw("e1");
	ZEE->Draw("sameh");
  	ZJ->Draw("sameh");
  	VV->Draw("sameh");
  	TT->Draw("sameh");
	W->Draw("sameh");
	if(QCD)QCD->Draw("sameh");
  	data_obs->Draw("e1same");
	bkgdErr->Draw("e2same");
	//Calculating chi2
	float chi2 = 0;
	for (int iB=1; iB<=nBins; ++iB) 
	{
		float xData = data_obs->GetBinContent(iB);
		float xMC = ZEE->GetBinContent(iB);
		if (xMC>1e-1) 
		{
      			float diff2 = (xData-xMC)*(xData-xMC);
      			chi2 += diff2/xMC;
    		}
  	}
  	std::cout << std::endl;
  	std::cout << "Chi2 = " << chi2 << std::endl;
  	std::cout << std::endl;

  	float x1Leg = 0.70;
  	float x2Leg = 0.95;
  	if (legLeft) 
	{
    		x1Leg = 0.20;
    		x2Leg = 0.45;
  	}
	TLegend * leg = new TLegend(x1Leg,0.6,x2Leg,0.88);
  	SetLegendStyle(leg);
  	leg->SetTextSize(0.05);
  	leg->AddEntry(data_obs,"Data","lp");
  	leg->AddEntry(VV,"Dibosons","f");
  	leg->AddEntry(W,"WJets","f");
 	leg->AddEntry(QCD,"QCD","f");
  	leg->AddEntry(TT,"t#bar{t}","f");
	leg->AddEntry(ZJ,"DY others","f");
  	leg->AddEntry(ZEE,"Z#rightarrow ee","f");
  	leg->Draw();
  	//plotchannel("e#mu");
    //	if (!applyPU) suffix = "_noPU";

    TLatex * cms = new TLatex(0.50,0.94,"L = 59.9 fb^{-1} at #sqrt{s} = 13 TeV");

  	cms->SetNDC();
  	cms->SetTextSize(0.05);
  	cms->Draw();
    
    TLatex * cmsLogo = new TLatex(0.20,0.85,"CMS");
    cmsLogo->SetNDC();
    cmsLogo->SetTextSize(0.05);
    cmsLogo->SetTextFont(61);
    cmsLogo->Draw();
    
    TLatex * workinprogress = new TLatex(0.20,0.80,"Work in progress");
    workinprogress->SetNDC();
    workinprogress->SetTextSize(0.05);
    workinprogress->SetTextFont(52);
    workinprogress->Draw();
    
    
  	if (logY) upper->SetLogy(true);

 	upper->Draw("SAME");
  	upper->RedrawAxis();
  	upper->Modified();
  	upper->Update();
 	canv1->cd();
	
	TH1D * ratioH = (TH1D*)data_obs->Clone("ratioH");
	TH1D * ratioErrH = (TH1D*)bkgdErr->Clone("ratioErrH");
  	ratioH->SetMarkerColor(1);
 	ratioH->SetMarkerStyle(20);
  	ratioH->SetMarkerSize(1.5);
  	ratioH->SetLineColor(1);
  	ratioH->GetYaxis()->SetRangeUser(0.3,1.8);
 	ratioH->GetYaxis()->SetNdivisions(505);
  	ratioH->GetXaxis()->SetLabelFont(42);
  	ratioH->GetXaxis()->SetLabelOffset(0.04);
  	ratioH->GetXaxis()->SetLabelSize(0.14);
  	ratioH->GetXaxis()->SetTitleSize(0.13);
  	ratioH->GetXaxis()->SetTitleOffset(1.2);
  	ratioH->GetYaxis()->SetTitle("obs/exp");
 	ratioH->GetYaxis()->SetLabelFont(42);
  	ratioH->GetYaxis()->SetLabelOffset(0.015);
  	ratioH->GetYaxis()->SetLabelSize(0.13);
  	ratioH->GetYaxis()->SetTitleSize(0.14);
  	ratioH->GetYaxis()->SetTitleOffset(0.5);
  	ratioH->GetXaxis()->SetTickLength(0.07);
  	ratioH->GetYaxis()->SetTickLength(0.04);
  	ratioH->GetYaxis()->SetLabelOffset(0.01);
	
	for (int iB=1; iB<=nBins; ++iB) 
	{
    		float x1 = data_obs->GetBinContent(iB);
    		float x2 = ZEE->GetBinContent(iB);
    		ratioErrH->SetBinContent(iB,1.0);
    		ratioErrH->SetBinError(iB,0.0);
		float xBkg = bkgdErr->GetBinContent(iB);
   		float errBkg = bkgdErr->GetBinError(iB);
		if (xBkg>0) 
		{
     			float relErr = errBkg/xBkg;
      			ratioErrH->SetBinError(iB,relErr);
    		}
		if (x1>0&&x2>0) 
		{
      			float e1 = data_obs->GetBinError(iB);
      			float ratio = x1/x2;
      			float eratio = e1/x2;
      			ratioH->SetBinContent(iB,ratio);
     			ratioH->SetBinError(iB,eratio);
    		}
   		else 
		{
      			ratioH->SetBinContent(iB,1000);
    		}
  	}


	TPad *lower = new TPad("lower", "pad",0,0,1,0.30);
  	lower->Draw();
  	lower->cd();
  	lower->SetFillColor(0);
  	lower->SetBorderMode(0);
  	lower->SetBorderSize(10);
 	lower->SetGridy();
  	lower->SetTickx(1);
  	lower->SetTicky(1);
  	lower->SetLeftMargin(0.17);
 	lower->SetRightMargin(0.05);
  	lower->SetTopMargin(0.026);
  	lower->SetBottomMargin(0.35);
  	lower->SetFrameFillStyle(0);
  	lower->SetFrameLineStyle(0);
  	lower->SetFrameLineWidth(2);
  	lower->SetFrameBorderMode(0);
  	lower->SetFrameBorderSize(10);
  	lower->SetFrameFillStyle(0);
  	lower->SetFrameLineStyle(0);
  	lower->SetFrameLineWidth(2);
  	lower->SetFrameBorderMode(0);
  	lower->SetFrameBorderSize(10);

  	ratioH->Draw("e1");
	ratioErrH->Draw("e2same");
  	lower->Modified();
  	lower->RedrawAxis();
  	canv1->cd();
  	canv1->Modified();
  	canv1->cd();
  	canv1->SetSelected(canv1);
	canv1->Print("./Plots/ETauFR"+suffixPassOrFail+eta+wp+suffix+".png");
	canv1->Print("./Plots/ETauFR"+suffixPassOrFail+eta+wp+suffix+".pdf","Portrait pdf");
	canv1->Print("macrosOfPlots/ETauFR"+suffixPassOrFail+eta+wp+suffix+".C","cxx");
	canv1->Print("rootFilesOfPlots/ETauFR"+suffixPassOrFail+eta+wp+suffix+".root","root");


}

