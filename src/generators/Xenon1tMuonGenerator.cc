#include "Xenon1tMuonGenerator.hh"

#include <G4SystemOfUnits.hh>
#include <Randomize.hh>

using namespace std;

Xenon1tMuonGenerator::Xenon1tMuonGenerator()
{
}

Xenon1tMuonGenerator::~Xenon1tMuonGenerator()
{
}

void Xenon1tMuonGenerator::GeneratePrimaryVertex(G4Event *pEvent)
{
  m_MuonType = "mu-";
  ReadEventFromMuonFile();

  param->m_dParticleTime = 0.0;
  param->m_pParticleDefinition = G4ParticleTable::GetParticleTable()->FindParticle(m_MuonType);

  G4double mass = param->m_pParticleDefinition->GetPDGMass();
  G4double energy = param->m_dParticleEnergy + mass;
  G4double pmom = std::sqrt(energy * energy - mass * mass);
  G4double px = pmom * param->m_hParticleMomentumDirection.x();
  G4double py = pmom * param->m_hParticleMomentumDirection.y();
  G4double pz = pmom * param->m_hParticleMomentumDirection.z();

  G4PrimaryParticle *particle =
    new G4PrimaryParticle(param->m_pParticleDefinition, px, py, pz);
  particle->SetMass(mass);
  particle->SetCharge(param->m_dParticleCharge);
  particle->SetPolarization(param->m_hParticlePolarization.x(),
                            param->m_hParticlePolarization.y(),
                            param->m_hParticlePolarization.z());

  G4PrimaryVertex * pVertex = new G4PrimaryVertex(param->m_hParticlePosition, param->m_dParticleTime);
  pVertex->SetPrimary(particle);
  pEvent->AddPrimaryVertex(pVertex);
}

void Xenon1tMuonGenerator::ReadEventFromMuonFile()
{
  G4ThreeVector rotX(1., 0., 0.), rotZ(0., 0., 1.);
  G4ThreeVector cosmu, atmp, btmp, xyzdisk;
  G4ThreeVector initialPos;

  G4int evi[2];
  G4float ev[7];

  // only the first time open file
  if (param->first)
  {
    infile.open(param->m_hInputFileName);
    param->first = false;

    G4double rnd = G4UniformRand();
    G4int iSkip = floor(rnd * 1000000);
    G4cout << "Open MUON file " << param->m_hInputFileName
           << " - Skip first " << iSkip << " lines. " << G4endl;

    for (int is = 0; is < iSkip;
         ++is)    // skip the first iSkip lines of the input file
    {
      for (int i = 0; i < 2; i++) infile >> evi[i];
      for (int i = 0; i < 7; i++) infile >> ev[i];
    }
  }

  if (infile.good())
  {
    for (int i = 0; i < 2; i++) infile >> evi[i];
    for (int i = 0; i < 7; i++) infile >> ev[i];
  }
  else
  {
    infile.close();
    infile.open(param->m_hInputFileName);
    G4cout << "EndOfFile ... Open again file " << param->m_hInputFileName << G4endl;
    for (int i = 0; i < 2; i++) infile >> evi[i];
    for (int i = 0; i < 7; i++) infile >> ev[i];
  }

  // Extract muon type
  if(evi[1] == 10) m_MuonType ="mu+";
  else if(evi[1] == 11) m_MuonType ="mu-";
  param->m_pParticleDefinition = G4ParticleTable::GetParticleTable()->FindParticle(m_MuonType);

  cosmu[0] = ev[4];
  cosmu[1] = ev[5];
  cosmu[2] = ev[6];

  atmp = rotX.cross(cosmu);
  btmp = cosmu.cross(rotZ);

  if (fabs(atmp[1]) < fabs(btmp[1]))
  {
    atmp = btmp.cross(cosmu);
  }
  else
  {
    btmp = cosmu.cross(atmp);
  }

  atmp.setMag(1.0);
  btmp.setMag(1.0);

  // Rmax is the radius of the disk orthogonal to the muon direction where the
  // impact point is uniformly sampled
  G4double Rmax = 15. * m;

  xyzdisk = FindDiskPos(atmp, btmp, Rmax);
  initialPos = FindInitPos(cosmu, xyzdisk);

  param->m_hParticlePosition = initialPos;
  param->m_hParticleMomentumDirection = G4ParticleMomentum(cosmu[0], cosmu[1], cosmu[2]);
  param->m_dParticleEnergy = ev[0] * GeV;
}

