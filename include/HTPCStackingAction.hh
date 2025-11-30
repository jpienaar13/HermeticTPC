#ifndef __HTPCSTACKINGACTION_H__
#define __HTPCSTACKINGACTION_H__


#include <globals.hh>

#include <G4UserStackingAction.hh>

class HTPCAnalysisManager;
class HTPCStackingActionMessenger;

class HTPCStackingAction : public G4UserStackingAction {
 public:
  HTPCStackingAction(HTPCAnalysisManager *pAnalysisManager = 0);
  ~HTPCStackingAction();

  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track *aTrack);
  virtual void NewStage();
  virtual void PrepareNewEvent();
  HTPCStackingActionMessenger *theMessenger;

 private:
  HTPCAnalysisManager *m_pAnalysisManager;
  G4bool PostponeFlag;
  G4double MaxLifeTime;
  G4String KillPostponedNucleusName = "None";
  G4StackManager* stackManager;


 public:
  inline void SetPostponeFlag(G4bool val) { PostponeFlag = val; };
  inline void SetMaxLifeTime(G4double val) { MaxLifeTime = val; };
  inline void SetKillPostponedNucleus(G4String val) { KillPostponedNucleusName = val; };
};

#endif
