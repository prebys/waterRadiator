#define Analyze_cxx
#include "Analyze.h"
#include <TH2.h>
#include <TH1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <TCanvas.h>

void Analyze::Loop()
{
//   In a ROOT session, you can do:
//      root> .L Analyze.C
//      root> Analyze t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch 
   if (fChain == 0) return;
   
   // Book histograms
   // Create a TCanvas to draw the histogram in a window
   TCanvas *c1 = new TCanvas("c1", "Histogram Canvas", 800, 600);

   // Book a 1D histogram (TH1F for float values)
   // Parameters: name, title, num_bins, x_low, x_high
   TH1F *hist = new TH1F("nphotons", "Number of Photons Through Window;N Photons;Frequency", 100, 0.0, 2000.0);
   TH1F *histSiPM = new TH1F("nphotonsSiPM", "Number of Photons Counted by SiPM;N Photons;Frequency", 100, 0.0, 2000.0);



   

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0,oldEventID=0,nphotons=0,nphotonsSiPM=0,nevents=0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      // Get the total any time the event number changes or on the last entry.
      if(jentry==0) oldEventID=eventID;
      
      nphotons += 1;
      if (ekin<4) nphotonsSiPM += 1;
      if ((eventID!=oldEventID)||(jentry==(nentries-1))) {
         // if it's not the last entry, subtract 1
         if (eventID!=oldEventID) {
            nphotons -= 1;
            nphotonsSiPM -= 1;
         }
         printf("Total of %lld photons found for event %lld\n",nphotons,oldEventID);
         hist->Fill(nphotons);
         histSiPM->Fill(nphotonsSiPM*.25);
         
         nphotons = 0;
      }
      oldEventID=eventID;
      
      
   }
   hist->Draw();
   //histSiPM->Draw();

}
