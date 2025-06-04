#ifndef __HTPCEVENTACTION_H__
#define __HTPCEVENTACTION_H__

#include <G4UserEventAction.hh>

#include "HTPCAnalysisManager.hh"

class G4Event;

class HTPCEventAction : public G4UserEventAction
{
public:
	HTPCEventAction(HTPCAnalysisManager *pAnalysisManager = 0);
	~HTPCEventAction();

public:
	void BeginOfEventAction(const G4Event *pEvent);
	void EndOfEventAction(const G4Event *pEvent);

private:
	HTPCAnalysisManager *m_pAnalysisManager;
};

#endif