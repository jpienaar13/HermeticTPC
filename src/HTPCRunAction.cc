#include <sys/time.h>
#include <Randomize.hh>

#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include "G4Threading.hh"
#include "HTPCAnalysisManager.hh"
#include "HTPCRunAction.hh"
#include "TRandom3.h"

HTPCRunAction::HTPCRunAction(HTPCAnalysisManager *pAnalysisManager) {
  m_hRanSeed = 0;  // default value
  m_pAnalysisManager = pAnalysisManager;
}

HTPCRunAction::~HTPCRunAction() { }

void HTPCRunAction::BeginOfRunAction(const G4Run *pRun) {
  if (m_pAnalysisManager) {
    m_pAnalysisManager->BeginOfRun(pRun);
  }

  // random seeding of the MC

  if (m_hRanSeed == 0) {
    // initialize with time
    struct timeval hTimeValue;
    gettimeofday(&hTimeValue, NULL);
    m_hRanSeed = hTimeValue.tv_usec;
  } else {
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  }

  G4cout << "RunAction type: "
       << (G4Threading::IsMasterThread() ? "MASTER" : "WORKER")
       << G4endl;

  G4cout << "BeginOfRunAction Initialize random numbers "
            "with seed = "
         << m_hRanSeed << G4endl;
  CLHEP::HepRandom::setTheSeed(m_hRanSeed);

  // Set seed for the GetRandom() method of TH1
  gRandom = new TRandom3(m_hRanSeed);

  if (G4VVisManager::GetConcreteInstance()) {
    G4UImanager *UI = G4UImanager::GetUIpointer();
    UI->ApplyCommand("/vis/scene/notifyHandlers");
  }
}

void HTPCRunAction::EndOfRunAction(const G4Run *pRun) {
  if (m_pAnalysisManager) m_pAnalysisManager->EndOfRun(pRun, m_hRanSeed);

  if (G4VVisManager::GetConcreteInstance()) {
    G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/update");
  }
}
