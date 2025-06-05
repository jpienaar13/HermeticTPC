// XENON Header Files
#include "Xenon1tGenericGenerator.hh"

// Additional Header Files
#include <cmath>
#include <sstream>
#include <vector>

using std::stringstream;
using std::vector;

using namespace std;

// Root Header Files
#include "TH1.h"

// G4 Header Files
#include <G4Event.hh>
#include <G4IonTable.hh>
#include <G4Ions.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTable.hh>
#include <G4PhysicalVolumeStore.hh>
#include <G4PrimaryParticle.hh>
#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4TransportationManager.hh>
#include <G4VPhysicalVolume.hh>
#include <Randomize.hh>
#include <G4PhysicalConstants.hh>
#include <G4SystemOfUnits.hh>

Xenon1tGenericGenerator::Xenon1tGenericGenerator()
{
  param = new GenericGeneratorParameters();
}

Xenon1tGenericGenerator::~Xenon1tGenericGenerator()
{
  delete param;
}

void Xenon1tGenericGenerator::SetEnergyFile(G4String hEnergyFile)
{
  param->m_hEnergyFile = hEnergyFile;
  ReadEnergySpectrum();
}

G4bool Xenon1tGenericGenerator::ReadEnergySpectrum()
{
  // read the energy spectrum from the file
  ifstream hIn(param->m_hEnergyFile.c_str());

  if (hIn.fail())
  {
    G4cout << "Error: Cannot open energy spectrum file [ " << param->m_hEnergyFile << " ] !"
           << G4endl;
    return false;
  }

  G4cout << "ReadEnergySpectrum(): Source energy spectrum from file: [ "
         << param->m_hEnergyFile << " ]" << G4endl;

  // read the header
  G4String hEnergyUnit;

  while (!hIn.eof())
  {
    G4String hHeader;
    hIn >> hHeader;

    if (hHeader == "unit:")
      hIn >> hEnergyUnit;
    else if (hHeader == "spectrum:")
      break;
    else
    {
      hIn.ignore(10000,'\n');
    }
    if(hIn.eof() && hEnergyUnit.length() == 0)
    {
      G4cout << "Error: Unknown tag in energy spectrum file!" << G4endl;
      return false;
    }
  }

  G4cout << "ReadEnergySpectrum(): Energy unit: " << hEnergyUnit << G4endl;
  G4double dFactor;
  if (hEnergyUnit == "eV")
    dFactor = eV / MeV;
  else if (hEnergyUnit == "keV")
    dFactor = keV / MeV;
  else if (hEnergyUnit == "MeV")
    dFactor = MeV / MeV;
  else if (hEnergyUnit == "GeV")
    dFactor = GeV / MeV;
  else 
    throw std::runtime_error("ReadEnergySpectrum(): Unrecognized Energy Unit!");

  while (!hIn.eof())
  {
    G4double dBinEnergy = 0., dProbability = 0.;

    hIn >> dBinEnergy >> dProbability;

    if (hIn.good())
    {
      if (param->m_iVerbosityLevel >= 2)
        G4cout << std::setprecision(3) << std::scientific << dBinEnergy << "  "
               << dProbability << G4endl;
      param->m_hEnergySpectrumBins->push_back(dBinEnergy * dFactor);
      param->m_hEnergySpectrumPDF->push_back(dProbability);
      param->m_hEnergySpectrumCDF->push_back(0); // Here is just an initialization for CDF
    }
  }

  size_t nPoints = param->m_hEnergySpectrumBins->size();

  // Calculate the area under the PDF curve for each interval
  for (size_t i = 1; i < nPoints; ++i) {
      double dx = param->m_hEnergySpectrumBins->at(i) - param->m_hEnergySpectrumBins->at(i - 1);
      double avgHeight = 0.5 * (param->m_hEnergySpectrumPDF->at(i) + param->m_hEnergySpectrumPDF->at(i - 1));
      param->m_hEnergySpectrumCDF->at(i) = param->m_hEnergySpectrumCDF->at(i - 1) + avgHeight * dx;
  }

  // Normalize the PDF & CDF
  for (size_t i = 0; i < nPoints; ++i) {
      param->m_hEnergySpectrumPDF->at(i) /= param->m_hEnergySpectrumCDF->back();
      param->m_hEnergySpectrumCDF->at(i) /= param->m_hEnergySpectrumCDF->back();
  }

  return true;
}

