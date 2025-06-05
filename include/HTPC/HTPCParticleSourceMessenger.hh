#ifndef __HTPCPARTICLESOURCEMESSENGER_H__
#define __HTPCPARTICLESOURCEMESSENGER_H__

// Additional Header Files
#include <globals.hh>

#include "generators/Xenon1tGenericGenerator.hh"

// G4 Header Files
#include <G4UImessenger.hh>
#include <globals.hh>

// Xenon Classes
class HTPCParticleSource;
class Xenon1tGenericGenerator;

// G4 Classes
class G4ParticleTable;
class G4IonTable;
class G4UIcommand;
class G4UIdirectory;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3Vector;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithABool;
class G4UIcmdWithoutParameter;

class HTPCParticleSourceMessenger : public G4UImessenger
{
public:
  HTPCParticleSourceMessenger(HTPCParticleSource *pSource);
  ~HTPCParticleSourceMessenger();

  void SetNewValue(G4UIcommand *pCommand, G4String hNewValues);

private:
  HTPCParticleSource *m_pSource;
  Xenon1tGenericGenerator *m_pGen;

  G4ParticleTable *m_pParticleTable;
  G4IonTable *m_pIonTable;
  G4UIdirectory *m_pDirectory;

  G4UIcmdWithAString *m_pGeneratorCmd;

  G4UIcmdWithAString *m_pMultiEventFromFileCmd;
  G4UIcmdWithAString *m_pDecay0EventFromFileCmd;
  G4UIcmdWithAString *m_pMuonsFromFileCmd;

  G4UIcmdWithAString *m_pTypeCmd;
  G4UIcmdWithAString *m_pShapeCmd;
  G4UIcmdWith3VectorAndUnit *m_pCenterCmd;
  G4UIcmdWithADoubleAndUnit *m_pHalfxCmd;
  G4UIcmdWithADoubleAndUnit *m_pHalfyCmd;
  G4UIcmdWithADoubleAndUnit *m_pHalfzCmd;
  G4UIcmdWithADoubleAndUnit *m_pThickness_topCmd;
  G4UIcmdWithADoubleAndUnit *m_pThickness_bottomCmd;
  G4UIcmdWithADoubleAndUnit *m_pRadiusCmd;
  G4UIcmdWithADoubleAndUnit *m_pInnerRadiusCmd;
  G4UIcmdWithAString *m_pConfineCmd;
  G4UIcmdWithAString *m_pAngTypeCmd;
  G4UIcmdWithAString *m_pEnergyTypeCmd;
  G4UIcmdWithAString *m_pEnergyFileCmd;
  G4UIcmdWithAnInteger *m_pVerbosityCmd;
  G4UIcommand *m_pIonCmd;
  G4UIcmdWithAString *m_pParticleCmd;
  G4UIcmdWith3VectorAndUnit *m_pPositionCmd;
  G4UIcmdWith3Vector *m_pDirectionCmd;
  G4UIcmdWithADoubleAndUnit *m_pEnergyCmd;
  G4UIcmdWithoutParameter *m_pListCmd;
  G4UIcmdWithAnInteger *m_pNumberOfParticlesToBeGeneratedCmd;

  G4bool m_bShootIon;
  G4int m_iAtomicNumber;
  G4int m_iAtomicMass;
  G4int m_iIonCharge;
  G4double m_dIonExciteEnergy;
};

#endif

