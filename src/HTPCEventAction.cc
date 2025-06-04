#include <G4Event.hh>

#include "HTPCEventAction.hh"

HTPCEventAction::HTPCEventAction(HTPCAnalysisManager *pAnalysisManager)
{
  m_pAnalysisManager = pAnalysisManager;
}

HTPCEventAction::~HTPCEventAction()
{
}

void HTPCEventAction::BeginOfEventAction(const G4Event *pEvent)
{
  if(pEvent->GetEventID() % 1000 == 0)
    {
      G4cout << "------ Begin event " << pEvent->GetEventID()<< "------" << G4endl;
    }

  if(m_pAnalysisManager)
    m_pAnalysisManager->BeginOfEvent(pEvent);
}

void HTPCEventAction::EndOfEventAction(const G4Event *pEvent)
{
  //G4cout <<"pEvent at End " << pEvent ->GetEventID()<< G4endl;
    if(m_pAnalysisManager)
    m_pAnalysisManager->EndOfEvent(pEvent);