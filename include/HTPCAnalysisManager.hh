#ifndef __HTPCPANALYSISMANAGER_H__
#define __HTPCPANALYSISMANAGER_H__

#include <globals.hh>

#include <TParameter.h>
#include <TDirectory.h>
#include <G4Timer.hh>
#include <G4ThreeVector.hh>

class G4Run;
class G4Event;
class G4Step;

class TFile;
class TTree;

class HTPCEventData;
class HTPCPrimaryGeneratorAction;

class HTPCAnalysisManager
{
public:
  HTPCAnalysisManager(HTPCPrimaryGeneratorAction *pPrimaryGeneratorAction);
  virtual ~HTPCAnalysisManager();

public:
  virtual void BeginOfRun(const G4Run *pRun);
  virtual void EndOfRun(const G4Run *pRun, G4int seed);
  virtual void BeginOfEvent(const G4Event *pEvent);
  virtual void EndOfEvent(const G4Event *pEvent);
  virtual void Step(const G4Step *pStep);

  void SetDataFilename(const G4String &hFilename) { m_hDataFilename = hFilename; }
  void SetNbEventsToSimulate(G4int iNbEventsToSimulate) { m_iNbEventsToSimulate = iNbEventsToSimulate;}

  void FillParticleInSave(G4int flag, G4int partPDGcode, G4ThreeVector pos, G4ThreeVector dir, G4float nrg, G4float time, G4int trackID);


private:
  G4bool FilterEvent(HTPCEventData *pEventData);

private:
  G4int m_iDetectorHitsCollectionID;

  G4String m_hDataFilename;
  G4int m_iNbEventsToSimulate;

  TFile      *m_pTreeFile;
  TTree      *m_pTree;
  TDirectory *_events;

  TParameter<int> *m_pNbEventsToSimulateParameter;

  HTPCPrimaryGeneratorAction *m_pPrimaryGeneratorAction;

  HTPCEventData *m_pEventData;
  G4bool            plotPhysics;

  G4Timer *runTime;
  G4bool            writeEmptyEvents;
};

#endif // __XENON10PANALYSISMANAGER_H__

