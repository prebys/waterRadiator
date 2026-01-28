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
  // I'm not sure what's going on, but some photons are entering at odd angles and 
  // hitting the outside, so we will cut them here.
  G4double angle = sqrt(mom.x()*mom.x()+mom.y()*mom.y())/mom.z();
  // Only keep forward going optical photons that are entering the volume from inside
  if((mom.z()>0.)&&
     (pdg==-22)&&
     (pre->GetStepStatus() == fGeomBoundary)&&
     (angle<1.)) {
    man->FillNtupleIColumn(0, eventID);   // <-- NEW
    man->FillNtupleIColumn(1, pdg);
    man->FillNtupleDColumn(2, pos.x()/mm);
    man->FillNtupleDColumn(3, pos.y()/mm);
    man->FillNtupleDColumn(4, pos.z()/mm);
    man->FillNtupleDColumn(5, mom.x()/MeV);
    man->FillNtupleDColumn(6, mom.y()/MeV);
    man->FillNtupleDColumn(7, mom.z()/MeV);
    man->FillNtupleDColumn(8, ekin/eV);
    man->AddNtupleRow();
  }  
  return true;
}
