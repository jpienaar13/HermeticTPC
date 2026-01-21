#ifndef __HTPCPhotoDetSensitiveDetector_H__
#define __HTPCPhotoDetSensitiveDetector_H__

#include <G4VSensitiveDetector.hh>

#include "HTPCPhotoDetHit.hh"

class G4Step;
class G4HCofThisEvent;

class HTPCPhotoDetSensitiveDetector: public G4VSensitiveDetector {
public:
	HTPCPhotoDetSensitiveDetector(G4String hName);
	~HTPCPhotoDetSensitiveDetector();

	void Initialize(G4HCofThisEvent *pHitsCollectionOfThisEvent);
	G4bool ProcessHits(G4Step *pStep, G4TouchableHistory *pHistory);
	void EndOfEvent(G4HCofThisEvent *pHitsCollectionOfThisEvent);

private:
	HTPCPhotoDetHitsCollection* m_pPhotoDetHitsCollection;
};

#endif

