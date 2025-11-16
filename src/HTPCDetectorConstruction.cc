#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Sphere.hh>
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
  G4cout << "[Construct] iCryostat_Alpha=" << iCryostat_Alpha
       << " LXeMedium_Alpha=" << LXeMedium_Alpha << G4endl;

  ApplyMessengers();
}

HTPCDetectorConstruction::~HTPCDetectorConstruction()
{
  //delete m_pDetectorMessenger;
}

void HTPCDetectorConstruction::ApplyMessengers() {

    fMessengerAlpha = new G4GenericMessenger(this,
                                        "/Alpha/",
                                        "Alpha");

    fMessengerAlpha->DeclareProperty("iCryostat_Alpha",
                                iCryostat_Alpha,
                                "iCryostat_Alpha");
 
    fMessengerAlpha->DeclareProperty("oCryostat_Alpha",
                                oCryostat_Alpha,
                                "oCryostat_Alpha");

    fMessengerAlpha->DeclareProperty("CryostatVacuum_Alpha",
                                CryostatVacuum_Alpha,
                                "CryostatVacuum_Alpha");

    fMessengerAlpha->DeclareProperty("Teflon_Alpha",
                                Teflon_Alpha,
                                "Teflon_Alpha");

    fMessengerAlpha->DeclareProperty("GXeMedium_Alpha",
                                GXeMedium_Alpha,
                                "GXeMedium_Alpha");

    fMessengerAlpha->DeclareProperty("LXeMedium_Alpha",
                                LXeMedium_Alpha,
                                "LXeMedium_Alpha");

    fMessengerAlpha->DeclareProperty("GXeActive_Alpha",
                                GXeActive_Alpha,
                                "GXeActive_Alpha");

    fMessengerAlpha->DeclareProperty("LXeActive_Alpha",
                                LXeActive_Alpha,
                                "LXeActive_Alpha");

    fMessengerAlpha->DeclareProperty("Sapphire_Alpha",
                                Sapphire_Alpha,
                                "Sapphire_Alpha");
                        
    // Defaults
    iCryostat_Alpha      = 1.0;
    oCryostat_Alpha      = 0.2;
    CryostatVacuum_Alpha = 0.0;
    Teflon_Alpha         = 1.0;
    GXeMedium_Alpha      = 0.3;
    LXeMedium_Alpha      = 0.3;
    GXeActive_Alpha      = 0.5;
    LXeActive_Alpha      = 0.5;
    Sapphire_Alpha       = 0.7;
}

void HTPCDetectorConstruction::DefineGeometryParameters()
{
     //============================Tolerances==================================
    m_hGeometryParameters["kTol"]      = 0.1 *mm;

    //============================Laboratory===================================
    m_hGeometryParameters["Lab_height"] = 19000. *mm;
    m_hGeometryParameters["Lab_width"]  = 19000. *mm;
    m_hGeometryParameters["Lab_depth"]  = 19000. *mm;

    //===============================Cryostats=================================
    m_hGeometryParameters["oCryostat_oD"] = 3400. *mm;
    m_hGeometryParameters["oCryostat_H"]  = 4000. *mm; 

    m_hGeometryParameters["iCryostat_oD"] = 3100. *mm;
    m_hGeometryParameters["iCryostat_H"]  = 3900. *mm;

    m_hGeometryParameters["oCryostatWall_thickness"] = 10. *mm;
    m_hGeometryParameters["iCryostatWall_thickness"] = 10. *mm;

    m_hGeometryParameters["curveLength"] = 1600 *mm;

    //===============================Flanges===================================
    m_hGeometryParameters["oFlangeRelativeHeight"] = 0.9;
    m_hGeometryParameters["oFlangeExtension"]      = 100 *mm;
    m_hGeometryParameters["oFlangeThickness"]      = 100 *mm;

    m_hGeometryParameters["iFlangeRelativeHeight"] = 0.9;
    m_hGeometryParameters["iFlangeExtension"]      = 100 *mm;
    m_hGeometryParameters["iFlangeThickness"]      = 100 *mm;

    //==========================Support-Rings==================================
    m_hGeometryParameters["sRingExtension"] = 100 *mm;
    m_hGeometryParameters["sRingThickness"] = 10 *mm;

    // =============================Media======================================
    m_hGeometryParameters["LiquidGasRatio"] = 0.85;

    //===============================TPC=======================================
    m_hGeometryParameters["TPC_oD"] = 3000. *mm;
    m_hGeometryParameters["TPC_H"]  = 3150. *mm;
    m_hGeometryParameters["PTFE_thickness"] = 3. *mm;

    m_hGeometryParameters["GXe_H"]    = 100. *mm;
    m_hGeometryParameters["PMTGXe_H"] = 150. *mm;

    m_hGeometryParameters["Sapphire_oD"]       = 750. *mm;
    m_hGeometryParameters["SapphireThickness"] = 3. *mm;

    m_hGeometryParameters["AnodeThickness"] = 3. *mm;

    m_hGeometryParameters["GasGap_H"]   = 5. *mm;
    m_hGeometryParameters["CathodeGap"] = 50. *mm;

}

