#include "HTPCPhysicsList.hh"

// Additional Header Files
#include <globals.hh>
#include <iomanip>
#include <vector>

// Geant4 headers
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include <G4DecayPhysics.hh>
#include <G4EmCalculator.hh>
#include <G4EmExtraPhysics.hh>
#include <G4EmPenelopePhysics.hh>
#include <G4EmStandardPhysics.hh>
#include <G4HadronicProcessStore.hh>
#include <G4NistManager.hh>
#include <G4StoppingPhysics.hh>
#include <G4ios.hh>

#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronInelasticQBBC.hh"
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
#include "G4HadronPhysicsINCLXX.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4HadronPhysicsShielding.hh"
#include "G4IonElasticPhysics.hh"
#include "G4IonINCLXXPhysics.hh"
#include "G4IonPhysics.hh"

#include "G4EmLivermorePhysics.hh"
#include "G4PhysListFactory.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4NuclideTable.hh"
#include "G4VModularPhysicsList.hh"
#include "G4BuilderType.hh"

// ======================================================================
// Helper: Safely Replace Physics Constructor
// ======================================================================
bool HTPCPhysicsList::SafeReplacePhysics(G4VPhysicsConstructor* newPhys)
{
    if (!newPhys) return false;

    G4int type = newPhys->GetPhysicsType();

    // Count how many constructors of this type exist BEFORE replacement
    G4int beforeCount = 0;
    for (G4int i = 0; ; ++i) {
        const G4VPhysicsConstructor* p = this->GetPhysics(i);
        if (p == nullptr) break;
        if (p->GetPhysicsType() == type) ++beforeCount;
    }

    // Perform replacement (void in modern Geant4)
    G4VModularPhysicsList::ReplacePhysics(newPhys);

    // Count how many constructors of this type exist AFTER replacement
    G4int afterCount = 0;
    for (G4int i = 0; ; ++i) {
        const G4VPhysicsConstructor* p = this->GetPhysics(i);
        if (p == nullptr) break;
        if (p->GetPhysicsType() == type) ++afterCount;
    }

    // Success if exactly one constructor of that type is present after replacement
    return (afterCount == 1);
}


// ======================================================================
// CONSTRUCTOR
// ======================================================================
HTPCPhysicsList::HTPCPhysicsList()
    : G4VModularPhysicsList()
{
    // Enable diagnostic output
    VerboseLevel = 0;
    OpVerbLevel = 0;
    SetVerboseLevel(VerboseLevel);

    G4cout << "\n\n=====================================================\n";
    G4cout << "  HTPCPhysicsList: Constructor Called\n";
    G4cout << "  VerboseLevel = 1 (Diagnostics Enabled)\n";
    G4cout << "=====================================================\n\n";

    // ------------------------------------------------------------------
    // Register Physics
    // ------------------------------------------------------------------
    RegisterPhysics(new G4HadronElasticPhysicsHP(VerboseLevel));

    auto hadron = new G4HadronPhysicsQGSP_BERT(VerboseLevel);
    hadron->SetPhysicsType(bHadronInelastic);
    RegisterPhysics(hadron);

    RegisterPhysics(new G4IonElasticPhysics(VerboseLevel));
    RegisterPhysics(new G4IonPhysics(VerboseLevel));

    RegisterPhysics(new G4EmStandardPhysics(VerboseLevel, ""));
    RegisterPhysics(new G4EmExtraPhysics(VerboseLevel));

    RegisterPhysics(new G4DecayPhysics(VerboseLevel));
    RegisterPhysics(new G4RadioactiveDecayPhysics(VerboseLevel));

    G4NuclideTable::GetInstance()->SetThresholdOfHalfLife(1 * nanosecond);
    G4NuclideTable::GetInstance()->SetLevelTolerance(1.0 * eV);

    RegisterPhysics(new G4StoppingPhysics(VerboseLevel));

    // ------------------------------------------------------------------
    // Print registered constructors
    // ------------------------------------------------------------------
    G4cout << "HTPCPhysicsList: Registered physics constructors:\n";
    for (G4int i = 0; ; ++i) {
        const G4VPhysicsConstructor* pc = this->GetPhysics(i);
        if (!pc) break;
        G4cout << "  [" << i << "] " << pc->GetPhysicsName()
               << " (type=" << pc->GetPhysicsType() << ")\n";
    }
    G4cout << G4endl;
}


