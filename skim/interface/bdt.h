#ifndef BDT_H
#define BDT_H
#include "helper.h"

// https://github.com/root-project/root/blob/master/tutorials/tmva/tmva003_RReader.C
using namespace TMVA::Experimental;

template <typename T>
auto jetPtRatio(T &df) {
  using namespace ROOT::VecOps;
  // https://github.com/srudra3/LeptonID/blob/master/ClassificationBDT_ele.py
  auto makeBDTvar = [](
		       const RVec<float> &electron_miniPFRelIso_all,
		       const RVec<int> &electron_jetIdx,
		       const RVec<float> &electron_pt,
		       const RVec<float> &jet_pt
		       ){
    // Electron_jetPtRatio
    // proxy for jet pt when no associated jets found
    size_t nsize = electron_pt.size();
    RVec<float> electron_jetPtRatio( nsize , 0. );
    
    float A; float B;
    for ( size_t i = 0 ; i < nsize ; i++ ) {
      float mini_Iso_all = electron_miniPFRelIso_all[i];
      int jIdx= electron_jetIdx[i];
      float pt = electron_pt[i];

      A=0.; B=0.;
      if (jIdx == -1) A = 1. / (1. + mini_Iso_all);
      if (jIdx >= 0 ) B = pt / jet_pt[jIdx];
      
      electron_jetPtRatio.push_back(A+B);
    }
    
    // return the first index 
    return electron_jetPtRatio;
  };

  return df
    .Define( "Electron_jetPtRatio" , makeBDTvar , { "Electron_miniPFRelIso_all" , "Electron_jetIdx" , "Electron_pt" , "Jet_pt" } )
    ;
}

template<typename T>
auto BDT_reader( T &df , const std::vector<TMVA::Reader*> &readers , const std::string &bdtname ) {
  using namespace ROOT::VecOps;
  
  auto predict = [&](
		    unsigned int nslot,
		    const RVec<float> &electron_miniPFRelIso_chg,
		    const RVec<float> &electron_miniPFRelIso_neu,
		    const RVec<float> &electron_dxy,
		    const RVec<float> &jet_btagDeepFlavB,
		    const RVec<float> &electron_jetPtRelv2,
		    const RVec<float> &electron_jetPtRatio
		    ){
    Helper::electron_miniPFRelIso_chg_[nslot] = electron_miniPFRelIso_chg[0];
    Helper::electron_miniPFRelIso_neu_[nslot] = electron_miniPFRelIso_neu[0];
    Helper::electron_dxy_[nslot]              = electron_dxy[0];
    Helper::jet_btagDeepFlavB_[nslot]         = jet_btagDeepFlavB[0];
    Helper::electron_jetPtRelv2_[nslot]       = electron_jetPtRelv2[0];
    Helper::electron_jetPtRatio_[nslot]       = electron_jetPtRatio[0];
    return readers[nslot]->EvaluateMVA(bdtname);
  };
  
  return df.DefineSlot( "mvaBDT" , predict , { "Electron_miniPFRelIso_chg" , "Electron_miniPFRelIso_neu" , "Electron_dxy" , "Jet_btagDeepFlavB" , "Electron_jetPtRelv2" , "Electron_jetPtRatio" } );
}

#endif
