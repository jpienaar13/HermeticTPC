#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SDManager.hh>
#include <G4SubtractionSolid.hh>
#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <globals.hh>
#include <G4SystemOfUnits.hh>

#include <vector>
#include <numeric>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cassert>

using std::vector;
using std::stringstream;
using std::max;

#include "TMath.h"
#include "TFile.h"
#include "TParameter.h"

//#include "PurdueDetectorMessenger.hh"-> To be updated still
//#include "PurdueEljenSensitiveDetector.hh"-> To be updated still
#include "HTPCDetectorConstruction.hh"
#include "G4PhysicalVolumeStore.hh"

map<G4String, G4double> PurdueDetectorConstruction::m_hGeometryParameters;

HTPCDetectorConstruction::HTPCDetectorConstruction(G4String fName)
{
  detRootFile = fName;
  m_pDetectorMessenger = new PurdueDetectorMessenger(this);
}

HTPCDetectorConstruction::~HTPCDetectorConstruction()
{
  delete m_pDetectorMessenger;
}

void HTPCDetectorConstruction::DefineGeometryParameters()
{
  m_hGeometryParameters["tolerance"] = 10. *mm;

  //============================Laboratory=====================================

  m_hGeometryParameters["Lab_height"] = 20000. *mm;
  m_hGeometryParameters["Lab_width"] = 20000. *mm;
  m_hGeometryParameters["Lab_depth"] = 20000. *mm;

  //===============================Cryostats===================================
  m_hGeometryParameters["oCryostat_oD"] = 3800. *mm;
  m_hGeometryParameters["oCryostat_H"] = 4500. *mm;

  m_hGeometryParameters["iCryostat_oD"] = 3700. *mm;
  m_hGeometryParameters["iCryostat_H"] = 4400. *mm;

  m_hGeometryParameters["oCryostatWall_thickness"] = 10. *mm;

  //===============================TPC===================================
  m_hGeometryParameters["TPC_oD"] = 3600. *mm;
  m_hGeometryParameters["TPC_H"] = 4000. *mm;
}

void HTPCDetectorConstruction:DefineMaterials()
{
    G4NistManager* pNistManager = G4NistManager::Instance();

    //========== Elements ==========
    G4Element *C = new G4Element("Carbon", "C", 6., 12.011 * g / mole);
    G4Element *Fe = new G4Element("Iron", "Fe", 26., 55.85 * g / mole);
    G4Element *Co = pNistManager->FindOrBuildElement("Co");

    //==== Water ====
    G4Material* water = pNistManager->FindOrBuildMaterial("G4_WATER")

    //==== Steel ====
    G4Material *Steel = new G4Material("Steel", 7.7 * g / cm3, 3);
    Steel->AddElement(C, 0.04);
    Steel->AddElement(Fe, 0.88);
    Steel->AddElement(Co, 0.08);

    //==== Vacuum ====
    G4Material *Vacuum = new G4Material("Vacuum", 1.e-20 * g / cm3, 2, kStateGas);
    Vacuum->AddElement(N, 0.755);
    Vacuum->AddElement(O, 0.245);

}

G4VPhysicalVolume* HTPCDetectorConstruction::Construct()
{
    DefineGeometryParameters();
    ConstructLab();
    ConstructCryostats();
    //ConstructDetector();

    return m_pLabPhysicalVolume
}

void HTPCDetectorConstruction:ConstructLab()
{
    G4Material *Water = G4Material::GetMaterial("G4_WATER");

    //Dimensions
    G4double Lab_height = GetGeometryParameter("Lab_height");
    G4double Lab_width = GetGeometryParameter("Lab_width");
    G4double Lab_depth = GetGeometryParameter("Lab_depth");

    //Lab
     G4Box *Lab = new G4Box("Lab", Lab_width/2, Lab_depth/2, Lab_height/2);

     m_pLabLogicalVolume =	new G4LogicalVolume(Lab, Water, "Lab_log");

     m_pLabPhysicalVolume =  new G4PVPlacement(0, G4ThreeVector(),
                                 m_pLabLogicalVolume, "Lab_phys", 0, false, 0);

     m_pLabLogicalVolume->SetVisAttributes(G4VisAttributes::Invisible);}
}

void HTPCDetectorConstruction:ConstructCryostats()
{
    G4Material *Steel = G4Material::GetMaterial("Steel");
    G4Material *Vacuum = G4Material::GetMaterial("Vacuum");

    //----- Outer Cryostat-----
    //Dimensions
    G4double opendeg = 0.0 *deg;
    G4double closedeg = 360.0 *deg;

    G4double oCryostat_oD = GetGeometryParameter("oCryostat_oD");
    G4double oCryostat_H = GetGeometryParameter("oCryostat_H");
    G4double oCryostatWall_thickness = GetGeometryParameter("oCryostatWall_thickness");

    //Container
    G4Tubs *oCryostat = new G4Tubs
        ("oCryostat", 0., oCryostat_oD/2, oCryostat_H/2, opendeg, closedeg);

    m_poCryostatLogicalVolume = new G4LogicalVolume(oCryostat, Steel, "oCryostat_log");

    m_poCryostatPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0,0,0),
		      m_poCryostatLogicalVolume, "oCryostat_phys",
		      m_pLabLogicalVolume, false, 0);

    //----- CryostatVacuum-----
    //Container
    G4Tubs *CryoVacuum = new G4Tubs
        ("oCryoVacuum",0, oCryostat_oD/2-oCryostatWall_thickness,
        oCryostat_H/2-oCryostatWall_thickness, opendeg, closedeg);

    m_pCryoVacuumLogicalVolume = new G4LogicalVolume(CryoVacuum, Vacuum, "CryoVacuum_log");

    m_pCryoVacuumPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0,0,0),
		      m_pCryoVacuumLogicalVolume, "CryoVacuum_phys",
		      m_poCryostatPhysicalVolume, false, 0);

    //----- Inner Cryostat-----
    //Dimensions
    G4double iCryostat_oD = GetGeometryParameter("iCryostat_oD");
    G4double iCryostat_H = GetGeometryParameter("iCryostat_H");

    //Container
    G4Tubs *iCryostat = new G4Tubs
        ("iCryostat", 0., iCryostat_oD/2, iCryostat_H/2, opendeg, closedeg);

    m_piCryostatLogicalVolume = new G4LogicalVolume(iCryostat, Steel, "iCryostat_log");

    m_piCryostatPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0,0,0),
		      m_piCryostatLogicalVolume, "iCryostat_phys",
		      m_pCryoVacuumPhysicalVolume, false, 0);

}


