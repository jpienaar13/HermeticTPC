#include "Xenon1tAmBeGenerator.hh"

#include <Randomize.hh>

using namespace std;

Xenon1tAmBeGenerator::Xenon1tAmBeGenerator()
{
}

Xenon1tAmBeGenerator::~Xenon1tAmBeGenerator()
{
}

void Xenon1tAmBeGenerator::SetAmBeGammaFraction(G4double gammaFraction)
{
  G4cout << "Xenon1tAmBeGenerator: Setting the gamma to neutron ratio to " << gammaFraction << G4endl;
  AmBeGamma_Fraction = gammaFraction;
}

void Xenon1tAmBeGenerator::GeneratePrimaryVertex(G4Event *pEvent)
{
  G4ThreeVector gunCenter = param->m_hCenterCoords;

  param->m_pParticleDefinition = G4ParticleTable::GetParticleTable()->FindParticle("neutron");
  param->m_hSourcePosType = "Volume";
  param->m_hEnergyDisType = "Spectrum";
  param->m_hShape = "Sphere";
  param->m_hAngDistType = "iso";
  param->m_dRadius = AmBe_SourceRadius;
  Xenon1tGenericGenerator::GeneratePrimaryVertex(pEvent);

  if(G4UniformRand() <= AmBeGamma_Fraction)
  {
    G4double neutronEnergy = param->m_dParticleEnergy;

    param->m_pParticleDefinition = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
    param->m_hCenterCoords = param->m_hParticlePosition;
    param->m_hSourcePosType = "Point";
    param->m_hEnergyDisType = "Mono";
    param->m_dMonoEnergy = AmBeGamma_Energy;
    Xenon1tGenericGenerator::GeneratePrimaryVertex(pEvent);
    param->m_dParticleEnergy += neutronEnergy;
  }

  param->m_hCenterCoords = gunCenter;
}
