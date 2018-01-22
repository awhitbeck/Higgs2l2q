#include "TString.h"
#include "TChain.h"
#include "TH1F.h"
#include "TROOT.h"
#include "THStack.h"
#include "TPad.h"

#include <vector>
#include <map>
#include <iostream>

#include "plotterUtils.cc"
#include "skimSamples.cc"
#include "definitions.cc"
#include "h2l2qTree.cc"

using namespace std;

int main(int argc, char** argv){

    int MAX_EVENTS = 99999999;
    if( argc > 1 )
        MAX_EVENTS = atoi(argv[1]);

    gROOT->ProcessLine(".L tdrstyle.C");
    gROOT->ProcessLine("setTDRStyle()");
  
    skimSamples skims;
    typedef plot<h2l2qTree> plot;

    plot NJetsplot(*fillNJets<h2l2qTree>,"NJets_ZH","n_{j}",14,1.5,15.5);
    plot BTagsplot(*fillBJets<h2l2qTree>,"BTags_ZH","n_{b-jets}",7,-0.5,6.5);
    plot MaxMjjplot(*fill_max_mjj<h2l2qTree>,"MaxMjj_ZH","m_{jj}",45,60,150);
    plot MinMllplot(*fill_min_mll<h2l2qTree>,"MinMll_ZH","m_{ll} [GeV]",48,12,60);
    plot MaxMllplot(*fill_max_mll<h2l2qTree>,"MaxMll_ZH","m_{ll} [GeV]",45,60,150);
    plot Mlljjplot(*fill_ZH_mlljj<h2l2qTree>,"Mlljj_ZH","m_{lljj}",50,0,300);

    vector<plot> plots;
    plots.push_back(NJetsplot);
    plots.push_back(BTagsplot);
    plots.push_back(MaxMjjplot);
    plots.push_back(MinMllplot);
    plots.push_back(MaxMllplot);
    plots.push_back(Mlljjplot);

    // background MC samples
    for( int iSample = 0 ; iSample < skims.ntuples.size() ; iSample++){

        h2l2qTree* ntuple = skims.ntuples[iSample];
        
        for( int iPlot = 0 ; iPlot < plots.size() ; iPlot++){
            plots[iPlot].addNtuple(ntuple,skims.sampleName[iSample]);
            plots[iPlot].setFillColor(ntuple,skims.fillColor[iSample]);
            plots[iPlot].setLineColor(ntuple,skims.lineColor[iSample]);
        }

        int numEvents = ntuple->fChain->GetEntries();
        ntupleBranchStatus<h2l2qTree>(ntuple);
        TString filename;
        double weight = 0.;
        int maxEvents;
        int maxTopEvents = 5000000;
        if( skims.sampleName[iSample] == "TTJets" ) maxEvents = min(maxTopEvents,numEvents);
        else maxEvents = min(MAX_EVENTS,numEvents);
        for( int iEvt = 0 ; iEvt < maxEvents ; iEvt++ ){
            ntuple->GetEntry(iEvt);
            weight = ntuple->PUWeight*ntuple->xsec*ntuple->genHEPMCweight*1000.*35.9/skims.originalWeight[iSample];
            if( skims.sampleName[iSample] == "TTJets" ) weight*=14616204./maxTopEvents;
            if( iEvt % 1000000 == 0 ) cout << skims.sampleName[iSample] << ": " << iEvt << "/" << numEvents << endl;
            if(! ZHCut(ntuple) ) continue;
            for( int iPlot = 0 ; iPlot < plots.size() ; iPlot++ ){
                plots[iPlot].fill(ntuple,weight);
            }
        }
    }

    // Signal samples
    vector<h2l2qTree*> sigSamples;
    for( int iSample = 0 ; iSample < skims.signalNtuples.size() ; iSample++){

        h2l2qTree* ntuple = skims.signalNtuples[iSample];
        sigSamples.push_back(ntuple);
        for( int iPlot = 0 ; iPlot < plots.size() ; iPlot++){
            plots[iPlot].addSignalNtuple(ntuple,skims.signalSampleName[iSample]);
            plots[iPlot].setLineColor(ntuple,skims.sigLineColor[iSample]);
        }

        int numEvents = ntuple->fChain->GetEntries();
        ntupleBranchStatus<h2l2qTree>(ntuple);
        for( int iEvt = 0 ; iEvt < min(MAX_EVENTS,numEvents) ; iEvt++ ){
            ntuple->GetEntry(iEvt);
            if( iEvt % 1000000 == 0 ) cout << skims.signalSampleName[iSample] << ": " << iEvt << "/" << numEvents << endl;
            if(! ZHCut(ntuple) ) continue;
            for( int iPlot = 0 ; iPlot < plots.size() ; iPlot++){
                if( skims.signalSampleName[iSample] == "VBF125" )
                    plots[iPlot].fillSignal(ntuple,lumi*3.748*0.023*.18/47893.);
                else if( skims.signalSampleName[iSample] == "ZH125" )
                    plots[iPlot].fillSignal(ntuple,lumi*0.8696*0.023*.18*0.25/47893.);                else if( skims.signalSampleName[iSample] == "ttH125" )
                    plots[iPlot].fillSignal(ntuple,lumi*0.5085*0.023*0.25*0.18/47893.);
                else
                    cout << "oops, I don't know how to weight your signal events..." << endl;
            }
        }
    }

    TFile* outputFile = new TFile("plotObs_ZH.root","RECREATE");

    for( int iPlot = 0 ; iPlot < plots.size() ; iPlot++){
        TCanvas* can = new TCanvas("can","can",500,500);
        plots[iPlot].dataHist = NULL;
        plots[iPlot].DrawNoRatio(can,skims.ntuples,sigSamples);
        plots[iPlot].Write();
        plots[iPlot].sum->Write();
    }
    
    outputFile->Close();
}