void HTPCDetectorConstruction::DefineMaterials()
{
    G4NistManager* pNistManager = G4NistManager::Instance();

    //========== Elements ==========
    G4Element *Xe = new G4Element("Xenon", "Xe", 54., 131.293 * g / mole);
    G4Element *C  = new G4Element("Carbon", "C", 6., 12.011 * g / mole);
    G4Element *Fe = new G4Element("Iron", "Fe", 26., 55.85 * g / mole);
    G4Element *N  = new G4Element("Nitrogen", "N", 7., 14.007 * g / mole);
    G4Element *O  = new G4Element("Oxygen", "O", 8., 15.999 * g / mole);
    G4Element *F  = new G4Element("Fluorine", "F", 9., 18.998 * g / mole);
    G4Element *Al = new G4Element("Aluminium", "Al", 13., 26.982 * g / mole);
    G4Element *Si = new G4Element("Silicon", "Si", 14., 28.086 * g / mole);
    G4Element *Ni = new G4Element("Nickel", "Ni", 28., 58.693 * g / mole);
    G4Element *Mn = new G4Element("Manganese", "Mn", 25., 54.938 * g / mole);
    G4Element *Co = pNistManager->FindOrBuildElement("Co");

    //==== Teflon ====
    G4Material *Teflon = new G4Material(
        "Teflon", 
        2.2 * g / cm3, 
        2, 
        kStateSolid
    );

    Teflon->AddElement(C, 0.240183);
    Teflon->AddElement(F, 0.759817);

     //==== Photocathode Aluminium ====
    G4Material *PhotoCathodeAluminium = new G4Material(
        "PhotoCathodeAluminium", 
        8.00 * g / cm3, 
        1, 
        kStateSolid
    );

    PhotoCathodeAluminium->AddElement(Al, 1);

    //==== PMT Ceramic ====
    G4Material *Ceramic = new G4Material(
        "Ceramic", 
        4. * g / cm3, 
        2, 
        kStateSolid,
        168.15 * kelvin, 
        1.5 * atmosphere
    );

    Ceramic->AddElement(Al, 2);
    Ceramic->AddElement(O, 3);

    //==== Kovar ====
    G4Material *Kovar = new G4Material(
        "Kovar", 
        8.33 * g / cm3, 
        6, 
        kStateSolid
    );

    Kovar->AddElement(Fe, 0.5358);
    Kovar->AddElement(Ni, 0.29);
    Kovar->AddElement(Co, 0.17);
    Kovar->AddElement(C, 0.0002);
    Kovar->AddElement(Si, 0.001);
    Kovar->AddElement(Mn, 0.003);

    //==== Water ====
    G4Material* Water = pNistManager->FindOrBuildMaterial("G4_WATER");

    //==== Steel ====
    G4Material *Steel = new G4Material(
        "Steel", 
        7.7 * g / cm3, 
        3
    );

    Steel->AddElement(C, 0.04);
    Steel->AddElement(Fe, 0.88);
    Steel->AddElement(Co, 0.08);

    //==== Vacuum ====
    G4Material *Vacuum = new G4Material("Vacuum", 
        1.e-20 * g / cm3, 
        2, 
        kStateGas
    );

    Vacuum->AddElement(N, 0.755);
    Vacuum->AddElement(O, 0.245);

    //==== Quartz ====
    // ref: http://www.sciner.com/Opticsland/FS.htm
    G4Material *Quartz = new G4Material(
        "Quartz", 
        2.201 * g / cm3, 
        2, 
        kStateSolid,
        168.15 * kelvin, 
        1.5 * atmosphere
    );

    Quartz->AddElement(Si, 1);
    Quartz->AddElement(O, 2);

    //==== Liquid Xenon ====
    G4Material *LXe = new G4Material(
        "LXe", 
        2.862 * g / cm3, 
        1, 
        kStateLiquid,
        177.05 * kelvin, 
        1.5 * atmosphere
    );

    LXe->AddElement(Xe, 1);

    // Optical properties LXe
    const G4int iNbEntries = 3;
    G4double pdLXePhotonMomentum[iNbEntries]   = {6.91 * eV, 6.98 * eV, 7.05 * eV};
    G4double pdLXeScintillation[iNbEntries]    = {0.1, 1.0, 0.1};
    G4double pdLXeRefractiveIndex[iNbEntries]  = {1.63, 1.61, 1.58};
    G4double pdLXeAbsorbtionLength[iNbEntries] = {100. * cm, 100. * cm, 100. * cm};
    G4double pdLXeScatteringLength[iNbEntries] = {30. * cm, 30. * cm, 30. * cm};

    G4MaterialPropertiesTable *pLXePropertiesTable = new G4MaterialPropertiesTable();
    pLXePropertiesTable->AddProperty(
        "FASTCOMPONENT", 
        pdLXePhotonMomentum, 
        pdLXeScintillation, 
        iNbEntries
    );
    pLXePropertiesTable->AddProperty(
        "SLOWCOMPONENT", 
        pdLXePhotonMomentum, 
        pdLXeScintillation, 
        iNbEntries
    );
    pLXePropertiesTable->AddProperty(
        "RINDEX", 
        pdLXePhotonMomentum, 
        pdLXeRefractiveIndex, 
        iNbEntries
    );
    pLXePropertiesTable->AddProperty(
        "ABSLENGTH", 
        pdLXePhotonMomentum, 
        pdLXeAbsorbtionLength, 
        iNbEntries
    );
    pLXePropertiesTable->AddProperty(
        "RAYLEIGH", 
        pdLXePhotonMomentum, 
        pdLXeScatteringLength, 
        iNbEntries
    );
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
    G4double pdGXePhotonMomentum[iNbEntries]   = {6.91 * eV, 6.98 * eV, 7.05 * eV};
    G4double pdGXeScintillation[iNbEntries]    = {0.1, 1.0, 0.1};
    G4double pdGXeRefractiveIndex[iNbEntries]  = {1.00, 1.00, 1.00};
    G4double pdGXeAbsorbtionLength[iNbEntries] = {100 * m, 100 * m, 100 * m};
    G4double pdGXeScatteringLength[iNbEntries] = {100 * m, 100 * m, 100 * m};

    G4MaterialPropertiesTable *pGXePropertiesTable = new G4MaterialPropertiesTable();
    pGXePropertiesTable->AddProperty(
        "FASTCOMPONENT", 
        pdGXePhotonMomentum, 
        pdGXeScintillation, 
        iNbEntries
    );
    pGXePropertiesTable->AddProperty(
        "SLOWCOMPONENT", 
        pdGXePhotonMomentum, 
        pdGXeScintillation, 
        iNbEntries
    );
    pGXePropertiesTable->AddProperty(
        "RINDEX", 
        pdGXePhotonMomentum, 
        pdGXeRefractiveIndex, 
        iNbEntries
    );
    pGXePropertiesTable->AddProperty(
        "ABSLENGTH", 
        pdGXePhotonMomentum, 
        pdGXeAbsorbtionLength, 
        iNbEntries
    );
    pGXePropertiesTable->AddProperty(
        "RAYLEIGH", 
        pdGXePhotonMomentum, 
        pdGXeScatteringLength, 
        iNbEntries
    );
    pGXePropertiesTable->AddConstProperty("SCINTILLATIONYIELD", 0. / (keV));
    pGXePropertiesTable->AddConstProperty("RESOLUTIONSCALE", 0);
    pGXePropertiesTable->AddConstProperty("FASTTIMECONSTANT", 3. * ns);
    pGXePropertiesTable->AddConstProperty("SLOWTIMECONSTANT", 27. * ns);
    pGXePropertiesTable->AddConstProperty("YIELDRATIO", 1.0);
    GXe->SetMaterialPropertiesTable(pGXePropertiesTable);

    G4double pdTeflonPhotonMomentum[iNbEntries]   = {6.91 * eV, 6.98 * eV, 7.05 * eV};
    G4double pdTeflonRefractiveIndex[iNbEntries]  = {1.63, 1.61, 1.58};
    G4double pdTeflonReflectivity[iNbEntries]     = {0.99, 0.99, 0.99};
    G4double pdTeflonSpecularLobe[iNbEntries]     = {0.01, 0.01, 0.01};
    G4double pdTeflonSpecularSpike[iNbEntries]    = {0.01, 0.01, 0.01};
    G4double pdTeflonBackscatter[iNbEntries]      = {0.01, 0.01, 0.01};
    G4double pdTeflonEfficiency[iNbEntries]       = {1.0, 1.0, 1.0};
    G4double pdTeflonAbsorbtionLength[iNbEntries] = {0.1 * cm, 0.1 * cm, 0.1 * cm};

    //==== Sapphire ====
    G4Material *Sapphire = new G4Material("Sapphire", 3.98 * g / cm3, 2, kStateSolid);
    Sapphire->AddElement(Al, 2);
    Sapphire->AddElement(O, 3);

}

// -- Geometry Helper Functions --
namespace {
    // Builds a cylinder with two domes
    G4VSolid* BuildCapsule(G4double Rmax,
                           G4double Rmin,
                           G4double Dz,
                           G4double DomeCurve);

    // Build a cylinder with only one dome (wheredome = 0 (bottom), 1 (top))                       
    G4VSolid* BuildHalfCapsule(G4double Rmax, 
                               G4double Dz, 
                               G4double DomeCurve,
                               G4int    whereDome);

    G4VSolid* AddFlange(G4VSolid* baseSolid,
                        G4double Rmax,
                        G4double Rmin,
                        G4double thickness,
                        G4double zpos);
}


G4VPhysicalVolume* HTPCDetectorConstruction::Construct()
{
    DefineMaterials();
    DefineGeometryParameters();
    ConstructLab();
    ConstructCryostats();
    ConstructMedia();
    ConstructTPC();

    return phys_Lab;
}

void HTPCDetectorConstruction::ConstructLab()
{
    G4Material *Water = G4Material::GetMaterial("G4_WATER");

    // ----- Lab --------------------------------------------------------------
    G4double Lab_height = GetGeometryParameter("Lab_height");
    G4double Lab_width  = GetGeometryParameter("Lab_width");
    G4double Lab_depth  = GetGeometryParameter("Lab_depth");

    G4Box *solid_Lab = new G4Box(
        "solid_Lab", 
        Lab_width/2, 
        Lab_depth/2, 
        Lab_height/2
    );

    logic_Lab = new G4LogicalVolume(
        solid_Lab, 
        Water, 
        "logic_Lab"
    );

    phys_Lab = new G4PVPlacement(
        0, 
        G4ThreeVector(),
        logic_Lab, 
        "phys_Lab", 
        0, 
        false, 
        0
    );

    // VisAttributes
    logic_Lab->SetVisAttributes(G4VisAttributes::Invisible);
}


