#ifndef HTPCSteppingAction_H
#define HTPCSteppingAction_H 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class HTPCAnalysisManager;

class HTPCSteppingAction : public G4UserSteppingAction
{
public:
  HTPCSteppingAction(HTPCAnalysisManager* );
  ~HTPCSteppingAction(){};

  void UserSteppingAction(const G4Step*);

private:

G4String particle;
HTPCAnalysisManager* myAnalysisManager;


};

#endif