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
#include "HTPCSensitiveDetector.hh"
#include "HTPCDetectorConstruction.hh"
#include "G4PhysicalVolumeStore.hh"

map<G4String, G4double> HTPCDetectorConstruction::m_hGeometryParameters;

HTPCDetectorConstruction::HTPCDetectorConstruction(G4String fName)
{
  detRootFile = fName;
  //m_pDetectorMessenger = new PurdueDetectorMessenger(this);
}

HTPCDetectorConstruction::~HTPCDetectorConstruction()
{
  //delete m_pDetectorMessenger;
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
  m_hGeometryParameters["iCryostatWall_thickness"] = 10. *mm;

  //===============================TPC===================================
  m_hGeometryParameters["TPC_oD"] = 3000. *mm;
  m_hGeometryParameters["TPC_H"] = 3150. *mm;
  m_hGeometryParameters["PTFE_thickness"] = 3. *mm;

  m_hGeometryParameters["GXe_H"] = 100. *mm;

  m_hGeometryParameters["Sapphire_oD"] = 950. *mm;
  m_hGeometryParameters["SapphireThickness"] = 3. *mm;

  m_hGeometryParameters["GasGap_H"] = 5. *mm;
  m_hGeometryParameters["CathodeGap"] = 50. *mm;
}

void HTPCDetectorConstruction::DefineMaterials()
{
    G4NistManager* pNistManager = G4NistManager::Instance();

    //========== Elements ==========
    G4Element *Xe = new G4Element("Xenon", "Xe", 54., 131.293 * g / mole);
    G4Element *C = new G4Element("Carbon", "C", 6., 12.011 * g / mole);
    G4Element *Fe = new G4Element("Iron", "Fe", 26., 55.85 * g / mole);
    G4Element *N = new G4Element("Nitrogen", "N", 7., 14.007 * g / mole);
    G4Element *O = new G4Element("Oxygen", "O", 8., 15.999 * g / mole);
    G4Element *F = new G4Element("Fluorine", "F", 9., 18.998 * g / mole);
    G4Element *Al = new G4Element("Aluminium", "Al", 13., 26.982 * g / mole);
    G4Element *Co = pNistManager->FindOrBuildElement("Co");

    //==== Water ====
    G4Material* Water = pNistManager->FindOrBuildMaterial("G4_WATER");

    //==== Steel ====
    G4Material *Steel = new G4Material("Steel", 7.7 * g / cm3, 3);
    Steel->AddElement(C, 0.04);
    Steel->AddElement(Fe, 0.88);
    Steel->AddElement(Co, 0.08);

    //==== Vacuum ====
    G4Material *Vacuum = new G4Material("Vacuum", 1.e-20 * g / cm3, 2, kStateGas);
    Vacuum->AddElement(N, 0.755);
    Vacuum->AddElement(O, 0.245);

    //==== Liquid Xenon ====
    G4Material *LXe = new G4Material("LXe", 2.862 * g / cm3, 1, kStateLiquid,
                                   177.05 * kelvin, 1.5 * atmosphere);
    LXe->AddElement(Xe, 1);

    // Optical properties LXe
    const G4int iNbEntries = 3;
    G4double pdLXePhotonMomentum[iNbEntries] = {6.91 * eV, 6.98 * eV, 7.05 * eV};
    G4double pdLXeScintillation[iNbEntries] = {0.1, 1.0, 0.1};
    G4double pdLXeRefractiveIndex[iNbEntries] = {1.63, 1.61, 1.58};
    G4double pdLXeAbsorbtionLength[iNbEntries] = {100. * cm, 100. * cm,
                                                100. * cm};
    G4double pdLXeScatteringLength[iNbEntries] = {30. * cm, 30. * cm, 30. * cm};
    G4MaterialPropertiesTable *pLXePropertiesTable = new G4MaterialPropertiesTable();
    pLXePropertiesTable->AddProperty("FASTCOMPONENT", pdLXePhotonMomentum, pdLXeScintillation, iNbEntries);
    pLXePropertiesTable->AddProperty("SLOWCOMPONENT", pdLXePhotonMomentum, pdLXeScintillation, iNbEntries);
    pLXePropertiesTable->AddProperty("RINDEX", pdLXePhotonMomentum, pdLXeRefractiveIndex, iNbEntries);
    pLXePropertiesTable->AddProperty("ABSLENGTH", pdLXePhotonMomentum, pdLXeAbsorbtionLength, iNbEntries);
    pLXePropertiesTable->AddProperty("RAYLEIGH", pdLXePhotonMomentum, pdLXeScatteringLength, iNbEntries);
    pLXePropertiesTable->AddConstProperty("SCINTILLATIONYIELD", 0. / keV);
    pLXePropertiesTable->AddConstProperty("RESOLUTIONSCALE", 0);
    pLXePropertiesTable->AddConstProperty("FASTTIMECONSTANT", 3. * ns);
    pLXePropertiesTable->AddConstProperty("SLOWTIMECONSTANT", 27. * ns);
    pLXePropertiesTable->AddConstProperty("YIELDRATIO", 1.0);
    pLXePropertiesTable->AddConstProperty("TOTALNUM_INT_SITES", -1);
    // initialize the number of interaction sites
    LXe->SetMaterialPropertiesTable(pLXePropertiesTable);

    //==== Gaseous Xenon ====
    G4Material *GXe = new G4Material("GXe", 0.005887 * g / cm3, 1, kStateGas,
                                   173.15 * kelvin, 1.5 * atmosphere);
    GXe->AddElement(Xe, 1);

    // Optical properties GXe
    G4double pdGXePhotonMomentum[iNbEntries] = {6.91 * eV, 6.98 * eV, 7.05 * eV};
    G4double pdGXeScintillation[iNbEntries] = {0.1, 1.0, 0.1};
    G4double pdGXeRefractiveIndex[iNbEntries] = {1.00, 1.00, 1.00};
    G4double pdGXeAbsorbtionLength[iNbEntries] = {100 * m, 100 * m, 100 * m};
    G4double pdGXeScatteringLength[iNbEntries] = {100 * m, 100 * m, 100 * m};
    G4MaterialPropertiesTable *pGXePropertiesTable = new G4MaterialPropertiesTable();
    pGXePropertiesTable->AddProperty("FASTCOMPONENT", pdGXePhotonMomentum, pdGXeScintillation, iNbEntries);
    pGXePropertiesTable->AddProperty("SLOWCOMPONENT", pdGXePhotonMomentum, pdGXeScintillation, iNbEntries);
    pGXePropertiesTable->AddProperty("RINDEX", pdGXePhotonMomentum, pdGXeRefractiveIndex, iNbEntries);
    pGXePropertiesTable->AddProperty("ABSLENGTH", pdGXePhotonMomentum, pdGXeAbsorbtionLength, iNbEntries);
    pGXePropertiesTable->AddProperty("RAYLEIGH", pdGXePhotonMomentum, pdGXeScatteringLength, iNbEntries);
    pGXePropertiesTable->AddConstProperty("SCINTILLATIONYIELD", 0. / (keV));
    pGXePropertiesTable->AddConstProperty("RESOLUTIONSCALE", 0);
    pGXePropertiesTable->AddConstProperty("FASTTIMECONSTANT", 3. * ns);
    pGXePropertiesTable->AddConstProperty("SLOWTIMECONSTANT", 27. * ns);
    pGXePropertiesTable->AddConstProperty("YIELDRATIO", 1.0);
    GXe->SetMaterialPropertiesTable(pGXePropertiesTable);

    //==== Teflon ====
    G4Material *Teflon = new G4Material("Teflon", 2.2 * g / cm3, 2, kStateSolid);
    Teflon->AddElement(C, 0.240183);
    Teflon->AddElement(F, 0.759817);

    G4double pdTeflonPhotonMomentum[iNbEntries] = {6.91 * eV, 6.98 * eV, 7.05 * eV};
    G4double pdTeflonRefractiveIndex[iNbEntries] = {1.63, 1.61, 1.58};
    G4double pdTeflonReflectivity[iNbEntries] = {0.99, 0.99, 0.99};
    G4double pdTeflonSpecularLobe[iNbEntries] = {0.01, 0.01, 0.01};
    G4double pdTeflonSpecularSpike[iNbEntries] = {0.01, 0.01, 0.01};
    G4double pdTeflonBackscatter[iNbEntries] = {0.01, 0.01, 0.01};
    G4double pdTeflonEfficiency[iNbEntries] = {1.0, 1.0, 1.0};
    G4double pdTeflonAbsorbtionLength[iNbEntries] = {0.1 * cm, 0.1 * cm, 0.1 * cm};

    //==== Quartz ====
    G4Material *Sapphire = new G4Material("Sapphire", 3.98 * g / cm3, 2, kStateSolid);
    Sapphire->AddElement(Al, 2);
    Sapphire->AddElement(O, 3);

}