void HTPCDetectorConstruction::ConstructCryostats()
{
    G4Material *Steel  = G4Material::GetMaterial("Steel");
    G4Material *Vacuum = G4Material::GetMaterial("Vacuum");

    // ----- Outer Cryostat (oCryostat) ---------------------------------------
    G4double opendeg  = 0.0 *deg;
    G4double closedeg = 360.0 *deg;

    G4double kTol = GetGeometryParameter("kTol");
    
    G4double oCryostat_oD = GetGeometryParameter("oCryostat_oD");
    G4double oCryostat_H  = GetGeometryParameter("oCryostat_H");
    G4double oCryostatWall_thickness = GetGeometryParameter("oCryostatWall_thickness");

    G4double curveLength = GetGeometryParameter("curveLength");

    G4double oFlangeRelativeHeight = GetGeometryParameter("oFlangeRelativeHeight");
    G4double oFlangeThickness      = GetGeometryParameter("oFlangeThickness");
    G4double oFlangeExtension      = GetGeometryParameter("oFlangeExtension");
    G4double oFlangeHeight = oFlangeRelativeHeight*oCryostat_H/2;
    
    G4double sRingExtension = GetGeometryParameter("sRingExtension");
    G4double sRingThickness = GetGeometryParameter("sRingThickness");
    
    auto solidCapsule1 = BuildCapsule(
        oCryostat_oD/2,
        0.,
        oCryostat_H/2, 
        curveLength
    );

    auto oCryostat_Temp1 = AddFlange(
        solidCapsule1, 
        oCryostat_oD/2 + oFlangeExtension, 
        oCryostat_oD/2, 
        oFlangeThickness,
        oFlangeHeight
    );

    // |__ Add bottom support ring
    auto oCryostat_Temp2 = AddFlange(
        oCryostat_Temp1, 
        oCryostat_oD/2 + sRingExtension, 
        oCryostat_oD/2, 
        sRingThickness,
        -0.5*oCryostat_H/2
    );

    // _|_ Add middle support ring
    auto oCryostat_Temp3 = AddFlange(
        oCryostat_Temp2, 
        oCryostat_oD/2 + sRingExtension, 
        oCryostat_oD/2, 
        sRingThickness,
        0
    );

    // __| Add top support ring
    auto solid_oCryostat = AddFlange(
        oCryostat_Temp3, 
        oCryostat_oD/2 + sRingExtension, 
        oCryostat_oD/2, 
        sRingThickness,
        0.5*oCryostat_H/2
    );

    logic_oCryostat = new G4LogicalVolume(
        solid_oCryostat,
        Steel,
        "logic_oCryostat"
    );

    auto pos_oCryostat  = G4ThreeVector(0., 0., 0.);
    phys_oCryostat = new G4PVPlacement(
        0,
        pos_oCryostat,
        logic_oCryostat,
        "phys_oCryostat",
        logic_Lab,
        false,
        0,
        true
    );


    // VisAttributes
    auto col_oCryostat = G4Colour(1., 1., 1., oCryostat_Alpha);
    G4VisAttributes* vis_oCryostat = new G4VisAttributes(col_oCryostat);
    vis_oCryostat  ->SetVisibility(true);
    vis_oCryostat  ->SetForceWireframe(true);
    vis_oCryostat  ->SetForceAuxEdgeVisible(true);
    logic_oCryostat->SetVisAttributes(vis_oCryostat);

    // ----- CryostatVacuum ---------------------------------------------------
    auto solid_CryostatVacuum = BuildCapsule(
        oCryostat_oD/2 - oCryostatWall_thickness, 
        0.*m, 
        oCryostat_H/2 , 
        curveLength
    );

    logic_CryostatVacuum = new G4LogicalVolume(
        solid_CryostatVacuum,
        Vacuum,
        "logic_CryostatVacuum"
    );

    auto pos_CryostatVacuum  = G4ThreeVector(0., 0., 0.);
    phys_CryostatVacuum = new G4PVPlacement(
        0,
        pos_CryostatVacuum,
        logic_CryostatVacuum,
        "phys_CryostatVacuum",
        logic_oCryostat,
        false,
        0,
        true
    );

    // VisAttributes
    auto col_CryostatVacuum = G4Colour(1., 1., 1., CryostatVacuum_Alpha);
    G4VisAttributes* vis_CryostatVacuum = new G4VisAttributes(col_CryostatVacuum);
    vis_CryostatVacuum  ->SetVisibility(true);
    vis_CryostatVacuum  ->SetForceSolid(true);
    logic_CryostatVacuum->SetVisAttributes(vis_CryostatVacuum);
    
    // ----- Inner Cryostat (iCryostat) ---------------------------------------
    G4double iCryostat_oD = GetGeometryParameter("iCryostat_oD");
    G4double iCryostat_H  = GetGeometryParameter("iCryostat_H");
    G4double iCryostatWall_thickness = GetGeometryParameter("iCryostatWall_thickness");

    G4double iFlangeRelativeHeight = GetGeometryParameter("iFlangeRelativeHeight");
    G4double iFlangeThickness = GetGeometryParameter("iFlangeThickness");
    G4double iFlangeExtension = GetGeometryParameter("iFlangeExtension");

    G4double iFlangeHeight = iFlangeRelativeHeight*iCryostat_H/2;
    
    auto solidCapsule2 = BuildCapsule(
        iCryostat_oD/2,
        0.,
        iCryostat_H/2, 
        curveLength
    );

    auto iCryostat_Temp1 = AddFlange(
        solidCapsule2, 
        iCryostat_oD/2 + iFlangeExtension, 
        iCryostat_oD/2, 
        iFlangeThickness, 
        iFlangeHeight
    );

    // |__ Add bottom support ring
    auto iCryostat_Temp2 = AddFlange(
        iCryostat_Temp1, 
        iCryostat_oD/2 + sRingExtension, 
        iCryostat_oD/2, 
        sRingThickness,
        -0.5*iCryostat_H/2
    );

    // _|_ Add middle support ring
    auto iCryostat_Temp3 = AddFlange(
        iCryostat_Temp2, 
        iCryostat_oD/2 + sRingExtension, 
        iCryostat_oD/2, 
        sRingThickness,
        0
    );

    // __| Add top support ring
    auto solid_iCryostat = AddFlange(
        iCryostat_Temp3, 
        iCryostat_oD/2 + sRingExtension, 
        iCryostat_oD/2, 
        sRingThickness,
        0.5*iCryostat_H/2
    );

    logic_iCryostat = new G4LogicalVolume(
        solid_iCryostat,
        Steel,
        "logic_iCryostat"
    );

    auto pos_iCryostat  = G4ThreeVector(0., 0., 0.);
    phys_iCryostat = new G4PVPlacement(
        0,
        pos_iCryostat,
        logic_iCryostat,
        "phys_iCryostat",
        logic_CryostatVacuum,
        false,
        0,
        true
    );

    // VisAttributes
    auto col_iCryostat = G4Colour(1., 1., 1., iCryostat_Alpha);
    G4VisAttributes* vis_iCryostat = new G4VisAttributes(col_iCryostat);
    vis_iCryostat  ->SetVisibility(true);
    vis_iCryostat  ->SetForceWireframe(true);
    vis_iCryostat  ->SetForceAuxEdgeVisible(true);
    logic_iCryostat->SetVisAttributes(vis_iCryostat);
}


