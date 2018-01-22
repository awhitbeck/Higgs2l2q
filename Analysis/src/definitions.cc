#include "TLorentzVector.h"
#include "TRandom3.h"
#include <iostream>

// constants
// ==============================================
// ==============================================

// utils
// ==============================================
template<class T> std::vector<std::pair<T,T> > distinct_pairs(std::vector<T> elems){
    std::vector<std::pair<T,T> > pairs;
    for( typename std::vector<T>::iterator i = elems.begin(); i!=elems.end(); i++ ){
        for( typename std::vector<T>::iterator j = i ; j!=elems.end(); j++ ){
            if( i == j ) continue;
            std::pair<T,T> p(*i,*j);
            pairs.push_back(p);
        }
    }
    return pairs;
}
// ==============================================

template<typename ntupleType>void ntupleBranchStatus(ntupleType* ntuple){
  ntuple->fChain->SetBranchStatus("*",0);
  ntuple->fChain->SetBranchStatus("nCleanedJets*",1);
  ntuple->fChain->SetBranchStatus("AllLep*",1);
  ntuple->fChain->SetBranchStatus("Jet*",1);
  ntuple->fChain->SetBranchStatus("xsec",1);
  ntuple->fChain->SetBranchStatus("PUWeight",1);
  ntuple->fChain->SetBranchStatus("genHEPMCweight",1);
}

template<typename ntupleType>double fillNJets(ntupleType* ntuple){
    return ntuple->nCleanedJetsPt30;
}

template<typename ntupleType>double fillBJets(ntupleType* ntuple){
    return ntuple->nCleanedJetsPt30BTagged;
}

template<typename ntupleType>vector<TLorentzVector> get_lep_4vec(ntupleType* ntuple){
    vector<TLorentzVector> leps;
    for( unsigned int ilep = 0 ; ilep < ntuple->AllLepPt->size() ; ilep++ ){
        TLorentzVector lep;
        lep.SetPtEtaPhiM(ntuple->AllLepPt->at(ilep),
                         ntuple->AllLepEta->at(ilep),
                         ntuple->AllLepPhi->at(ilep),
                         0.);
        leps.push_back(lep);
    }
    return leps;
}
template<typename ntupleType>vector<TLorentzVector> get_jet_4vec(ntupleType* ntuple){
    vector<TLorentzVector> jets;
    vector<TLorentzVector> leps = get_lep_4vec<ntupleType>(ntuple);
    for( unsigned int ijet = 0 ; ijet < ntuple->JetPt->size() ; ijet++ ){
        TLorentzVector jet;
        jet.SetPtEtaPhiM(ntuple->JetPt->at(ijet),
                         ntuple->JetEta->at(ijet),
                         ntuple->JetPhi->at(ijet),
                         ntuple->JetMass->at(ijet));

        bool clean=true;
        for( auto& lep : leps ){
            if( lep.DeltaR(jet) < 0.4 )
                clean = false;
        }
        if( clean ) jets.push_back(jet);
    }
    return jets;
}

template<typename ntupleType>vector<TLorentzVector> build_dileps(ntupleType* ntuple, vector<int>* exl=0){
    vector<TLorentzVector> dileps;
    vector<TLorentzVector> leps = get_lep_4vec<ntupleType>(ntuple);
    for( unsigned int ilep = 0 ; ilep < leps.size() ; ilep++ ){
        if( exl && std::find(exl->begin(),exl->end(),ilep) != exl->end() ) continue;
        for( unsigned int jlep = ilep+1 ; jlep < leps.size() ; jlep++ ){
            if( exl && std::find(exl->begin(),exl->end(),jlep) != exl->end() ) continue;
            int id1=ntuple->AllLepLepId->at(ilep);
            int id2=ntuple->AllLepLepId->at(jlep);
            if( abs(id1) == abs(id2) && id1*id2<0 )
                dileps.push_back(leps[ilep]+leps[jlep]);
        }
    }
    return dileps;
}   

template<typename ntupleType>vector<TLorentzVector> build_dijets(ntupleType* ntuple, vector<int>* exl=0){
    vector<TLorentzVector> dijets;
    vector<TLorentzVector> jets = get_jet_4vec<ntupleType>(ntuple);
    for( unsigned int ijet = 0 ; ijet < jets.size() ; ijet++ ){
        if( exl && std::find(exl->begin(),exl->end(),ijet) != exl->end() ) continue;
        for( unsigned int jjet = ijet+1 ; jjet < jets.size() ; jjet++ ){
            if( exl && std::find(exl->begin(),exl->end(),jjet) != exl->end() ) continue;
            dijets.push_back(jets[ijet]+jets[jjet]);
        }
    }
    return dijets;
}

template<typename ntupleType>double fill_max_mll(ntupleType* ntuple){
    double max_mass = 0.;
    vector<TLorentzVector> dileps = build_dileps<ntupleType>(ntuple);
    for( auto& dilep : dileps ){
        if( dilep.M() > max_mass )
            max_mass = dilep.M();
    }
    return max_mass;
}

template<typename ntupleType>double fill_min_mll(ntupleType* ntuple){
    double max_mass = fill_max_mll(ntuple);
    double min_mass = 99999.;
    vector<TLorentzVector> leps = get_lep_4vec<ntupleType>(ntuple);
    vector<TLorentzVector> dileps;
    vector<int> indices(leps.size(),0);
    std::generate(indices.begin(),indices.end(),[n=0]()mutable{return n++;});
    for( auto& p : distinct_pairs<int>(indices) ){
        if( (leps[p.first]+leps[p.second]).M() == max_mass ){
            vector<int> exl={p.first,p.second};
            dileps = build_dileps<ntupleType>(ntuple,&exl);
            break;
        }            
    }
    for( auto& dilep : dileps ){
        if( dilep.M() < min_mass )
            min_mass = dilep.M();
    }
    return min_mass;
}

