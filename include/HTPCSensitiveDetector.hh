#ifndef __HTPCSENSITIVEDETECTOR_H__
#define __HTPCSENSITIVEDETECTOR_H__

#include <G4VSensitiveDetector.hh>

#include "HTPCDetectorHit.hh"

class G4Step;
class G4HCofThisEvent;

class HTPCSensitiveDetector: public G4VSensitiveDetector
{
public:
	HTPCSensitiveDetector(G4String hName);
	~HTPCSensitiveDetector();

	void Initialize(G4HCofThisEvent *pHitsCollectionOfThisEvent);
	G4bool ProcessHits(G4Step *pStep, G4TouchableHistory *pHistory);
	void EndOfEvent(G4HCofThisEvent *pHitsCollectionOfThisEvent);

private:
	HTPCDetectorHitsCollection* m_pHTPDetectorHitsCollection;
        std::map<int,G4String> m_hParticleTypes;
};

#endif