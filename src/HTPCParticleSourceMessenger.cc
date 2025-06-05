#include "HTPCParticleSource.hh"
#include "HTPCParticleSourceMessenger.hh"

// Additional Header Files
#include <fstream>
#include <iomanip>

// G4 Header Files
#include <G4Geantino.hh>
#include <G4ParticleTable.hh>
#include <G4ThreeVector.hh>
#include <G4Tokenizer.hh>
#include <G4UIcmdWith3Vector.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>
#include <G4UIcmdWithABool.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIdirectory.hh>
#include <G4ios.hh>
#include <G4IonTable.hh>
#include <G4SystemOfUnits.hh>

HTPCParticleSourceMessenger::HTPCParticleSourceMessenger(
  HTPCParticleSource *pSource)
  : m_pSource(pSource), m_bShootIon(false)
{
  m_pGen = m_pSource->GetCurrentGenerator();

  m_pParticleTable = G4ParticleTable::GetParticleTable();
  m_pIonTable = G4IonTable::GetIonTable();
  // create directory
  m_pDirectory = new G4UIdirectory("/xe/gun/");
  m_pDirectory->SetGuidance("Particle Source control commands.");

  // list available particles
  m_pListCmd = new G4UIcmdWithoutParameter("/xe/gun/List", this);
  m_pListCmd->SetGuidance("List available particles.");
  m_pListCmd->SetGuidance(" Invoke G4ParticleTable.");

  // set particle
  m_pParticleCmd = new G4UIcmdWithAString("/xe/gun/particle", this);
  m_pParticleCmd->SetGuidance("Set particle to be generated.");
  m_pParticleCmd->SetGuidance(" (geantino is default)");
  m_pParticleCmd->SetGuidance(" (ion can be specified for shooting ions)");
  m_pParticleCmd->SetParameterName("particleName", true);
  m_pParticleCmd->SetDefaultValue("geantino");
  G4String candidateList;
  G4int nPtcl = m_pParticleTable->entries();

  for (G4int i = 0; i < nPtcl; i++)
  {
    candidateList += m_pParticleTable->GetParticleName(i);
    candidateList += " ";
  }
  candidateList += "ion ";
  m_pParticleCmd->SetCandidates(candidateList);

  // particle direction
  m_pDirectionCmd = new G4UIcmdWith3Vector("/xe/gun/direction", this);
  m_pDirectionCmd->SetGuidance("Set momentum direction.");
  m_pDirectionCmd->SetGuidance("Direction needs not to be a unit vector.");
  m_pDirectionCmd->SetParameterName("Px", "Py", "Pz", true, true);
  m_pDirectionCmd->SetRange("Px != 0 || Py != 0 || Pz != 0");

  // particle energy
  m_pEnergyCmd = new G4UIcmdWithADoubleAndUnit("/xe/gun/energy", this);
  m_pEnergyCmd->SetGuidance("Set kinetic energy.");
  m_pEnergyCmd->SetParameterName("Energy", true, true);
  m_pEnergyCmd->SetDefaultUnit("GeV");
  // m_pEnergyCmd->SetUnitCategory("Energy");
  // m_pEnergyCmd->SetUnitCandidates("eV keV MeV GeV TeV");

  m_pPositionCmd = new G4UIcmdWith3VectorAndUnit("/xe/gun/position", this);
  m_pPositionCmd->SetGuidance("Set starting position of the particle.");
  m_pPositionCmd->SetParameterName("X", "Y", "Z", true, true);
  m_pPositionCmd->SetDefaultUnit("cm");
  // m_pPositionCmd->SetUnitCategory("Length");
  // m_pPositionCmd->SetUnitCandidates("microm mm cm m km");

  // ion
  m_pIonCmd = new G4UIcommand("/xe/gun/ion", this);
  m_pIonCmd->SetGuidance("Set properties of ion to be generated.");
  m_pIonCmd->SetGuidance("[usage] /gun/ion Z A Q E");
  m_pIonCmd->SetGuidance("        Z:(int) AtomicNumber");
  m_pIonCmd->SetGuidance("        A:(int) AtomicMass");
  m_pIonCmd->SetGuidance("        Q:(int) Charge of Ion (in unit of e)");
  m_pIonCmd->SetGuidance("        E:(double) Excitation energy (in keV)");

  G4UIparameter *param;

  param = new G4UIparameter("Z", 'i', false);
  param->SetDefaultValue("1");
  m_pIonCmd->SetParameter(param);
  param = new G4UIparameter("A", 'i', false);
  param->SetDefaultValue("1");
  m_pIonCmd->SetParameter(param);
  param = new G4UIparameter("Q", 'i', true);
  param->SetDefaultValue("0");
  m_pIonCmd->SetParameter(param);
  param = new G4UIparameter("E", 'd', true);
  param->SetDefaultValue("0.0");
  m_pIonCmd->SetParameter(param);

  // particle generator type
  m_pGeneratorCmd = new G4UIcmdWithAString("/xe/gun/generator", this);
  m_pGeneratorCmd->SetGuidance("Sets particle generator type.");
  m_pGeneratorCmd->SetGuidance("[ generic | muon | decay0 | multivertex | ambe ]");
  m_pGeneratorCmd->SetParameterName("GenType", true, true);
  m_pGeneratorCmd->SetDefaultValue("generic");
  m_pGeneratorCmd->SetCandidates("generic muon decay0 multivertex ambe");

  // source distribution type
  m_pTypeCmd = new G4UIcmdWithAString("/xe/gun/type", this);
  m_pTypeCmd->SetGuidance("Sets source distribution type.");
  m_pTypeCmd->SetGuidance("Either Point, Volume or Surface");
  m_pTypeCmd->SetParameterName("DisType", true, true);
  m_pTypeCmd->SetDefaultValue("Point");
  m_pTypeCmd->SetCandidates("Point Volume Surface");

  // source shape
  m_pShapeCmd = new G4UIcmdWithAString("/xe/gun/shape", this);
  m_pShapeCmd->SetGuidance("Sets source shape type.");
  m_pShapeCmd->SetParameterName("Shape", true, true);
  m_pShapeCmd->SetDefaultValue("NULL");
  m_pShapeCmd->SetCandidates("Sphere Cylinder Box");

  // center coordinates
  m_pCenterCmd = new G4UIcmdWith3VectorAndUnit("/xe/gun/center", this);
  m_pCenterCmd->SetGuidance("Set center coordinates of source.");
  m_pCenterCmd->SetParameterName("X", "Y", "Z", true, true);
  m_pCenterCmd->SetDefaultUnit("cm");
  m_pCenterCmd->SetUnitCandidates("nm mum mm cm m km");

  // half x of source(if source shape is Box)
  m_pHalfxCmd = new G4UIcmdWithADoubleAndUnit("/xe/gun/halfx", this);
  m_pHalfxCmd->SetGuidance("Set x half length of source.");
  m_pHalfxCmd->SetParameterName("Halfx", true, true);
  m_pHalfxCmd->SetDefaultUnit("cm");
  m_pHalfxCmd->SetUnitCandidates("nm mum mm cm m km");

  // half y of source (if source shape is Box)
  m_pHalfyCmd = new G4UIcmdWithADoubleAndUnit("/xe/gun/halfy", this);
  m_pHalfyCmd->SetGuidance("Set y half length of source.");
  m_pHalfyCmd->SetParameterName("Halfy", true, true);
  m_pHalfyCmd->SetDefaultUnit("cm");
  m_pHalfyCmd->SetUnitCandidates("nm mum mm cm m km");

  // half height of source
  m_pHalfzCmd = new G4UIcmdWithADoubleAndUnit("/xe/gun/halfz", this);
  m_pHalfzCmd->SetGuidance("Set z half length of source.");
  m_pHalfzCmd->SetParameterName("Halfz", true, true);
  m_pHalfzCmd->SetDefaultUnit("cm");
  m_pHalfzCmd->SetUnitCandidates("nm mum mm cm m km");

  // radius of source
  m_pRadiusCmd = new G4UIcmdWithADoubleAndUnit("/xe/gun/radius", this);
  m_pRadiusCmd->SetGuidance("Set radius of source.");
  m_pRadiusCmd->SetParameterName("Radius", true, true);
  m_pRadiusCmd->SetDefaultUnit("cm");
  m_pRadiusCmd->SetUnitCandidates("nm mum mm cm m km");

  // inner radius for tube shape
  m_pInnerRadiusCmd = new G4UIcmdWithADoubleAndUnit("/xe/gun/innerradius", this);
  m_pInnerRadiusCmd->SetGuidance("Set inner radius of the tubular source.");
  m_pInnerRadiusCmd->SetParameterName("InnerRadius", true, true);
  m_pInnerRadiusCmd->SetDefaultUnit("cm");
  m_pInnerRadiusCmd->SetUnitCandidates("nm mum mm cm m km");

  // Top height for tube shape
  m_pThickness_topCmd = new G4UIcmdWithADoubleAndUnit("/xe/gun/topthickness", this);
  m_pThickness_topCmd->SetGuidance("Set top Thickness of the tubular source.");
  m_pThickness_topCmd->SetParameterName("TopThickness", true, true);
  m_pThickness_topCmd->SetDefaultUnit("cm");
  m_pThickness_topCmd->SetUnitCandidates("nm mum mm cm m km");

  // Bottom height for tube shape
  m_pThickness_bottomCmd = new G4UIcmdWithADoubleAndUnit("/xe/gun/bottomthickness", this);
  m_pThickness_bottomCmd->SetGuidance("Set bottom Thickness of the tubular source.");
  m_pThickness_bottomCmd->SetParameterName("BottomThickness", true, true);
  m_pThickness_bottomCmd->SetDefaultUnit("cm");
  m_pThickness_bottomCmd->SetUnitCandidates("nm mum mm cm m km");


  // confine to volume(s)
  m_pConfineCmd = new G4UIcmdWithAString("/xe/gun/confine", this);
  m_pConfineCmd->SetGuidance("Confine source to volume(s) (NULL to unset).");
  m_pConfineCmd->SetGuidance("usage: confine VolName1 VolName2 ...");
  m_pConfineCmd->SetParameterName("VolName", true, true);
  m_pConfineCmd->SetDefaultValue("NULL");

  // angular distribution
  m_pAngTypeCmd = new G4UIcmdWithAString("/xe/gun/angtype", this);
  m_pAngTypeCmd->SetGuidance("Sets angular source distribution type");
  m_pAngTypeCmd->SetGuidance("Possible variables are: iso semiiso semiisoup semiisodown direction");
  m_pAngTypeCmd->SetParameterName("AngDis", true, true);
  m_pAngTypeCmd->SetDefaultValue("iso");
  m_pAngTypeCmd->SetCandidates("iso semiiso semiisoup semiisodown direction");

  // energy distribution
  m_pEnergyTypeCmd = new G4UIcmdWithAString("/xe/gun/energytype", this);
  m_pEnergyTypeCmd->SetGuidance("Sets energy distribution type");
  m_pEnergyTypeCmd->SetGuidance("Possible variables are: Mono Spectrum");
  m_pEnergyTypeCmd->SetParameterName("EnergyDis", true, true);
  m_pEnergyTypeCmd->SetDefaultValue("Mono");
  m_pEnergyTypeCmd->SetCandidates("Mono Spectrum");

  // number of particles to be generated
  m_pNumberOfParticlesToBeGeneratedCmd =
    new G4UIcmdWithAnInteger("/xe/gun/numberofparticles", this);
  m_pNumberOfParticlesToBeGeneratedCmd->SetGuidance(
    "Number of particles generated in one event");
  m_pNumberOfParticlesToBeGeneratedCmd->SetParameterName("NumParticles", true,
      true);
  m_pNumberOfParticlesToBeGeneratedCmd->SetDefaultValue(1);

  // energy distribution file
  m_pEnergyFileCmd = new G4UIcmdWithAString("/xe/gun/energyspectrum", this);
  m_pEnergyFileCmd->SetGuidance("File containing energy spectrum");
  m_pEnergyFileCmd->SetParameterName("EnergySpectrum", false);

  // verbosity
  m_pVerbosityCmd = new G4UIcmdWithAnInteger("/xe/gun/verbose", this);
  m_pVerbosityCmd->SetGuidance("Set Verbose level for gun");
  m_pVerbosityCmd->SetGuidance(" 0 : Silent");
  m_pVerbosityCmd->SetGuidance(" 1 : Limited information");
  m_pVerbosityCmd->SetGuidance(" 2 : Detailed information");
  m_pVerbosityCmd->SetParameterName("level", false);
  m_pVerbosityCmd->SetRange("level>=0 && level <=2");

  // DECAY0 input file
  // (event's number, time of event's start, number of emitted particles)
  // (GEANT number of particle, (x,y,z) components of momentum,
  // time shift from previous time)
  m_pDecay0EventFromFileCmd =
    new G4UIcmdWithAString("/xe/gun/decay0eventfromfile", this);
  m_pDecay0EventFromFileCmd->SetGuidance("Insert file with DECAY0 events");
  m_pDecay0EventFromFileCmd->SetParameterName("InputFileName", true, true);

  // FV MULTI PARTICLE input file  (3-position 3-direction KineticEnergy type)
  m_pMultiEventFromFileCmd =
    new G4UIcmdWithAString("/xe/gun/multieventfromfile", this);
  m_pMultiEventFromFileCmd->SetGuidance(
    "Insert file with multiple particles per event");
  m_pMultiEventFromFileCmd->SetGuidance(
    "3-position (cm), 3-direction,  KinEnergy (MeV) type (PDG)");
  m_pMultiEventFromFileCmd->SetParameterName("InputFileName", true, true);

  // EDIT PAOLO
  // input file with the cosmic muon momentum (3-direction KineticEnergy)
  m_pMuonsFromFileCmd = new G4UIcmdWithAString("/xe/gun/muonsfromfile", this);
  m_pMuonsFromFileCmd->SetGuidance("Insert file with the cosmic muon momentum");
  m_pMuonsFromFileCmd->SetGuidance("3-direction,  KinEnergy (MeV)");
  m_pMuonsFromFileCmd->SetParameterName("InputFileName", true, true);
}

