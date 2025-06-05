#include "Xenon1tDecay0Generator.hh"

#include <G4SystemOfUnits.hh>
#include <Randomize.hh>

using namespace std;

Xenon1tDecay0Generator::Xenon1tDecay0Generator() : Xenon1tGenericGenerator()
{
  param->first = true;
  stats = true;

  m_hParticleMomentum = new vector<G4ParticleMomentum>;
  m_hNumberOfProducedParticles = new vector<int>;
  m_hParticleCode = new vector<int>;
  m_hParticleTime = new vector<double>;
  m_dTotalEnergyDecay0 = 0.;
}

Xenon1tDecay0Generator::~Xenon1tDecay0Generator()
{
  delete m_hParticleMomentum;
  delete m_hNumberOfProducedParticles;
  delete m_hParticleCode;
  delete m_hParticleTime;
}

void Xenon1tDecay0Generator::GeneratePrimaryVertex(G4Event *pEvent)
{
  param->m_hParticleType->clear();
  m_dTotalEnergyDecay0 = 0.;

  ReadEventFromDecay0File();

  // generate it according to the specification found in the macro
  ////////////////////////////////////////////////////////////

  // Position
  G4bool srcconf = false;
  G4int LoopCount = 0;

  while (srcconf == false)
  {
    if (param->m_hSourcePosType == "Point")
      GeneratePointSource();
    else if (param->m_hSourcePosType == "Volume")
      GeneratePointsInVolume();
    else if (param->m_hSourcePosType == "Surface")
      GeneratePointsInSurface();
    else
    {
      G4cout << "Error: SourcePosType undefined" << G4endl;
      G4cout << "Generating point source" << G4endl;
      GeneratePointSource();
    }

    if (param->m_bConfine == true)
    {
      if (pEvent->GetEventID() == 0) GetConfinedVolume();
      srcconf = IsSourceConfined();
      // if source in confined srcconf = true terminating the loop
      // if source isnt confined srcconf = false and loop continues
    }
    else if (param->m_bConfine == false)
      srcconf = true;  // terminate loop

    LoopCount++;
    if (LoopCount == 1000000)
    {
      G4cout << "*************************************" << G4endl;
      G4cout << "LoopCount = 1000000" << G4endl;
      G4cout << "Either the source distribution >> confinement" << G4endl;
      G4cout << "or any confining volume may not overlap with" << G4endl;
      G4cout << "the source distribution or any confining volumes" << G4endl;
      G4cout << "may not exist" << G4endl;
      G4cout << "If you have set confine then this will be ignored" << G4endl;
      G4cout << "for this event." << G4endl;
      G4cout << "*************************************" << G4endl;
      srcconf = true;  // Avoids an infinite loop
    }
  }

  // DECAY0 file
  if(param->m_iVerbosityLevel > 1)
    G4cout << "fromDecay0File" << G4endl;

  pEvent->SetEventID(param->EvID);
  param->EvID += 1;
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle_def;
  

  for(int ip=0; ip<m_hNumberOfProducedParticles->back(); ip++)
  {
    G4PrimaryVertex *pVertex = 
      new G4PrimaryVertex(param->m_hParticlePosition, (*m_hParticleTime)[ip]);

    particle_def = particleTable->FindParticle((*m_hParticleCode)[ip]);
    param->m_pParticleDefinition = particle_def;
    // Set mass, direction and momentum (kinE)
    G4PrimaryParticle *particle =
      new G4PrimaryParticle(particle_def,
                            (*m_hParticleMomentum)[ip].x(),
                            (*m_hParticleMomentum)[ip].y(),
                            (*m_hParticleMomentum)[ip].z());

    G4double particleMass = param->m_pParticleDefinition->GetPDGMass();
    G4double particleMomentum2 =
      ((*m_hParticleMomentum)[ip].x()*(*m_hParticleMomentum)[ip].x())
      + ((*m_hParticleMomentum)[ip].y()*(*m_hParticleMomentum)[ip].y())
      + ((*m_hParticleMomentum)[ip].z()*(*m_hParticleMomentum)[ip].z());

    G4double particleEnergy = std::sqrt(particleMass*particleMass + particleMomentum2);

    if(param->m_iVerbosityLevel > 1)
    {
      G4cout << "************* Particle definition (" << ip << "): " << G4endl;
      G4cout << "Particle name: " << particle_def->GetParticleName() << G4endl;
      G4cout << "Particle mass: " << particle->GetMass() << G4endl;
      G4cout << "     Position: " << pVertex->GetPosition() << G4endl;
      G4cout << "     Momentum: " << particle->GetMomentum() << G4endl;
    }

    param->m_hParticleType->push_back(param->m_pParticleDefinition->GetParticleName());
    m_dTotalEnergyDecay0 += particleEnergy;
    pVertex->SetPrimary(particle);

    pEvent->AddPrimaryVertex(pVertex);
  }
}

