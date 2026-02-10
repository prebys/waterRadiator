//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4TwoVector.hh"
#include "G4ExtrudedSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4Polycone.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4IntersectionSolid.hh"
#include "G4RotationMatrix.hh"
#include "G4ios.hh"

#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"

// Stuff for the sensitive surface
#include "G4SDManager.hh"
#include "SurfaceSD.hh"
#include "Radiator.hh"

#include "CADMesh.hh"


namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  // Create a bunch of materials, including optical properties.
  G4NistManager* nist = G4NistManager::Instance();
  
  // Create fused Silica (straight from ChatGPT)
  // Elements
  auto* elSi = new G4Element("Silicon",  "Si", 14., 28.0855*g/mole);
  auto* elO  = new G4Element("Oxygen",   "O",  8., 15.999*g/mole);

  G4Material* quartz = new G4Material("Quartz", 2.200*g/cm3, 2);
  quartz->AddElement(elSi, 1);
  quartz->AddElement(elO, 2);

  const G4int NUM = 10;
	
  G4double photonEnergy[NUM] = {
		1.77*eV, 2.00*eV, 2.25*eV, 2.50*eV, 2.75*eV,
		3.00*eV, 3.25*eV, 3.50*eV, 3.75*eV, 4.13*eV
  };
	
	// Refractive index of fused silica (approximate)
  G4double refractiveIndex[NUM] = {
		1.455, 1.456, 1.457, 1.458, 1.459,
		1.460, 1.461, 1.462, 1.463, 1.465
  };
	
	// Absorption length (very long in visible; drops in UV)
	G4double absorptionLength[NUM] = {
		100*m, 100*m, 100*m, 100*m, 100*m,
		100*m,  50*m,  20*m,  10*m,   1*m
	};
	
	auto* mptQuartz = new G4MaterialPropertiesTable();
	mptQuartz->AddProperty("RINDEX",    photonEnergy, refractiveIndex,  NUM);
	mptQuartz->AddProperty("ABSLENGTH", photonEnergy, absorptionLength, NUM);
	
	// Optional: Rayleigh scattering (rough order-of-magnitude)
	G4double rayleighLength[NUM] = {
		40*m, 40*m, 35*m, 30*m, 25*m,
		20*m, 15*m, 10*m,  7*m,  5*m
	};
	mptQuartz->AddProperty("RAYLEIGH", photonEnergy, rayleighLength, NUM);

	quartz->SetMaterialPropertiesTable(mptQuartz);
	
	// Add the index of refraction for air
  
    G4Material* air = nist->FindOrBuildMaterial("G4_AIR");
  
    G4double airRindex[NUM] = { 1.0003, 1.0003,1.0003, 1.0003,1.0003, 1.0003,1.0003, 1.0003,
     1.0003, 1.0003  };

    auto mptAir = new G4MaterialPropertiesTable();
    mptAir->AddProperty("RINDEX", photonEnergy, airRindex, NUM);
    
    // Uncomment this line to add optical properties.  Comment out if you don't want
    // internal reflection
  
    air->SetMaterialPropertiesTable(mptAir);

    // Water
  G4Material* water = nist->FindOrBuildMaterial("G4_WATER");
    
  G4double H2OrefractiveIndex[NUM] = {
		1.33, 1.33, 1.33, 1.33, 1.33,
		1.33, 1.33, 1.33, 1.33, 1.33
  };
	
	// Absorption length (very long in visible; drops in UV)
	G4double H2OabsorptionLength[NUM] = {
		70*m, 60*m, 55*m, 45*m, 35*m,
		20*m,  15*m,  10*m,  8*m,   4*m
	};
	
	auto* mptH2O= new G4MaterialPropertiesTable();
	mptH2O->AddProperty("RINDEX",    photonEnergy, H2OrefractiveIndex,  NUM);
	mptH2O->AddProperty("ABSLENGTH", photonEnergy, H2OabsorptionLength, NUM);
	
	// Add the index of refraction and absorber length to the material
	water->SetMaterialPropertiesTable(mptH2O);


  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //
  // World
  //
  G4double world_sizeX = 1000. * mm;
  G4double world_sizeY = 1000.* mm;
  G4double world_sizeZ = 2400. * mm;


  auto solidWorld =
    new G4Box("World",  // its name
              0.5 * world_sizeX, 0.5 * world_sizeY, 0.5 * world_sizeZ);  // its size

  auto logicWorld = new G4LogicalVolume(solidWorld,  // its solid
                                        air,  // its material
                                        "World");  // its name

  auto physWorld = new G4PVPlacement(nullptr,  // no rotation
                                     G4ThreeVector(0,0,0),  // at (0,0,0)
                                     logicWorld,  // its logical volume
                                     "World",  // its name
                                     nullptr,  // its mother  volume
                                     false,  // no boolean operation
                                     0,  // copy number
                                     checkOverlaps);  // overlaps checking

   // Make the shapes for the radiator and transport sections
   G4double beamRadius = 3.*cm;
   G4double lenRadiator = 6.*cm;
   G4double lenTransport = 6.*cm;
   Radiator rad(beamRadius,lenRadiator,lenTransport);
   
   // Logical for radiator
   auto radiatorLV = new G4LogicalVolume(
      rad.radiatorSolid,
      water,
      "RadiatorLV"
    );
   // Logical volume for transport
   auto transportLV = new G4LogicalVolume(
      rad.transportSolid,
      water,
      "TransportLV"
   );
   

  // Place them in the world (no rotation, centered at origin)
  new G4PVPlacement(
     nullptr,                 // no rotation
     G4ThreeVector(0,0,0),    // position
     radiatorLV,           // logical volume
     "Radiator",          // name
     logicWorld,              // mother volume
     false,                   // no boolean operation
     0,                       // copy number
     true                     // check overlaps
  );
    new G4PVPlacement(
     nullptr,                 // no rotation
     G4ThreeVector(0,0,0),    // position
     transportLV,           // logical volume
     "Transport",          // name
     logicWorld,              // mother volume
     false,                   // no boolean operation
     0,                       // copy number
     true                     // check overlaps
  );

  
  //Make a detector to detect particle leaving the volume
  // Create the solid

  G4double zWindow[2] = {rad.z[3],rad.z[4]};
  G4double rInnerWindow[2] = {rad.rOuter[3],rad.rOuter[4]};
  G4double rOuterWindow[2] = {rad.rOuter[3]+1.*mm,rad.rOuter[4]+1.*mm};

   // Full 360-degree polycone
   G4double startPhi = 0.0*deg;
   G4double deltaPhi = 360.0*deg;

  auto windowSolid = new G4Polycone(
    "Window",     // name
    startPhi,         // start angle
    deltaPhi,         // opening angle
    2,       // number of z planes
    zWindow,          // z coordinates
    rInnerWindow,           // inner radii
    rOuterWindow            // outer radii
  );

   // Logical volume, made of quartz
   auto windowLV = new G4LogicalVolume(
      windowSolid,
      quartz,
      "WindowLV"
    );
  

   //Make it a sensitive detector
  auto sdManager = G4SDManager::GetSDMpointer();
  auto surfaceSD = new SurfaceSD("SurfaceSD");
  sdManager->AddNewDetector(surfaceSD);

  windowLV->SetSensitiveDetector(surfaceSD);
 
  // Place it in the world (no rotation, centered at origin)
  
  new G4PVPlacement(
     nullptr,                 // no rotation
     G4ThreeVector(0,0,0),    // position
     windowLV,           // logical volume
    "Window",          // name
    logicWorld,              // mother volume
    false,                   // no boolean operation
    0,                       // copy number
    true                     // check overlaps
  );
  
 // Add a beryllium window at the end to block photons from getting back into 
 // transport
 
  G4Material* beryllium = nist->FindOrBuildMaterial("G4_Ti");
    // Use a polycone because I'm lazy
  G4double zBeamWindow[2]={rad.z[2],rad.z[2]+.1*mm};
  G4double rInnerBeamWindow[2]={0.,0.};
  G4double rOuterBeamWindow[2]={rad.rInner[2],rad.rInner[2]};

  auto beamWindowSolid = new G4Polycone(
    "BeamWindow",     // name
    startPhi,         // start angle
    deltaPhi,         // opening angle
    2,       // number of z planes
    zBeamWindow,          // z coordinates
    rInnerBeamWindow,           // inner radii
    rOuterBeamWindow            // outer radii
  );

   // Logical volume, made of quartz
   auto beamWindowLV = new G4LogicalVolume(
      beamWindowSolid,
      beryllium,
      "BeamWindowLV"
    );
  // Place it in the world (no rotation, centered at origin)
  
    new G4PVPlacement(
      nullptr,                 // no rotation
      G4ThreeVector(0,0,0),    // position
      beamWindowLV,           // logical volume
      "BeamWindow",          // name
      logicWorld,              // mother volume
      false,                   // no boolean operation
      0,                       // copy number
      true                     // check overlaps
    );
 
 // Add a reflector
  G4Material* stainlessSteel = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");

 // Make it reflective
 auto mirrorSurface = new G4OpticalSurface("MirrorSurface");

 mirrorSurface->SetType(dielectric_metal);   // metal mirror
 mirrorSurface->SetModel(unified);            // most flexible
 mirrorSurface->SetFinish(polished);          // or ground
 const G4int nEntries = 2;
 G4double energy[nEntries] = {
  1.0*eV, 5.0*eV
 };

 G4double reflectivity[nEntries] = {
  0.95, 0.95
 };

 auto surfaceMPT = new G4MaterialPropertiesTable();
 surfaceMPT->AddProperty("REFLECTIVITY", energy, reflectivity, nEntries);

 mirrorSurface->SetMaterialPropertiesTable(surfaceMPT);
 
  // Read in the stl file defining the reflector
  // Import the .stl file
  /*
  auto mesh = CADMesh::TessellatedMesh::FromSTL("Mirror.stl");
  mesh->SetScale(1.0);  // mm

   G4VSolid* reflectorSolid = mesh->GetSolid();
  */
  
  // Construct a polycone that will be the envelope for  the mirror
  G4double ang=0.713532378;   
  G4double zEnv[2] = {lenRadiator,lenRadiator/2.+50*cm};
  G4double rEnvInner[2] = {beamRadius-1*cm,beamRadius-1.*cm+(zEnv[1]-zEnv[0])*tan(ang)};
  G4double rEnvOuter[2] = {rEnvInner[0]+beamRadius+2.*cm,rEnvInner[1]+beamRadius+2.*cm};
  
  auto envelope = new G4Polycone("Envelope",1.*deg,
         178.*deg,
         2,
         zEnv,
         rEnvInner,
         rEnvOuter);
         
  // Now create the section of the spherical mirror
  
  G4double yDetector = 350*mm;
  
  auto sphere = new G4Sphere("Reflector",
                           500*mm,
                           502*mm,
                           0.*deg, 360.*deg,
                           0.*deg, 60.*deg);
                           
  // Now make the mirror from the union of the two
   auto reflectorSolid = new G4IntersectionSolid(
    "Mirror",
    envelope,
    sphere,
    nullptr,      // <-- no rotation
    G4ThreeVector(0.,yDetector/2.,0.)     // <-- translation only
);
    
  auto reflectorLV = new G4LogicalVolume (
    reflectorSolid,
    stainlessSteel,
    "ReflectorLV");
    
    // Add the reflective surface
    
   new G4LogicalSkinSurface(
     "MirrorSkin",
     reflectorLV,
     mirrorSurface
   );    
   // Place four of these, rotating by 90 degrees each time
     // rotate
    const G4int NREF=4;    
    
    G4double xoffs[NREF] = {0.,yDetector/2.,0.,-yDetector/2.};
    G4double yoffs[NREF] = {yDetector/2.,0.,-yDetector/2.,0.};
        
    for(int i = 1; i<NREF; i+=2) {
      auto refRot = new G4RotationMatrix();
      
      refRot->rotateZ(i*90.*degree);


      new G4PVPlacement(
        refRot,                 // rotate
        G4ThreeVector(0.,0.,0.),    // position
        reflectorLV,           // logical volume
        "Reflector1",          // name
        logicWorld,              // mother volume
        false,                   // no boolean operation
        i,                       // copy number
        false                     // check overlaps
      );
    }
 
 // kill particles that get to a certain point
  auto shieldTube = new G4Tubs(
    "Shield",
    yDetector-10.*cm,
    yDetector+10.*cm,
    1*mm,
    0*degree,
    360*degree
 );
  auto shieldLV = new G4LogicalVolume(
      shieldTube,
      stainlessSteel,
      "ShieldLV"
    );
  // Place it in the world (no rotation, centered at origin)
  
    new G4PVPlacement(
      nullptr,                 // no rotation
      G4ThreeVector(0,0,-5.*mm),    // position
      shieldLV,           // logical volume
      "Shield",          // name
      logicWorld,              // mother volume
      false,                   // no boolean operation
      0,                       // copy number
      true                     // check overlaps
    );

 // Set Shape2 as scoring volume
  //
  fScoringVolume = windowLV;

  //
  // always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B1
