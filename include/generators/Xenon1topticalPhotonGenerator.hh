#ifndef __XENON1TOPTICALPHOTONGENERATOR__
#define __XENON1TOPTICALPHOTONGENERATOR__

#include "Xenon1tGenericGenerator.hh"

// Include Geant4 headers
#include <globals.hh>
#include <G4ios.hh>
#include <G4SystemOfUnits.hh>


class Xenon1topticalPhotonGenerator : public Xenon1tGenericGenerator
{
public:
  Xenon1topticalPhotonGenerator();
  ~Xenon1topticalPhotonGenerator();
public:
  void GeneratePrimaryVertex(G4Event *pEvent);
private:
  G4double LXeMedium_oD = 3100. -2*10. *mm; // iCryostat_oD - 2*iCryostatWall_thickness
  G4double LXeMedium_H = 3900. * 0.85 *mm; // iCryostat_H * LiquidGasRatio 
  G4double opticalphoton_Energy = 6.98*eV;

};
#endif