void HTPCDetectorConstruction::ConstructMedia()
{
    G4Material *LXe = G4Material::GetMaterial("LXe");
    G4Material *GXe = G4Material::GetMaterial("GXe");

    G4double kTol = GetGeometryParameter("kTol");

    G4double iCryostat_oD = GetGeometryParameter("iCryostat_oD");
    G4double iCryostat_H  = GetGeometryParameter("iCryostat_H");
    G4double iCryostatWall_thickness = GetGeometryParameter("iCryostatWall_thickness");

    G4double curveLength = GetGeometryParameter("curveLength");

    G4double LiquidGasRatio = GetGeometryParameter("LiquidGasRatio");

    // ----- GXe Medium (GXeMedium) -------------------------------------------
    G4double GXeMedium_oD = iCryostat_oD - 2*iCryostatWall_thickness;
    G4double GXeMedium_H  = iCryostat_H * (1-LiquidGasRatio);

    auto solid_GXeMedium = BuildHalfCapsule(
        GXeMedium_oD/2, 
        GXeMedium_H/2, 
        curveLength, 
        1 // 'top'
    );

    logic_GXeMedium = new G4LogicalVolume(
        solid_GXeMedium,
        GXe,
        "logic_GXeMedium"
    );

    auto pos_GXeMedium  = G4ThreeVector(0., 0., ((iCryostat_H/2) - GXeMedium_H/2));
    phys_GXeMedium = new G4PVPlacement(
        0,
        pos_GXeMedium,
        logic_GXeMedium,
        "phys_GXeMedium",
        logic_iCryostat,
        false,
        0,
        true
    );

    // VisAttributes
    auto col_GXeMedium = G4Colour(0., 1, 0., GXeMedium_Alpha);
    G4VisAttributes* vis_GXeMedium = new G4VisAttributes(col_GXeMedium);
    vis_GXeMedium  ->SetVisibility(true);
    vis_GXeMedium  ->SetForceSolid(true);
    logic_GXeMedium->SetVisAttributes(vis_GXeMedium);


    // ----- LXe Medium (LXeMedium) -------------------------------------------
    G4double LXeMedium_oD = iCryostat_oD - 2*iCryostatWall_thickness;
    G4double LXeMedium_H  = iCryostat_H * LiquidGasRatio;

    auto solid_LXeMedium = BuildHalfCapsule(
        LXeMedium_oD/2, 
        LXeMedium_H/2, 
        curveLength, 
        0 // 'bottom'
    );

    logic_LXeMedium = new G4LogicalVolume(
        solid_LXeMedium,
        LXe,
        "logic_LXeMedium"
    );

    auto pos_LXeMedium  = G4ThreeVector(0., 0., -((iCryostat_H/2) - LXeMedium_H/2));
    phys_LXeMedium = new G4PVPlacement(
        0,
        pos_LXeMedium,
        logic_LXeMedium,
        "phys_LXeMedium",
        logic_iCryostat,
        false,
        0,
        true
    );

    // VisAttributes
    auto col_LXeMedium = G4Colour(1.0, 0.0, 1.0, GXeMedium_Alpha);
    G4VisAttributes* vis_LXeMedium = new G4VisAttributes(col_LXeMedium);
    vis_LXeMedium  ->SetVisibility(true);
    vis_LXeMedium  ->SetForceSolid(true);
    logic_LXeMedium->SetVisAttributes(vis_LXeMedium);
}


