// MyMaterials.hh
// This will build all the materials needed by the Cerenkov radiator simulation
// All this was originally in DetectorConstruction, but it got too busy
//
#pragma once

#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"
#include "G4SystemOfUnits.hh"



class MyMaterials {
public:
  MyMaterials();
  G4Material *air;
  G4Material *water;
  G4Material *quartz;
  G4Material *titanium;
  G4Material *stainlessSteel;
  G4OpticalSurface *mirrorSurface;
private:
  G4MaterialPropertiesTable *mptQuartz;
  G4MaterialPropertiesTable  *mptAir;
  G4MaterialPropertiesTable *mptH2O;
  G4MaterialPropertiesTable *mptSurface;
  
};
