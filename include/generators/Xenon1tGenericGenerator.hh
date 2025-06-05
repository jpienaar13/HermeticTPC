#ifndef __XENONGENERICGENERATOR_H__
#define __XENONGENERICGENERATOR_H__

// XENON Header Files
#include "HTPCParticleSourceMessenger.hh"
#include "Xenon1tGenericGeneratorParameters.hh"

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
#include <G4ParticleTable.hh>
#include <G4ParticleMomentum.hh>
#include <G4Track.hh>
#include <G4VPrimaryGenerator.hh>

class Xenon1tGenericGenerator : public G4VPrimaryGenerator
{
public:
  Xenon1tGenericGenerator();
  ~Xenon1tGenericGenerator();

  void GeneratePrimaryVertex(G4Event * pEvent);
  void GeneratePrimaryVertexFromTrack(G4Track * pTrack, G4Event * pEvent);

public:
  GenericGeneratorParameters * param;

public:
  void PrintCurrentGenericGeneratorParameters()
  {
    param->PrintParameters();
  }

  void SetPosDisType(G4String hSourcePosType)
  {
    param->m_hSourcePosType = hSourcePosType;
  }
  void SetPosDisShape(G4String hShape)
  {
    param->m_hShape = hShape;
  }
  void SetCenterCoords(G4ThreeVector hCenterCoords)
  {
    param->m_hCenterCoords = hCenterCoords;
  }

  void SetHalfX(G4double dHalfx)
  {
    param->m_dHalfx = dHalfx;
  }
  void SetHalfY(G4double dHalfy)
  {
    param->m_dHalfy = dHalfy;
  }
  void SetHalfZ(G4double dHalfz)
  {
    param->m_dHalfz = dHalfz;
  }
  void SetThickness_top(G4double dThickness_top)
  {
    param->m_dThickness_top = dThickness_top;
  }
  void SetThickness_bottom(G4double dThickness_bottom)
  {
    param->m_dThickness_bottom = dThickness_bottom;
  }
  void SetRadius(G4double dRadius)
  {
    param->m_dRadius = dRadius;
    param->m_dRadius2 = pow(dRadius,2);
  }
  void SetInnerRadius(G4double dInnerRadius)
  {
    param->m_dInnerRadius = dInnerRadius;
    param->m_dInnerRadius2 = pow(dInnerRadius,2);
  }
  // mod per semiiso //EDIT PAOLO
  void SetAngDistType(G4String hAngDistType)
  {
    param->m_hAngDistType = hAngDistType;

    if (hAngDistType == "semiiso")
    {
      param->m_dMinTheta = 0.5 *CLHEP::pi;
      param->m_dMaxTheta = CLHEP::pi;
    }
    else if (hAngDistType == "semiisoup")
    {
      param->m_dMinTheta = 0.0 ;
      param->m_dMaxTheta = 0.5 *CLHEP::pi;
    }
    else if (hAngDistType == "semiisodown")
    {
      param->m_dMinTheta = 0.5 *CLHEP::pi;
      param->m_dMaxTheta = CLHEP::pi;
    }
    else
    {
      param->m_dMinTheta = 0.0;
      param->m_dMaxTheta = CLHEP::pi;
    }
  }

  void SetParticleMomentumDirection(G4ParticleMomentum hMomentum)
  {
    param->m_hParticleMomentumDirection = hMomentum.unit();
  }

  void SetEnergyDisType(G4String hEnergyDisType)
  {
    param->m_hEnergyDisType = hEnergyDisType;
  }
  void SetEnergyFile(G4String hEnergyFile);
  void SetMonoEnergy(G4double dMonoEnergy)
  {
    param->m_dMonoEnergy = dMonoEnergy;
  }

  void SetParticleDefinition(G4ParticleDefinition *pParticleDefinition);

  inline void SetParticleCharge(G4double dCharge)
  {
    param->m_dParticleCharge = dCharge;
  }

  void SetVerbosity(G4int iVerbosityLevel)
  {
    param->m_iVerbosityLevel = iVerbosityLevel;
  }

  const G4String &GetParticleType()
  {
    return param->m_pParticleDefinition->GetParticleName();
  }

  std::vector<G4String> * GetParticleTypeVector()
  {
    return param->m_hParticleType;
  }

  G4double GetParticleEnergy()
  {
    return param->m_dParticleEnergy;
  }
  const G4ThreeVector &GetParticlePosition()
  {
    return param->m_hParticlePosition;
  }
  inline G4ParticleDefinition *GetParticleDefinition()
  {
    return param->m_pParticleDefinition;
  };
  inline G4ParticleMomentum GetParticleMomentumDirection()
  {
    return param->m_hParticleMomentumDirection;
  };

  G4bool ReadEnergySpectrum();
  void GeneratePointSource();
  void GeneratePointsInVolume();
  void GeneratePointsInSurface();
  G4bool IsSourceConfined();
  void ConfineSourceToVolume(G4String);

  void GenerateThetaFlux();

  void GenerateMonoEnergetic();
  void GenerateEnergyFromSpectrum();
  G4double GetConfinedVolume();

  void SetShapeVolume(G4double dShapeVolume)
  {
    param->m_dShapeVolume = dShapeVolume;
  }
  G4String GetEventInputFileName()
  {
    return param->m_hInputFileName;
  }
  void SetInputFileName(G4String hInputFileName)
  {
    param->m_hInputFileName = hInputFileName;
  }
  G4double GetShapeVolume()
  {
    return param->m_dShapeVolume;
  }
  void SetNumberOfParticlesToBeGenerated(
    G4int iNumberOfParticlesToBeGenerated)
  {
    param->m_iNumberOfParticlesToBeGenerated = iNumberOfParticlesToBeGenerated;
  }
};

#endif