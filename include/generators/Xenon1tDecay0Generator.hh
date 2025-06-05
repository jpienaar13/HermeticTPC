#ifndef __XENON1TDECAY0GENERATOR__
#define __XENON1TDECAY0GENERATOR__

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

class Xenon1tDecay0Generator : public Xenon1tGenericGenerator
{
public:
  Xenon1tDecay0Generator();
  ~Xenon1tDecay0Generator();
public:
  void ReadEventFromDecay0File();
  int ConvertGeant3toGeant4ParticleCode(int G3ParticleCode);
  G4double GetTotalEnergyDecay0();
  void GeneratePrimaryVertex(G4Event *pEvent);

private:
  G4bool stats;
  std::ifstream raw;
  G4String linebuffer;

  vector<G4ParticleMomentum>	*m_hParticleMomentum;
  vector<int> *m_hNumberOfProducedParticles;
  vector<int> *m_hParticleCode;
  vector<double> *m_hParticleTime;
  G4double m_dTotalEnergyDecay0;
};
#endif
