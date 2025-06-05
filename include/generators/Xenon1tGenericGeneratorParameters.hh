#ifndef __XENONGENERICGENERATORPARAMETERS_H__
#define __XENONGENERICGENERATORPARAMETERS_H__

#include <set>
#include <vector>

#include <TFile.h>
#include "TH1.h"

#include <G4ParticleDefinition.hh>
#include <G4ParticleMomentum.hh>

class GenericGeneratorParameters
{
public:
  GenericGeneratorParameters()
  {
    first = true;
    EvID = 0;
    G4ThreeVector hZero(0., 0., 0.);

    m_dShapeVolume = 0.;
    m_hInputFileName = "NULL";

    m_iNumberOfParticlesToBeGenerated = 1;
    m_pParticleDefinition = 0;
    m_hParticleType = new std::vector<G4String>();
    m_hParticleMomentumDirection = G4ParticleMomentum(1., 0., 0.);
    m_dParticleEnergy = 1.0 * CLHEP::MeV;
    m_hParticlePosition = hZero;
    m_dParticleTime = 0.0;
    m_hParticlePolarization = hZero;
    m_dParticleCharge = 0.0;
    m_bIsOpticalphoton = false;

    m_hSourcePosType = "Volume";
    m_hShape = "NULL";
    m_dHalfz = 0.;
    m_dRadius = 0.;
    m_dRadius2=0.;
    m_dInnerRadius = 0.;
    m_dInnerRadius2 = 0.;
    m_dThickness_top = 0.;
    m_dThickness_bottom = 0.;
    m_hCenterCoords = hZero;
    m_bConfine = false;

    m_hVolumeNames = new std::set<G4String>();

    m_hAngDistType = "iso";
    m_dMinTheta = 0.;
    m_dMaxTheta = CLHEP::pi;
    m_dMinPhi = 0.;
    m_dMaxPhi = CLHEP::twopi;

    m_hEnergyDisType = "Mono";
    m_dMonoEnergy = 1 * CLHEP::MeV;
    m_hEnergyFile = "";
    m_hEnergySpectrumBins = new std::vector<G4double>();
    m_hEnergySpectrumPDF = new std::vector<G4double>();
    m_hEnergySpectrumCDF = new std::vector<G4double>();

    m_iVerbosityLevel = 0;
  }

