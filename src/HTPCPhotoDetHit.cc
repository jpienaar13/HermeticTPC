#include <G4UnitsTable.hh>
#include <G4VVisManager.hh>
#include <G4Circle.hh>
#include <G4Colour.hh>
#include <G4VisAttributes.hh>

#include "G4SystemOfUnits.hh"
#include "HTPCPhotoDetHit.hh"

typedef G4THitsCollection<HTPCPhotoDetHit> HTPCPhotoDetHitsCollection;

G4Allocator<HTPCPhotoDetHit> HTPCPhotoDetHitAllocator;

G4int HTPCPhotoDetHit::operator==(const HTPCPhotoDetHit &hHTPCPhotoDetHit) const {
	return ((this == &hHTPCPhotoDetHit) ? (1) : (0));
}

void HTPCPhotoDetHit::Draw()
{
    //G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    
    //if(pVVisManager)
    //{
    //    G4Circle hCircle(m_hPosition);
    //    G4Colour hColour(1.000, 0.973, 0.184);
    //    G4VisAttributes hVisAttributes(hColour);
    //    
    //    hCircle.SetScreenSize(0.1);
    //    hCircle.SetFillStyle(G4Circle::filled);
    //    hCircle.SetVisAttributes(hVisAttributes);
    //    pVVisManager->Draw(hCircle);
    //}
}

void HTPCPhotoDetHit::Print()
{
	G4cout << "PhotoDet hit ---> " 
		<< "PhotoDet#" << m_iPhotoDetNb
		<< " Position: " << m_hPosition.x()/mm
		<< " " << m_hPosition.y()/mm
		<< " " << m_hPosition.z()/mm
		<< " mm"
		<< " Time: " << m_dTime/s << " s" << G4endl;
}

