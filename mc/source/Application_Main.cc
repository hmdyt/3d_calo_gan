#include "Geometry.hh"
#include "UserActionInitialization.hh"
#include "SensitiveDetector.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "FTFP_BERT.hh"
#include "G4ios.hh"

#include <iostream>
#include <random>

int main(int argc, char** argv){
    // random seed
    std::random_device rd{};
    CLHEP::RanecuEngine* engine = new CLHEP::RanecuEngine;
    engine->setSeed(rd());
    G4Random::setTheEngine(engine);

    G4RunManager* runManager = new G4RunManager;
    runManager->SetUserInitialization(new Geometry);
    runManager->SetUserInitialization(new FTFP_BERT);
    runManager->SetUserInitialization(new UserActionInitialization);
    runManager->Initialize();

    G4VisExecutive *visManager = new G4VisExecutive;
    visManager->Initialize();

    G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    G4UImanager* uiManager = G4UImanager::GetUIpointer();

    if (argc == 1){
        // interactive mode
        G4UIExecutive* uiExec = new G4UIExecutive(argc, argv);
        // vis_init
        uiManager->ApplyCommand("/control/execute visSetup_Simplex.mac");
        uiExec->SessionStart();
        delete uiExec;
    } else if (argc == 2) {
        // macro mode
        G4String macroName = argv[1];
        uiManager->ApplyCommand("/control/execute " + macroName);
    } else if (argc == 3){
        // macro mode with output
        G4String macroName = argv[1];
        G4String outFileName = argv[2];
        SensitiveDetector* sensitiveDetector = (SensitiveDetector*)sdManager->FindSensitiveDetector("SensitiveDetector");
        sensitiveDetector->setOutFileName(outFileName);
        uiManager->ApplyCommand("/control/execute " + macroName);
        sensitiveDetector->saveTTreeAsRootFile();
    } else {
        G4cerr << "Invalid argument." << G4endl;
        return -1;
    }

    delete runManager;
}