#include <iostream>
#include "TString.h"
#include "TChain.h"
#include "TH3D.h"

void visuallize_an_event(
    TString file_name = "/Users/yuto/VS/3d_calo_gan/mc/bench/test_01_0.root",
    long long int event_number = 99
) { 
    constexpr int N_BINX = 25;
    constexpr int N_BINY = 25;
    constexpr int N_BINZ = 25;
    constexpr double X_MIN = -5.0;
    constexpr double X_MAX = 5.0;
    constexpr double Y_MIN = -5.0;
    constexpr double Y_MAX = 5.0;
    constexpr double Z_MIN = -5.0;
    constexpr double Z_MAX = 5.0;
    TH3D* h_3d = new TH3D("h_3d", "h_3d", N_BINX, X_MIN, X_MAX, N_BINY, Y_MIN, Y_MAX, N_BINZ, Z_MIN, Z_MAX);
    
    TChain* chain = new TChain("tree");
    chain->Add(file_name);
    double eDep[N_BINX * N_BINY * N_BINZ];
    chain->SetBranchAddress("eDep", eDep);
    long long int n_events = chain->GetEntries();
    if (event_number >= n_events) {
        std::cout << "event_number is too large" << std::endl;
        return;
    }
    chain->GetEntry(event_number);
    int index_counter = 0;
    for (int i_binx = 0; i_binx < N_BINX; i_binx++) {
        for (int i_biny = 0; i_biny < N_BINY; i_biny++) {
            for (int i_binz = 0; i_binz < N_BINZ; i_binz++) {
                h_3d->SetBinContent(i_binx + 1, i_biny + 1, i_binz + 1, eDep[index_counter]);
                index_counter++;
            }
        }
    }
    h_3d->Draw("box");
}