void Xenon1tGenericGenerator::SetParticleDefinition(
  G4ParticleDefinition *aParticleDefinition)
{
  param->m_pParticleDefinition = aParticleDefinition;
  param->m_dParticleCharge = param->m_pParticleDefinition->GetPDGCharge();
}

void Xenon1tGenericGenerator::ConfineSourceToVolume(G4String hVolumeList)
{
  stringstream hStream;
  hStream.str(hVolumeList);
  G4String hVolumeName;

  param->m_hVolumeNames->clear();

  G4cout << "Xenon1tGenericGenerator: Confine generation to the following volumes: " << G4endl;
  // store all the volume names
  while (!hStream.eof())
  {
    hStream >> hVolumeName;

    if(hVolumeName.compare("") != 0)
    {
      param->m_hVolumeNames->insert(hVolumeName);
      G4cout << hVolumeName << G4endl;
    }
  }
  G4cout << G4endl;

  // checks if the selected volumes exist and store all volumes that match
  G4PhysicalVolumeStore *PVStore = G4PhysicalVolumeStore::GetInstance();
  G4bool bFoundAll = true;

  set<G4String> hActualVolumeNames;
  for (set<G4String>::iterator pIt = param->m_hVolumeNames->begin();
       pIt != param->m_hVolumeNames->end(); pIt++)
  {
    G4String hRequiredVolumeName = *pIt;
    G4bool bMatch = false;

    if ((bMatch = ((G4int)hRequiredVolumeName.last('*') !=
                   (G4int)std::string::npos)))
      hRequiredVolumeName = hRequiredVolumeName.strip(G4String::trailing, '*');

    G4bool bFoundOne = false;
    for (G4int iIndex = 0; iIndex < (G4int)PVStore->size(); iIndex++)
    {
      G4String hName = (*PVStore)[iIndex]->GetName();

      if (param->m_iVerbosityLevel >= 2)
        G4cout << " hRequiredVolumeName " << hRequiredVolumeName
               << " hVolumeName " << hName << G4endl;
      if ((bMatch && (hName.substr(0, hRequiredVolumeName.size())) ==
           hRequiredVolumeName) ||
          hName == hRequiredVolumeName)
      {
        hActualVolumeNames.insert(hName);
        bFoundOne = true;
        if (param->m_iVerbosityLevel >= 2) G4cout << "OK FOUND ONE " << G4endl;
      }
    }

    bFoundAll = bFoundAll && bFoundOne;
  }

  if (bFoundAll && !param->m_hVolumeNames->empty())
  {
    *(param->m_hVolumeNames) = hActualVolumeNames;
    param->m_bConfine = true;

    if (param->m_iVerbosityLevel >= 1)
      G4cout << "Source confined to volumes: " << hVolumeList << G4endl;

    if (param->m_iVerbosityLevel >= 2)
    {
      G4cout << "Volume list: " << G4endl;

      for (set<G4String>::iterator pIt = param->m_hVolumeNames->begin();
           pIt != param->m_hVolumeNames->end(); pIt++)
        G4cout << *pIt << G4endl;
    }
  }
  else if (param->m_hVolumeNames->empty())
  {
    G4cout << " **** /xe/gun/confine: WARNING: No physical volumes specified, confining to the selected shape! ****\n" << G4endl;
    param->m_bConfine = false;
  }
  else
  {
    G4cout << " **** /xe/gun/confine: ERROR: One or more volumes do not exist **** " << G4endl;
    exit(1);
  }
}

void Xenon1tGenericGenerator::GeneratePointSource()
{
  // Generates Points given the point source.
  if (param->m_hSourcePosType == "Point")
    param->m_hParticlePosition = param->m_hCenterCoords;
  else if (param->m_iVerbosityLevel >= 1)
    G4cout << "Error SourcePosType is not set to Point" << G4endl;
}

