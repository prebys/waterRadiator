// VirtualSD.hh
#pragma once

#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"

class VirtualSD : public G4VSensitiveDetector {
public:
  VirtualSD(const G4String& name);
  virtual ~VirtualSD() = default;

  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory*) override;
};
