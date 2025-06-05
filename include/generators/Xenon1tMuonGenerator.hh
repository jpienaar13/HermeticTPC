#ifndef __XENON1TMUONGENERATOR__
#define __XENON1TMUONGENERATOR__

// Include Geant4 headers
#include <globals.hh>
#include <G4ThreeVector.hh>
#include <G4PrimaryParticle.hh>
#include <G4ParticleMomentum.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4PrimaryVertex.hh>
#include <G4ios.hh>

#include "Xenon1tGenericGenerator.hh"

#include <fstream>

class Xenon1tMuonGenerator : public Xenon1tGenericGenerator
{
public:
  Xenon1tMuonGenerator();
  ~Xenon1tMuonGenerator();
public:
  G4String m_MuonType;
  void ReadEventFromMuonFile();
  void GeneratePrimaryVertex(G4Event *pEvent);
private:
  std::fstream infile;
  G4ThreeVector FindDiskPos(G4ThreeVector, G4ThreeVector, G4double);
  G4ThreeVector FindInitPos(G4ThreeVector, G4ThreeVector);
};
#endif
