#include "HTPCSteppingAction.hh"
#include "HTPCAnalysisManager.hh"

#include "G4SteppingManager.hh"

#include <string.h>
#include <cmath>

HTPCSteppingAction::HTPCSteppingAction(HTPCAnalysisManager *myAM):myAnalysisManager(myAM)
{
}

void HTPCSteppingAction::UserSteppingAction(const G4Step* aStep)
{
    G4int  trackID = aStep->GetTrack()->GetTrackID();
    particle = aStep->GetTrack()->GetDefinition()->GetParticleName();
    G4int particlePDGcode = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
    //G4float xP = aStep->GetPostStepPoint()->GetPosition().x();
    //G4float yP = aStep->GetPostStepPoint()->GetPosition().y();
    //G4float zP = aStep->GetPostStepPoint()->GetPosition().z();
    G4float eP = aStep->GetPostStepPoint()->GetKineticEnergy();
    G4float timeP = aStep->GetPostStepPoint()->GetGlobalTime();
    //G4float eDep = aStep->GetTotalEnergyDeposit();

    // Direction of the particle Pre
    //  G4ParticleMomentum *Momentum = aStep->GetPostStepPoint()->GetMomentum();
    G4float preMomModulo = sqrt( pow(aStep->GetPreStepPoint()->GetMomentum().x(),2) +
                                pow(aStep->GetPreStepPoint()->GetMomentum().y(),2) +
                                pow(aStep->GetPreStepPoint()->GetMomentum().z(),2) );
    G4ThreeVector preDirection( aStep->GetPreStepPoint()->GetMomentum().x()/preMomModulo ,
                               aStep->GetPreStepPoint()->GetMomentum().y()/preMomModulo ,
                               aStep->GetPreStepPoint()->GetMomentum().z()/preMomModulo );

    // Direction of the particle Post
    //  G4ParticleMomentum *Momentum = aStep->GetPostStepPoint()->GetMomentum();
    G4float MomModulo = sqrt( pow(aStep->GetPostStepPoint()->GetMomentum().x(),2) +
                             pow(aStep->GetPostStepPoint()->GetMomentum().y(),2) +
                             pow(aStep->GetPostStepPoint()->GetMomentum().z(),2) );
    G4ThreeVector direction( aStep->GetPostStepPoint()->GetMomentum().x()/MomModulo ,
                            aStep->GetPostStepPoint()->GetMomentum().y()/MomModulo ,
                            aStep->GetPostStepPoint()->GetMomentum().z()/MomModulo );

}



