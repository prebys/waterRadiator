// SurfaceSD.hh
#pragma once

#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"

class SurfaceSD : public G4VSensitiveDetector {
public:
  SurfaceSD(const G4String& name);
  virtual ~SurfaceSD() = default;

  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory*) override;
};
