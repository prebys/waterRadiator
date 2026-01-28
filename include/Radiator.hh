// Radiator.hh
//
//  This is a class that will create the radiator and transport for the
// Mu2e extinction Monitor
//
#pragma once

#include "G4SystemOfUnits.hh"
#include "G4Polycone.hh"


class Radiator  {
public:
  static const G4int NSEG=5;
  G4double z[NSEG];
  G4double rInner[NSEG];
  G4double rOuter[NSEG];
  G4Polycone *radiatorSolid;  // Solid corresponding to radiator
  G4Polycone *transportSolid; // Solid corresponding to transport
  Radiator(G4double beamRadius, G4double lenRadiator, G4double lenTransport);

};
