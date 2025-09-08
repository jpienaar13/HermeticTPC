
#include <G4SDManager.hh>
#include <G4Run.hh>
#include <G4Event.hh>
#include <G4HCofThisEvent.hh>
#include <G4EmCalculator.hh>
#include <G4Material.hh>
#include <G4HadronicProcessStore.hh>
#include <G4ParticleTable.hh>
#include <G4NistManager.hh>
#include <G4ElementTable.hh>
#include <G4Version.hh>
#include <G4SystemOfUnits.hh>
#include <numeric>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TParameter.h>
#include <TDirectory.h>
#include <TH1.h>

#include "HTPCDetectorConstruction.hh"
#include "HTPCDetectorHit.hh"
#include "HTPCPrimaryGeneratorAction.hh"
#include "HTPCEventData.hh"

#include "HTPCAnalysisManager.hh"

HTPCAnalysisManager::HTPCAnalysisManager(HTPCPrimaryGeneratorAction *pPrimaryGeneratorAction) :
  m_iDetectorHitsCollectionID(-1), m_hDataFilename("events.root"), m_iNbEventsToSimulate(0),
  m_pTreeFile(0), m_pTree(0), _events(0),
  m_pNbEventsToSimulateParameter(0), m_pPrimaryGeneratorAction(pPrimaryGeneratorAction),
  m_pEventData(0), plotPhysics(true), runTime(0),
  writeEmptyEvents(true)

{
  runTime = new G4Timer();
  m_pEventData = new HTPCEventData();
}

HTPCAnalysisManager::~HTPCAnalysisManager()
{}

void
HTPCAnalysisManager::BeginOfRun(const G4Run *)
{
  // start a timer for this run....
  runTime->Start();
  // do we write empty events or not?
  writeEmptyEvents = m_pPrimaryGeneratorAction->GetWriteEmpty();

  m_pTreeFile = new TFile(m_hDataFilename.c_str(), "RECREATE");//, "File containing event data for Xenon1T");
  // make tree structure
  TNamed *G4version = new TNamed("G4VERSION_TAG",G4VERSION_TAG);
  G4version->Write();

  _events = m_pTreeFile->mkdir("events");
  _events->cd();

  G4cout <<"HTPCAnalysisManager:: Init data tree ..."<<G4endl;
  m_pTree = new TTree("events", "Tree containing event data for HTPC Detector");

  gROOT->ProcessLine("#include <vector>");

  m_pTree->Branch("eventid", &m_pEventData->m_iEventId, "eventid/I");
  //Figure Uit watter tak om te hou en verander verder soos nodig
  m_pTree->Branch("ndetectorhits", &m_pEventData->m_iNbPMTHits);
  m_pTree->Branch("detectorhits", "vector<int>", &m_pEventData->m_pPMTHits);

  m_pTree->Branch("etot", &m_pEventData->m_fTotalEnergyDeposited, "etot/F");
  m_pTree->Branch("nsteps", &m_pEventData->m_iNbSteps, "nsteps/I");
  m_pTree->Branch("trackid", "vector<int>", &m_pEventData->m_pTrackId);
  m_pTree->Branch("type", "vector<string>", &m_pEventData->m_pParticleType);
  m_pTree->Branch("parentid", "vector<int>", &m_pEventData->m_pParentId);
  m_pTree->Branch("parenttype", "vector<string>", &m_pEventData->m_pParentType);
  m_pTree->Branch("creaproc", "vector<string>", &m_pEventData->m_pCreatorProcess);
  m_pTree->Branch("edproc", "vector<string>", &m_pEventData->m_pDepositingProcess);
  m_pTree->Branch("PreStepEnergy", "vector<float>", &m_pEventData->m_pPreStepEnergy);
  m_pTree->Branch("PostStepEnergy", "vector<float>", &m_pEventData->m_pPostStepEnergy);
  m_pTree->Branch("xp", "vector<float>", &m_pEventData->m_pX);
  m_pTree->Branch("yp", "vector<float>", &m_pEventData->m_pY);
  m_pTree->Branch("zp", "vector<float>", &m_pEventData->m_pZ);
  m_pTree->Branch("ed", "vector<float>", &m_pEventData->m_pEnergyDeposited);
  m_pTree->Branch("time", "vector<float>", &m_pEventData->m_pTime);

  m_pTree->Branch("type_pri", "vector<string>", &m_pEventData->m_pPrimaryParticleType);
  m_pTree->Branch("xp_pri", &m_pEventData->m_fPrimaryX, "xp_pri/F");
  m_pTree->Branch("yp_pri", &m_pEventData->m_fPrimaryY, "yp_pri/F");
  m_pTree->Branch("zp_pri", &m_pEventData->m_fPrimaryZ, "zp_pri/F");
  m_pTree->Branch("cx_pri", &m_pEventData->m_fPrimaryCx, "cx_pri/F");
  m_pTree->Branch("cy_pri", &m_pEventData->m_fPrimaryCy, "cy_pri/F");
  m_pTree->Branch("cz_pri", &m_pEventData->m_fPrimaryCz, "cz_pri/F");
  m_pTree->Branch("xp_fcd", &m_pEventData->m_fForcedPrimaryX, "xp_fcd/F");
  m_pTree->Branch("yp_fcd", &m_pEventData->m_fForcedPrimaryY, "yp_fcd/F");
  m_pTree->Branch("zp_fcd", &m_pEventData->m_fForcedPrimaryZ, "zp_fcd/F");
  m_pTree->Branch("e_pri",  &m_pEventData->m_fPrimaryE, "e_pri/F");
  m_pTree->Branch("w_pri",  &m_pEventData->m_fPrimaryW, "w_pri/F");

  m_pNbEventsToSimulateParameter = new TParameter<int>("nbevents", m_iNbEventsToSimulate);
  m_pNbEventsToSimulateParameter->Write();

  m_pTreeFile->cd();

}

