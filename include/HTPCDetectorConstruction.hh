#ifndef __HTPCDETECTORCONSTRUCTION_H__
#define __HTPCDETECTORCONSTRUCTION_H__

#include <globals.hh>
#include <vector>
#include <map>
#include "TFile.h"
#include "TDirectory.h"

using std::vector;
using std::map;

class G4Sphere;
class G4Colour;
class G4LogicalVolume;
class G4VPhysicalVolume;
//class PurdueDetectorMessenger; ->To be updated still

#include <G4UnionSolid.hh>
#include <G4UImanager.hh>
#include <G4Polycone.hh>
#include <G4VUserDetectorConstruction.hh>
#include "HTPCSensitiveDetector.hh"

class HTPCDetectorConstruction: public G4VUserDetectorConstruction
{
public:
    HTPCDetectorConstruction(G4String fname);
    ~HTPCDetectorConstruction();

    G4VPhysicalVolume* Construct();

    void DefineGeometryParameters();
    void DefineMaterials();
    void ConstructLab();
    void ConstructCryostats();
    void ConstructDetector();

    G4LogicalVolume *ConstructPMT();

    G4double GetGeometryParameter(const char *szParameter);
    G4ThreeVector GetPMTPosition(G4int iPMTnB, G4int i_nmbPMTS);


private:

    static map<G4String, G4double> m_hGeometryParameters;

    // ROOT Setup
    TFile      *_fGeom;
    G4String    detRootFile;
    TDirectory *_detector;

    // Messenger Setup
    //PurdueDetectorMessenger *m_pDetectorMessenger;

    // -- Hargy's declarations
    G4LogicalVolume*   logicCapsule1wFlange;
    G4VPhysicalVolume* physCapsule1wFlange;

    G4LogicalVolume*   logicCryoVacuum;
    G4VPhysicalVolume* physCryoVacuum;

    G4LogicalVolume*   logicCapsule2wFlange;
    G4VPhysicalVolume* physCapsule2wFlange;

    G4LogicalVolume*   logicLXeTub;
    G4VPhysicalVolume* physLXeTub;

    // -----------------------

    //Laboratory
    G4LogicalVolume   *m_pLabLogicalVolume;
    G4VPhysicalVolume *m_pLabPhysicalVolume;

    //Outer Cryostat
    G4LogicalVolume   *m_poCryostatLogicalVolume;
    G4VPhysicalVolume *m_poCryostatPhysicalVolume;

    //CryostatVacuum
    G4LogicalVolume   *m_pCryoVacuumLogicalVolume;
    G4VPhysicalVolume *m_pCryoVacuumPhysicalVolume;

    //CryostatVacuum
    G4LogicalVolume   *m_piCryostatLogicalVolume;
    G4VPhysicalVolume *m_piCryostatPhysicalVolume;

    //LXe Volume
    G4LogicalVolume   *m_pLXeLogicalVolume;
    G4VPhysicalVolume *m_pLXePhysicalVolume;

    //PTFECyl Volume
    G4LogicalVolume   *m_pPTFECylLogicalVolume;
    G4VPhysicalVolume *m_pPTFECylPhysicalVolume;

    //ALXe Volume
    G4LogicalVolume   *m_pALXeLogicalVolume;
    G4VPhysicalVolume *m_pALXePhysicalVolume;

    //GXe Volume
    G4LogicalVolume   *m_pGXeLogicalVolume;
    G4VPhysicalVolume *m_pGXePhysicalVolume;

    //PMTGXe Volume
    G4LogicalVolume   *m_pPMTGXeLogicalVolume;
    G4VPhysicalVolume *m_pPMTGXePhysicalVolume;

    //Sapphire Volumes
    G4LogicalVolume   *m_pSapphireLXeLogicalVolume;
    G4VPhysicalVolume *m_pSapphireLXePhysicalVolume;

    G4LogicalVolume   *m_pSapphireGXeLogicalVolume;
    G4VPhysicalVolume *m_pSapphireGXePhysicalVolume;

    G4LogicalVolume   *m_pSapphireAnodeLogicalVolume;
    G4VPhysicalVolume *m_pSapphireAnodePhysicalVolume;

    G4LogicalVolume   *m_pSapphireBotLogicalVolume;
    G4VPhysicalVolume *m_pSapphireBotPhysicalVolume;

    //PMT VOlumes
    G4LogicalVolume *m_pPMTLogicalVolume;

    G4LogicalVolume *m_pPMTInnerVacuumLogicalVolume;
    G4LogicalVolume *m_pPMTWindowLogicalVolume;
    G4LogicalVolume *m_pPMTPhotocathodeLogicalVolume;
    G4LogicalVolume *m_PMTCeramicLogicalVolume;

    G4VPhysicalVolume *m_pPMTInnerVacuumPhysicalVolume;
    G4VPhysicalVolume *m_pPMTWindowPhysicalVolume;
    G4VPhysicalVolume *m_pPMTPhotocathodePhysicalVolume;
    G4VPhysicalVolume *m_PMTCeramicPhysicalVolume;

    G4LogicalVolume *m_pPmtR11410LogicalVolume;
    //G4LogicalVolume *m_pPmtBasesLogicalVolume;
    //vector<G4VPhysicalVolume *> m_pPmtBasesPhysicalVolumes;
    vector<G4VPhysicalVolume *> m_pPMTPhysicalVolumes;

    HTPCSensitiveDetector *pHTPC_SD;
};
#endif
