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

    plot NJetsplot(*fillNJets<h2l2qTree>,"NJets_VBF","n_{j}",14,1.5,15.5);
    plot BTagsplot(*fillBJets<h2l2qTree>,"BTags_VBF","n_{b-jets}",7,-0.5,6.5);
    plot BestMjjplot(*fill_best_mjj<h2l2qTree>,"BestMjj_VBF","m_{jj}",45,60,150);
    plot MaxMjjplot(*fill_max_mjj<h2l2qTree>,"MaxMjj_VBF","m_{jj}",60,0,5000);
    plot MaxMassDetaplot(*fill_max_deta<h2l2qTree>,"MaxMassDeta_VBF","#Delta#eta",40,0,10);
    plot MaxMllplot(*fill_max_mll<h2l2qTree>,"MaxMll_VBF","m_{ll}",48,12,60);
    plot BestMlljjplot(*fill_best_mlljj<h2l2qTree>,"BestMlljj_VBF","m_{lljj}",40,90,250);

    vector<plot> plots;
    plots.push_back(NJetsplot);
    plots.push_back(BTagsplot);
    plots.push_back(BestMjjplot);
    plots.push_back(MaxMjjplot);
    plots.push_back(MaxMllplot);
    plots.push_back(MaxMassDetaplot);
    plots.push_back(BestMlljjplot);

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
        //if( skims.sampleName[iSample] == "TTJets" ) 
        //    maxEvents = min(1000000,numEvents);
        //else 
        maxEvents = min(MAX_EVENTS,numEvents);
        for( int iEvt = 0 ; iEvt < maxEvents ; iEvt++ ){
            ntuple->GetEntry(iEvt);
            weight = ntuple->PUWeight*ntuple->xsec*ntuple->genHEPMCweight*1000*35.9/skims.originalWeight[iSample];
            //if( skims.sampleName[iSample] == "TTJets" ) weight*=14.;
            if( iEvt % 1000000 == 0 ) cout << skims.sampleName[iSample] << ": " << iEvt << "/" << numEvents << endl;
            if(! VBFCut(ntuple) ) continue;
            for( int iPlot = 0 ; iPlot < plots.size() ; iPlot++ ){
                plots[iPlot].fill(ntuple,weight);
            }
        }// end loop over events
    }// end loop over samples

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
            if(! VBFCut(ntuple) ) continue;
            for( int iPlot = 0 ; iPlot < plots.size() ; iPlot++){
                plots[iPlot].fillSignal(ntuple,lumi*3.748*0.023*.18/47893.);
            }
        }
    }

    TFile* outputFile = new TFile("plotObs_VBF.root","RECREATE");

    for( int iPlot = 0 ; iPlot < plots.size() ; iPlot++){
        TCanvas* can = new TCanvas("can","can",500,500);
        plots[iPlot].dataHist = NULL;
        plots[iPlot].DrawNoRatio(can,skims.ntuples,sigSamples);
        plots[iPlot].Write();
        plots[iPlot].sum->Write();
    }
    
    outputFile->Close();
}
