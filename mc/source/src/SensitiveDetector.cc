#include "SensitiveDetector.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4HCofThisEvent.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "Geometry.hh"

#include "TTree.h"
#include "TFile.h"
#include "TString.h"
#include <vector>
#include <string>

SensitiveDetector::SensitiveDetector(G4String name)
:G4VSensitiveDetector(name)
{   
    i_event = 0;
    i_tree = 0;
    outFileName = "tmp.root";
    initTree();
}

SensitiveDetector::~SensitiveDetector(){}

void SensitiveDetector::initTree(){
    tree = new TTree("tree", "mc output");
    tree->Branch("eDep", eDep, TString::Format("eDep[%d]/D", n_split * n_split * n_split).Data());
    tree->Branch("n_split", &n_split_tree);
}

G4String SensitiveDetector::getOutFileName(){ return outFileName; }
void SensitiveDetector::setOutFileName(G4String outFileName_arg)
{
    outFileName = outFileName_arg;
}

void SensitiveDetector::saveTTreeAsRootFile()
{   
    TString savingFileName;
    savingFileName = Form("%s_%d.root", outFileName.c_str(), i_tree);
    tfile = new TFile(savingFileName.Data(), "recreate");
    tree->Write();
    tfile->Close();
    delete tree;
    initTree();
}

void SensitiveDetector::Initialize(G4HCofThisEvent*)
{   
    n_split_tree = n_split;
    for (G4int i = 0; i < n_split; i++) eDep[0] = 0;
}

G4bool SensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{   
    G4int copy_number = aStep
        ->GetPreStepPoint()
        ->GetPhysicalVolume()
        ->GetCopyNo();
    copy_number -= Geometry::copyNum_Pb_offset;
    G4double a_edep = aStep->GetTotalEnergyDeposit() / eV;
    eDep[copy_number] += a_edep;
    return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
    tree->Fill();
    // save 100 man event
    if (i_event % 1000000 == 0 && i_event != 0){
        saveTTreeAsRootFile();
        G4cout << "saved tree" << i_tree << G4endl;
        i_tree++;
    }
    i_event++;
    n_events = G4RunManager::GetRunManager()->GetNumberOfEventsToBeProcessed();
    if ((100*i_event) % n_events == 0){
        G4cout << 100*i_event/n_events << "% done" << G4endl;
    }
}