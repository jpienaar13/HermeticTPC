#ifndef __HTPCSTACKINGACTIONMESSENGER_H__
#define __HTPCSTACKINGACTIONMESSENGER_H__

#include "G4UImessenger.hh"
#include "HTPCStackingAction.hh"
#include "globals.hh"

class HTPCStackingAction;
class G4UIcmdWithABool;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;

class HTPCStackingActionMessenger : public G4UImessenger
{
public:
  HTPCStackingActionMessenger(HTPCStackingAction* pStackAction);
  ~HTPCStackingActionMessenger();

public:
  void SetNewValue(G4UIcommand*, G4String);

private:
  HTPCStackingAction* HTPCAction;

private:
  G4UIcmdWithABool* PostponeCmd;
  G4UIcmdWithADoubleAndUnit* MaxLifeTimeCmd;
  G4UIcmdWithAString* KillPostponedNucleusCmd;
};

#endif