// ======================================================================
// EM Physics Selection
// ======================================================================
void HTPCPhysicsList::SetEMlowEnergyModel(G4String name)
{
    G4cout << "HTPCPhysicsList::SetEMlowEnergyModel(): switching to " << name << "\n";

    G4VPhysicsConstructor* em = nullptr;

    if (name == "emstandard")
        em = new G4EmStandardPhysics(VerboseLevel, "");
    else if (name == "emlivermore")
        em = new G4EmLivermorePhysics(VerboseLevel, "");
    else if (name == "empenelope")
        em = new G4EmPenelopePhysics(VerboseLevel, "");
    else
        G4Exception("HTPCPhysicsList::SetEMlowEnergyModel",
                    "PhysicsList", FatalException,
                    "Invalid EM model choice.");

    G4cout << "Attempting ReplacePhysics()...\n";

    if (!SafeReplacePhysics(em))
        G4cout << "WARNING: EM physics replacement FAILED.\n";
    else
        G4cout << "EM model successfully replaced.\n";

    G4cout << G4endl;
}


// ======================================================================
// Hadronic Physics Selection
// ======================================================================
void HTPCPhysicsList::SetHadronicModel(G4String name)
{
    G4cout << "HTPCPhysicsList::SetHadronicModel(): switching to " << name << "\n";

    G4VPhysicsConstructor* had = nullptr;

    if (name == "QGSP_BIC_HP")
        had = new G4HadronPhysicsQGSP_BIC_HP(VerboseLevel);
    else if (name == "QGSP_BIC")
        had = new G4HadronPhysicsQGSP_BIC(VerboseLevel);
    else if (name == "FTFP_BERT_HP")
        had = new G4HadronPhysicsFTFP_BERT_HP(VerboseLevel);
    else if (name == "QBBC")
        had = new G4HadronInelasticQBBC(VerboseLevel);
    else if (name == "INCLXX")
        had = new G4HadronPhysicsINCLXX(VerboseLevel);
    else if (name == "QGSP_BERT_HP")
        had = new G4HadronPhysicsQGSP_BERT_HP(VerboseLevel);
    else if (name == "QGSP_BERT")
        had = new G4HadronPhysicsQGSP_BERT(VerboseLevel);
    else if (name == "Shielding")
        had = new G4HadronPhysicsShielding(VerboseLevel);
    else
        G4Exception("HTPCPhysicsList::SetHadronicModel",
                    "PhysicsList", FatalException,
                    "Invalid hadronic model.");

    had->SetPhysicsType(bHadronInelastic);

    G4cout << "Attempting ReplacePhysics()...\n";

    if (!SafeReplacePhysics(had))
        G4cout << "WARNING: Hadronic physics replacement FAILED.\n";
    else
        G4cout << "Hadronic model successfully replaced.\n";

    G4cout << G4endl;
}


// ======================================================================
// SET CUTS
// ======================================================================
void HTPCPhysicsList::SetCuts()
{
    G4cout << "\nHTPCPhysicsList::SetCuts(): Applying production cuts\n";

    G4double lowlimit = 250 * eV;
    G4ProductionCutsTable::GetProductionCutsTable()
        ->SetEnergyRange(lowlimit, 100. * GeV);

    G4cout << "  Cut gamma:  " << GetCutValue("gamma") / mm << " mm\n";
    G4cout << "  Cut e-:     " << GetCutValue("e-") / mm << " mm\n";
    G4cout << "  Cut e+:     " << GetCutValue("e+") / mm << " mm\n";
    G4cout << "  Cut proton: " << GetCutValue("proton") / mm << " mm\n";

    G4cout << "\nDumping Production Cuts Table:\n";
    DumpCutValuesTable();

    G4cout << "\n";
}


// ======================================================================
HTPCPhysicsList::~HTPCPhysicsList() {}