template<typename ntupleType>double fill_max_mjj(ntupleType* ntuple){
    double max_mass = 0.;
    vector<TLorentzVector> dijets = build_dijets<ntupleType>(ntuple);
    for( auto& dijet : dijets ){
        double dijet_mass = dijet.M();
        if( dijet_mass > max_mass )
            max_mass = dijet_mass;
    }
    return max_mass;
}

template<typename ntupleType>double fill_max_deta(ntupleType* ntuple){
    double max_mass = fill_max_mjj(ntuple);   
    vector<TLorentzVector> jets = get_jet_4vec<ntupleType>(ntuple);
    vector<int> indices(jets.size(),0);
    std::generate(indices.begin(), indices.end(), [n = 0] () mutable { return n++; });
    for( auto& p : distinct_pairs<int>(indices) ){
        if( (jets[p.first]+jets[p.second]).M() == max_mass ){
            return fabs(jets[p.first].Eta()-jets[p.second].Eta());
        }
    }
    return -1.;
}

template<typename ntupleType>double fill_best_mjj(ntupleType* ntuple){
    double min_distance = 0.;
    double best_mass = 0.;
    double max_mass = fill_max_mjj(ntuple);
    vector<TLorentzVector> jets = get_jet_4vec<ntupleType>(ntuple);
    vector<TLorentzVector> dijets;
    vector<int> indices(jets.size(),0);
    std::generate(indices.begin(), indices.end(), [n = 0] () mutable { return n++; });
    for( auto& p : distinct_pairs<int>(indices) ){
        if( (jets[p.first]+jets[p.second]).M() == max_mass ){
            vector<int> exl = {p.first,p.second};
            dijets = build_dijets<ntupleType>(ntuple,&exl);
            break;
        }
    }

    for( auto& dijet : dijets ){
        double dijet_mass = dijet.M();
        if( fabs(dijet_mass-90.) > min_distance )
            best_mass = dijet_mass;
    }
    return best_mass;
}

template<typename ntupleType>double fill_best_mlljj(ntupleType* ntuple){
    double best_mjj = fill_best_mjj(ntuple);
    double best_mll = fill_max_mll(ntuple);
    vector<TLorentzVector> dijets = build_dijets<ntupleType>(ntuple);
    vector<TLorentzVector> dileps = build_dileps<ntupleType>(ntuple);
    TLorentzVector best_dijet,best_dilep;
    
    for( auto& dijet : dijets ){
        if( dijet.M() == best_mjj ){
            best_dijet = dijet;
            break;
        }
    }
    for( auto& dilep : dileps ){
        if( dilep.M() == best_mll ){
            best_dilep = dilep;
            break;
        }
    }
    return (best_dilep+best_dijet).M();
}

template<typename ntupleType>double fill_ZH_mlljj(ntupleType* ntuple){
    double best_mjj = fill_max_mjj(ntuple);
    double best_mll = fill_min_mll(ntuple);
    vector<TLorentzVector> dijets = build_dijets<ntupleType>(ntuple);
    vector<TLorentzVector> dileps = build_dileps<ntupleType>(ntuple);
    TLorentzVector best_dijet,best_dilep;
    
    for( auto& dijet : dijets ){
        if( dijet.M() == best_mjj ){
            best_dijet = dijet;
            break;
        }
    }
    for( auto& dilep : dileps ){
        if( dilep.M() == best_mll ){
            best_dilep = dilep;
            break;
        }
    }
    return (best_dilep+best_dijet).M();
}

template<typename ntupleType>bool ZHCut(ntupleType* ntuple){
    double max_mll = fill_max_mll(ntuple);
    double min_mll = fill_min_mll(ntuple);
    double btags = fillBJets(ntuple);
    double max_mjj = fill_max_mjj(ntuple);
    double NLeps = ntuple->AllLepPt->size();
    double NJets = ntuple->nCleanedJetsPt30;
    return NLeps>=4 && 
        NJets>=2 && 
        NJets<4 && 
        btags < 2 && 
        max_mll > 60. &&
        max_mll < 150. && 
        min_mll > 12. &&
        min_mll < 60. && 
        max_mjj > 60. &&
        max_mjj < 150.;
}   

template<typename ntupleType>bool VBFCut(ntupleType* ntuple){
    if( ntuple->AllLepPt->size() == 0 )
        return false;
    std::vector<TLorentzVector> dijets = build_dileps<ntupleType>(ntuple);
    int NMll = dijets.size();
    double max_mll = fill_max_mll(ntuple);
    double btags = fillBJets(ntuple);
    std::vector<TLorentzVector> dileps = build_dijets<ntupleType>(ntuple);
    double best_mjj = fill_best_mjj(ntuple);
    double best_deta = fill_max_deta(ntuple);
    int NMjj = dileps.size();
    double NLeps = ntuple->AllLepPt->size();
    double NJets = ntuple->nCleanedJetsPt30;
    return NLeps>=2 && 
        NJets>=4 && 
        NJets<=6 &&
        NMll>=1 && 
        NMjj>=2 && 
        max_mll > 12 &&
        max_mll < 60 && 
        best_mjj > 60 &&
        best_mjj < 120 &&
        best_deta > 3. &&
        btags < 2;    
}
