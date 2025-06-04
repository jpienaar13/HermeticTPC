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
#include <G4VUserDetectorConstruction.hh>
//#include "PurdueEljenSensitiveDetector.hh" ->To be updated still

class HTPCDetectorConstruction: public G4VUserDetectorConstruction
{
public:
    HTPCDetectorConstruction(G4String fname);
    ~HTPCDetectorConstruction();

    G4VPhysicalVolume* Construct()

    void DefineGeometryParameters();
    void DefineMaterials();


private:

    static map<G4String, G4double> m_hGeometryParameters;

    // ROOT Setup
    TFile      *_fGeom;
    G4String    detRootFile;
    TDirectory *_detector;

    // Messenger Setup
    //PurdueDetectorMessenger *m_pDetectorMessenger;

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
    G4LogicalVolume   *i_pCryoVacuumLogicalVolume;
    G4VPhysicalVolume *i_pCryoVacuumPhysicalVolume;