void Xenon1tGenericGenerator::GeneratePointsInVolume()
{
  G4ThreeVector RandPos;
  G4double x = 0., y = 0., z = 0.;
  G4double dShapeVolume = 0.;

  if (param->m_hSourcePosType != "Volume"
      && param->m_iVerbosityLevel >= 1)
    G4cout << "Error SourcePosType not Volume" << G4endl;

  if (param->m_hShape == "Sphere")
  {
    G4double radius = param->m_dRadius;

    x = radius * 2.;
    y = radius * 2.;
    z = radius * 2.;
    while (((x * x) + (y * y) + (z * z)) > (radius * radius))
    {
      x = G4UniformRand();
      y = G4UniformRand();
      z = G4UniformRand();

      x = (x * 2. * radius) - radius;
      y = (y * 2. * radius) - radius;
      z = (z * 2. * radius) - radius;
    }
    dShapeVolume = 4. / 3. * pi * pow(radius, 3);
  }

  else if (param->m_hShape == "Cylinder")
  {
    if ( ((param->m_dThickness_bottom + param->m_dThickness_top)  > 2. * param->m_dHalfz) or
         (param->m_dInnerRadius2 > param->m_dRadius2) )
    {
      G4cout << "Error: The size of the hollow tube must be smaller than the main cylinder." << G4endl;
      assert (!( ((param->m_dThickness_bottom + param->m_dThickness_top)  > 2. * param->m_dHalfz)
                 or (param->m_dInnerRadius2 > param->m_dRadius2) ));
    }
    else
    {
      G4double r=0., phi=0.;
      G4double z_min_top = param->m_dHalfz - param->m_dThickness_top;
      G4double z_min_bottom = -(param->m_dHalfz - param->m_dThickness_bottom);
      phi = twopi*G4UniformRand();
      do
      {
        z = (2.*G4UniformRand() - 1.) * param->m_dHalfz;
        r = sqrt(G4UniformRand()*param->m_dRadius2);
      }
      while  ( ((z <= z_min_top) and (z >= z_min_bottom)) and (pow(r,2) <= param->m_dInnerRadius2) );

      x = r*cos(phi);
      y = r*sin(phi);

      dShapeVolume = pi * ( param->m_dRadius2 * param->m_dHalfz * 2.0
                          - param->m_dInnerRadius2 * (z_min_top-z_min_bottom));
    }
  }

  else if (param->m_hShape == "Box")
  {
    x = 2 * (G4UniformRand() - 0.5) * param->m_dHalfx;
    y = 2 * (G4UniformRand() - 0.5) * param->m_dHalfy;
    z = 2 * (G4UniformRand() - 0.5) * param->m_dHalfz;

    dShapeVolume = 8. * param->m_dHalfx * param->m_dHalfy * param->m_dHalfz;
  }
  else
    G4cout << "GeneratePointsInVolume() Error: Volume Shape Does Not Exist" << G4endl;

  RandPos.setX(x);
  RandPos.setY(y);
  RandPos.setZ(z);
  param->m_hParticlePosition = param->m_hCenterCoords + RandPos;

  SetShapeVolume(dShapeVolume);
}