void HTPCDetectorConstruction::ConstructTPC()
{
    G4Material* LXe      = G4Material::GetMaterial("LXe");
    G4Material* Teflon   = G4Material::GetMaterial("Teflon");
    G4Material* GXe      = G4Material::GetMaterial("GXe");
    G4Material* Sapphire = G4Material::GetMaterial("Sapphire");
    
    G4double kTol = GetGeometryParameter("kTol");

    G4double TPC_oD         = GetGeometryParameter("TPC_oD");
    G4double TPC_H          = GetGeometryParameter("TPC_H");
    G4double PTFE_thickness = GetGeometryParameter("PTFE_thickness");
    G4double GXe_H          = GetGeometryParameter("GXe_H");

    G4double iCryostat_H    = GetGeometryParameter("iCryostat_H");
    G4double LiquidGasRatio = GetGeometryParameter("LiquidGasRatio");

    G4double GXeTeflonTub_H = GXe_H;
    G4double LXeTeflonTub_H = TPC_H - GXe_H;


    // ----- Teflon Tub -------------------------------------------------------

    // GXeTeflonTub
    G4Tubs* solid_GXeTeflonTub = new G4Tubs(
        "solid_GXeTeflonTub",
        0, // TPC_oD/2 - PTFE_thickness,
        TPC_oD/2,
        GXeTeflonTub_H/2,
        0.   *deg,
        360. *deg
    );

    logic_GXeTeflonTub = new G4LogicalVolume(
        solid_GXeTeflonTub,
        Teflon,
        "logic_GXeTeflonTub"
    );

    G4double z_GXeTeflonTub = (iCryostat_H/2) * (1-LiquidGasRatio) - GXeTeflonTub_H/2;
    auto pos_GXeTeflonTub = G4ThreeVector(0., 0., - z_GXeTeflonTub);
    phys_GXeTeflonTub = new G4PVPlacement(
        0,
        pos_GXeTeflonTub,
        logic_GXeTeflonTub,
        "phys_GXeTeflonTub",
        logic_GXeMedium,
        false,
        0,
        true
    );

    // LXeTeflonTub
    G4Tubs* solid_LXeTeflonTub = new G4Tubs(
        "solid_LXeTeflonTub",
        0, //TPC_oD/2 - PTFE_thickness,
        TPC_oD/2,
        LXeTeflonTub_H/2,
        0.   *deg,
        360. *deg
    );

    logic_LXeTeflonTub = new G4LogicalVolume(
        solid_LXeTeflonTub,
        Teflon,
        "logic_LXeTeflonTub"
    );

    G4double z_LXeTeflonTub = (iCryostat_H/2) * (LiquidGasRatio) - LXeTeflonTub_H/2;
    auto pos_LXeTeflonTub = G4ThreeVector(0., 0., + z_LXeTeflonTub);
    phys_LXeTeflonTub = new G4PVPlacement(
        0,
        pos_LXeTeflonTub,
        logic_LXeTeflonTub,
        "phys_LXeTeflonTub",
        logic_LXeMedium,
        false,
        0,
        true
    );

    // VisAttributes
    auto col_Teflon = G4Colour(0.0, 0.0, 1.0, Teflon_Alpha);
    G4VisAttributes* vis_Teflon = new G4VisAttributes(col_Teflon);
    vis_Teflon        ->SetVisibility(true);
    //vis_Teflon      ->SetForceSolid(true);
    vis_Teflon        ->SetForceWireframe(true);
    vis_Teflon        ->SetForceAuxEdgeVisible(true);
    logic_GXeTeflonTub->SetVisAttributes(vis_Teflon);
    logic_LXeTeflonTub->SetVisAttributes(vis_Teflon);


    // ----- Active Media -----------------------------------------------------
    
    // GXeActive
    G4Tubs* solid_GXeActive = new G4Tubs(
        "solid_GXeActive",
        0.,
        TPC_oD/2 - PTFE_thickness,
        GXeTeflonTub_H/2 - PTFE_thickness/2,
        0.   *deg,
        360. *deg
    );

    logic_GXeActive = new G4LogicalVolume(
        solid_GXeActive,
        GXe,
        "logic_GXeActive"
    );

    auto pos_GXeActive = G4ThreeVector(0., 0., -PTFE_thickness/2);
    phys_GXeActive = new G4PVPlacement(
        0,
        pos_GXeActive,
        logic_GXeActive,
        "phys_GXeActive",
        logic_GXeTeflonTub,
        false,
        0,
        true
    );

    // VisAttributes
    auto col_GXeActive = G4Colour(0., 1., 0., GXeActive_Alpha);
    G4VisAttributes* vis_GXeActive = new G4VisAttributes(col_GXeActive);
    vis_GXeActive   ->SetVisibility(true);
    vis_GXeActive   ->SetForceSolid(true);
    logic_GXeActive ->SetVisAttributes(vis_GXeActive);

    // LXeActive
    G4Tubs* solid_LXeActive = new G4Tubs(
        "solid_LXeActive",
        0.,
        TPC_oD/2 - PTFE_thickness,
        LXeTeflonTub_H/2 - PTFE_thickness/2,
        0.   *deg,
        360. *deg
    );

    logic_LXeActive = new G4LogicalVolume(
        solid_LXeActive,
        LXe,
        "logic_LXeActive"
    );

    auto pos_LXeActive = G4ThreeVector(0., 0., PTFE_thickness/2);
    phys_LXeActive = new G4PVPlacement(
        0,
        pos_LXeActive,
        logic_LXeActive,
        "phys_LXeActive",
        logic_LXeTeflonTub,
        false,
        0,
        true
    );

    // Defining LXeActive as a sensitive detector
    G4SDManager *pSDManager = G4SDManager::GetSDMpointer();
    LXeSensDet = new HTPCSensitiveDetector("LXeSensDet");
    pSDManager->AddNewDetector(LXeSensDet);
    logic_LXeActive->SetSensitiveDetector(LXeSensDet);

    // VisAttributes
    auto col_LXeActive = G4Colour(1., 0., 1., LXeActive_Alpha);
    G4VisAttributes* vis_LXeActive = new G4VisAttributes(col_LXeActive);
    vis_LXeActive   ->SetVisibility(true);
    vis_LXeActive   ->SetForceSolid(true);
    logic_LXeActive ->SetVisAttributes(vis_LXeActive);

    // ---- Sapphire Tub ------------------------------------------------------
    G4double d_Sapphire_oD       = GetGeometryParameter("Sapphire_oD");
    G4double d_SapphireThickness = GetGeometryParameter("SapphireThickness");
    G4double AnodeThickness      = GetGeometryParameter("AnodeThickness");

    // GXeSapphireTub
    G4Tubs* solid_GXeSapphireTub = new G4Tubs(
        "solid_GXeSapphireTub",
        d_Sapphire_oD/2 - d_SapphireThickness,
        d_Sapphire_oD/2,
        GXeTeflonTub_H/2 - PTFE_thickness/2 - AnodeThickness/2,
        0.,
        360. *deg
    );

    logic_GXeSapphireTub = new G4LogicalVolume(
        solid_GXeSapphireTub,
        Sapphire,
        "logic_GXeSapphireTub"
    );

    G4double z_GXeSapphireTub = -AnodeThickness/2;
    auto pos_GXeSapphireTub = G4ThreeVector(0., 0., z_GXeSapphireTub);
    phys_GXeSapphireTub = new G4PVPlacement(
        0,
        pos_GXeSapphireTub,
        logic_GXeSapphireTub,
        "phys_GXeSapphireTub",
        logic_GXeActive,
        false,
        0,
        true
    );

    // GXeSapphireCap
    G4Tubs* solid_GXeSapphireCap = new G4Tubs(
        "solid_GXeSapphireCap",
        0,
        TPC_oD/2 - PTFE_thickness,
        AnodeThickness/2,
        0.   *deg,
        360. *deg
    );

    logic_GXeSapphireCap = new G4LogicalVolume(
        solid_GXeSapphireCap,
        Sapphire,
        "logic_GXeSapphireCap"
    );

    G4double z_GXeSapphireCap = GXeTeflonTub_H/2 - PTFE_thickness - AnodeThickness/2;
    auto pos_GXeSapphireCap = G4ThreeVector(0., 0., z_GXeSapphireCap);
    phys_GXeSapphireCap = new G4PVPlacement(
        0,
        pos_GXeSapphireCap,
        logic_GXeSapphireCap,
        "phys_GXeSapphireCap",
        logic_GXeActive,
        false,
        0,
        true
    );

    // LXeSapphireTub
    G4Tubs* solid_LXeSapphireTub = new G4Tubs(
        "solid_LXeSapphireTub",
        d_Sapphire_oD/2 - d_SapphireThickness,
        d_Sapphire_oD/2,
        LXeTeflonTub_H/2 - PTFE_thickness/2 - AnodeThickness/2,
        0.   *deg,
        360. *deg
    );

    logic_LXeSapphireTub = new G4LogicalVolume(
        solid_LXeSapphireTub,
        Sapphire,
        "logic_LXeSapphireTub"
    );

    G4double z_LXeSapphireTub = AnodeThickness/2;
    auto pos_LXeSapphireTub = G4ThreeVector(0., 0., z_LXeSapphireTub);
    phys_LXeSapphireTub = new G4PVPlacement(
        0,
        pos_LXeSapphireTub,
        logic_LXeSapphireTub,
        "phys_LXeSapphireTub",
        logic_LXeActive,
        false,
        0,
        true
    );

    // LXeSapphireCap
    G4Tubs* solid_LXeSapphireCap = new G4Tubs(
        "solid_LXeSapphireCap",
        0,
        TPC_oD/2 - PTFE_thickness,
        AnodeThickness/2,
        0.   *deg,
        360. *deg
    );

    logic_LXeSapphireCap = new G4LogicalVolume(
        solid_LXeSapphireCap,
        Sapphire,
        "logic_LXeSapphireCap"
    );

    G4double z_LXeSapphireCap = -LXeTeflonTub_H/2 + PTFE_thickness + AnodeThickness/2;
    auto pos_LXeSapphireCap = G4ThreeVector(0., 0., z_LXeSapphireCap);
    phys_LXeSapphireCap = new G4PVPlacement(
        0,
        pos_LXeSapphireCap,
        logic_LXeSapphireCap,
        "phys_LXeSapphireCap",
        logic_LXeActive,
        false,
        0,
        true
    );

    // VisAttributes
    auto col_Sapphire = G4Colour(1., 1., 0., Sapphire_Alpha);
    G4VisAttributes* vis_Sapphire = new G4VisAttributes(col_Sapphire);
    vis_Sapphire         ->SetVisibility(true);
    vis_Sapphire         ->SetForceSolid(true);
    logic_GXeSapphireTub ->SetVisAttributes(vis_Sapphire);
    logic_LXeSapphireTub ->SetVisAttributes(vis_Sapphire);

    auto col_SapphireCap = G4Colour(1., 1., 0., 1);
    G4VisAttributes* vis_SapphireCap = new G4VisAttributes(col_SapphireCap);
    vis_SapphireCap      ->SetVisibility(true);
    vis_SapphireCap      ->SetForceSolid(true);
    //vis_SapphireCap    ->SetForceWireframe(true);
    vis_SapphireCap      ->SetForceAuxEdgeVisible(true);
    logic_GXeSapphireCap ->SetVisAttributes(vis_SapphireCap);
    logic_LXeSapphireCap ->SetVisAttributes(vis_SapphireCap);

    // ----- PMTs -------------------------------------------------------------

    G4double dPMTHeight = 114. * mm;
    stringstream hVolumeName;

    // Construct Top PMT array
    G4int iNbPMTs = 1120;
    G4double dPMTOffsetZTop = -(iCryostat_H/2) * (1-LiquidGasRatio) 
                              + GXeTeflonTub_H 
                              + dPMTHeight/2;

    m_pPmtR11410LogicalVolume = ConstructPMT();
    for (G4int iPMT = 0; iPMT < iNbPMTs; ++iPMT) {
        hVolumeName.str("");
        hVolumeName << "PmtTpcTop_" << iPMT;
        G4ThreeVector PmtPosition = GetPMTPosition(iPMT, iNbPMTs);
        m_pPMTPhysicalVolumes.push_back(new G4PVPlacement(0,
                              PmtPosition+G4ThreeVector(0., 0., dPMTOffsetZTop),
                             m_pPmtR11410LogicalVolume, hVolumeName.str(),
                             logic_GXeMedium, false, iPMT));
    }

    // Construct Bottom PMT array
    G4double dPMTOffsetZBot = (iCryostat_H/2) * LiquidGasRatio 
                              - LXeTeflonTub_H 
                              - dPMTHeight/2;
    
    G4RotationMatrix *pRotX180 = new G4RotationMatrix();
    pRotX180->rotateX(180. * deg);

    for (G4int iPMT = 0; iPMT < iNbPMTs; ++iPMT) {
        G4ThreeVector PmtPosition = GetPMTPosition(iPMT, iNbPMTs);
        G4int iPMT_label = iPMT + iNbPMTs;
        hVolumeName.str("");
        hVolumeName << "PmtTpcTop_" << iPMT_label;
        m_pPMTPhysicalVolumes.push_back(new G4PVPlacement(pRotX180,
                              PmtPosition+G4ThreeVector(0., 0., dPMTOffsetZBot),
                              m_pPmtR11410LogicalVolume, hVolumeName.str(),
                              logic_LXeMedium, false, iPMT_label));
    }

}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------


