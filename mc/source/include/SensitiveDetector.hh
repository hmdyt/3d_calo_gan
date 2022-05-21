#ifndef SensitiveDetector_h
#define SensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "Geometry.hh"

#include "TTree.h"
#include "TFile.h"
#include <vector>
#include <string>
class G4Step;

class SensitiveDetector : public G4VSensitiveDetector
{
    public:
        SensitiveDetector(G4String);
        ~SensitiveDetector();

        void Initialize(G4HCofThisEvent*);
        G4bool ProcessHits(G4Step*, G4TouchableHistory*);
        void EndOfEvent(G4HCofThisEvent*);

        void initTree();
        void setOutFileName(G4String outFileName_arg);
        G4String getOutFileName();

        void saveTTreeAsRootFile();

    private:
        G4String outFileName;
        TTree* tree;
        TFile* tfile;
        std::vector<G4double> eDep;
        static const G4int n_split = (G4int) Geometry::n_split;
        G4double eDepIndex[n_split][n_split][n_split];
        G4int i_event;
        G4int n_events;
        G4int i_tree;
};

#endif 