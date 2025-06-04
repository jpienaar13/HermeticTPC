#include "HTPCEventData.hh"

HTPCEventData::HTPCEventData()
{
	m_iEventId = 0;

	//Vind uit wat nodig is, verwys na Analysis Manager
	m_iNbPMTDetectorHits = 0;
	m_pPMTHits = new vector<int>;

	m_fTotalEnergyDeposited = 0.;
	m_iNbSteps = 0;

	m_pTrackId = new vector<int>;
	m_pParentId = new vector<int>;
	m_pParticleType = new vector<string>;
	m_pParentType = new vector<string>;
	m_pCreatorProcess = new vector<string>;
	m_pDepositingProcess = new vector<string>;
	m_pX = new vector<float>;
	m_pY = new vector<float>;
	m_pZ = new vector<float>;
	m_pEnergyDeposited = new vector<float>;
	m_pKineticEnergy = new vector<float>;
	m_pPreStepEnergy = new vector<float>;
	m_pPostStepEnergy = new vector<float>;

	m_pTime = new vector<float>;

	m_pPrimaryParticleType = new vector<string>;
	m_fPrimaryX = 0.;
	m_fPrimaryY = 0.;
	m_fPrimaryZ = 0.;
	m_fForcedPrimaryX = 0.;
	m_fForcedPrimaryY = 0.;
	m_fForcedPrimaryZ = 0.;
	m_fPrimaryCx = 0.;
    m_fPrimaryCy = 0.;
    m_fPrimaryCz = 0.;
	m_fPrimaryE = 0.;

}

HTPCEventData::HTPCEventData()
{
	delete m_pPMTHits;
	delete m_pTrackId;
	delete m_pParentId;
	delete m_pParticleType;
	delete m_pParentType;
	delete m_pCreatorProcess;
	delete m_pDepositingProcess;
	delete m_pX;
	delete m_pY;
	delete m_pZ;
	delete m_pEnergyDeposited;
	delete m_pKineticEnergy;
	delete m_pPreStepEnergy;
 	delete m_pPostStepEnergy;
	delete m_pTime;

	delete m_pPrimaryParticleType;
}

void HTPCEventData::Clear()
{
	m_iEventId = 0;
	m_iNbPMTHits = 0;

	m_pPMTHits->clear();

	m_fTotalEnergyDeposited = 0.0;
	m_iNbSteps = 0;

	m_pTrackId->clear();
	m_pParentId->clear();
	m_pParticleType->clear();
	m_pParentType->clear();
	m_pCreatorProcess->clear();
	m_pDepositingProcess->clear();
	m_pX->clear();
	m_pY->clear();
	m_pZ->clear();
	m_pEnergyDeposited->clear();
	m_pKineticEnergy->clear();
	m_pPreStepEnergy->clear();
 	m_pPostStepEnergy->clear();

	m_pTime->clear();

	m_pPrimaryParticleType->clear();
	m_fPrimaryX = 0.;
	m_fPrimaryY = 0.;
	m_fPrimaryZ = 0.;
	m_fPrimaryE = 0.;
	m_fPrimaryW = 0.;
    m_fPrimaryCx = 0.;
    m_fPrimaryCy = 0.;
    m_fPrimaryCz = 0.;
}

