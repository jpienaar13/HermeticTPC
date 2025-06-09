#include "HTPCStackingActionMessenger.hh"
#include <sstream>
#include "HTPCStackingAction.hh"

#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcommand.hh"
#include "globals.hh"

HTPCStackingActionMessenger::HTPCStackingActionMessenger(
  HTPCStackingAction* msa)
  : HTPCAction(msa)
{
  PostponeCmd = new G4UIcmdWithABool("/xe/Postponedecay", this);
  PostponeCmd->SetGuidance("Postpone radioactive decays");
  PostponeCmd->SetGuidance("Default = false");
  PostponeCmd->SetParameterName("PostponeFlag", false);
  PostponeCmd->SetDefaultValue(false);

  MaxLifeTimeCmd = new G4UIcmdWithADoubleAndUnit("/xe/MaxLifeTime", this);
  MaxLifeTimeCmd->SetGuidance("Define max lifetime for daughter isotopes to be kept in one event");
  MaxLifeTimeCmd->SetGuidance("Default = 1 ns");
  MaxLifeTimeCmd->SetParameterName("MaxLifeTime", false);
  MaxLifeTimeCmd->SetDefaultValue(1.);
  MaxLifeTimeCmd->SetUnitCategory("Time");
  MaxLifeTimeCmd->SetDefaultUnit("ns");

  KillPostponedNucleusCmd = new G4UIcmdWithAString("/xe/KillPostponedNucleusName", this);
  KillPostponedNucleusCmd->SetGuidance("Kill the track with the specified nucleus name if it was postponed "
                                       "(to stop decay chains at a given step)");
  KillPostponedNucleusCmd->SetParameterName("KillPostNuc", false);
  KillPostponedNucleusCmd->SetDefaultValue("None");
  KillPostponedNucleusCmd->AvailableForStates(G4State_Idle);
}

HTPCStackingActionMessenger::~HTPCStackingActionMessenger()
{
  delete PostponeCmd;
  delete MaxLifeTimeCmd;
  delete KillPostponedNucleusCmd;
}

void HTPCStackingActionMessenger::SetNewValue(G4UIcommand* command,
    G4String newValue)
{
  if (command == PostponeCmd)
    HTPCAction->SetPostponeFlag(PostponeCmd->GetNewBoolValue(newValue));

  if (command == MaxLifeTimeCmd)
    HTPCAction->SetMaxLifeTime(MaxLifeTimeCmd->GetNewDoubleValue(newValue));

  if (command == KillPostponedNucleusCmd)
    HTPCAction->SetKillPostponedNucleus(newValue);
}