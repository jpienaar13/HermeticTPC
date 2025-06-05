#ifndef __XENON1TAMBEGENERATOR__
#define __XENON1TAMBEGENERATOR__

#include "Xenon1tGenericGenerator.hh"

// Include Geant4 headers
#include <globals.hh>
#include <G4ios.hh>
#include <G4SystemOfUnits.hh>

class Xenon1tAmBeGenerator : public Xenon1tGenericGenerator
{
public:
  Xenon1tAmBeGenerator();
  ~Xenon1tAmBeGenerator();
public:
  void GeneratePrimaryVertex(G4Event *pEvent);
  void SetAmBeGammaFraction(G4double gammaFraction);
private:
  G4double AmBe_SourceRadius = 2.0*mm;
  G4double AmBeGamma_Energy=4.438*MeV;
  G4double AmBeGamma_Fraction=0.6;
};
#endif
