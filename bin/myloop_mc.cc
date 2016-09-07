#include <iostream>
#include <sstream>
#include <TChain.h>
#include <TTree.h>
#include <TFile.h>
#include <TMath.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include "UserCode/B_production_x_sec_13_TeV/interface/format.h"
#include "UserCode/B_production_x_sec_13_TeV/interface/myloop.h"
#include "UserCode/B_production_x_sec_13_TeV/interface/channel.h"

//input example: myloop_mc --channel 1
int main(int argc, char** argv)
{
  int channel = 0;

  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;
      
      if(argument == "--channel")
        {
          convert << argv[++i];
          convert >> channel;
        }
    }
  
  if(channel==0)
    {
      std::cout << "No channel was provided as input. Please use --channel. Example: signal_yield_new --channel 1" << std::endl;
      return 0;
    }
  
  TChain *root = new TChain("demo/root");
  TChain *HltTree = new TChain("hltanalysis/HltTree");

  switch(channel)
    {
    default:
    case 1:
      //for BMuonFilter processed with Bfinder_mc.py
      
      //root->Add("/gstore/t3cms/store/user/martinsg/Bfinder_25ns_mc_v3/BuToJpsiKV2_BMuonFilter_TuneCUEP8M1_13TeV-pythia8-evtgen/crab_Bfinder_25ns_mc_v3/160518_151554/0000/Bfinder_25ns_mc_*.root");
      //HltTree->Add("/gstore/t3cms/store/user/martinsg/Bfinder_25ns_mc_v3/BuToJpsiKV2_BMuonFilter_TuneCUEP8M1_13TeV-pythia8-evtgen/crab_Bfinder_25ns_mc_v3/160518_151554/0000/Bfinder_25ns_mc_*.root");
      
      //for Bfilter processed with Bfinder_mc.py
      
      root->Add("/gstore/t3cms/store/user/martinsg/Bfinder_mc_Bu_Bfilter_v1/BuToJpsiKV2_BFilter_TuneCUEP8M1_13TeV-pythia8-evtgen/crab_Bfinder_mc_Bu_Bfilter_v1/160812_095709/0000/Bfinder_mc_*.root");
      HltTree->Add("/gstore/t3cms/store/user/martinsg/Bfinder_mc_Bu_Bfilter_v1/BuToJpsiKV2_BFilter_TuneCUEP8M1_13TeV-pythia8-evtgen/crab_Bfinder_mc_Bu_Bfilter_v1/160812_095709/0000/Bfinder_mc_*.root");

      //for BMuonFilter processed with Bfinder_mc (should give the same result as the first one

      //root->Add("/gstore/t3cms/store/user/martinsg/Bfinder_test_v2/BuToJpsiKV2_BMuonFilter_TuneCUEP8M1_13TeV-pythia8-evtgen/crab_Bfinder_test_v2/160811_210322/0000/Bfinder_mc_*.root");
      //HltTree->Add("/gstore/t3cms/store/user/martinsg/Bfinder_test_v2/BuToJpsiKV2_BMuonFilter_TuneCUEP8M1_13TeV-pythia8-evtgen/crab_Bfinder_test_v2/160811_210322/0000/Bfinder_mc_*.root");
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    }

  //-----------------------------------------------------------------
  // some basic integrity checks
  
  int n_entries = root->GetEntries();
  if (n_entries!=HltTree->GetEntries()) {
    printf("Error: # of entries are different in two main trees.\n");
    return 0;
  }
  printf("Going to process %d entries.\n",n_entries);
  
  //-----------------------------------------------------------------
  // setting memory addresses to the branches
    // create new output trees
  
  EvtInfoBranches *EvtInfo = new EvtInfoBranches;
  VtxInfoBranches *VtxInfo = new VtxInfoBranches;
  MuonInfoBranches *MuonInfo = new MuonInfoBranches;
  TrackInfoBranches *TrackInfo = new TrackInfoBranches;
  BInfoBranches *BInfo = new BInfoBranches;
  GenInfoBranches *GenInfo = new GenInfoBranches;
    
  EvtInfo->setbranchadd(root);
  VtxInfo->setbranchadd(root);
  MuonInfo->setbranchadd(root);
  TrackInfo->setbranchadd(root);
  BInfo->setbranchadd(root);
  GenInfo->setbranchadd(root);
    
  ULong64_t HltTree_Event;
  int HltTree_Run;
  int HLT_book[N_HLT_BOOKINGS];
  
  HltTree->SetBranchAddress("Event",&HltTree_Event);
  HltTree->SetBranchAddress("Run",&HltTree_Run);
  
  for (int i=0;i<N_HLT_BOOKINGS;i++)
    HltTree->SetBranchAddress(HLT_paths[i],&HLT_book[i]);
  
  TString file_name = "myloop_mc_" + channel_to_ntuple_name(channel) + "_bfilter_no_cuts.root";  
  TFile *fout = new TFile(file_name,"recreate");
    
  ReducedBranches *br = new ReducedBranches();
  
  TString ntuple_name = channel_to_ntuple_name(channel) + "_mc";
  TTree *nt = new TTree(ntuple_name, ntuple_name);
  
  br->regTree(nt);

  for (int evt=0; evt<n_entries; evt++) 
    {
    if (evt%1000==0 || evt==n_entries-1) printf("processing %d/%d (%.2f%%).\n",evt,n_entries-1,(double)evt/(double)(n_entries-1)*100.);
        
    root->GetEntry(evt);
    HltTree->GetEntry(evt);
        
    //-----------------------------------------------------------------
    // verify the Run # and Event #
    if (EvtInfo->EvtNo!=(int)HltTree_Event || EvtInfo->RunNo!=HltTree_Run) 
      {
      printf("Error: mismatch of event # and run #.\n");
      return 0;
      }
        
    //-----------------------------------------------------------------
    // Look for indices of the whole decay tree
    int gen_bp_count = 0;
    TLorentzVector gen_v4_bp[8];
    TVector3 gen_vtx_bp[8], gen_vtx_uj[8];
    for (int idx = 0; idx < GenInfo->size; idx++) 
      {            
	if (abs(GenInfo->pdgId[idx])==521) // B+ find 
	  {
	    int idx_bp   = idx;
	    int idx_jpsi = GenInfo->da1[idx_bp];
	    int idx_kp   = GenInfo->da2[idx_bp];
	    int idx_mu1  = GenInfo->da1[idx_jpsi];
	    int idx_mu2  = GenInfo->da2[idx_jpsi];
	    
	    if (GenInfo->pdgId[idx_jpsi]!=443) continue; // not J/psi
	    if (abs(GenInfo->pdgId[idx_kp])!=321) continue; // not K+-
	    if (abs(GenInfo->pdgId[idx_mu1])!=13) continue; // not mu+-
	    if (abs(GenInfo->pdgId[idx_mu2])!=13) continue; // not mu+-
	    
	    TLorentzVector v4_bp, v4_mu1, v4_mu2;
	    v4_bp.SetPtEtaPhiM(GenInfo->pt[idx_bp],GenInfo->eta[idx_bp],GenInfo->phi[idx_bp],GenInfo->mass[idx_bp]);
	    v4_mu1.SetPtEtaPhiM(GenInfo->pt[idx_mu1],GenInfo->eta[idx_mu1],GenInfo->phi[idx_mu1],GenInfo->mass[idx_mu1]);
	    v4_mu2.SetPtEtaPhiM(GenInfo->pt[idx_mu2],GenInfo->eta[idx_mu2],GenInfo->phi[idx_mu2],GenInfo->mass[idx_mu2]);
	    
	    bool muon1Filter = fabs(GenInfo->eta[idx_mu1])<2.4 && GenInfo->pt[idx_mu1]>2.8;
	    bool muon2Filter = fabs(GenInfo->eta[idx_mu2])<2.4 && GenInfo->pt[idx_mu2]>2.8;
	    
	    if (muon1Filter && muon2Filter)
	      {
		if (gen_bp_count>=8)
		  {
		    printf("ERROR: too many B+ in GenInfo.\n");
		    return 0;
		  }
		gen_v4_bp[gen_bp_count] = v4_bp;
		gen_vtx_bp[gen_bp_count].SetXYZ(GenInfo->vx[idx_bp],GenInfo->vy[idx_bp],GenInfo->vz[idx_bp]);
		gen_vtx_uj[gen_bp_count].SetXYZ(GenInfo->vx[idx_jpsi],GenInfo->vy[idx_jpsi],GenInfo->vz[idx_jpsi]);
		gen_bp_count++;
	      }
	  }
      }
    
    // Start of BInfo loop
    for (int bidx = 0; bidx < BInfo->size; bidx++) 
      {	
	if (BInfo->type[bidx]!=1) continue; // skip any non K+ events
	
	// cleanup
	memset(br,0x00,sizeof(ReducedBranches));
	
	br->run = EvtInfo->RunNo;
	br->event = EvtInfo->EvtNo;
	br->type = BInfo->type[bidx];
	
	//-----------------------------------------------------------------
	// save HLT paths
	br->nhltbook = N_HLT_BOOKINGS;
	
	for (int i=0;i<N_HLT_BOOKINGS;i++)
	  br->hltbook[i] = HLT_book[i];
	
	int ujidx = BInfo->rfuj_index[bidx];
	int tk1idx = BInfo->rftk1_index[bidx];
	int tk2idx = BInfo->rftk2_index[bidx];
	int mu1idx = BInfo->uj_rfmu1_index[ujidx];
	int mu2idx = BInfo->uj_rfmu2_index[ujidx];
	
	/*
	//-----------------------------------------------------------------
	// Basic muon selections
	if (MuonInfo->pt[mu1idx]<=4.) continue;
	if (MuonInfo->pt[mu2idx]<=4.) continue;
	if (fabs(MuonInfo->eta[mu1idx])>=2.4) continue;
	if (fabs(MuonInfo->eta[mu2idx])>=2.4) continue;
	if (!MuonInfo->SoftMuID[mu1idx]) continue;
	if (!MuonInfo->SoftMuID[mu2idx]) continue;
	
	//-----------------------------------------------------------------
	// Basic track selections
	if (TrackInfo->pt[tk1idx]<=0.8) continue;
	if (fabs(TrackInfo->eta[tk1idx])>=2.5) continue;
	if (TrackInfo->chi2[tk1idx]/TrackInfo->ndf[tk1idx]>=5.) continue;
	if (TrackInfo->striphit[tk1idx]+TrackInfo->pixelhit[tk1idx]<5) continue;
	
	//-----------------------------------------------------------------
	// J/psi cut
	// KFC: May need to consider an y dependent cut?
	if (fabs(BInfo->uj_mass[ujidx]-JPSI_MASS)>=0.150) continue;
	if (BInfo->uj_pt[ujidx]<=8.0) continue;
	*/
	
	//-----------------------------------------------------------------
	// Find the best pointing PV
	//
	// KFC@20150713: keep the selecton code but PV is replaced with BS in the end.
	
	TVector3 bvtx(BInfo->vtxX[bidx],BInfo->vtxY[bidx],BInfo->vtxZ[bidx]);
	TVector3 bvtx_err(BInfo->vtxXE[bidx],BInfo->vtxYE[bidx],BInfo->vtxZE[bidx]);
	TVector3 bmom(BInfo->px[bidx],BInfo->py[bidx],BInfo->pz[bidx]);
	int vidx = -1;
	double max_cosang = -1.;
	
	for (int idx = 0; idx < VtxInfo->Size; idx++) 
	  {
	    TVector3 vtx(VtxInfo->x[idx],VtxInfo->y[idx],VtxInfo->z[idx]);      
	    double cosang = bmom.Dot(bvtx-vtx)/(bmom.Mag()*(bvtx-vtx).Mag());
	    
	    if (cosang>max_cosang) 
	      {
		vidx = idx;
		max_cosang = cosang;
	      }
	  }
	
	if (vidx==-1) 
	  {
	    printf("Error: no PV found. Run: %d, Event: %d.\n",EvtInfo->RunNo,EvtInfo->EvtNo);
	continue;
	  }
	
	TVector3 BS(EvtInfo->PVx,EvtInfo->PVy,EvtInfo->PVz);
	TVector3 BS_err(EvtInfo->PVxE,EvtInfo->PVyE,EvtInfo->PVzE);
	TVector3 PV(VtxInfo->x[vidx],VtxInfo->y[vidx],VtxInfo->z[vidx]);
	TVector3 PV_err(VtxInfo->xE[vidx],VtxInfo->yE[vidx],VtxInfo->zE[vidx]);
	
	// KFC@20150713: Don't do PV finding for now. Always use the beamspot according to the agreement
	PV = BS; PV_err = BS_err;
	
	TLorentzVector v4_uj;
	v4_uj.SetPtEtaPhiM(BInfo->uj_pt[ujidx],BInfo->uj_eta[ujidx],BInfo->uj_phi[ujidx],BInfo->uj_mass[ujidx]);
	TLorentzVector v4_b;
	v4_b.SetPtEtaPhiM(BInfo->pt[bidx],BInfo->eta[bidx],BInfo->phi[bidx],BInfo->mass[bidx]);
	
	double min_dR = 100.;
	int min_idx = -1;
	for (int idx=0; idx<gen_bp_count; idx++) 
	{
	  double dR = v4_b.DeltaR(gen_v4_bp[idx]);
	  
	  if (dR<min_dR) 
	    {
	      min_idx = idx;
	      min_dR = dR;
	    }
	}
	
	if (min_idx>=0) 
	  {
	    br->genmass = gen_v4_bp[min_idx].Mag();
	    br->genpt   = gen_v4_bp[min_idx].Pt();
	    br->geneta  = gen_v4_bp[min_idx].Eta();
	    br->genphi  = gen_v4_bp[min_idx].Phi();
	    br->geny    = gen_v4_bp[min_idx].Rapidity();
	    br->genvx   = gen_vtx_bp[min_idx].x();
	    br->genvy   = gen_vtx_bp[min_idx].y();
	    br->genvz   = gen_vtx_bp[min_idx].z();
	    br->genujvx = gen_vtx_uj[min_idx].x();
	    br->genujvy = gen_vtx_uj[min_idx].y();
	    br->genujvz = gen_vtx_uj[min_idx].z();
	  }
	
	TLorentzVector v4_tktk;
	TVector3 tktkvtx, tktkvtx_err;
	
	//-----------------------------------------------------------------
	// Start to fill the B hadron information
	
	br->mass = BInfo->mass[bidx];
	br->pt = BInfo->pt[bidx];
	br->eta = BInfo->eta[bidx];
	br->phi = BInfo->phi[bidx];
	br->y = v4_b.Rapidity();
	br->vx = BInfo->vtxX[bidx];
	br->vy = BInfo->vtxY[bidx];
	br->vz = BInfo->vtxZ[bidx];
	br->PVx = PV.x();
	br->PVy = PV.y();
	br->PVz = PV.z();
	br->lxy = (bvtx-PV).Perp();
	br->lxyz = (bvtx-PV).Mag();
	br->errxy = sqrt(bvtx_err.Perp2()+PV_err.Perp2());
	br->errxyz = sqrt(bvtx_err.Mag2()+PV_err.Mag2());
	br->vtxprob = TMath::Prob(BInfo->vtxchi2[bidx],BInfo->vtxdof[bidx]);
	br->cosalpha2d = bmom.XYvector()*(bvtx-PV).XYvector()/(bmom.Perp()*(bvtx-PV).Perp());
	br->cosalpha3d = bmom.Dot(bvtx-PV)/(bmom.Mag()*(bvtx-PV).Mag());
	
	//-----------------------------------------------------------------
	// calculate the proper decay time
	
	TVector3 v_l = bvtx-PV, v_lerr2; // displace vector, error^2 vector for displacement
	v_lerr2.SetX(bvtx_err.x()*bvtx_err.x()+PV_err.x()*PV_err.x());
	v_lerr2.SetY(bvtx_err.y()*bvtx_err.y()+PV_err.y()*PV_err.y());
	v_lerr2.SetZ(bvtx_err.z()*bvtx_err.z()+PV_err.z()*PV_err.z());
	
	// B hadron mass for normalization of proper decay time
	double default_bmass = BP_MASS; // B+ channels (type 1 or 2)
	
	TVector3 v_p = v4_b.Vect();
	TVector3 v_p2(v_p.x()*v_p.x(),v_p.y()*v_p.y(),v_p.z()*v_p.z());
	
	br->ctau3d = v_l.Dot(v_p)*default_bmass/v_p.Mag2();
	br->ctau3derr = sqrt(v_lerr2.Dot(v_p2))*default_bmass/v_p.Mag2();
	br->ctau2d = v_l.XYvector()*v_p.XYvector()*default_bmass/v_p.Perp2();
	br->ctau2derr = sqrt(v_lerr2.XYvector()*v_p2.XYvector())*default_bmass/v_p.Perp2();
	
	//-----------------------------------------------------------------
	// fill J/psi, tracks, muons, etc.
	
	br->ujmass = BInfo->uj_mass[ujidx];
	br->ujpt = BInfo->uj_pt[ujidx];
	br->ujeta = BInfo->uj_eta[ujidx];
	br->ujphi = BInfo->uj_phi[ujidx];
	br->ujy = v4_uj.Rapidity();
	br->ujvtxprob = TMath::Prob(BInfo->uj_vtxchi2[ujidx],BInfo->uj_vtxdof[ujidx]);
	
	br->mu1idx = mu1idx;
	br->mu1pt  = MuonInfo->pt[mu1idx];
	br->mu1eta = MuonInfo->eta[mu1idx];
	br->mu1phi = MuonInfo->phi[mu1idx];
	br->mu2idx = mu2idx;
	br->mu2pt  = MuonInfo->pt[mu2idx];
	br->mu2eta = MuonInfo->eta[mu2idx];
	br->mu2phi = MuonInfo->phi[mu2idx];
	
	br->tk1idx = tk1idx;
	br->tk1pt  = TrackInfo->pt[tk1idx];
	br->tk1eta = TrackInfo->eta[tk1idx];
	br->tk1phi = TrackInfo->phi[tk1idx];
	br->tk1charge = TrackInfo->charge[tk1idx];
	br->tk2idx = tk2idx;
	br->tk2pt  = TrackInfo->pt[tk2idx];
	br->tk2eta = TrackInfo->eta[tk2idx];
	br->tk2phi = TrackInfo->phi[tk2idx];
	br->tk2charge = TrackInfo->charge[tk2idx];
	
	br->nhltmatch = N_HLT_MATCHINGS;
	
	for (int i=0; i<N_HLT_MATCHINGS; i++) 
	  {
	    br->mu1hltpt[i]  = MuonInfo->MuTrgMatchTrgObjPt->at(mu1idx)[i];
	    br->mu1hlteta[i] = MuonInfo->MuTrgMatchTrgObjEta->at(mu1idx)[i];
	    br->mu1hltphi[i] = MuonInfo->MuTrgMatchTrgObjPhi->at(mu1idx)[i];
	    br->mu2hltpt[i]  = MuonInfo->MuTrgMatchTrgObjPt->at(mu2idx)[i];
	    br->mu2hlteta[i] = MuonInfo->MuTrgMatchTrgObjEta->at(mu2idx)[i];
	    br->mu2hltphi[i] = MuonInfo->MuTrgMatchTrgObjPhi->at(mu2idx)[i];
	  }
	
	nt->Fill();
	
      } // end of BInfo loop
    } // end of evt loop
  
  fout->Write();
  fout->Close();
  
  delete EvtInfo;
  delete VtxInfo;
  delete MuonInfo;
  delete TrackInfo;
  delete BInfo;
  delete GenInfo;
  delete br;
}
