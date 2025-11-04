
#include <chrono>
#include <iostream>
#include <string>
#include <stdexcept>
#include <random>
#include <vector>

#include <TFile.h>
#include <TTree.h>

#include "../utils/utils.hpp"


int main() {
    // Create ROOT file and TTree
    std::string filename = "synthetic_data.root";
    TFile* file = TFile::Open(filename.c_str(), "RECREATE");
    if (!file || file->IsZombie()) {
        throw std::runtime_error("Failed to create ROOT file"); 
    }

    // Create vector of vectors
    // Each inner vector has 100,000 random floats from a uniform distribution
    const int numEntries = 10'000;
    const int innerSize = 30;

    std::vector<float> smallDistribution(innerSize);
    std::vector<float> largeDistribution(innerSize);

    std::mt19937 gen(42); // Fixed seed for reproducibility
    std::uniform_real_distribution<float> smallDist(1.0, 9.0);
    std::uniform_real_distribution<float> largeDist(-4.0, 4.0);

    TTree* tree = new TTree("Tree", "Synthetic Data Tree");
    tree->Branch("SmallDistribution", &smallDistribution);
    tree->Branch("LargeDistribution", &largeDistribution);

    for (int i = 0; i < numEntries; ++i) {
        // Generate new data
        for (int j = 0; j < innerSize; ++j) {
            smallDistribution[j] = smallDist(gen);
            largeDistribution[j] = largeDist(gen);
        }
        tree->Fill();        
    }

    // Write and close
    tree->Write();
    file->Close();
    delete file;
}