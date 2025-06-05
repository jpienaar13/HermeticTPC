#ifndef __XENON1TMULTIVERTEXGENERATOR__
#define __XENON1TMULTIVERTEXGENERATOR__

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

class Xenon1tMultiVertexGenerator : public Xenon1tGenericGenerator
{
public:
  Xenon1tMultiVertexGenerator();
  ~Xenon1tMultiVertexGenerator();
public:
  void ReadEventFromMultiFile();
  void GeneratePrimaryVertex(G4Event *pEvent);
private:
  std::fstream infile;

  G4int m_iNumberOfInteractionSites;
  vector<G4ThreeVector> *m_hInteractionVertexPosition;
  vector<int> *m_hNumberOfProducedParticles;
  vector<int> *m_hParticleCode;
  vector<double> *m_hParticleTime;
  vector<double> *m_hParticleEnergy;
  vector<double> *m_hS2timeWidth;
  vector<int> *m_hRecoil_type;

private:
  G4double GetPhotonTime(Int_t Recoil_type);
};
#endif