void Xenon1tGenericGenerator::GeneratePointsInSurface()
{
  G4ThreeVector RandPos;
  G4double x = 0., y = 0., z = 0.;

  if (param->m_hSourcePosType != "Surface"
      && param->m_iVerbosityLevel >= 1)
    G4cout << "Error SourcePosType not Surface" << G4endl;

  if (param->m_hShape == "Sphere")
  {
    G4double costh = G4UniformRand() * 2. - 1.;
    G4double sinth = sqrt(1 - costh * costh);
    G4double phi = G4UniformRand() * twopi;

    x = param->m_dRadius * sinth * cos(phi);
    y = param->m_dRadius * sinth * sin(phi);
    z = param->m_dRadius * costh;
  }

  else if (param->m_hShape == "Cylinder")
  {
    // compute areas
    G4double BaseArea = pi * param->m_dRadius * param->m_dRadius;
    G4double LateralArea = twopi * param->m_dRadius * 2 * param->m_dHalfz;
    G4double TotalArea = 2 * BaseArea + LateralArea;

    G4double rnd = G4UniformRand();

    if (rnd < LateralArea / TotalArea)    // sample in lateral
    {
      G4double phi = G4UniformRand() * twopi;
      x = param->m_dRadius * cos(phi);
      y = param->m_dRadius * sin(phi);

      z = param->m_dHalfz * (2 * G4UniformRand() - 1.);
    }
    else      // sample in base
    {
      G4double r = sqrt(G4UniformRand()) * param->m_dRadius;
      G4double phi = G4UniformRand() * twopi;
      x = r * cos(phi);
      y = r * sin(phi);

      G4double sign = G4UniformRand();
      if (sign > 0.5)
        z = param->m_dHalfz;
      else
        z = -1. * param->m_dHalfz;
    }
  }
  else if (param->m_hShape == "Box")
  {
    G4double SurfXY = param->m_dHalfx * param->m_dHalfy;
    G4double SurfXZ = param->m_dHalfx * param->m_dHalfz;
    G4double SurfYZ = param->m_dHalfy * param->m_dHalfz;
    G4double SurfTot = SurfXY + SurfXZ + SurfYZ;

    G4double rnd = G4UniformRand();

    if (rnd < SurfXY / SurfTot)    // sample in XY
    {
      x = 2 * (G4UniformRand() - 0.5) * param->m_dHalfx;
      y = 2 * (G4UniformRand() - 0.5) * param->m_dHalfy;

      G4double sign = G4UniformRand();
      if (sign > 0.5)
        z = param->m_dHalfz;
      else
        z = -1. * param->m_dHalfz;
    }
    else if (rnd < (SurfXY + SurfXZ) / SurfTot)      // sample in XZ
    {
      x = 2 * (G4UniformRand() - 0.5) * param->m_dHalfx;
      z = 2 * (G4UniformRand() - 0.5) * param->m_dHalfz;

      G4double sign = G4UniformRand();
      if (sign > 0.5)
        y = param->m_dHalfy;
      else
        y = -1. * param->m_dHalfy;
    }
    else      // sample in YZ
    {
      y = 2 * (G4UniformRand() - 0.5) * param->m_dHalfy;
      z = 2 * (G4UniformRand() - 0.5) * param->m_dHalfz;

      G4double sign = G4UniformRand();
      if (sign > 0.5)
        x = param->m_dHalfx;
      else
        x = -1. * param->m_dHalfx;
    }
  }

  else
    G4cout << "GeneratePointsInSurface() Error: Volume Shape Does Not Exist" << G4endl;

  RandPos.setX(x);
  RandPos.setY(y);
  RandPos.setZ(z);
  param->m_hParticlePosition = param->m_hCenterCoords + RandPos;
}

G4bool Xenon1tGenericGenerator::IsSourceConfined()
{
  // Method to check point is within the volume specified
  if (param->m_bConfine == false) G4cout << "Error: Confine is false" << G4endl;
  G4ThreeVector null(0., 0., 0.);
  G4ThreeVector *ptr;

  ptr = &null;

  // Check m_hParticlePosition is within a volume in our list
  G4VPhysicalVolume *theVolume;

  theVolume =
    G4TransportationManager::GetTransportationManager()
    ->GetNavigatorForTracking()->LocateGlobalPointAndSetup(param->m_hParticlePosition, ptr, true);
  G4String theVolName = theVolume->GetName();

  set<G4String>::iterator pIt;
  if ((pIt = param->m_hVolumeNames->find(theVolName)) != param->m_hVolumeNames->end())
  {
    if (param->m_iVerbosityLevel >= 1)
      G4cout << "CONFINED: Particle is in volume " << *pIt << G4endl;
    return (true);
  }
  else
  {
    return (false);
  }
}

