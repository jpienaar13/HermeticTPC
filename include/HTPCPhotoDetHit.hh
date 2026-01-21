#ifndef __HTPCPhotoDetHit_H__
#define __HTPCPhotoDetHit_H__

#include <G4VHit.hh>
#include <G4THitsCollection.hh>
#include <G4Allocator.hh>
#include <G4ThreeVector.hh>

class HTPCPhotoDetHit: public G4VHit {
public:
	G4int operator==(const HTPCPhotoDetHit &) const;

	inline void* operator new(size_t);
	inline void  operator delete(void *);

	void Draw();
	void Print();

public:
	void SetPosition(G4ThreeVector hPosition) { m_hPosition = hPosition; }
	void SetDirection(G4ThreeVector hDirection) { m_hDirection = hDirection; }
    void SetTime(G4double dTime) { m_dTime = dTime; }
	void SetPhotoDetNb(G4int iPhotoDetNb) { m_iPhotoDetNb = iPhotoDetNb; }
    void SetTrackId(G4int iTrackId) { m_iTrackId = iTrackId; }
    void SetVolumeName(const G4String &hVolumeName) { m_pVolumeName = hVolumeName; }
    void SetEnergy(G4double dEnergy) { m_dEnergy = dEnergy; }
  
	G4ThreeVector GetPosition() const { return m_hPosition; }
    G4ThreeVector GetDirection() const { return m_hDirection; }
	G4double GetTime() const { return m_dTime; }
	G4int GetPhotoDetNb() const { return m_iPhotoDetNb; }
    G4int GetTrackId() const { return m_iTrackId; }
    const G4String &GetVolumeName() const { return m_pVolumeName; }
    G4double GetEnergy() const { return m_dEnergy; }
  
private:
  G4ThreeVector m_hPosition;
  G4ThreeVector m_hDirection;
  G4double m_dTime;
  G4int m_iPhotoDetNb;
  G4int m_iTrackId;
  G4String m_pVolumeName;
  G4double m_dEnergy;
};

typedef G4THitsCollection<HTPCPhotoDetHit> HTPCPhotoDetHitsCollection;

extern G4Allocator<HTPCPhotoDetHit> HTPCPhotoDetHitAllocator;

inline void* HTPCPhotoDetHit::operator new(size_t) {
	return((void *) HTPCPhotoDetHitAllocator.MallocSingle());
}

inline void HTPCPhotoDetHit::operator delete(void *pHTPCPhotoDetHit) {
	HTPCPhotoDetHitAllocator.FreeSingle((HTPCPhotoDetHit*) pHTPCPhotoDetHit);
}

#endif

