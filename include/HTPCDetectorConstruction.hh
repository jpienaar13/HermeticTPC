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
#include <G4IntersectionSolid.hh>
#include <G4SubtractionSolid.hh>
#include <G4UImanager.hh>
#include <G4Polycone.hh>
#include <G4VUserDetectorConstruction.hh>
#include "HTPCSensitiveDetector.hh"
#include "G4ios.hh"
#include "G4GenericMessenger.hh"

class HTPCDetectorConstruction: public G4VUserDetectorConstruction
{
public:
    HTPCDetectorConstruction(G4String fname);
    ~HTPCDetectorConstruction();

    G4VPhysicalVolume* Construct();

    void ApplyMessengers();
    void DefineGeometryParameters();
    void DefineMaterials();
    void ConstructLab();
    void ConstructCryostats();
    void ConstructMedia();
    void ConstructTPC();

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
    G4double iCryostat_Alpha;
    G4double oCryostat_Alpha;
    G4double CryostatVacuum_Alpha;
    G4double Teflon_Alpha;
    G4double GXeMedium_Alpha;
    G4double LXeMedium_Alpha;
    G4double GXeActive_Alpha;
    G4double LXeActive_Alpha;
    G4double Sapphire_Alpha;

    G4GenericMessenger *fMessengerAlpha;

    // Laboratory
    G4LogicalVolume*   logic_Lab;
    G4VPhysicalVolume* phys_Lab;

    // Cryostats
    G4LogicalVolume*   logic_oCryostat;
    G4VPhysicalVolume* phys_oCryostat;

    G4LogicalVolume*   logic_CryostatVacuum;
    G4VPhysicalVolume* phys_CryostatVacuum;

    G4LogicalVolume*   logic_iCryostat;
    G4VPhysicalVolume* phys_iCryostat;

    // TeflonTub
    G4LogicalVolume*   logic_GXeTeflonTub;
    G4VPhysicalVolume* phys_GXeTeflonTub;

    G4LogicalVolume*   logic_LXeTeflonTub;
    G4VPhysicalVolume* phys_LXeTeflonTub;

    G4LogicalVolume*   logic_TeflonCap;
    G4VPhysicalVolume* phys_GXeTeflonCap;
    G4VPhysicalVolume* phys_LXeTeflonCap;

    // Media
    G4LogicalVolume* logic_GXeMedium;
    G4VPhysicalVolume* phys_GXeMedium;

    G4LogicalVolume* logic_LXeMedium;
    G4VPhysicalVolume* phys_LXeMedium;

    G4LogicalVolume* logic_GXeActive;
    G4VPhysicalVolume* phys_GXeActive;

    G4LogicalVolume* logic_LXeActive;
    G4VPhysicalVolume* phys_LXeActive;

    // Sapphire
    G4LogicalVolume*   logic_GXeSapphireTub;
    G4VPhysicalVolume* phys_GXeSapphireTub;

    G4LogicalVolume*   logic_GXeSapphireCap;
    G4VPhysicalVolume* phys_GXeSapphireCap;

    G4LogicalVolume*   logic_LXeSapphireTub;
    G4VPhysicalVolume* phys_LXeSapphireTub;

    G4LogicalVolume*    logic_LXeSapphireCap;
    G4VPhysicalVolume* phys_LXeSapphireCap;

    // -----------------------

    //PMTGXe Volume
    G4LogicalVolume   *m_pPMTGXeLogicalVolume;
    G4VPhysicalVolume *m_pPMTGXePhysicalVolume;

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
    vector<G4VPhysicalVolume *> m_pPMTPhysicalVolumes;

    HTPCSensitiveDetector *pHTPC_SD;
};
#endif
