#include "HTPCAnalysisManager.hh"
#include "HTPCStackingAction.hh"

// G4 Header Files
#include <G4Event.hh>
#include <G4EventManager.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTypes.hh>
#include <G4StackManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4Track.hh>
#include <G4TransportationManager.hh>
#include <G4VProcess.hh>
#include <G4ios.hh>

HTPCStackingAction::HTPCStackingAction(
    HTPCAnalysisManager *pAnalysisManager) {
  m_pAnalysisManager = pAnalysisManager;
  PostponeFlag = false;
  MaxLifeTime = 1 * ns;
}

HTPCStackingAction::~HTPCStackingAction() {}

G4ClassificationOfNewTrack HTPCStackingAction::ClassifyNewTrack(
    const G4Track *pTrack) {
  G4ClassificationOfNewTrack hTrackClassification = fUrgent;

  G4float timeP = pTrack->GetGlobalTime();
  G4String particleType = pTrack->GetDefinition()->GetParticleType();
  G4String volumeName = " ";
  if (particleType == "nucleus" && pTrack->GetParentID() > 0) {
    G4String processName = pTrack->GetCreatorProcess()->GetProcessName();
    G4Ions *ion = (G4Ions *)pTrack->GetDefinition();
    G4double lifetime = ion->GetPDGLifeTime();
    G4String particleName = pTrack->GetDefinition()->GetParticleName();
    volumeName = G4TransportationManager::GetTransportationManager()
                     ->GetNavigatorForTracking()
                     ->LocateGlobalPointAndSetup(pTrack->GetPosition())
                     ->GetName();
    G4int Z = ion->GetAtomicNumber();
    G4double excitationEnergy = ion->GetExcitationEnergy();
    G4int eventID =
        G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    if (lifetime > 1e18 * second) lifetime = -1;

  }
  if (pTrack->GetDefinition() == G4Triton::TritonDefinition()) {
    volumeName = G4TransportationManager::GetTransportationManager()
                     ->GetNavigatorForTracking()
                     ->LocateGlobalPointAndSetup(pTrack->GetPosition())
                     ->GetName();
    G4String processName = pTrack->GetCreatorProcess()->GetProcessName();
    G4ThreeVector position = pTrack->GetPosition();
    G4int eventID =
        G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
  }

  // Radioactive decays
  if (PostponeFlag) {
    // Postpone radioactive decays if you want, for backward compatability
    if (pTrack->GetDefinition()->GetParticleType() == "nucleus" &&
        !pTrack->GetDefinition()->GetPDGStable()) {
      //G4cout<<"Postpone decays "<<pTrack->GetDefinition()->GetParticleName()<<G4endl;
      if (pTrack->GetParentID() > 0 &&
          pTrack->GetCreatorProcess()->GetProcessName().find("RadioactiveDecay") != std::string::npos &&
          pTrack->GetDefinition()->GetPDGLifeTime() > MaxLifeTime)
          {
              hTrackClassification = fPostpone;
          }

      if (pTrack->GetDefinition()->GetParticleName() == KillPostponedNucleusName)
          {
             hTrackClassification = fKill;
          }

      }
  }

  return hTrackClassification;
}

void HTPCStackingAction::NewStage() {}

void HTPCStackingAction::PrepareNewEvent() {}