  ~GenericGeneratorParameters()
  {
    delete m_hParticleType;
    delete m_hVolumeNames;
    delete m_hEnergySpectrumBins;
    delete m_hEnergySpectrumPDF;
    delete m_hEnergySpectrumCDF;
  };

public:
  void PrintParameters()
  {
    G4cout << "*************************************" << G4endl;
    G4cout << "Current GenericGeneratorParameters:" << G4endl;
    G4cout << "*************************************" << G4endl;
    G4cout << "\t first : [ " << first << " ] " << G4endl;
    G4cout << "\t m_hInputFileName : [ " << m_hInputFileName << " ] " << G4endl;
    G4cout << "\t m_hSourcePosType : [ " << m_hSourcePosType << " ] " << G4endl;
    G4cout << "\t m_hShape : [ " << m_hShape << " ] " << G4endl;
    G4cout << "\t m_hCenterCoords : [ " << m_hCenterCoords << " ] " << G4endl;
    G4cout << "\t m_dHalfx : [ " << m_dHalfx << " ] " << G4endl;
    G4cout << "\t m_dHalfy : [ " << m_dHalfy << " ] " << G4endl;
    G4cout << "\t m_dHalfz : [ " << m_dHalfz << " ] " << G4endl;
    G4cout << "\t m_dThickness_top : [ " << m_dThickness_top << " ] " << G4endl;
    G4cout << "\t m_dThickness_bottom : [ " << m_dThickness_bottom << " ] " << G4endl;
    G4cout << "\t m_dRadius : [ " << m_dRadius << " ] " << G4endl;
    G4cout << "\t m_dInnerRadius : [ " << m_dInnerRadius << " ] " << G4endl;
    G4cout << "\t m_dRadius2 : [ " << m_dRadius2 << " ] " << G4endl;
    G4cout << "\t m_dInnerRadius2 : [ " << m_dInnerRadius2 << " ] " << G4endl;
    G4cout << "\t m_bConfine : [ " << m_bConfine << " ] " << G4endl;
    G4cout << "\t m_dShapeVolume : [ " << m_dShapeVolume/(CLHEP::cm3) << " cm3 ] " << G4endl;
    G4cout << "\t m_hAngDistType : [ " << m_hAngDistType << " ] " << G4endl;
    G4cout << "\t m_dMinTheta : [ " << m_dMinTheta << " ] " << G4endl;
    G4cout << "\t m_dMaxTheta : [ " << m_dMaxTheta << " ] " << G4endl;
    G4cout << "\t m_dMinPhi : [ " << m_dMinPhi << " ] " << G4endl;
    G4cout << "\t m_dMaxPhi : [ " << m_dMaxPhi << " ] " << G4endl;
    G4cout << "\t m_dTheta : [ " << m_dTheta << " ] " << G4endl;
    G4cout << "\t m_dPhi : [ " << m_dPhi << " ] " << G4endl;
    G4cout << "\t m_hEnergyDisType : [ " << m_hEnergyDisType << " ] " << G4endl;
    G4cout << "\t m_hEnergyFile : [ " << m_hEnergyFile << " ] " << G4endl;
    G4cout << "\t m_dMonoEnergy : [ " << m_dMonoEnergy << " ] " << G4endl;
    G4cout << "\t m_iNumberOfParticlesToBeGenerated : [ " << m_iNumberOfParticlesToBeGenerated << " ] " << G4endl;
    G4cout << "\t m_hParticleMomentumDirection : [ " << m_hParticleMomentumDirection << " ] " << G4endl;
    G4cout << "\t m_dParticleEnergy : [ " << m_dParticleEnergy << " ] " << G4endl;
    G4cout << "\t m_dParticleCharge : [ " << m_dParticleCharge << " ] " << G4endl;
    G4cout << "\t m_hParticlePosition : [ " << m_hParticlePosition << " ] " << G4endl;
    G4cout << "\t m_dParticleTime : [ " << m_dParticleTime << " ] " << G4endl;
    G4cout << "\t m_hParticlePolarization : [ " << m_hParticlePolarization << " ] " << G4endl;
    G4cout << "\t m_hMultiParticleMomentumDirection : [ " << m_hMultiParticleMomentumDirection << " ] " << G4endl;
    G4cout << "\t m_iVerbosityLevel : [ " << m_iVerbosityLevel << " ] " << G4endl;
    G4cout << "\t m_bIsOpticalphoton : [ " << m_bIsOpticalphoton << " ] " << G4endl;
    G4cout << "\t EvID : [ " << EvID << " ] " << G4endl;

    G4cout << "\t m_hEnergySpectrumBins : [ " << G4endl;
    std::vector<G4double>::iterator pVecIt;
    for (pVecIt = m_hEnergySpectrumBins->begin(); pVecIt != m_hEnergySpectrumBins->end(); pVecIt++)
    {
      G4cout << *pVecIt << G4endl;
    }
    G4cout << "\t ] " << G4endl;

    G4cout << "\t m_hEnergySpectrumPDF : [ " << G4endl;
    for (pVecIt = m_hEnergySpectrumPDF->begin(); pVecIt != m_hEnergySpectrumPDF->end(); pVecIt++)
    {
      G4cout << *pVecIt << G4endl;
    }
    G4cout << "\t ] " << G4endl;

    G4cout << "\t m_hEnergySpectrumCDF : [ " << G4endl;
    for (pVecIt = m_hEnergySpectrumCDF->begin(); pVecIt != m_hEnergySpectrumCDF->end(); pVecIt++)
    {
      G4cout << *pVecIt << G4endl;
    }
    G4cout << "\t ] " << G4endl;

    if(m_pParticleDefinition != 0)
      G4cout << "\t m_pParticleDefinition->GetParticleName() : [ " << m_pParticleDefinition->GetParticleName() << " ] " << G4endl;
    else
      G4cout << "\t m_pParticleDefinition->GetParticleName() : [ " << m_pParticleDefinition << " ] " << G4endl;

    G4cout << "\t m_hParticleType : [" << G4endl;
    std::vector<G4String>::iterator pStringIt;
    for (pStringIt = m_hParticleType->begin(); pStringIt != m_hParticleType->end(); pVecIt++)
    {
      G4cout << *pStringIt << G4endl;
    }
    G4cout << "\t ] "  << G4endl;

    G4cout << "\t m_hVolumeNames : [" << G4endl;
    std::set<G4String>::iterator pIt;
    for (pIt = m_hVolumeNames->begin(); pIt != m_hVolumeNames->end(); pIt++)
    {
      G4cout << *pIt << G4endl;
    }
    G4cout << "\t ] "  << G4endl;

    G4cout << "*************************************" << G4endl;
  }

public:
  std::vector<G4String> *m_hParticleType;
  G4ParticleDefinition *m_pParticleDefinition;
  std::set<G4String> *m_hVolumeNames;

  G4bool first;

  G4double m_dShapeVolume;
  G4String m_hInputFileName;
  G4String m_hSourcePosType;
  G4String m_hShape;
  G4ThreeVector m_hCenterCoords;
  G4double m_dHalfx;
  G4double m_dHalfy;
  G4double m_dHalfz;
  G4double m_dThickness_top, m_dThickness_bottom;
  G4double m_dRadius;
  G4double m_dInnerRadius;
  G4double m_dRadius2, m_dInnerRadius2;
  G4bool m_bConfine;

  G4String m_hAngDistType;
  G4double m_dMinTheta, m_dMaxTheta, m_dMinPhi, m_dMaxPhi;
  G4double m_dTheta, m_dPhi;
  G4String m_hEnergyDisType;
  G4String m_hEnergyFile;
  std::vector<G4double> *m_hEnergySpectrumBins;
  std::vector<G4double> *m_hEnergySpectrumPDF;
  std::vector<G4double> *m_hEnergySpectrumCDF;
  G4double m_dMonoEnergy;
  G4int m_iNumberOfParticlesToBeGenerated;

  G4ParticleMomentum m_hParticleMomentumDirection;
  G4double m_dParticleEnergy;
  G4double m_dParticleCharge;
  G4ThreeVector m_hParticlePosition;
  G4double m_dParticleTime;
  G4ThreeVector m_hParticlePolarization;
  G4ParticleMomentum m_hMultiParticleMomentumDirection;
  G4int m_iVerbosityLevel;

  G4bool m_bIsOpticalphoton;

  G4int EvID;
};

#endif