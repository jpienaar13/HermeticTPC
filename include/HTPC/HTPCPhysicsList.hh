#ifndef __HTPCPHYSICSLIST_H__
#define __HTPCPHYSICSLIST_H__


// XENON Header Files


// Additional Header Files
#include <globals.hh>
#include <vector>
using namespace std;

// G4 Header Files
#include "G4VModularPhysicsList.hh"

class HTPCPhysicsList : public G4VModularPhysicsList {
 public:
  HTPCPhysicsList();
  virtual ~HTPCPhysicsList();

 public:
  void SetCuts();

  // functions for the messenger interface
  void SetCerenkov(G4bool useCerenkov);
  void SetEMlowEnergyModel(G4String theModel);
  void SetHadronicModel(G4String theModel);

 private:
  G4VPhysicsConstructor *OpticalPhysicsModel;

  G4int VerboseLevel;
  G4int OpVerbLevel;

  G4String m_hEMlowEnergyModel;
  G4String m_hHadronicModel;
  G4bool m_bCerenkov;
};
#endif