G4LogicalVolume *HTPCDetectorConstruction::ConstructPMT() {

  G4double dPMTOuterRadius      = 38. * mm;
  G4double dPMTOuterRingBottomZ = 9. * mm;
  G4double dPMTOuterRingHeight  = 5. * mm;
  G4double dPMTOuterRingRadius  = 38.75 * mm;

  G4double dPMTWindowRadius    = 35. * mm;
  G4double dPMTWindowThickness = 3.5 * mm;
  G4double dPMTWindowGap       = 0.6 * mm;

  G4double dPMTThickness  = 1. * mm;
  G4double dPMTHeight     = 114. * mm;
  G4double dPMTDinodeCutZ = 11.8 * mm;
  G4double dPMTFunnelCutZ = 11.8 * mm;
  G4double dPMTBaseRadius = 26.65 * mm;

  G4double dPMTPhotocathodeRadius    = 32. * mm;
  G4double dPMTPhotocathodeThickness = 0.1 * mm;

  G4double dPMTCeramicRadius    = dPMTBaseRadius - 5. * mm;
  G4double dPMTCeramicThickness = 4. * mm;

  G4Material *Quartz = G4Material::GetMaterial("Quartz");
  G4Material *Kovar  = G4Material::GetMaterial("Kovar");
  G4Material *Vacuum = G4Material::GetMaterial("Vacuum");
  G4Material *PhotoCathodeAluminium = G4Material::GetMaterial("PhotoCathodeAluminium");
  G4Material *Ceramic = G4Material::GetMaterial("Ceramic");

  //------------------------- PMT body ---------------------
  const G4double dPMTZ0  = -0.5 * dPMTHeight;
  const G4double dPMTZ1  = dPMTZ0 + dPMTOuterRingBottomZ;
  const G4double dPMTZ2  = dPMTZ1;
  const G4double dPMTZ3  = dPMTZ2 + dPMTOuterRingHeight;
  const G4double dPMTZ4  = dPMTZ3;
  const G4double dPMTZ5  = dPMTZ4 + dPMTDinodeCutZ;
  const G4double dPMTZ6  = dPMTZ5 + dPMTFunnelCutZ;
  const G4double dPMTZ7  = dPMTZ6 + 1. * cm;
  const G4double dPMTZ8  = dPMTZ7;
  const G4double dPMTZ9  = dPMTZ8 + 1. * mm;
  const G4double dPMTZ10 = dPMTZ9;
  const G4double dPMTZ11 = 0.5 * dPMTHeight - 6. * mm;
  const G4double dPMTZ12 = 0.5 * dPMTHeight - 6. * mm;
  const G4double dPMTZ13 = 0.5 * dPMTHeight - 5. * mm;
  const G4double dPMTZ14 = 0.5 * dPMTHeight - 5. * mm;
  const G4double dPMTZ15 = 0.5 * dPMTHeight;

  const G4double dPMTR0  = dPMTOuterRadius;
  const G4double dPMTR1  = dPMTR0;
  const G4double dPMTR2  = dPMTOuterRingRadius;
  const G4double dPMTR3  = dPMTR2;
  const G4double dPMTR4  = dPMTR0;
  const G4double dPMTR5  = dPMTR0;
  const G4double dPMTR6  = dPMTBaseRadius;
  const G4double dPMTR7  = dPMTR6;
  const G4double dPMTR8  = dPMTR6 + 0.5 * mm;
  const G4double dPMTR9  = dPMTR6 + 0.5 * mm;
  const G4double dPMTR10 = dPMTR6;
  const G4double dPMTR11 = dPMTR6;
  const G4double dPMTR12 = dPMTR6 + 0.5 * mm;
  const G4double dPMTR13 = dPMTR6 + 0.5 * mm;
  const G4double dPMTR14 = dPMTR6;
  const G4double dPMTR15 = dPMTR6;

  const G4double dPMTZPlane[] = {
      dPMTZ0, dPMTZ1, dPMTZ2,  dPMTZ3,  dPMTZ4,  dPMTZ5,  dPMTZ6,  dPMTZ7,
      dPMTZ8, dPMTZ9, dPMTZ10, dPMTZ11, dPMTZ12, dPMTZ13, dPMTZ14, dPMTZ15};
  const G4double dPMTInner[] = {0., 0., 0., 0., 0., 0., 0., 0.,
                                0., 0., 0., 0., 0., 0., 0., 0.};
  const G4double dPMTOuter[] = {
      dPMTR0, dPMTR1, dPMTR2,  dPMTR3,  dPMTR4,  dPMTR5,  dPMTR6,  dPMTR7,
      dPMTR8, dPMTR9, dPMTR10, dPMTR11, dPMTR12, dPMTR13, dPMTR14, dPMTR15};

  G4Polycone *pPMTPolycone = new G4Polycone(
    "pPMTPolycone", 
    0., 
    2 * M_PI, 
    16,
    dPMTZPlane, 
    dPMTInner, 
    dPMTOuter
    );

  G4Tubs *pPmtCut1Tubs = new G4Tubs(
    "pPmtCut1Tubs", 
    dPMTWindowRadius, 
    dPMTOuterRadius,
    dPMTWindowGap, 
    0., 
    2 * M_PI
    );

  G4SubtractionSolid *pPmtSubtractionSolid = new G4SubtractionSolid(
    "pPmtSubtractionSolid1", 
    pPMTPolycone,
    pPmtCut1Tubs, 
    0, 
    G4ThreeVector(0., 0., dPMTZ0)
    );

  G4Tubs *pPmtCut2Tubs = new G4Tubs(
      "pPmtCut2Tubs", 
      dPMTWindowRadius, 
      dPMTOuterRadius - dPMTThickness,
      dPMTWindowThickness * 0.5 - dPMTThickness * 0.5, 
      0., 
      2 * M_PI
    );

  pPmtSubtractionSolid = new G4SubtractionSolid(
      "pPmtSubtractionSolid2", 
      pPmtSubtractionSolid, 
      pPmtCut2Tubs, 
      0,
      G4ThreeVector(0., 0., dPMTZ0 + dPMTWindowThickness * 0.5)
    );

  m_pPMTLogicalVolume = new G4LogicalVolume(
    pPmtSubtractionSolid, 
    Kovar,
    "PMTLogicalVolume", 
    0, 
    0, 
    0
    );

  //------------------------ Inner Vacuum -----------------------
  const G4double dPMTCutZ0 = 0. * cm;
  const G4double dPMTCutZ1 = dPMTCutZ0 + dPMTDinodeCutZ + dPMTOuterRingBottomZ +
                             dPMTOuterRingHeight - dPMTWindowThickness;
  const G4double dPMTCutZ2 = dPMTCutZ1 + dPMTFunnelCutZ;
  const G4double dPMTCutZ3 = dPMTHeight - dPMTWindowThickness - dPMTThickness;

  const G4double dPMTCutR0 = dPMTOuterRadius - dPMTThickness;
  const G4double dPMTCutR1 = dPMTCutR0;
  const G4double dPMTCutR2 = dPMTBaseRadius - dPMTThickness;
  const G4double dPMTCutR3 = dPMTCutR2;

  const G4double dPMTInnerVacuumOffsetZ =
      -dPMTHeight * 0.5 + dPMTWindowThickness;

  const G4double dPMTZCutPlane[] = {dPMTCutZ0, dPMTCutZ1, dPMTCutZ2, dPMTCutZ3};
  const G4double dPMTCutInner[] = {0., 0., 0., 0.};
  const G4double dPMTCutOuter[] = {dPMTCutR0, dPMTCutR1, dPMTCutR2, dPMTCutR3};

  G4Polycone *pPMTInnerVacuumPolycone = new G4Polycone(
    "pPMTInnerVacuumPolycone", 
    0. * deg, 
    360. * deg, 
    4,
    dPMTZCutPlane, 
    dPMTCutInner, 
    dPMTCutOuter
    );

  G4Tubs *pPMTVacuumCut = new G4Tubs(
    "pPMTVacuumCut", 
    dPMTPhotocathodeRadius, 
    dPMTOuterRadius,
    dPMTThickness * 0.5, 
    0. * deg, 
    360. * deg
    );

  G4SubtractionSolid *pPMTVacuumSubtractionSolid = new G4SubtractionSolid(
      "pPMTVacuumSubtractionSolid", 
      pPMTInnerVacuumPolycone, 
      pPMTVacuumCut, 
      0,
      G4ThreeVector(0., 0., dPMTCutZ0 + dPMTThickness * 0.5)
    );

  G4Tubs *pCut = new G4Tubs(
    "pPMTCut", 
    0. * mm, 
    dPMTCeramicRadius,
    dPMTCeramicThickness * 0.5, 
    0. * deg, 
    60. * deg
    );

  G4SubtractionSolid *pPMTcut1 = new G4SubtractionSolid(
      "pPMTVacuumSubtractionSolid", 
      pPMTVacuumSubtractionSolid, 
      pCut, 
      0,
      G4ThreeVector(0., 0.,
                    dPMTCutZ3 + dPMTThickness - dPMTCeramicThickness * 0.5)
    );

  m_pPMTInnerVacuumLogicalVolume = new G4LogicalVolume(
      pPMTcut1, 
      Vacuum, 
      "PMTInnerVacuumLogicalVolume", 
      0, 
      0, 
      0
    );

  m_pPMTInnerVacuumPhysicalVolume = new G4PVPlacement(
    0, 
    G4ThreeVector(0, 0, dPMTInnerVacuumOffsetZ),
    m_pPMTInnerVacuumLogicalVolume, 
    "PMTInnerVacuum",
    m_pPMTLogicalVolume, 
    false, 
    0
    );

  //------------------------ Quartz window -----------------------
  const G4double dPMTWindowOffsetZ = 0.5 * (dPMTWindowThickness - dPMTHeight);

  G4Tubs *pPMTWindow = new G4Tubs(
    "pPMTWindow", 
    0. * mm, 
    dPMTWindowRadius,
    dPMTWindowThickness * 0.5, 
    0. * deg, 
    360. * deg
    );

  m_pPMTWindowLogicalVolume = new G4LogicalVolume(
      pPMTWindow, 
      Quartz, 
      "PMTWindowLogicalVolume", 
      0, 
      0, 
      0
    );

  m_pPMTWindowPhysicalVolume = new G4PVPlacement(
      0, 
      G4ThreeVector(0, 0, dPMTWindowOffsetZ), m_pPMTWindowLogicalVolume,
      "Quartz_PMTWindow", 
      m_pPMTLogicalVolume, 
      false, 
      0
    );

  //------------------------ Photocathode ------------------------
  G4Tubs *pPMTPhotocathode = new G4Tubs(
    "pPMTPhotocathode", 
    0. * mm, 
    dPMTPhotocathodeRadius,
    dPMTPhotocathodeThickness * 0.5, 
    0. * deg, 
    360. * deg
    );

  m_pPMTPhotocathodeLogicalVolume = new G4LogicalVolume(
    pPMTPhotocathode, 
    PhotoCathodeAluminium,
    "PMTPhotocathodeLogicalVolume", 
    0, 
    0, 
    0
    );

  m_pPMTPhotocathodePhysicalVolume = new G4PVPlacement(
      0, 
      G4ThreeVector(0, 0, dPMTCutZ0 + dPMTPhotocathodeThickness * 0.5),
      m_pPMTPhotocathodeLogicalVolume, 
      "PMTPhotocathode",
      m_pPMTInnerVacuumLogicalVolume, 
      false, 
      0
    );

  //------------------------ PMTs Ceramic Stem ------------------------
  G4double dCeramicOffsetZ = (dPMTHeight - dPMTCeramicThickness) * 0.5;

  G4Tubs *pPMTCeramic = new G4Tubs(
    "pPMTCeramic", 
    0. * mm, 
    dPMTCeramicRadius,
    dPMTCeramicThickness * 0.5, 
    0. * deg, 
    360. * deg
    );

  m_PMTCeramicLogicalVolume = new G4LogicalVolume(
      pPMTCeramic, 
      Ceramic, 
      "PMTCeramicLogicalVolume", 
      0, 
      0, 
      0
    );

  m_PMTCeramicPhysicalVolume = new G4PVPlacement(
      0, 
      G4ThreeVector(0, 0, dCeramicOffsetZ), 
      m_PMTCeramicLogicalVolume,
      "Ceramic_PMTStem", 
      m_pPMTLogicalVolume, 
      false, 
      0
    );

  //---------------------------------- attributes

  // m_pPMTInnerVacuumLogicalVolume->SetVisAttributes(G4VisAttributes::Invisible);

  //G4Colour hPMTWindowColor(1., 0.757, 0.024);
  //G4VisAttributes *pPMTWindowVisAtt = new G4VisAttributes(hPMTWindowColor);
  //pPMTWindowVisAtt->SetVisibility(true);
  //m_pPMTWindowLogicalVolume->SetVisAttributes(pPMTWindowVisAtt);

  //G4Colour hPMTPhotocathodeColor(1., 0.082, 0.011);
  //G4VisAttributes *pPMTPhotocathodeVisAtt =
  //    new G4VisAttributes(hPMTPhotocathodeColor);
  //pPMTPhotocathodeVisAtt->SetVisibility(true);
  //m_pPMTPhotocathodeLogicalVolume->SetVisAttributes(pPMTPhotocathodeVisAtt);

  G4Colour hPMTColor(1., 0.486, 0.027);
  G4VisAttributes *pPMTVisAtt = new G4VisAttributes(hPMTColor);
  pPMTVisAtt->SetVisibility(true);
  pPMTVisAtt->SetForceSolid(true);
  m_pPMTLogicalVolume->SetVisAttributes(pPMTVisAtt);

  //G4Colour hVacuumColor(1., 1., 1.);
  //G4VisAttributes *pVacuumVisAtt = new G4VisAttributes(hVacuumColor);
  //pVacuumVisAtt->SetVisibility(true);
  //m_pPMTInnerVacuumLogicalVolume->SetVisAttributes(pVacuumVisAtt);

  return m_pPMTLogicalVolume;
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

G4ThreeVector HTPCDetectorConstruction::GetPMTPosition(G4int index, G4int i_nmbPMTS) {

    if (index < 0 || index >= i_nmbPMTS) {
        throw std::out_of_range("Index must be between 0 and 1181");
    }

    G4double outer_radius = GetGeometryParameter("TPC_oD") / 2; // Container radius
    const G4double circle_diameter = 0.0762 * m;
    const G4double circle_radius =  0.081/2 *m; //circle_diameter / 2.0;
    const G4double dy = circle_radius * std::sqrt(3.0); // Vertical spacing

    G4double y = -outer_radius;
    int row = 0;
    int current_index = 0;

    while (y <= outer_radius) {
        G4double x_offset = (row % 2 == 0) ? 0.0 : circle_radius;
        G4double x = -outer_radius + x_offset;

        while (x <= outer_radius) {
            G4double dist_to_center = std::sqrt(x * x + y * y);
            if (dist_to_center + circle_diameter <= outer_radius) {
                if (current_index == index) {
                    return G4ThreeVector(x, y, 0.0);
                }
                current_index++;
                if (current_index >= i_nmbPMTS) {
                    break;
                }
            }
            x += 2*circle_radius;
        }

        y += dy;
        row++;
    }

    // Should never reach here if index is valid
    throw std::runtime_error("Could not find packing center for given index : " + std::to_string(index));
}

/* ----------------------------------------------------------------------- */

// ---- GEOMETRY FUNCTIONS --- //
namespace {
    // Put it in an unnamed namespace so it's local to this translation unit
    G4VSolid* BuildCapsule(
        G4double Rmax, 
        G4double Rmin, 
        G4double Dz, 
        G4double DomeCurve) {

        // -- Globals --
        G4double zOffset   = Dz - DomeCurve; // * Dz;
        G4RotationMatrix* RotFlip = new G4RotationMatrix;
        RotFlip->rotateX(180*degree);
        G4ThreeVector Trans(0, 0, zOffset);
        
        // Build oCylinder
        G4double oCylSPhi = 0.  *deg;
        G4double oCylDPhi = 360.*deg;
    
        G4Tubs* oSolidCyl = new G4Tubs(
            "oSolidCyl",
            0.,
            Rmax,
            Dz,
            oCylSPhi,
            oCylDPhi
        );

        // Build oSphere
        G4double oSphRmin   = 0.*m;
        G4double oSphRmax   = sqrt(pow(Rmax,2) + pow(Dz - zOffset,2));
        G4double oSphSPhi   = 0.   *deg;
        G4double oSphDPhi   = 360. *deg;
        G4double oSphSTheta = 0.   *deg;
        G4double oSphDTheta = atan(Rmax / (Dz - zOffset)) * (180/M_PI) *deg;
        
        
        G4Sphere* oSolidSph = new G4Sphere(
            "oSolidSph",
            oSphRmin,
            oSphRmax,
            oSphSPhi,
            oSphDPhi,
            oSphSTheta,
            oSphDTheta
        );

        // Build oCapsule
        G4UnionSolid* oSolidUnion1 = new G4UnionSolid(
            "oSolidCyl+oSolidSph1", 
            oSolidCyl, 
            oSolidSph,
            0,
            Trans
        );
    
        G4UnionSolid* oSolidUnion2 = new G4UnionSolid(
            "oSolidUnion1+oSolidSph2", 
            oSolidUnion1, 
            oSolidSph,
            RotFlip,
            -Trans
        );

        if (Rmin <= 0) {
            return oSolidUnion2;
                }
        else {
            // Build iCylinder
            G4double iCylSPhi = 0.  *deg;
            G4double iCylDPhi = 360.*deg;
        
            G4Tubs* iSolidCyl = new G4Tubs(
                "iSolidCyl",
                0.,
                Rmin,
                Dz,
                iCylSPhi,
                iCylDPhi
            );
    
            // Build iSphere
            G4double iSphRmin   = 0.*m;
            G4double iSphRmax   = sqrt(pow(Rmin,2) + pow(Dz - zOffset,2));
            G4double iSphSPhi   = 0.   *deg;
            G4double iSphDPhi   = 360. *deg;
            G4double iSphSTheta = 0.   *deg;
            G4double iSphDTheta = atan(Rmin / (Dz - zOffset)) * (180/M_PI) *deg;
            
            
            G4Sphere* iSolidSph = new G4Sphere(
                "iSolidSph",
                iSphRmin,
                iSphRmax,
                iSphSPhi,
                iSphDPhi,
                iSphSTheta,
                iSphDTheta
            );
    
            // Build iCapsule
            G4UnionSolid* iSolidUnion1 = new G4UnionSolid(
                "iSolidCyl+iSolidSph1", 
                iSolidCyl, 
                iSolidSph,
                0,
                Trans
            );
        
            G4UnionSolid* iSolidUnion2 = new G4UnionSolid(
                "iSolidUnion1+iSolidSph2", 
                iSolidUnion1, 
                iSolidSph,
                RotFlip,
                -Trans
            );
                
            // Build Capsule
            G4SubtractionSolid* solidCapsule = new G4SubtractionSolid(
                "solidCapsule", 
                oSolidUnion2, 
                iSolidUnion2
            );

            return solidCapsule;
        }
    }
}

namespace {
    // Put it in an unnamed namespace so it's local to this translation unit
    G4VSolid* BuildHalfCapsule(
        G4double Rmax, 
        G4double Dz, 
        G4double DomeCurve,
        G4int    whereDome) {

        // -- Globals --
        G4double zOffset   = Dz - DomeCurve;
        G4RotationMatrix* RotFlip = new G4RotationMatrix;
        RotFlip->rotateX(180*degree);
        G4ThreeVector Trans(0, 0, zOffset);

        G4double Rmax_eff = Rmax* 1; //0.995;

        // Build oCylinder
        G4double oCylSPhi = 0.  *deg;
        G4double oCylDPhi = 360.*deg;
    
        G4Tubs* oSolidCyl = new G4Tubs(
            "oSolidCyl",
            0.,
            Rmax_eff,
            Dz,
            oCylSPhi,
            oCylDPhi
        );
                                        
        // Build oSphere
        G4double oSphRmin   = 0.*m;
        G4double oSphRmax   = sqrt(pow(Rmax_eff,2) + pow(Dz - zOffset,2));
        G4double oSphSPhi   = 0.   *deg;
        G4double oSphDPhi   = 360. *deg;
        G4double oSphSTheta = 0.   *deg;
        G4double oSphDTheta = atan(Rmax_eff / (Dz - zOffset)) * (180/M_PI) *deg;
        
        
        G4Sphere* oSolidSph = new G4Sphere(
            "oSolidSph",
            oSphRmin,
            oSphRmax, // *0.99
            oSphSPhi,
            oSphDPhi,
            oSphSTheta,
            oSphDTheta
        );

        // Build helperBox
        G4Box* helpSolidBox = new G4Box(
            "helpSolidBox", 
            2*Rmax, 
            2*Rmax, 
            4*Dz
        );

        // Build oCapsule
        if (whereDome == 1) {
        G4UnionSolid* oSolidUnion1 = new G4UnionSolid(
            "oSolidCyl+oSolidSph1", 
            oSolidCyl, 
            oSolidSph,
            0,
            Trans
        );

        G4IntersectionSolid* oSolidInt1 = new G4IntersectionSolid(
            "oSolidInt1",
            oSolidUnion1,
            helpSolidBox,
            0,
            G4ThreeVector(0,0,3*Dz)
        );

        return oSolidInt1;
                                    }

        else if (whereDome == 0) {
        G4UnionSolid* oSolidUnion1 = new G4UnionSolid(
            "oSolidCyl+oSolidSph1", 
            oSolidCyl, 
            oSolidSph,
            RotFlip,
            -Trans
        );
        return oSolidUnion1;
        }
    }
}


namespace {
    // Put it in an unnamed namespace so it's local to this translation unit
    G4VSolid* AddFlange(
        G4VSolid* baseSolid, 
        G4double Rmax, 
        G4double Rmin, 
        G4double thickness, 
        G4double zpos) {

        // Build Flange
        G4double FlangeSPhi = 0.  *deg;
        G4double FlangeDPhi = 360.*deg;
    
        G4Tubs* SolidFlange = new G4Tubs(
            "SolidFlange",
            Rmin,
            Rmax,
            thickness,
            FlangeSPhi,
            FlangeDPhi
        );

        // Join with Flange
        G4ThreeVector flangeTrans(0, 0, zpos);
        G4UnionSolid* solidCapsulewFlange = new G4UnionSolid(
            "solidCapsulewFlange",
            baseSolid, 
            SolidFlange,
            0,
            flangeTrans
        );
        
        return solidCapsulewFlange;
    }
}