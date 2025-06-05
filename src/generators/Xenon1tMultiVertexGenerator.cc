#include "Xenon1tMultiVertexGenerator.hh"

#include <G4SystemOfUnits.hh>
#include <Randomize.hh>

using namespace std;

Xenon1tMultiVertexGenerator::Xenon1tMultiVertexGenerator()
{
  m_hInteractionVertexPosition = new vector<G4ThreeVector>;
  m_hNumberOfProducedParticles = new vector<int>;
  m_hParticleCode = new vector<int>;
  m_hParticleTime = new vector<double>;
  m_hParticleEnergy = new vector<double>;
  m_hRecoil_type = new vector<int>;
  m_hS2timeWidth = new vector<double>;
}

Xenon1tMultiVertexGenerator::~Xenon1tMultiVertexGenerator()
{
  delete m_hInteractionVertexPosition;
  delete m_hNumberOfProducedParticles;
  delete m_hParticleCode;
  delete m_hParticleTime;
  delete m_hParticleEnergy;
  delete m_hRecoil_type;
  delete m_hS2timeWidth;
}

void Xenon1tMultiVertexGenerator::GeneratePrimaryVertex(G4Event *pEvent)
{
  G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition *particle_def = particleTable->FindParticle("opticalphoton");

  param->m_pParticleDefinition = particle_def;

  ReadEventFromMultiFile();

  pEvent->SetEventID(param->EvID);
  for (G4int i = 0; i < m_iNumberOfInteractionSites; ++i)
  {
    for (G4int j = 0; j < (*m_hNumberOfProducedParticles)[i]; j++)
    {
      G4double mass = particle_def->GetPDGMass();

      // Generate random direction
      G4double cost = 1. - 2. * G4UniformRand();
      G4double sint = std::sqrt((1. - cost) * (1. + cost));
      G4double phi = CLHEP::twopi * G4UniformRand();
      G4double sinp = std::sin(phi);
      G4double cosp = std::cos(phi);
      G4double px = sint * cosp;
      G4double py = sint * sinp;
      G4double pz = cost;
      // Create momentum direction vector
      G4ParticleMomentum photonMomentum(px, py, pz);

      // Determine polarization of new photon
      G4double sx = cost * cosp;
      G4double sy = cost * sinp;
      G4double sz = -sint;
      G4ThreeVector photonPolarization(sx, sy, sz);
      G4ThreeVector perp = photonMomentum.cross(photonPolarization);
      phi = CLHEP::twopi * G4UniformRand();
      sinp = std::sin(phi);
      cosp = std::cos(phi);
      photonPolarization = cosp * photonPolarization + sinp * perp;
      photonPolarization = photonPolarization.unit();

      G4double time_for_photon = 0. * ns;

      if ((*m_hS2timeWidth)[i] != 0.)
      {
        // it is S2
        time_for_photon =
          G4RandGauss::shoot((*m_hParticleTime)[i], (*m_hS2timeWidth)[i]);
      }
      else
      {
        // it is S1
        time_for_photon =
          (*m_hParticleTime)[i] + GetPhotonTime((*m_hRecoil_type)[i]);
      }

      G4PrimaryVertex *pVertex = new G4PrimaryVertex(
        (*m_hInteractionVertexPosition)[i], time_for_photon);

      G4PrimaryParticle *particle =
        new G4PrimaryParticle(particle_def, px, py, pz);
      particle->SetKineticEnergy((*m_hParticleEnergy)[i]);
      particle->SetMass(mass);
      particle->SetPolarization(photonPolarization.x(),
                                photonPolarization.y(),
                                photonPolarization.z());

      //G4cout << time_for_photon << G4endl;

      pVertex->SetPrimary(particle);
      pEvent->AddPrimaryVertex(pVertex);
    }  // end loop on photons
  }    // end loop on interaction sites
}

void Xenon1tMultiVertexGenerator::ReadEventFromMultiFile()
{
  // only the first time open file
  if (param->first)
  {
    infile.open(param->m_hInputFileName);

    G4cout << "****************** WAVEFORM GENERATOR IN ACTION *************"
           << G4endl;
    G4cout << "********************* Open file " << param->m_hInputFileName << G4endl;
    G4cout << "*************************************************************"
           << G4endl;

    param->first = false;
  }

  G4float ev[7];
  m_iNumberOfInteractionSites = 0;
  m_hInteractionVertexPosition->clear();
  m_hNumberOfProducedParticles->clear();
  m_hParticleCode->clear();
  m_hParticleTime->clear();
  m_hRecoil_type->clear();
  m_hS2timeWidth->clear();
  m_hParticleEnergy->clear();

  if (!infile.good())
  {
    infile.close();
    infile.open(param->m_hInputFileName);
    G4cout << "EndOfFile ... Open again file " << param->m_hInputFileName << G4endl;
  }

  infile >> param->EvID >> m_iNumberOfInteractionSites;

  // loop on the interaction sites
  for (int ip = 0; ip < m_iNumberOfInteractionSites; ip++)
  {
    // loop to get info on a specific interaction site
    for (int i = 0; i < 7; i++)
    {
      infile >> ev[i];
    }

    // position of the spcefici interaction site
    m_hInteractionVertexPosition->push_back(
      G4ThreeVector(ev[0] * mm, ev[1] * mm, ev[2] * mm));
    // time of the spcefici interaction site
    m_hParticleTime->push_back(ev[3] * ns);
    // number of particels to generate in that site
    m_hNumberOfProducedParticles->push_back(ev[4]);
    // Recoil Type
    m_hRecoil_type->push_back(ev[5]);
    // S2 time width (set to 0 for S1)
    m_hS2timeWidth->push_back(ev[6] * ns);
    // particle's energy
    m_hParticleEnergy->push_back(6.98 * eV);
  }
}

G4double Xenon1tMultiVertexGenerator::GetPhotonTime(Int_t Recoil_type)
{
  G4double aSecondaryTime = 0. * ns;

  if (Recoil_type == 0)  // electron recoil
    aSecondaryTime -= (8.96115 * ns) * log(G4UniformRand());
  else  // nuclear recoil
    aSecondaryTime -= 3.33502 * log(G4UniformRand());

  if (aSecondaryTime < 0.) aSecondaryTime = 0. * ns;

  return aSecondaryTime;
}
