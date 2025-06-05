#include <string>
#include <sstream>
#include <unistd.h>

#include <G4RunManager.hh>
#include <G4UImanager.hh>
#include <G4UItcsh.hh>
#include <G4UIterminal.hh>
#include <G4VisExecutive.hh>
#include <G4UIExecutive.hh>
#include <G4GeneralParticleSource.hh>

#include "HTPCDetectorConstruction.hh"
#include "HTPCPhysicsList.hh"
#include "HTPCPrimaryGeneratorAction.hh"
#include "HTPCAnalysisManager.hh"
#include "HTPCStackingAction.hh"
#include "HTPCSteppingAction.hh"
#include "HTPCRunAction.hh"
#include "HTPCEventAction.hh"
#include "fileMerger.hh"

void usage();

int
main(int argc, char **argv)
{
  // switches
  int c = 0;

  std::stringstream hStream;

  bool bInteractive = false;
  bool bPreInitFromFile = false;

  bool bMacroFile = false;
  std::string hMacroFilename, hDataFilename, hPreInitFilename;
  std::string hCommand;
  int iNbEventsToSimulate = 0;

  // parse switches
  while((c = getopt(argc,argv,"f:o:p:n:i")) != -1)
  {
    switch(c)	{

      case 'f':
        bMacroFile = true;
        hMacroFilename = optarg;
        break;

      case 'o':
        hDataFilename = optarg;
        break;

      case 'p':
        bPreInitFromFile = true;
        hPreInitFilename = optarg;
        break;

      case 'n':
        hStream.str(optarg);
        hStream.clear();
        hStream >> iNbEventsToSimulate;
        break;

      case 'i':
        bInteractive = true;
        break;

      default:
        usage();
    }
  }

  //
  if(hDataFilename.empty()) hDataFilename = "events.root";

  // create the run manager
  G4RunManager *pRunManager = new G4RunManager;

  // Detector Construction
  G4String detectorRoot = hDataFilename+"_DET";
  HTPCDetectorConstruction *detCon = new HTPCDetectorConstruction(detectorRoot);
  pRunManager->SetUserInitialization(detCon);

  // Physics List
  HTPCPhysicsList *physList = new HTPCPhysicsList();
  pRunManager->SetUserInitialization(physList);

  // Visualization Manager
  G4VisManager* pVisManager = new G4VisExecutive;
  pVisManager->SetVerboseLevel(0);
  pVisManager->Initialize();

  // create the primary generator action
  HTPCPrimaryGeneratorAction *pPrimaryGeneratorAction = new HTPCPrimaryGeneratorAction();

  // create an analysis manager object
  HTPCAnalysisManager *pAnalysisManager = new HTPCAnalysisManager(pPrimaryGeneratorAction);
  pAnalysisManager->SetDataFilename(hDataFilename);

  if(iNbEventsToSimulate) pAnalysisManager->SetNbEventsToSimulate(iNbEventsToSimulate);

  // set user-defined action classes
  pRunManager->SetUserAction(pPrimaryGeneratorAction);
  pRunManager->SetUserAction(new HTPCStackingAction(pAnalysisManager));
  pRunManager->SetUserAction(new HTPCSteppingAction(pAnalysisManager));
  pRunManager->SetUserAction(new HTPCRunAction(pAnalysisManager));
  pRunManager->SetUserAction(new HTPCEventAction(pAnalysisManager));

  // geometry IO
  G4UImanager* pUImanager = G4UImanager::GetUIpointer();

    if(bPreInitFromFile)
  {
    hCommand = "/control/execute " + hPreInitFilename;
    pUImanager->ApplyCommand(hCommand);
  }

  // initialize it all....
  pRunManager->Initialize();

  G4UIExecutive *pUI = 0;
  G4UIsession * pUIsession = 0;

  if (bInteractive) {  // Interactive mode
        pUI = new G4UIExecutive(argc, argv);

        pUImanager->ApplyCommand("/vis/scene/create");
        pUImanager->ApplyCommand("/vis/open OGL");
        pUImanager->ApplyCommand("/vis/drawVolume");
        pUImanager->ApplyCommand("/vis/viewer/set/viewpointThetaPhi 90 0 deg");
        pUImanager->ApplyCommand("/vis/viewer/set/upVector 0 0 1");
        pUImanager->ApplyCommand("/vis/viewer/zoom 1.0");
        pUImanager->ApplyCommand("/vis/scene/add/trajectories"); // Show particle tracks
        pUImanager->ApplyCommand("/vis/scene/add/hits");
        pUImanager->ApplyCommand("/tracking/storeTrajectory 1"); // Ensure trajectories are stored
        pUImanager->ApplyCommand("/vis/scene/endOfEventAction accumulate");
        pUI->SessionStart();
        delete pUI;
  }

  // run time parameter settings
  if(bMacroFile)
  {
    hCommand = "/control/execute " + hMacroFilename;
    pUImanager->ApplyCommand(hCommand);
  }

  if(iNbEventsToSimulate)
  {
    hStream.str("");
    hStream.clear();
    hStream << "/run/beamOn " << iNbEventsToSimulate;
    pUImanager->ApplyCommand(hStream.str());
  }

  delete pAnalysisManager;
  //if(bVisualize) delete pVisManager;
  delete pRunManager;

  // merge the output files to one .... events.root
  //vector<std::string> auxfiles;
  //auxfiles.push_back(detectorRoot);

  //fileMerger myMerger(hDataFilename,auxfiles);
  //myMerger.Merge();
  //myMerger.CleanUp();

  return 0;
}

void
usage()
{
  exit(0);
}
