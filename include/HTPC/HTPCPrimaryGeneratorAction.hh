#ifndef __HTPCPRIMARYGENERATORACTION_H__
#define __HTPCPRIMARYGENERATORACTION_H__

// Additional Header Files
#include <globals.hh>

// G4Header Files
#include <G4EmCalculator.hh>
#include <G4GeneralParticleSource.hh>
#include <G4Material.hh>
#include <G4Navigator.hh>
#include <G4ParticleGun.hh>
#include <G4ThreeVector.hh>
#include <G4VUserPrimaryGeneratorAction.hh>

enum { HIST_WEIGHT };

class HTPCParticleSource;
class G4Event;

class HTPCPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
 public:
  HTPCPrimaryGeneratorAction();
  ~HTPCPrimaryGeneratorAction();

 public:
  const long *GetEventSeeds() { return m_lSeeds; }
  const G4String &GetParticleTypeOfPrimary() {
    return m_hParticleTypeOfPrimary;
  }
  std::vector<G4String> * GetParticleTypeVectorOfPrimary();
  G4double GetEnergyOfPrimary() { return m_dEnergyOfPrimary; }
  G4ThreeVector GetPositionOfPrimary() { return m_hPositionOfPrimary; }
  G4ThreeVector GetForcedPositionOfPrimary() {
    return m_hForcedPositionOfPrimary;
  }
  G4ThreeVector GetDirectionOfPrimary() { return m_hDirectionOfPrimary; }

  G4double ComputeForcedTransportWeight(G4ThreeVector x0, G4ThreeVector dir,
                                        G4double L, G4double e);
  void SetVarianceReduction(G4bool doit) { VarianceReduction = doit; };
  void SetVarianceReductionMode(G4int val) { VarianceReductionMode = val; };
  void SetSurvivalProbabilityCut(G4double val) { p_survive_cut = val; };
  void SetFT_cyl_center(G4ThreeVector xc) { ft_cyl_center = xc; };
  void SetFT_cyl_length(G4double L) { ft_cyl_L = L; };
  void SetFT_cyl_radius(G4double R) { ft_cyl_R = R; };

  void SetWriteEmpty(G4bool doit) { writeEmpty = doit; };
  G4bool GetWriteEmpty() { return writeEmpty; };

  void GeneratePrimaries(G4Event *pEvent);

  void GeneratePrimariesStandard(G4Event *pEvent);
  void GeneratePrimariesDecay0(G4Event *pEvent);
  void GeneratePrimariesAccelerated();
  void FillPrimaryType(G4Event *pEvent);
  G4int IntersectWithTarget(G4ThreeVector x0, G4ThreeVector dir);
  G4double GetNumberOfRejectedPrimaries() { return numberOfRejectedPrimaries; };
  G4double GetNumberOfAcceptedPrimaries() { return numberOfAcceptedPrimaries; };
  G4double ComputeGammaMeanFreePath(G4double e, G4String matName);

 private:
  enum { KILL_ONLY, KILL_RR, KILL_FT_EDGE, KILL_FT_PATH };

  //
  G4bool writeEmpty;

  //
  long m_lSeeds[2];
  G4String m_hParticleTypeOfPrimary;
  G4double m_dEnergyOfPrimary;
  G4ThreeVector m_hPositionOfPrimary;
  G4ThreeVector m_hDirectionOfPrimary;

  // if you choose to force the vertex position to somewhere else....
  G4bool VarianceReduction;
  G4int VarianceReductionMode;
  G4ThreeVector m_hForcedPositionOfPrimary;
  G4ParticleTable *particleTable;
  G4ParticleGun *particleGun;
  G4Navigator *theNavigator;

  // survival probability cut
  G4double p_survive_cut;

  // forced transport target cylinder
  G4double ft_cyl_R;
  G4double ft_cyl_L;
  G4ThreeVector ft_cyl_center;

  std::vector<G4ThreeVector> ft_cyl_intersect;
  std::vector<G4double> ft_cyl_intersect_s;

  // accounting
  G4int ntry_max;
  G4double numberOfRejectedPrimaries;
  G4double numberOfAcceptedPrimaries;
  G4double runWeight;
  // em calculator
  G4EmCalculator emCalc;

  HTPCParticleSource *m_pParticleSource;
};

#endif