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


class Radix {
private:
  const char* s;
  int a[128];
public:
  Radix(const char* s = "0123456789ABCDEF") : s(s) {
    int i;
    for(i = 0; s[i]; ++i)
      a[(int)s[i]] = i;
  }
  std::string to(long long p, int q) {
    int i;
    if(!p)
      return "0";
    char t[64] = { };
    for(i = 62; p; --i) {
      t[i] = s[p % q];
      p /= q;
    }
    return std::string(t + i + 1);
  }
  std::string to(const std::string& t, int p, int q) {
    return to(to(t, p), q);
  }
  long long to(const std::string& t, int p) {
    int i;
    long long sm = a[(int)t[0]];
    for(i = 1; i < (int)t.length(); ++i)
      sm = sm * p + a[(int)t[i]];
    return sm;
  }
};

int ctoi(char c) {
	if (c >= '0' && c <= '9') {
		return c - '0';
	}
	return 0;
}

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
    tree->Branch("eDep", &eDep);
    tree->Branch("eDepIndex", &eDepIndex);
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
    eDep = std::vector<G4double>(n_split * n_split * n_split);
    for (G4int i = 0; i < n_split; i++) eDep.at(0) = 0;
    for (G4int i = 0; i < n_split; i++) {
        for (G4int j = 0; j < n_split; j++) {
            for (G4int k = 0; k < n_split; k++) {
                eDepIndex[i][j][k] = 0;
            }
        }
    }
}

G4bool SensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{   
    G4int copy_number = aStep
        ->GetPreStepPoint()
        ->GetPhysicalVolume()
        ->GetCopyNo();
    copy_number -= Geometry::copyNum_Pb_offset;
    G4double a_edep = aStep->GetTotalEnergyDeposit() / eV;
    eDep.at(copy_number) += a_edep;

    std::string copy_number_index_str = Radix().to(std::to_string(copy_number), 10, 25);
    G4int eDep_x_index = ctoi(copy_number_index_str[0]);
    G4int eDep_y_index = ctoi(copy_number_index_str[1]);
    G4int eDep_z_index = ctoi(copy_number_index_str[2]);
    eDepIndex[eDep_x_index][eDep_y_index][eDep_z_index] += a_edep;
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