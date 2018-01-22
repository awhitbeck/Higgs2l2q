// ROOT/custom libraries
#include "TChain.h"
#include "h2l2qTree.cc"
#include "TString.h"

// STL libraries
#include <iostream>
#include <vector>

static const TString BASE_DIR="root://cmseos.fnal.gov//store/user/awhitbe1/Higgs2l2q/skims2/LepJetBkg2/";
static const TString TREE_NAME="ZZTree/candTree";
class skimSamples{

public : 

    TChain *DY10Jets,*DY50Jets,*TTJets,*WZJets,*ZZJets;
    TChain *VBF125,*ZH125,*WpH125,*WmH125,*ttH125;
    TChain *data;
    std::vector<h2l2qTree*> ntuples,signalNtuples;
    h2l2qTree* dataNtuple;
    std::vector<TString> sampleName, signalSampleName;
    std::vector<TString> dataSampleName; 
    std::vector<int> fillColor, lineColor, sigLineColor;
    std::vector<double> originalWeight;
    std::vector<double> sigOriginalWeight;

    enum region {kSignal,kNumRegions};
    TString regionNames[kNumRegions]={"signal"};

    TString skimType;

    skimSamples(region r=kSignal){

        skimType="";

        if( r == kSignal ){
            skimType=BASE_DIR+"/";
        }

        ///////////////////////////////////////////////////////////////////////
        // - - - - - - - - - - BACKGROUND INPUTS - - - - - - - - - - - - - - //
        ///////////////////////////////////////////////////////////////////////

        std::vector<TString> TTJetsFileNames;
        TTJetsFileNames.push_back("TTincl/ZZ2l2qAnalysis.root");
        TTJets = new TChain(TREE_NAME);
        for( unsigned int i = 0 ; i < TTJetsFileNames.size() ; i++ ){
            TTJets->Add(skimType+"/"+TTJetsFileNames[i]);
        }
        if( r == kSignal ){
            ntuples.push_back(new h2l2qTree(TTJets));
            sampleName.push_back("TTJets");
            fillColor.push_back(kMagenta);
            lineColor.push_back(1);
            TFile* tempFile = new TFile("/eos/uscms/store/user/awhitbe1/Higgs2l2q/skims/"+TTJetsFileNames[0]);
            originalWeight.push_back(((TH1F*)tempFile->Get("ZZTree/Counters"))->GetBinContent(40));
            tempFile->Close();
        }

        std::vector<TString> DY10FileNames;
        DY10FileNames.push_back("DYJets_10to50/ZZ2l2qAnalysis.root");
        DY10Jets = new TChain(TREE_NAME);
        for( unsigned int i = 0 ; i < DY10FileNames.size() ; i++ ){
            DY10Jets->Add(skimType+"/"+DY10FileNames[i]);
        }
        if( r == kSignal ){
            ntuples.push_back(new h2l2qTree(DY10Jets));
            sampleName.push_back("DY10");
            fillColor.push_back(kGreen);
            lineColor.push_back(kGreen);
            TFile* tempFile = new TFile("/eos/uscms/store/user/awhitbe1/Higgs2l2q/skims/"+DY10FileNames[0]);
            originalWeight.push_back(((TH1F*)tempFile->Get("ZZTree/Counters"))->GetBinContent(40));
            tempFile->Close();
        }

        std::vector<TString> DY50FileNames;
        DY50FileNames.push_back("DYJets_50/ZZ2l2qAnalysis.root");
        DY50Jets = new TChain(TREE_NAME);
        for( unsigned int i = 0 ; i < DY50FileNames.size() ; i++ ){
            DY50Jets->Add(skimType+"/"+DY50FileNames[i]);
        }
        if( r == kSignal ){
            ntuples.push_back(new h2l2qTree(DY50Jets));
            sampleName.push_back("DY50");
            fillColor.push_back(kGreen);
            lineColor.push_back(1);
            TFile* tempFile = new TFile("/eos/uscms/store/user/awhitbe1/Higgs2l2q/skims/"+DY50FileNames[0]);
            originalWeight.push_back(((TH1F*)tempFile->Get("ZZTree/Counters"))->GetBinContent(40));
            tempFile->Close();
        }

        std::vector<TString> WZJetsFileNames;
        WZJetsFileNames.push_back("WZDib/ZZ2l2qAnalysis.root");
        WZJets = new TChain(TREE_NAME);
        for( unsigned int i = 0 ; i < WZJetsFileNames.size() ; i++ ){
            WZJets->Add(skimType+"/"+WZJetsFileNames[i]);
        }
        if( r == kSignal ){
            ntuples.push_back(new h2l2qTree(WZJets));
            sampleName.push_back("WZJets");
            fillColor.push_back(kCyan);
            lineColor.push_back(1);
            TFile* tempFile = new TFile("/eos/uscms/store/user/awhitbe1/Higgs2l2q/skims/"+WZJetsFileNames[0]);
            originalWeight.push_back(((TH1F*)tempFile->Get("ZZTree/Counters"))->GetBinContent(40));
            tempFile->Close();
        }

        std::vector<TString> ZZJetsFileNames;
        ZZJetsFileNames.push_back("ZZ/ZZ2l2qAnalysis.root");
        ZZJets = new TChain(TREE_NAME);
        for( unsigned int i = 0 ; i < ZZJetsFileNames.size() ; i++ ){
            ZZJets->Add(skimType+"/"+ZZJetsFileNames[i]);
        }
        if( r == kSignal ){
            ntuples.push_back(new h2l2qTree(ZZJets));
            sampleName.push_back("ZZJets");
            fillColor.push_back(kBlue);
            lineColor.push_back(1);
            TFile* tempFile = new TFile("/eos/uscms/store/user/awhitbe1/Higgs2l2q/skims/"+ZZJetsFileNames[0]);
            originalWeight.push_back(((TH1F*)tempFile->Get("ZZTree/Counters"))->GetBinContent(40));
            tempFile->Close();
        }


        ////////////////////////////////////////////////////////////
        // - - - - - - - - - - - DATA INPUTS - - - - - - - - - -  //
        ////////////////////////////////////////////////////////////

        std::vector<TString> DataFileNames;
        DataFileNames.push_back("");
        if( r == kSignal ){
            data = new TChain(TREE_NAME);
            for( unsigned int i = 0 ; i < DataFileNames.size() ; i++ ){
                data->Add(skimType+"/"+DataFileNames[i]);
            }    
            dataNtuple = new h2l2qTree(data);
            sampleName.push_back("data"); 
            fillColor.push_back(kWhite);
            lineColor.push_back(1);
        }

        std::vector<TString> VBF125FilesNames;
        VBF125FilesNames.push_back("VBF125/ZZ2l2qAnalysis.root");
        if( r == kSignal ){
            VBF125 = new TChain(TREE_NAME);
            for( unsigned int i = 0 ; i < VBF125FilesNames.size() ; i++ ){
                VBF125->Add(BASE_DIR+VBF125FilesNames[i]);
            }
            signalNtuples.push_back(new h2l2qTree(VBF125));
            signalSampleName.push_back("VBF125");
            sigLineColor.push_back(kRed);
            TFile* tempFile = new TFile("/eos/uscms/store/user/awhitbe1/Higgs2l2q/skims/"+VBF125FilesNames[0]);
            sigOriginalWeight.push_back(((TH1F*)tempFile->Get("ZZTree/Counters"))->GetBinContent(40));
            tempFile->Close();
            
        }

        std::vector<TString> ZH125FilesNames;
        ZH125FilesNames.push_back("ZH125/ZZ2l2qAnalysis.root");
        if( r == kSignal ){
            ZH125 = new TChain(TREE_NAME);
            for( unsigned int i = 0 ; i < ZH125FilesNames.size() ; i++ ){
                ZH125->Add(BASE_DIR+ZH125FilesNames[i]);
            }
            signalNtuples.push_back(new h2l2qTree(ZH125));
            signalSampleName.push_back("ZH125");
            sigLineColor.push_back(kRed+2);
            TFile* tempFile = new TFile("/eos/uscms/store/user/awhitbe1/Higgs2l2q/skims/"+ZH125FilesNames[0]);
            sigOriginalWeight.push_back(((TH1F*)tempFile->Get("ZZTree/Counters"))->GetBinContent(40));
            tempFile->Close();

        }

        std::vector<TString> ttH125FilesNames;
        ttH125FilesNames.push_back("ttH125/ZZ2l2qAnalysis.root");
        if( r == kSignal ){
            ttH125 = new TChain(TREE_NAME);
            for( unsigned int i = 0 ; i < ttH125FilesNames.size() ; i++ ){
                ttH125->Add(BASE_DIR+ttH125FilesNames[i]);
            }
            signalNtuples.push_back(new h2l2qTree(ttH125));
            signalSampleName.push_back("ttH125");
            sigLineColor.push_back(kRed+2);
            TFile* tempFile = new TFile("/eos/uscms/store/user/awhitbe1/Higgs2l2q/skims/"+ttH125FilesNames[0]);
            sigOriginalWeight.push_back(((TH1F*)tempFile->Get("ZZTree/Counters"))->GetBinContent(40));
            tempFile->Close();

        }

    };

    h2l2qTree* findNtuple(TString name){
        for( unsigned int iSam = 0 ; iSam < sampleName.size() ; iSam++ ){
            if( sampleName[iSam] == name )
                return ntuples[iSam] ;
        }
        for( unsigned int iSam = 0 ; iSam < signalSampleName.size() ; iSam++ ){
            if( signalSampleName[iSam] == name )
                return signalNtuples[iSam] ;
        }
        return NULL;
    };

};