void HTPCAnalysisManager::EndOfRun(const G4Run *, G4int seed) {
  runTime->Stop();
  G4double dt = runTime->GetRealElapsed();

  // Info to the output file
  TParameter<G4double> *dtPar = new TParameter<G4double>("G4RUNTIME", dt);
  dtPar->Write();
  TParameter<G4int> *m_pRanSeed = new TParameter<int>("RANDOM_SEED", seed);
  m_pRanSeed->Write();

  m_pTreeFile->cd();

  m_pTreeFile->Write();
  m_pTreeFile->Close();
}

void
HTPCAnalysisManager::BeginOfEvent(const G4Event *)
{
  if(m_iDetectorHitsCollectionID == -1)
    {
      G4SDManager *pSDManager = G4SDManager::GetSDMpointer();
      m_iDetectorHitsCollectionID = pSDManager->GetCollectionID("HTPCDetectorHitsCollection");
    }
  //G4cout<<"The HC ID is "<<m_iDetectorHitsCollectionID<<G4endl;
}

void
HTPCAnalysisManager::EndOfEvent(const G4Event *pEvent)
{
  _events->cd();

  G4HCofThisEvent* pHCofThisEvent = pEvent->GetHCofThisEvent();
  //G4cout<<"pHCofThisEvent is"<<pHCofThisEvent<<G4endl;
  //G4cout << pHCofThisEvent->GetCapacity() << G4endl;
  //G4cout << pHCofThisEvent->GetNumberOfCollections() << G4endl;
  HTPCDetectorHitsCollection* pDetectorHitsCollection = 0;


  G4int iNbDetectorHits = 0;

  //G4cout<<"pDetectorHitsCollection is "<<pDetectorHitsCollection<<G4endl;
  //G4cout<<"m_iDetectorHitsCollectionID is "<<m_iDetectorHitsCollectionID<<G4endl;

  if(pHCofThisEvent)
    {
      if(m_iDetectorHitsCollectionID != -1)
	{
	  pDetectorHitsCollection = (HTPCDetectorHitsCollection *)(pHCofThisEvent->GetHC(m_iDetectorHitsCollectionID));
	  iNbDetectorHits = (pDetectorHitsCollection)?(pDetectorHitsCollection->entries()):(0);
	}
    }

  // G4cout<<"pDetectorHitsCollection is "<<pDetectorHitsCollection<<G4endl;
  // G4cout<<"pDetectorHitsCollection name is "<<pDetectorHitsCollection->GetName()<<G4endl;


  // get the event ID and primary particle information
  m_pEventData->m_iEventId = pEvent->GetEventID();
  m_pEventData->m_pPrimaryParticleType->push_back(m_pPrimaryGeneratorAction->GetParticleTypeOfPrimary());

  m_pEventData->m_fPrimaryX = m_pPrimaryGeneratorAction->GetPositionOfPrimary().x();
  m_pEventData->m_fPrimaryY = m_pPrimaryGeneratorAction->GetPositionOfPrimary().y();
  m_pEventData->m_fPrimaryZ = m_pPrimaryGeneratorAction->GetPositionOfPrimary().z();

  m_pEventData->m_fPrimaryCx = m_pPrimaryGeneratorAction->GetDirectionOfPrimary().x();
  m_pEventData->m_fPrimaryCy = m_pPrimaryGeneratorAction->GetDirectionOfPrimary().y();
  m_pEventData->m_fPrimaryCz = m_pPrimaryGeneratorAction->GetDirectionOfPrimary().z();

  m_pEventData->m_fForcedPrimaryX = m_pPrimaryGeneratorAction->GetForcedPositionOfPrimary().x();
  m_pEventData->m_fForcedPrimaryY = m_pPrimaryGeneratorAction->GetForcedPositionOfPrimary().y();
  m_pEventData->m_fForcedPrimaryZ = m_pPrimaryGeneratorAction->GetForcedPositionOfPrimary().z();

  m_pEventData->m_fPrimaryE = m_pPrimaryGeneratorAction->GetEnergyOfPrimary() / keV;
  m_pEventData->m_fPrimaryW = pEvent->GetPrimaryVertex()->GetWeight();


  G4int iNbSteps = 0;
  G4float fTotalEnergyDeposited = 0.;

  if(iNbDetectorHits)
    {
      //  hits
      //G4cout << " I HAVE " << iNbDetectorHits << " HITS " << G4endl;
      for(G4int i=0; i<iNbDetectorHits; i++)
	{
	  HTPCDetectorHit *pHit = (*pDetectorHitsCollection)[i];
	  if(pHit->GetParticleType() != "opticalphoton")
	    {
	      m_pEventData->m_pTrackId->push_back(pHit->GetTrackId());
	      m_pEventData->m_pParentId->push_back(pHit->GetParentId());

	      m_pEventData->m_pParticleType->push_back(pHit->GetParticleType());
	      m_pEventData->m_pParentType->push_back(pHit->GetParentType());
	      m_pEventData->m_pCreatorProcess->push_back(pHit->GetCreatorProcess());
	      m_pEventData->m_pDepositingProcess->push_back(pHit->GetDepositingProcess());

	      m_pEventData->m_pX->push_back(pHit->GetPosition().x()/mm);
	      m_pEventData->m_pY->push_back(pHit->GetPosition().y()/mm);
	      m_pEventData->m_pZ->push_back(pHit->GetPosition().z()/mm);

	      fTotalEnergyDeposited += pHit->GetEnergyDeposited()/keV;
	      m_pEventData->m_pEnergyDeposited->push_back(pHit->GetEnergyDeposited()/keV);
	      m_pEventData->m_pKineticEnergy->push_back(pHit->GetKineticEnergy()/keV);
	      m_pEventData->m_pPreStepEnergy->push_back(pHit->GetPreStepEnergy()/keV);
	      m_pEventData->m_pPostStepEnergy->push_back(pHit->GetPostStepEnergy()/keV);
	      m_pEventData->m_pTime->push_back(pHit->GetTime()/second);

	      iNbSteps++;
	    }
	  // G4cout <<"SUCCESS"<<G4endl;
	}
    }

  // also write the header information + primary vertex of the empty events....
  m_pEventData->m_iNbSteps = iNbSteps;
  m_pEventData->m_fTotalEnergyDeposited = fTotalEnergyDeposited;

  // save only energy depositing events
 if(writeEmptyEvents) {
    m_pTree->Fill(); // write all events to the tree
  } else {
    if(fTotalEnergyDeposited > 0. || iNbDetectorHits > 0) m_pTree->Fill(); // only events with some activity are written to the tree
  }

  m_pEventData->Clear();
  m_pTreeFile->cd();
}

void HTPCAnalysisManager::Step(const G4Step *)
{
}