G4double Xenon1tDecay0Generator::GetTotalEnergyDecay0()
{
  return m_dTotalEnergyDecay0;
}

void Xenon1tDecay0Generator::ReadEventFromDecay0File()
{
  // Read raw file and determine data structure
  char* context = NULL;
  const char* DELIMITER = " ";
  char* token[100] = {}; // initialize to 0
  char* buf = 0;

  if ((param->first == false) && (!raw.good()))
  {
    raw.close();
    if (param->m_iVerbosityLevel >= 1)
    {
      G4cout << "EndOfFile - not enough DECAY0 events!" << G4endl;
    }
    param->first = true;
  }

  // only the first time open file
  if(param->first)
  {
    raw.open(param->m_hInputFileName);

    // for each event    - event's number, time of event's start,
    //                     number of emitted particles;
    // for each particle - GEANT number of particle,
    //                     (x,y,z) components of momentum,
    //                     time shift from previous time
    // check if "DECAY0" is the first tag
    while ((getline(raw, linebuffer)) &&
           (linebuffer[strspn(linebuffer, " \t\v\r\n")] == '\0'))
    {
      // Skip blank lines
    }
    buf = strdup(linebuffer.c_str());
    token[0] = strtok_r(buf, DELIMITER, &context); // first token
    if (strcmp (token[0],"DECAY0") != 0)
    {
      G4cout << "Error: DECAY0 file has unusual structure" << G4endl;
      return;
    }
    // skip header and read "first event" and "last event"
    while (strcmp (token[0],"First") != 0)
    {
      getline(raw, linebuffer);
      if (linebuffer[strspn(linebuffer, " \t\v\r\n")] != '\0')   // not blank
      {
        buf = strdup(linebuffer.c_str());
        token[0] = strtok_r(buf, DELIMITER, &context);
      }
    }
    getline(raw, linebuffer);
    buf = strdup(linebuffer.c_str());
    token[0] = strtok_r(buf, DELIMITER, &context);
    token[1] = strtok_r(0, DELIMITER, &context);
    param->first = false;

    if ((param->m_iVerbosityLevel >= 1) || (stats))
    {
      G4cout << "****************** DECAY0 GENERATOR IN ACTION ***************"
             << G4endl;
      G4cout << "********************* Open file " << param->m_hInputFileName <<  G4endl;
      G4cout << "*************************************************************"
             << G4endl;
      G4cout << "First event:      " << token[0] << G4endl;
      G4cout << "Full # of events: " << token[1] << G4endl;
      G4cout << "*************************************************************"
             << G4endl;
      stats = false;
    }

    while ((getline(raw, linebuffer)) &&
           (linebuffer[strspn(linebuffer, " \t\v\r\n")] == '\0'))
    {
      // Skip blank lines
    }
  }
  if (param->m_iVerbosityLevel >= 1)
  {
    G4cout << "Read Event!" << G4endl;
  }

  m_hNumberOfProducedParticles->clear();
  m_hParticleCode->clear();
  m_hParticleMomentum->clear();
  m_hParticleTime->clear();

  // read event
  buf = strdup(linebuffer.c_str());
  token[0] = strtok_r(buf, DELIMITER, &context);
  token[1] = strtok_r(0, DELIMITER, &context);
  token[2] = strtok_r(0, DELIMITER, &context);
  if (param->m_iVerbosityLevel >= 1)
  {
    G4cout << "Event #: " << token[0] << G4endl;
  }
  m_hNumberOfProducedParticles->push_back(atoi(token[2]));

  for(int ip=0; ip<m_hNumberOfProducedParticles->back(); ip++)
  {
    getline(raw, linebuffer);
    buf = strdup(linebuffer.c_str());
    token[0] = strtok_r(buf, DELIMITER, &context);
    token[1] = strtok_r(0, DELIMITER, &context);
    token[2] = strtok_r(0, DELIMITER, &context);
    token[3] = strtok_r(0, DELIMITER, &context);
    token[4] = strtok_r(0, DELIMITER, &context);
    m_hParticleCode->push_back(ConvertGeant3toGeant4ParticleCode(atoi(token[0])));
    m_hParticleMomentum->push_back(G4ThreeVector(atof(token[1])*MeV,
                                   atof(token[2])*MeV, atof(token[3])*MeV));
    m_hParticleTime->push_back(atof(token[4])*s);
  }

  while ((getline(raw, linebuffer)) &&
         (linebuffer[strspn(linebuffer, " \t\v\r\n")] == '\0'))
  {
    // Skip blank lines
  }
}

