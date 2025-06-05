#ifndef __HTPCPARTICLESOURCE_H__
#define __HTPCPARTICLESOURCE_H__

// XENON Header Files
#include "generators/Xenon1tGenericGenerator.hh"
#include "generators/Xenon1tMuonGenerator.hh"
#include "generators/Xenon1tDecay0Generator.hh"
#include "generators/Xenon1tMultiVertexGenerator.hh"
#include "generators/Xenon1tAmBeGenerator.hh"

#include "HTPCParticleSourceMessenger.hh"

// Additional Header Files
#include <cmath>
#include <fstream>
#include <set>
#include <sstream>
#include <vector>

using std::set;
using std::vector;

// Root Header Files
#include <TFile.h>
#include "TH1.h"

// G4 Header Files
#include <G4GeneralParticleSource.hh>
#include <G4Navigator.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleMomentum.hh>
#include <G4Track.hh>
#include <G4VPrimaryGenerator.hh>

class Xenon1tGenericGenerator;
class Xenon1tMuonGenerator;
class Xenon1tDecay0Generator;
class Xenon1tMultiVertexGenerator;
class Xenon1tAmBeGenerator;

class HTPCParticleSource : public G4VPrimaryGenerator
{
public:
  HTPCParticleSource();
  ~HTPCParticleSource();

public:
  void GeneratePrimaryVertex(G4Event *pEvent);
  void GeneratePrimaryVertexFromTrack(G4Track *pTrack, G4Event *pEvent);

  Xenon1tGenericGenerator * GetCurrentGenerator();

  G4String GetCurrentGeneratorType();
  void SetGeneratorType(G4String GenType_str);
  void PrintCurrentGeneratorType();
  G4bool ValidateGeneratorType(G4String gt);

private:
  G4String currentGenType;
  Xenon1tGenericGenerator *m_pGenerator;
  HTPCParticleSourceMessenger *m_pMessenger;
};

#endif