G4VPhysicalVolume* HTPCDetectorConstruction::Construct()
{
    DefineMaterials();
    DefineGeometryParameters();
    ConstructLab();
    ConstructCryostats();
    ConstructDetector();

    return m_pLabPhysicalVolume;
}

void HTPCDetectorConstruction::ConstructLab()
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

     m_pLabLogicalVolume->SetVisAttributes(G4VisAttributes::Invisible);
}

void HTPCDetectorConstruction::ConstructCryostats()
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
		      m_poCryostatLogicalVolume, false, 0);

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
		      m_pCryoVacuumLogicalVolume, false, 0);

	//Vis Attributes
    G4VisAttributes* iCryostatVis = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0)); // Green
    m_piCryostatLogicalVolume->SetVisAttributes(iCryostatVis);

}

void HTPCDetectorConstruction::ConstructDetector()
{
    G4Material *LXe = G4Material::GetMaterial("LXe");
    G4Material *Teflon = G4Material::GetMaterial("Teflon");
    G4Material *GXe = G4Material::GetMaterial("GXe");
    G4Material *Sapphire = G4Material::GetMaterial("Sapphire");

    //----- LXe Volume-----
    //Dimensions
    G4double opendeg = 0.0 *deg;
    G4double closedeg = 360.0 *deg;

    G4double d_XeRes_oD = GetGeometryParameter("iCryostat_oD");
    G4double d_XeRes_H = GetGeometryParameter("iCryostat_H");
    G4double d_iCryoThick = GetGeometryParameter("iCryostatWall_thickness");

    //Container
    G4Tubs *LXeTub = new G4Tubs
        ("LXe", 0., d_XeRes_oD/2-d_iCryoThick, d_XeRes_H/2-d_iCryoThick, opendeg, closedeg);

    m_pLXeLogicalVolume = new G4LogicalVolume(LXeTub, LXe, "LXe_log");

    m_pLXePhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0,0,0),
		      m_pLXeLogicalVolume, "LXe_phys",
		      m_piCryostatLogicalVolume, false, 0);

    //Vis Attributes
    G4VisAttributes* LXeVis = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)); //Red
    m_pLXeLogicalVolume->SetVisAttributes(LXeVis);

    //----- PTFE Cylinder----
    //Dimensions

    G4double d_TPC_oD = GetGeometryParameter("TPC_oD");
    G4double d_TPC_H = GetGeometryParameter("TPC_H");
    G4double d_PTFEThickness = GetGeometryParameter("PTFE_thickness");

   //Container
    G4Tubs *PTFETub = new G4Tubs
        ("PTFETub", 0., d_TPC_oD/2, d_TPC_H/2, opendeg, closedeg);

    m_pPTFECylLogicalVolume = new G4LogicalVolume(PTFETub, Teflon, "PTFE_log");

    m_pPTFECylPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0,0,0),
		      m_pPTFECylLogicalVolume, "PTFE_phys",
		      m_pLXeLogicalVolume, false, 0);

    //Vis Attributes
    G4VisAttributes* PTFEVis = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0)); 
    m_pPTFECylLogicalVolume->SetVisAttributes(PTFEVis);

    //----- Active LXe----
    //Dimensions

    G4double d_GXe_H = GetGeometryParameter("GXe_H");
    G4double d_ALXe_H = (d_TPC_H-d_GXe_H)/2-d_PTFEThickness;

   //Container
    G4Tubs *ALXeTubs = new G4Tubs
        ("ALXeTubs", 0., d_TPC_oD/2-d_PTFEThickness, (d_TPC_H-d_GXe_H)/2-d_PTFEThickness, opendeg, closedeg);

    m_pALXeLogicalVolume = new G4LogicalVolume(ALXeTubs, LXe, "ALXe_log");

    G4SDManager *pSDManager = G4SDManager::GetSDMpointer();
      pHTPC_SD = new HTPCSensitiveDetector("HTPC_SD");
      pSDManager->AddNewDetector(pHTPC_SD);
      m_pALXeLogicalVolume->SetSensitiveDetector(pHTPC_SD);

    m_pALXePhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0,0,-d_GXe_H/2),
		      m_pALXeLogicalVolume, "ALXe_phys",
		      m_pPTFECylLogicalVolume, false, 0);

    //Vis Attributes
    G4VisAttributes* ALXeVis = new G4VisAttributes(G4Colour(1.0, 0.0, 1.0)); //Magenta
    m_pALXeLogicalVolume->SetVisAttributes(ALXeVis);

    //----- GXe----
	
   //Container
    G4Tubs *GXeTubs = new G4Tubs
        ("GXeTubs", 0., d_TPC_oD/2-d_PTFEThickness, d_GXe_H/2, opendeg, closedeg);

    m_pGXeLogicalVolume = new G4LogicalVolume(GXeTubs, GXe, "GXe_log");

    m_pGXePhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0,0, d_TPC_H/2 - d_GXe_H/2 - d_PTFEThickness),
		      m_pGXeLogicalVolume, "GXe_phys",
		      m_pPTFECylLogicalVolume, false, 0);

    //Vis Attributes
    G4VisAttributes* GXeVis = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0)); //Cyan
    m_pGXeLogicalVolume->SetVisAttributes(GXeVis);

    //----- Sapphire----
    //Consists of cylinder in gas and liquid.	

    //Dimensions
    G4double d_Sapphire_oD = GetGeometryParameter("Sapphire_oD");
    G4double d_SapphireThickness = GetGeometryParameter("SapphireThickness");
    G4double d_GasGap_H = GetGeometryParameter("GasGap_H");

    //LXe Container
    G4Tubs *SapphireLXeTubs = new G4Tubs
        ("SapphireLXeTubs", d_Sapphire_oD/2-d_SapphireThickness, d_Sapphire_oD/2, 
        (d_TPC_H-d_GXe_H)/2-d_PTFEThickness, opendeg, closedeg);

    m_pSapphireLXeLogicalVolume = new G4LogicalVolume(SapphireLXeTubs, Sapphire, "SapphireLXe_log");

    m_pSapphireLXePhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0, 0, 0),
		      m_pSapphireLXeLogicalVolume, "SapphireLXe_phys",
		      m_pALXeLogicalVolume, false, 0);

    //GXe Container
    G4Tubs *SapphireGXeTubs = new G4Tubs
        ("SapphireGXeTubs", d_Sapphire_oD/2-d_SapphireThickness, d_Sapphire_oD/2, d_GasGap_H/2, opendeg, closedeg);

    m_pSapphireGXeLogicalVolume = new G4LogicalVolume(SapphireGXeTubs, Sapphire, "SapphireGXe_log");

    m_pSapphireLXePhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0, 0, -d_GXe_H/2+d_GasGap_H/2),
		      m_pSapphireGXeLogicalVolume, "SapphireGXe_phys",
		      m_pGXeLogicalVolume, false, 0);

    //----- Sapphire Anode----
    //Solid plane within GXe

    //Anode Plane
    G4Tubs *SapphireAnode = new G4Tubs
        ("SapphireAnode", 0, d_TPC_oD/2-d_PTFEThickness, d_SapphireThickness/2, opendeg, closedeg);

    m_pSapphireAnodeLogicalVolume = new G4LogicalVolume(SapphireAnode, Sapphire, "SapphireAnode_log");

    m_pSapphireAnodePhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0, 0, -d_GXe_H/2+d_GasGap_H+d_SapphireThickness/2),
		      m_pSapphireAnodeLogicalVolume, "SapphireAnode_phys",
		      m_pGXeLogicalVolume, false, 0);

    //----- Sapphire Cylinder Bottom----
    //Solid plane within LXe
    G4double d_CathodeGap = GetGeometryParameter("CathodeGap");

    G4Tubs *SapphireBot = new G4Tubs
        ("SapphireBot", 0, d_Sapphire_oD/2-d_SapphireThickness, d_SapphireThickness/2, opendeg, closedeg);

    m_pSapphireBotLogicalVolume = new G4LogicalVolume(SapphireBot, Sapphire, "SapphireBot_log");

    m_pSapphireBotPhysicalVolume = new G4PVPlacement(0,
              G4ThreeVector(0, 0, -d_ALXe_H+d_CathodeGap+d_SapphireThickness/2),
		      m_pSapphireBotLogicalVolume, "SapphireBot_phys",
		      m_pALXeLogicalVolume, false, 0);

    //Vis Attributes
    G4VisAttributes* SapphireVis = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0)); //Yellow
    m_pSapphireLXeLogicalVolume->SetVisAttributes(SapphireVis);
    m_pSapphireGXeLogicalVolume->SetVisAttributes(SapphireVis);
    m_pSapphireAnodeLogicalVolume->SetVisAttributes(SapphireVis);
    m_pSapphireBotLogicalVolume->SetVisAttributes(SapphireVis);

}

G4double HTPCDetectorConstruction::GetGeometryParameter(const char *szParameter)
{
  if (m_hGeometryParameters.find(szParameter) != m_hGeometryParameters.end()){
    return m_hGeometryParameters[szParameter];
  }
  else {
    G4cout<< "Parameter: " << szParameter << " is not defined!!!!!" << G4endl;
    return 0;
  }
}


