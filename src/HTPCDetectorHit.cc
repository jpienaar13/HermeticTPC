#include <G4UnitsTable.hh>
#include <G4VVisManager.hh>
#include <G4Circle.hh>
#include <G4Colour.hh>
#include <G4VisAttributes.hh>
#include <G4SystemOfUnits.hh>

#include "HTPCDetectorHit.hh"

G4Allocator<HTPCDetectorHit> HTPCDetectorHitAllocator;

HTPCDetectorHit::HTPCDetectorHit(){}

HTPCDetectorHit::~HTPCDetectorHit()
{
	if(m_pParticleType) delete m_pParticleType;
	if(m_pParentType) delete m_pParentType;
	if(m_pCreatorProcess) delete m_pCreatorProcess;
	if(m_pDepositingProcess) delete m_pDepositingProcess;
}

HTPCDetectorHit::HTPCDetectorHit(const HTPCDetectorHit &hHTPCDetectorHit):G4VHit()
{
	m_iTrackId = hHTPCDetectorHit.m_iTrackId;
	m_iParentId = hHTPCDetectorHit.m_iParentId;
	m_pParticleType = hHTPCDetectorHit.m_pParticleType;
	m_pParentType = hHTPCDetectorHit.m_pParentType ;
	m_pCreatorProcess = hHTPCDetectorHit.m_pCreatorProcess ;
	m_pDepositingProcess = hHTPCDetectorHit.m_pDepositingProcess ;
	m_hPosition = hHTPCDetectorHit.m_hPosition;
	m_dEnergyDeposited = hHTPCDetectorHit.m_dEnergyDeposited;
	m_dKineticEnergy = hHTPCDetectorHit.m_dKineticEnergy ;
	m_dPreStepEnergy = hHTPCDetectorHit.m_dPreStepEnergy ;
	m_dPostStepEnergy = hHTPCDetectorHit.m_dPostStepEnergy ;
	m_dTime = hHTPCDetectorHit.m_dTime;
}

const HTPCDetectorHit &
HTPCDetectorHit::operator=(const HTPCDetectorHit &hHTPCDetectorHit)
{
	m_iTrackId = hHTPCDetectorHit.m_iTrackId;
	m_iParentId = hHTPCDetectorHit.m_iParentId;
	m_pParticleType = hHTPCDetectorHit.m_pParticleType;
	m_pParentType = hHTPCDetectorHit.m_pParentType ;
	m_pCreatorProcess = hHTPCDetectorHit.m_pCreatorProcess ;
	m_pDepositingProcess = hHTPCDetectorHit.m_pDepositingProcess ;
	m_hPosition = hPHTPCDetectorHit.m_hPosition;
	m_dEnergyDeposited = hHTPCDetectorHit.m_dEnergyDeposited;
	m_dKineticEnergy = hHTPCDetectorHit.m_dKineticEnergy ;
	m_dPreStepEnergy = hHTPCDetectorHit.m_dPreStepEnergy ;
	m_dPostStepEnergy = hHTPCDetectorHit.m_dPostStepEnergy ;
	m_dTime = hHTPCDetectorHit.m_dTime;

	return *this;
}

G4int
HTPCDetectorHit::operator==(const HTPCDetectorHit &hHTPCDetectorHit) const
{
	return ((this == &hHTPCDetectorHit) ? (1) : (0));
}

void HTPCDetectorHit::Draw()
{
	G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();

	if(pVVisManager)
	{
		G4Circle hCircle(m_hPosition);
		G4Colour hColour(1.000, 0.973, 0.184);
		G4VisAttributes hVisAttributes(hColour);

		hCircle.SetScreenSize(0.1);
		hCircle.SetFillStyle(G4Circle::filled);
		hCircle.SetVisAttributes(hVisAttributes);
		pVVisManager->Draw(hCircle);
	}
}

void HTPCnDetectorHit::Print()
{
	G4cout << "-------------------- HTPCDetectorHit --------------------"
		<< "Id: " << m_iTrackId
		<< " Particle: " << *m_pParticleType
		<< " ParentId: " << m_iParentId
		<< " ParentType: " << *m_pParentType << G4endl
		<< "CreatorProcess: " << *m_pCreatorProcess
		<< " DepositingProcess: " << *m_pDepositingProcess << G4endl
		<< "Position: " << m_hPosition.x()/mm
		<< " " << m_hPosition.y()/mm
		<< " " << m_hPosition.z()/mm
		<< " mm" << G4endl
		<< " EnergyDeposited: " << m_dEnergyDeposited/keV << " keV"
		<< " KineticEnergyLeft: " << m_dKineticEnergy/keV << " keV"
	  	<< " PreStepEnergy:" << m_dPreStepEnergy/keV << " keV"
		<< " PostStepEnergy: " << m_dPostStepEnergy/keV << " keV"
		<< " Time: " << m_dTime/s << " s" << G4endl;
}

