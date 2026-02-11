// VirtualSD.cc

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "VirtualSD.hh"
#include "G4AnalysisManager.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "Radiator.hh"

#include <cmath>



VirtualSD::VirtualSD(const G4String& name)
  : G4VSensitiveDetector(name) {}


G4bool VirtualSD::ProcessHits(G4Step* step, G4TouchableHistory*) {

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
  G4double angle = sqrt(mom.x()*mom.x()+mom.y()*mom.y())/mom.z();
  // Only keep forward going optical photons that are entering the volume from inside
  if((pdg==-22)&&
     (pre->GetStepStatus() == fGeomBoundary)) {
    man->FillNtupleIColumn(1,0, eventID);   // <-- NEW
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
  return true;
}