HTPCParticleSourceMessenger::~HTPCParticleSourceMessenger()
{
  delete m_pGeneratorCmd;
  delete m_pTypeCmd;
  delete m_pShapeCmd;
  delete m_pCenterCmd;
  delete m_pHalfxCmd;
  delete m_pHalfyCmd;
  delete m_pHalfzCmd;
  delete m_pThickness_topCmd;
  delete m_pThickness_bottomCmd;
  delete m_pRadiusCmd;
  delete m_pInnerRadiusCmd;
  delete m_pConfineCmd;
  delete m_pAngTypeCmd;
  delete m_pEnergyTypeCmd;
  delete m_pEnergyFileCmd;
  delete m_pVerbosityCmd;
  delete m_pIonCmd;
  delete m_pParticleCmd;
  delete m_pPositionCmd;
  delete m_pMultiEventFromFileCmd;
  delete m_pDecay0EventFromFileCmd;
  delete m_pMuonsFromFileCmd;
  delete m_pDirectionCmd;
  delete m_pEnergyCmd;
  delete m_pListCmd;

  delete m_pDirectory;
}

void HTPCParticleSourceMessenger::SetNewValue(G4UIcommand *command,
    G4String newValues)
{
  if (command == m_pGeneratorCmd)
  {
    m_pSource->SetGeneratorType(newValues);
    m_pGen = m_pSource->GetCurrentGenerator();
  }

  else if (command == m_pTypeCmd)
    m_pGen->SetPosDisType(newValues);

  else if (command == m_pShapeCmd)
    m_pGen->SetPosDisShape(newValues);

  else if (command == m_pCenterCmd)
    m_pGen->SetCenterCoords(
      m_pCenterCmd->GetNew3VectorValue(newValues));

  else if (command == m_pHalfxCmd)
    m_pGen->SetHalfX(m_pHalfxCmd->GetNewDoubleValue(newValues));

  else if (command == m_pHalfyCmd)
    m_pGen->SetHalfY(m_pHalfyCmd->GetNewDoubleValue(newValues));

  else if (command == m_pHalfzCmd)
    m_pGen->SetHalfZ(m_pHalfzCmd->GetNewDoubleValue(newValues));

  else if (command == m_pThickness_topCmd)
    m_pGen->SetThickness_top(m_pThickness_topCmd->GetNewDoubleValue(newValues));

  else if (command == m_pThickness_bottomCmd)
    m_pGen->SetThickness_bottom(m_pThickness_bottomCmd->GetNewDoubleValue(newValues));

  else if (command == m_pRadiusCmd)
    m_pGen->SetRadius(m_pRadiusCmd->GetNewDoubleValue(newValues));

  else if (command == m_pInnerRadiusCmd)
    m_pGen->SetInnerRadius(
      m_pInnerRadiusCmd->GetNewDoubleValue(newValues));

  else if (command == m_pAngTypeCmd)
    m_pGen->SetAngDistType(newValues);

  else if (command == m_pConfineCmd)
  {
    m_pGen->ConfineSourceToVolume(newValues);
  }
  else if (command == m_pEnergyTypeCmd)
    m_pGen->SetEnergyDisType(newValues);

  else if (command == m_pEnergyFileCmd)
    m_pGen->SetEnergyFile(newValues);

  else if (command == m_pVerbosityCmd)
    m_pGen->SetVerbosity(m_pVerbosityCmd->GetNewIntValue(newValues));

  else if (command == m_pParticleCmd)
  {
    if(!(m_pSource->ValidateGeneratorType("generic")))
    {
      G4cout << "\nWARNING! Attempt to change the particle definition of a custom particle generator; "
             << "ignored." << G4endl;
      m_pSource->PrintCurrentGeneratorType();
      G4cout << G4endl;
    }

    if (newValues == "ion")
    {
      m_bShootIon = true;
    }
    else
    {
      m_bShootIon = false;
      G4ParticleDefinition *pd = m_pParticleTable->FindParticle(newValues);

      if (pd != NULL)
      {
        m_pGen->SetParticleDefinition(pd);
      }
    }
  }

  else if (command == m_pIonCmd)
  {
    if (m_bShootIon)
    {
      G4Tokenizer next(newValues);

      // check argument
      m_iAtomicNumber = StoI(next());
      m_iAtomicMass = StoI(next());
      G4String sQ = next();

      if (sQ.isNull())
      {
        m_iIonCharge = m_iAtomicNumber;
      }
      else
      {
        m_iIonCharge = StoI(sQ);
        sQ = next();
        if (sQ.isNull())
        {
          m_dIonExciteEnergy = 0.0;
        }
        else
        {
          m_dIonExciteEnergy = StoD(sQ) * keV;
        }
      }

      G4ParticleDefinition *ion;

      ion = m_pIonTable->GetIon(m_iAtomicNumber, m_iAtomicMass,
                                m_dIonExciteEnergy);
      if (ion == 0)
      {
        G4cout << "Ion with Z=" << m_iAtomicNumber;
        G4cout << " A=" << m_iAtomicMass << "is not be defined" << G4endl;
      }
      else
      {
        m_pGen->SetParticleDefinition(ion);
        m_pGen->SetParticleCharge(m_iIonCharge * eplus);
      }
    }
    else
    {
      G4cout << "Set /xe/gun/particle to ion before using /xe/gun/ion command";
      G4cout << G4endl;
    }
  }

  else if (command == m_pListCmd)
    m_pParticleTable->DumpTable();

  else if (command == m_pDirectionCmd)
  {
    m_pGen->SetAngDistType("direction");
    m_pGen->SetParticleMomentumDirection(
      m_pDirectionCmd->GetNew3VectorValue(newValues));
  }
  else if(command == m_pDecay0EventFromFileCmd)
  {
    if(m_pSource->ValidateGeneratorType("decay0"))
    {
      m_pGen->SetInputFileName(newValues);
      G4cout << "INPUT: DECAY0 events from file [ " << m_pGen->GetEventInputFileName()
             << " ]" << G4endl;
    }
    else
    {
      G4cout << "Particle generator must be set to decay0 to use this input file: [ "
             << newValues << " ] !" << G4endl;
      exit(-1);
    }
  }
  else if (command == m_pMultiEventFromFileCmd)
  {
    if(m_pSource->ValidateGeneratorType("multivertex"))
    {
      m_pGen->SetInputFileName(newValues);
      G4cout << "INPUT: MULTIVERTEX events from file [ " << m_pGen->GetEventInputFileName()
             << " ]" << G4endl;
    }
    else
    {
      G4cout << "Particle generator must be set to multivertex to use this input file: [ "
             << newValues << " ] !" << G4endl;
      exit(-1);
    }
  }
  else if (command == m_pMuonsFromFileCmd)    // EDIT PAOLO
  {
    if(m_pSource->ValidateGeneratorType("muon"))
    {
      m_pGen->SetInputFileName(newValues);
      G4cout << "INPUT: MUON events from file [ " << m_pGen->GetEventInputFileName()
             << " ]" << G4endl;
    }
    else
    {
      G4cout << "Particle generator must be set to muon to use this input file: [ "
             << newValues << " ] !" << G4endl;
      exit(-1);
    }
  }
  else if (command == m_pEnergyCmd)
  {
    m_pGen->SetEnergyDisType("Mono");
    m_pGen->SetMonoEnergy(
      m_pEnergyCmd->GetNewDoubleValue(newValues));
  }

  else if (command == m_pPositionCmd)
  {
    m_pGen->SetPosDisType("Point");
    m_pGen->SetCenterCoords(
      m_pPositionCmd->GetNew3VectorValue(newValues));
  }
  else if (command == m_pNumberOfParticlesToBeGeneratedCmd)
  {
    m_pGen->SetNumberOfParticlesToBeGenerated(
      m_pNumberOfParticlesToBeGeneratedCmd->GetNewIntValue(newValues));
  }
  else
    G4cout << "HTPCParticleSourceMessenger: Unknown command [ "
           << command << " ] !" << G4endl;
}


