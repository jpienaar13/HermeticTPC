#ifndef __HTPCRUNACTION_H__
#define __HTPCRUNACTION_H__

#include <G4UserRunAction.hh>

class G4Run;

class HTPCAnalysisManager;

class HTPCRunAction : public G4UserRunAction {
 public:
  HTPCRunAction(HTPCAnalysisManager *pAnalysisManager = 0);
  ~HTPCRunAction();

 public:
  void BeginOfRunAction(const G4Run *pRun);
  void EndOfRunAction(const G4Run *pRun);

  void SetRanSeed(G4int hRanSeed) { m_hRanSeed = hRanSeed; }
  //	void SetForcedTransport(G4bool doit) { m_hForcedTransport = doit; }
 private:
  G4int m_hRanSeed;
  //        G4bool m_hForcedTransport;
  HTPCAnalysisManager *m_pAnalysisManager;
};

#endif