// Radiator.hh
//
//  This is a class that will create the radiator and window for the
// Mu2e extinction Monitor 
#pragma once

#include "G4SystemOfUnits.hh"
#include "G4Polycone.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "MyMaterials.hh"


class Radiator  {
public:
  static const G4int NSEG=7;
  G4double z[NSEG];
  G4double rInner[NSEG];
  G4double rOuter[NSEG];
  G4LogicalVolume *radiatorLV;
  G4LogicalVolume *windowLV;
  Radiator(MyMaterials *mat,G4double beamRadius, G4double lenRadiator,G4double windowThickness);

};