void Xenon1tGenericGenerator::GenerateThetaFlux()
{
  G4double rndm, rndm2;
  G4double px, py, pz;

  G4double sintheta, sinphi, costheta, cosphi;

  rndm = G4UniformRand();
  costheta = std::cos(param->m_dMinTheta) +
             rndm * (std::cos(param->m_dMaxTheta) - std::cos(param->m_dMinTheta));
  sintheta = std::sqrt(1. - costheta * costheta);

  rndm2 = G4UniformRand();
  param->m_dPhi = param->m_dMinPhi + (param->m_dMaxPhi - param->m_dMinPhi) * rndm2;
  sinphi = std::sin(param->m_dPhi);
  cosphi = std::cos(param->m_dPhi);

  px = sintheta * cosphi;
  py = sintheta * sinphi;
  pz = costheta;

  G4double ResMag = std::sqrt((px * px) + (py * py) + (pz * pz));

  px = px / ResMag;
  py = py / ResMag;
  pz = pz / ResMag;

  param->m_hParticleMomentumDirection.setX(px);
  param->m_hParticleMomentumDirection.setY(py);
  param->m_hParticleMomentumDirection.setZ(pz);

  // m_hParticleMomentumDirection now holds unit momentum vector.
  if (param->m_iVerbosityLevel >= 2)
    G4cout << "Generating isotropic vector: " << param->m_hParticleMomentumDirection
           << G4endl;
  // set polarization if the particle is opticalphoton
  if (param->m_bIsOpticalphoton)
  {
    G4double sx = -costheta * cosphi;
    G4double sy = -costheta * sinphi;
    G4double sz = sintheta;

    ResMag = std::sqrt((sx * sx) + (sy * sy) + (sz * sz));

    sx = sx / ResMag;
    sy = sy / ResMag;
    sz = sz / ResMag;

    const G4double rotate = twopi * G4UniformRand();
    const G4double sin = std::sin(rotate);
    const G4double cos = std::cos(rotate);

    param->m_hParticlePolarization = G4ThreeVector(sx, sy, sz);
    G4ThreeVector hPerpendicular = param->m_hParticleMomentumDirection.cross(param->m_hParticlePolarization);
    param->m_hParticlePolarization = cos * param->m_hParticlePolarization + sin * hPerpendicular;
    param->m_hParticlePolarization = param->m_hParticlePolarization.unit();
  }
}

void Xenon1tGenericGenerator::GenerateMonoEnergetic()
{
  param->m_dParticleEnergy = param->m_dMonoEnergy;
}

void Xenon1tGenericGenerator::GenerateEnergyFromSpectrum()
{
  // Inverse transform sampling
  double u = G4UniformRand();

  // Find the x value corresponding to the random CDF value
  for (size_t j = 1; j < param->m_hEnergySpectrumCDF->size(); ++j)
  {
    if (u <= param->m_hEnergySpectrumCDF->at(j))
    {
      double pdf0 = param->m_hEnergySpectrumPDF->at(j - 1), pdf1 = param->m_hEnergySpectrumPDF->at(j);
      double x0 = param->m_hEnergySpectrumBins->at(j - 1), x1 = param->m_hEnergySpectrumBins->at(j);
      double cdf0 = param->m_hEnergySpectrumCDF->at(j - 1);

      // Linear interpolation of the PDF
      double slope = (pdf1 - pdf0) / (x1 - x0);

      // Solve the quadratic equation ax^2 + bx + c = 0
      double a = 0.5 * slope;
      double b = pdf0;
      double c = cdf0 - u;

      // Be careful: quadratic equation -> linear equation
      if (std::abs(a) < 1e-15)
      {
        param->m_dParticleEnergy = (x0 - c / b) * MeV;
        return;
      }

      double discriminant = b * b - 4 * a * c;

      if (discriminant >= 0)
      {
        double sqrtDiscriminant = std::sqrt(discriminant);
        double root1 = (-b + sqrtDiscriminant) / (2 * a);
        double root2 = (-b - sqrtDiscriminant) / (2 * a);

        // Select the appropriate root and adjust by adding x0
        if (x0 <= root1 + x0 && root1 + x0 <= x1)
        {
          param->m_dParticleEnergy = (root1 + x0) * MeV;
          return;
        } 
        else if (x0 <= root2 + x0 && root2 + x0 <= x1)
        {
          param->m_dParticleEnergy = (root2 + x0) * MeV;
          return;
        } 
      }
      throw std::runtime_error(
        "Xenon1tGenericGenerator::GenerateEnergyFromSpectrum: algorithm failed. Perhaps the spectrum is invalid."
      );
    }
  }
}

