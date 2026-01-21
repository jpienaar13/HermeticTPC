#include "Xenon1topticalPhotonGenerator.hh"

#include <G4SystemOfUnits.hh>
#include <Randomize.hh>

using namespace std;

Xenon1topticalPhotonGenerator::Xenon1topticalPhotonGenerator()
{
}

Xenon1topticalPhotonGenerator::~Xenon1topticalPhotonGenerator()
{
}

void Xenon1topticalPhotonGenerator::GeneratePrimaryVertex(G4Event *pEvent)
{
  G4ThreeVector gunCenter = param->m_hCenterCoords;

  param->m_pParticleDefinition = G4ParticleTable::GetParticleTable()->FindParticle("opticalphoton");
  param->m_hSourcePosType = "Volume";
  param->m_hEnergyDisType = "Mono";
  param->m_hShape = "Cylinder";
  param->m_hAngDistType = "iso";
  param->m_dMonoEnergy = opticalphoton_Energy;
  param->m_dRadius2 = LXeMedium_oD; // need the LXe Volume radius
  param->m_dHalfz = LXeMedium_H; // need the LXe Volume half-height
  param->m_dThickness_bottom = 0.;
  param->m_dThickness_top = 0.;
  param->m_dInnerRadius2 = 0.;
  Xenon1tGenericGenerator::GeneratePrimaryVertex(pEvent);

}
