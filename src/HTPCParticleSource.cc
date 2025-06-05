//Header Files
#include "HTPCParticleSource.hh"

HTPCParticleSource::HTPCParticleSource()
{
  m_pGenerator = new Xenon1tGenericGenerator();
  currentGenType = "generic";

  m_pMessenger = new HTPCParticleSourceMessenger(this);
}

HTPCParticleSource::~HTPCParticleSource()
{
  delete m_pMessenger;
  delete m_pGenerator;
}

void HTPCParticleSource::GeneratePrimaryVertex(G4Event *pEvent)
{
  //m_pGenerator->PrintCurrentGenericGeneratorParameters();
  m_pGenerator->GeneratePrimaryVertex(pEvent);
}

void HTPCParticleSource::GeneratePrimaryVertexFromTrack(G4Track *pTrack,
    G4Event *pEvent)
{
  m_pGenerator->GeneratePrimaryVertexFromTrack(pTrack, pEvent);
}

Xenon1tGenericGenerator * HTPCParticleSource::GetCurrentGenerator()
{
  return m_pGenerator;
}

G4String HTPCParticleSource::GetCurrentGeneratorType()
{
  return currentGenType;
}

void HTPCParticleSource::SetGeneratorType(G4String genType)
{
  if(genType != currentGenType)
  {
    G4cout << "HTPCParticleSource::SetGeneratorType(): Previous type [ "
           << currentGenType << " ] , new type [ " << genType << " ]." << G4endl;

    delete m_pGenerator;

    if (genType == "generic")
    {
      m_pGenerator = new Xenon1tGenericGenerator();
      currentGenType = "generic";
    }
    else if (genType == "muon")
    {
      m_pGenerator = new Xenon1tMuonGenerator();
      currentGenType = "muon";
    }
    else if (genType == "decay0")
    {
      m_pGenerator = new Xenon1tDecay0Generator();
      currentGenType = "decay0";
    }
    else if (genType == "multivertex")
    {
      m_pGenerator = new Xenon1tMultiVertexGenerator();
      currentGenType = "multivertex";
    }
    else if (genType == "ambe")
    {
      m_pGenerator = new Xenon1tAmBeGenerator();
      currentGenType = "ambe";
    }
    else
    {
      G4cout << "HTPCParticleSource: ERROR - Unknown particle generator type [ "
             << genType << " ] !" << G4endl;

      m_pGenerator = new Xenon1tGenericGenerator();
      currentGenType = "generic";
    }
  }

  PrintCurrentGeneratorType();
}

G4bool HTPCParticleSource::ValidateGeneratorType(G4String gt)
{
  return (currentGenType == gt);
}

void HTPCParticleSource::PrintCurrentGeneratorType()
{
  G4cout << "HTPCParticleSource: Selected particle generator type is [ "
         << currentGenType << " ]."  << G4endl;
}