int Xenon1tDecay0Generator::ConvertGeant3toGeant4ParticleCode(int G3ParticleCode)
{
  // create several ParticleGuns
  /***************
   DECAY0 particle code - in accordance with GEANT 3.21 manual of October, 1994:
   1 - gamma         2 - positron     3 - electron
   4 - neutrino      5 - muon+        6 - muon-
   7 - pion0         8 - pion+        9 - pion-
  10 - kaon0 long   11 - kaon+       12 - kaon-
  13 - neutron      14 - proton      15 - antiproton
  16 - kaon0 short  17 - eta         18 - lambda
  19 - sigma+       20 - sigma0      21 - sigma-
  22 - xi0          23 - xi-         24 - omega
  25 - antineutron  26 - antilambda  27 - antisigma-
  28 - antisigma0   29 - antisigma+  30 - antixi0
  31 - antixi+      32 - antiomega+  45 - deuteron
  46 - tritium      47 - alpha       48 - geantino
  49 - He3          50 - Cerenkov
  0-dummy
  **************/

  switch(G3ParticleCode)
  {
  case 1   :
    return 22;       // photon
  case 25  :
    return -2112;    // anti-neutron
  case 2   :
    return -11;      // e+
  case 26  :
    return -3122;    // anti-Lambda
  case 3   :
    return 11;       // e-
  case 27  :
    return -3222;    // Sigma-
  case 4   :
    return 12;       // e-neutrino (NB: flavour undefined by Geant)
  case 28  :
    return -3212;    // Sigma0
  case 5   :
    return -13;      // mu+
  case 29  :
    return -3112;    // Sigma+ (PB)*/
  case 6   :
    return 13;       // mu-
  case 30  :
    return -3322;    // Xi0
  case 7   :
    return 111;      // pi0
  case 31  :
    return -3312;    // Xi+
  case 8   :
    return 211;      // pi+
  case 32  :
    return -3334;    // Omega+ (PB)
  case 9   :
    return -211;     // pi-
  case 33  :
    return -15;      // tau+
  case 10  :
    return 130;      // K long
  case 34  :
    return 15;       // tau-
  case 11  :
    return 321;      // K+
  case 35  :
    return 411;      // D+
  case 12  :
    return -321;     // K-
  case 36  :
    return -411;     // D-
  case 13  :
    return 2112;     // n
  case 37  :
    return 421;      // D0
  case 14  :
    return 2212;     // p
  case 38  :
    return -421;     // D0
  case 15  :
    return -2212;    // anti-proton
  case 39  :
    return 431;      // Ds+
  case 16  :
    return 310;      // K short
  case 40  :
    return -431;     // anti Ds-
  case 17  :
    return 221;      // eta
  case 41  :
    return 4122;     // Lamba_c+
  case 18  :
    return 3122;     // Lambda
  case 42  :
    return 24;       // W+
  case 19  :
    return 3222;     // Sigma+
  case 43  :
    return -24;      // W-
  case 20  :
    return 3212;     // Sigma0
  case 44  :
    return 23;       // Z
  case 21  :
    return 3112;     // Sigma-
  case 45  :
    return 0;        // deuteron
  case 22  :
    return 3322;     // Xi0
  case 46  :
    return 0;        // triton
  case 23  :
    return 3312;     // Xi-
  case 47  :
    return 0;        // alpha
  case 24  :
    return 3334;     // Omega- (PB)
  case 48  :
    return 0;        // G nu ? PDG ID 0 is undefined
  default  :
    return 0;
  }
}