void Xenon1tGenericGenerator::GeneratePrimaryVertex(G4Event * pEvent)
{
  if(param->m_pParticleDefinition == 0)
  {
    G4cout << "No particle has been defined!" << G4endl;
    return;
  }

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

  if ((param->m_iVerbosityLevel >= 1))
  {
    G4cout << "Particle name: " << param->m_pParticleDefinition->GetParticleName()
           << G4endl;
    G4cout << "       Energy: " << param->m_dParticleEnergy << G4endl;
    G4cout << "     Position: " << param->m_hParticlePosition << G4endl;
    G4cout << "    Direction: " << param->m_hParticleMomentumDirection << G4endl;
    G4cout << " NumberOfParticlesToBeGenerated: "
           << param->m_iNumberOfParticlesToBeGenerated << G4endl;
  }

  G4PrimaryVertex *pVertex =
    new G4PrimaryVertex(param->m_hParticlePosition, param->m_dParticleTime);

  for (G4int i = 0; i < param->m_iNumberOfParticlesToBeGenerated; i++)
  {
    if (param->m_pParticleDefinition->GetParticleName() == "opticalphoton")
      param->m_bIsOpticalphoton = true;

    if (param->m_hAngDistType == "iso"
        || param->m_hAngDistType == "semiiso"
        || param->m_hAngDistType == "semiisoup"
        || param->m_hAngDistType == "semiisodown")
      GenerateThetaFlux();
    else if (param->m_hAngDistType == "direction")
      SetParticleMomentumDirection(param->m_hParticleMomentumDirection);
    else
      G4cout << "Error: AngDistType has unusual value" << G4endl;

    if (param->m_hEnergyDisType == "Mono")
      GenerateMonoEnergetic();
    else if (param->m_hEnergyDisType == "Spectrum")
      GenerateEnergyFromSpectrum();
    else
      G4cout << "Error: EnergyDisType has unusual value" << G4endl;

    if (param->m_iVerbosityLevel >= 2)
      G4cout << "Creating primaries and assigning to vertex" << G4endl;

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
    pVertex->SetPrimary(particle);
  }

  if (param->m_iVerbosityLevel > 1) G4cout << " Primary Vertex generated " << G4endl;

  pEvent->AddPrimaryVertex(pVertex);
}

void Xenon1tGenericGenerator::GeneratePrimaryVertexFromTrack(G4Track *pTrack, G4Event * pEvent)
{
  G4double dPX = pTrack->GetMomentum().x();
  G4double dPY = pTrack->GetMomentum().y();
  G4double dPZ = pTrack->GetMomentum().z();

  G4PrimaryVertex *pVertex =
    new G4PrimaryVertex(pTrack->GetPosition(), param->m_dParticleTime);

  G4PrimaryParticle *pPrimary =
    new G4PrimaryParticle(pTrack->GetDefinition(), dPX, dPY, dPZ);
  pPrimary->SetMass(pTrack->GetDefinition()->GetPDGMass());
  pPrimary->SetCharge(pTrack->GetDefinition()->GetPDGCharge());

  pVertex->SetPrimary(pPrimary);
  pEvent->AddPrimaryVertex(pVertex);
}

G4double Xenon1tGenericGenerator::GetConfinedVolume()
{
  G4int Ngoal = 100000, NinVolume = 0, NinShape = 0;

  while (NinVolume < Ngoal)
  {
    GeneratePointsInVolume();
    NinShape++;
    if (IsSourceConfined())
    {
      NinVolume++;

      //if(NinVolume%1000==0)
      //{
      //  G4cout << NinVolume << " " << NinShape
      //         << " " << (1.*NinVolume)/NinShape << G4endl;
      //}
    }
    G4double efficiency = (1. * NinVolume) / NinShape;
    if (NinShape > 1.e6 && efficiency < 1.e-5)
    {
      if (efficiency > 0)
        G4cout << " WARNING: Very INEFFICIENT GENERATION ... TRY TO ZOOM MORE "
               "ON THE DESIRED VOLUMES"
               << G4endl;
      else
      {
        G4cout << " ERROR: No events in volume confined. Aborting." << G4endl;
        exit(-1);
      }
    }
  }

  G4double dVolumeInCm3 = GetShapeVolume() / cm3 * NinVolume / NinShape;
  // G4cout << " N in Shape " << NinShape << " , N in Volume " << NinVolume <<
  // G4endl;
  // G4cout << " Shape Volume (cm3) " << GetShapeVolume()/cm3 << " , Volume
  // Volume (cm3) " << dVolumeInCm3 << G4endl;
  G4cout << " ****************************" << G4endl;
  G4cout << " ** Xenon1tGenericGenerator: Total volume of the regions where the "
         "events are generated (confined):  "
         << dVolumeInCm3 << " cm3 " << G4endl;
  G4cout << " ****************************" << G4endl;

  return dVolumeInCm3 * cm3;
}