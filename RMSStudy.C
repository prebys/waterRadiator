#define RMSStudy_cxx
#include "RMSStudy.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void RMSStudy::Loop()
{
//   In a ROOT session, you can do:
//      root> .L RMSStudy.C
//      root> RMSStudy t
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

   // Get the Z slices. These should match DetectorConstruction.cc
   const int NDET=24;
   double z0=-200.;
   double deltaZ=20.;

   // Shift slightly to put it in the middle of the bin
   z0 -= deltaZ/2.;
   double z1=z0+(NDET)*deltaZ;
   
   TH2F *nphotons = new TH2F("nphotons","Number of Photons in Cuts vs. Z",
     NDET,z0,z1,400,0.,4000.);
   TH2F *xrms = new TH2F("xrms","X RMS vs. Z",NDET,z0,z1,100,0.,100.);   
   TH2F *yrms = new TH2F("yrms","Y RMS vs. Z",NDET,z0,z1,100,0.,100.);   
   TH2F *rrms = new TH2F("rrms","R RMS vs. Z",NDET,z0,z1,100,0.,200.);
   
   
 
   int nphot[NDET];
   double xmean[NDET],x2mean[NDET],ymean[NDET],y2mean[NDET];
   for(int i;i<NDET;i++) {
     nphot[i]=0;
     xmean[i]=0;
     x2mean[i]=0;
     ymean[i]=0;
     y2mean[i]=0;
   }

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0,oldEventID=0,nevents=0,incEvent=-1;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      // Get the total any time the event number changes or on the last entry.
      if(jentry==0) oldEventID=eventID;
      // Calculate the index
      int iz = int((z-z0)/deltaZ);
      if((iz<0)||(iz>(NDET-1))) {
        printf("iz = %d\n",iz);
      }
      // Count top and bottom
      double absy = abs(y);
       
      nphot[iz]+=1;
      xmean[iz]+=x;
      x2mean[iz]+=x*x;
      ymean[iz]+=absy;
      y2mean[iz]+=absy*absy;
      incEvent = eventID;   // Last event to increment counts

      // Add everything up whenever the event changes or on the 
      // Last event.
      // This actually puts the 
      if ((eventID!=oldEventID)||(jentry==(nentries-1))) {
        for(int i=0;i<NDET;i++){
             double zBin=z0+deltaZ*(i+.5);
             xmean[i] /=nphot[i];
             x2mean[i] /=nphot[i];
             ymean[i]  /=nphot[i];
             y2mean[i] /=nphot[i];
             nphotons->Fill(zBin,nphot[i]);
             double xRMS = sqrt(x2mean[i]-xmean[i]*xmean[i]);
             double yRMS = sqrt(y2mean[i]-ymean[i]*ymean[i]);
             double rRMS = sqrt(xRMS*xRMS+yRMS*yRMS);
             xrms->Fill(zBin,xRMS);
             yrms->Fill(zBin,yRMS);
             rrms->Fill(zBin,rRMS);
             nphot[i]=0;
             xmean[i]=0;
             x2mean[i]=0;
             ymean[i]=0;
             y2mean[i]=0;
           }       
         
       
       }
      oldEventID=eventID;
      
      
   }
   auto c = new TCanvas();
   c->Divide(2,2);
   c->cd(1); nphotons->Draw();
   c->cd(2); xrms->Draw();
   c->cd(3); yrms->Draw();
   c->cd(4); rrms->Draw();
   
   //histSiPM->Draw();

}