G4ThreeVector Xenon1tMuonGenerator::FindDiskPos(G4ThreeVector a, G4ThreeVector b, G4double R)
{
  G4double r = R * sqrt(G4UniformRand());
  G4double alpha = (G4UniformRand() * 2 * acos(-1.));
  G4double xr = r * cos(alpha);
  G4double zr = r * sin(alpha);
  G4ThreeVector anew, bnew, xyz;

  anew = a*xr;
  bnew = b*zr;
  xyz = anew+bnew;
  return xyz;
}

G4ThreeVector Xenon1tMuonGenerator::FindInitPos(G4ThreeVector cosd,
    G4ThreeVector disk)
// returns the intersection point between the world volume and the muon track
// it works only for down-going muons
{
  G4float t, xzc, yzc, xyc, zyc, yxc, zxc;
  //WARNING this MUST match with the WORLD dimensions
  const G4float Xmin = -20 * m, Xmax = 20 * m, Ymin = -60.5 * m,
                Ymax = 60.5 * m, Zmin = -20. * m,
                Zmax = 20. * m;

  G4float x0 = disk[0], y0 = disk[1], z0 = disk[2];
  G4float cx = cosd[0], cy = cosd[1], cz = cosd[2];
  G4float epsilon = 1.e-9;

  G4ThreeVector Vtmp, Vtmp2;

  G4int iexit = 0;

  // Plane at Z=top
  if (cz == 0) cz = epsilon;
  t = (Zmax - z0) / cz;
  xzc = x0 + t * cx;
  yzc = y0 + t * cy;
  if (xzc > Xmin && xzc < Xmax && yzc > Ymin && yzc < Ymax)
  {
    iexit++;
    Vtmp.set(xzc, yzc, Zmax);
    return Vtmp;  // if it touch the ceiling .. it is the highest one, thus
    // return
  }

  // Plane at Z=bottom
  if (cz == 0) cz = epsilon;
  t = (Zmin - z0) / cz;
  xzc = x0 + t * cx;
  yzc = y0 + t * cy;
  if (xzc > Xmin && xzc < Xmax && yzc > Ymin && yzc < Ymax)
  {
    iexit++;
    if (iexit == 1)
      Vtmp.set(xzc, yzc, Zmin);
    else
      Vtmp2.set(xzc, yzc, Zmin);
  }

  // Plane at X=max
  if (cx == 0) cx = epsilon;
  t = (Xmax - x0) / cx;
  zxc = z0 + t * cz;
  yxc = y0 + t * cy;
  if (zxc > Zmin && zxc < Zmax && yxc > Ymin && yxc < Ymax)
  {
    iexit++;
    if (iexit == 1)
      Vtmp.set(Xmax, yxc, zxc);
    else
      Vtmp2.set(Xmax, yxc, zxc);
  }

  // Plane at X=min
  if (cx == 0) cx = epsilon;
  t = (Xmin - x0) / cx;
  zxc = z0 + t * cz;
  yxc = y0 + t * cy;
  if (zxc > Zmin && zxc < Zmax && yxc > Ymin && yxc < Ymax)
  {
    iexit++;
    if (iexit == 1)
      Vtmp.set(Xmin, yxc, zxc);
    else
      Vtmp2.set(Xmin, yxc, zxc);
  }

  // Plane at Y=max
  if (cy == 0) cy = epsilon;
  t = (Ymax - y0) / cy;
  zyc = z0 + t * cz;
  xyc = x0 + t * cx;
  if (zyc > Zmin && zyc < Zmax && xyc > Xmin && xyc < Xmax)
  {
    iexit++;
    if (iexit == 1)
      Vtmp.set(xyc, Ymax, zyc);
    else
      Vtmp2.set(xyc, Ymax, zyc);
  }

  // Plane at Y=min
  if (cy == 0) cy = epsilon;
  t = (Ymin - y0) / cy;
  zyc = z0 + t * cz;
  xyc = x0 + t * cx;
  if (zyc > Zmin && zyc < Zmax && xyc > Xmin && xyc < Xmax)
  {
    iexit++;
    if (iexit == 1)
      Vtmp.set(xyc, Ymin, zyc);
    else
      Vtmp2.set(xyc, Ymin, zyc);
  }

  // choose the first intersection point
  if (iexit == 0)
  {
    Vtmp.set(0, 0, 0);
    return Vtmp;
  }

  if (iexit == 1)
    return Vtmp;
  else
  {
    if (Vtmp.z() > Vtmp2.z())
      return Vtmp;
    else
      return Vtmp2;
  }
}
