// SurfaceSD.cc

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "SurfaceSD.hh"
#include "G4AnalysisManager.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "Radiator.hh"

#include <cmath>



SurfaceSD::SurfaceSD(const G4String& name)
  : G4VSensitiveDetector(name) {}


G4bool SurfaceSD::ProcessHits(G4Step* step, G4TouchableHistory*) {

  if (step->GetPreStepPoint()->GetStepStatus() != fGeomBoundary)
    return false;

  auto track = step->GetTrack();
  auto pre   = step->GetPreStepPoint();
  

  auto pos  = pre->GetPosition();
  auto mom  = pre->GetMomentum();
  auto pdg  = track->GetParticleDefinition()->GetPDGEncoding();
  auto ekin = pre->GetKineticEnergy();

  // --- NEW: get event number ---
  auto event =
    G4RunManager::GetRunManager()->GetCurrentEvent();
  auto eventID = event->GetEventID();

  auto man = G4AnalysisManager::Instance();
  
  // Figure out which volume we're in. I got this from ChatGPT because it's ridiculously
  // hard
  G4StepPoint* prePoint = step->GetPreStepPoint();
  G4TouchableHandle touchable = prePoint->GetTouchableHandle();

  G4VPhysicalVolume* physVol = touchable->GetVolume();
  G4LogicalVolume*  logVol  = physVol->GetLogicalVolume();

  G4String physName = physVol->GetName();
  G4String logName  = logVol->GetName();  // Test on this
  
  
  if(logName=="WindowLV") {  // In the quartz window

  // Only keep forward going optical photons that are entering the volume from inside
  // I'm not sure what's going on, but some photons are entering at odd angles and 
  // hitting the outside, so we will cut them here.
    G4double angle = sqrt(mom.x()*mom.x()+mom.y()*mom.y())/mom.z();
    if((mom.z()>0.)&&
      (pdg==-22)&&
     ( pre->GetStepStatus() == fGeomBoundary)&&
      (angle<1.)) {
      man->FillNtupleIColumn(0,0, eventID);   // <-- NEW
      man->FillNtupleIColumn(0,1, pdg);
      man->FillNtupleDColumn(0,2, pos.x()/mm);
      man->FillNtupleDColumn(0,3, pos.y()/mm);
      man->FillNtupleDColumn(0,4, pos.z()/mm);
      man->FillNtupleDColumn(0,5, mom.x()/MeV);
      man->FillNtupleDColumn(0,6, mom.y()/MeV);
      man->FillNtupleDColumn(0,7, mom.z()/MeV);
      man->FillNtupleDColumn(0,8, ekin/eV);
      man->AddNtupleRow(0);
    }
  }  else if(logName=="DetectorLV"){  // In detector planes
    // These all appear to go in the right direction (ie, backwards)
    if(pdg==-22) {
       man->FillNtupleIColumn(1,0, eventID);  
       man->FillNtupleIColumn(1,1, pdg);
       man->FillNtupleDColumn(1,2, pos.x()/mm);
       man->FillNtupleDColumn(1,3, pos.y()/mm);
       man->FillNtupleDColumn(1,4, pos.z()/mm);
       man->FillNtupleDColumn(1,5, mom.x()/MeV);
       man->FillNtupleDColumn(1,6, mom.y()/MeV);
       man->FillNtupleDColumn(1,7, mom.z()/MeV);
       man->FillNtupleDColumn(1,8, ekin/eV);
       man->AddNtupleRow(1);   
 	}
  }
  return true;
}
