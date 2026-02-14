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
#include "G4UnionSolid.hh"
#include "G4RotationMatrix.hh"
#include "G4ios.hh"

#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"

// Stuff for the sensitive surface
#include "G4SDManager.hh"
#include "SurfaceSD.hh"
#include "Radiator.hh"

#include "CADMesh.hh"

#include "MyMaterials.hh"  // This is where the materials are built


namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
   // Set up some basic parameters
   // World
   //
   G4double world_sizeX = 2000. * mm;
   G4double world_sizeY = 2000.* mm;
   G4double world_sizeZ = 2400. * mm;
  
    // Make the shapes for the radiator and transport sections
   G4double ang=0.713532378;   // Cherenkov angle

   G4double beamRadius = 3.*cm;
   G4double lenRadiator = 10.*cm;
   G4double beamWindowThickness = .1*mm;
   G4double windowThickness = 1*mm;   // quartz window
   G4int  nMirrors = 2;   // Number of mirrors (can only be 2 or 4)
   G4double mirrorRadius = 50.*cm;
   G4double mirrorThickness = 2.*mm;
   G4double yDetector = 50*cm; // Offset to detector.  It will rotate for each mirror
   // Build the materials 
   // This builds the radiator Solid
   static MyMaterials mat;

   static Radiator rad(&mat,beamRadius,lenRadiator,windowThickness);


   G4cout << "About to create materials" << G4endl;
   G4cout << "Created materials" << G4endl;


   // Option to switch on/off checking of volumes overlaps
   //
   G4bool checkOverlaps = true;




  auto solidWorld =
    new G4Box("World",  // its name
              0.5 * world_sizeX, 0.5 * world_sizeY, 0.5 * world_sizeZ);  // its size

  auto logicWorld = new G4LogicalVolume(solidWorld,  // its solid
                                        mat.air,  // its material
                                        "World");  // its name

  auto physWorld = new G4PVPlacement(nullptr,  // no rotation
                                     G4ThreeVector(0,0,0),  // at (0,0,0)
                                     logicWorld,  // its logical volume
                                     "World",  // its name
                                     nullptr,  // its mother  volume
                                     false,  // no boolean operation
                                     0,  // copy number
                                     checkOverlaps);  // overlaps checking

    
   // Full 360-degree polycone
   G4double startPhi = 0.0*deg;
   G4double deltaPhi = 360.0*deg;
      

   // Place the radiator
   new G4PVPlacement(
     nullptr,                 // no rotation
     G4ThreeVector(0,0,0),    // position
     rad.radiatorLV,           // logical volume
     "Radiator",          // name
     logicWorld,              // mother volume
     false,                   // no boolean operation
     0,                       // copy number
     true                     // check overlaps
   );


   //Make the window a sensitive detector
   auto sdManager = G4SDManager::GetSDMpointer();
   auto surfaceSD = new SurfaceSD("Window");
   sdManager->AddNewDetector(surfaceSD);

   rad.windowLV->SetSensitiveDetector(surfaceSD);

   // Place it in the world (no rotation, centered at origin)
  
   new G4PVPlacement(
     nullptr,                 // no rotation
     G4ThreeVector(0,0,0),    // position
     rad.windowLV,           // logical volume
     "Window",          // name
     logicWorld,              // mother volume
     false,                   // no boolean operation
     0,                       // copy number
     true                     // check overlaps
   );
   

  
  
   // Add a titanium window at the end to block photons from getting back into 
   // transport
 
   
   auto beamWindowSolid = new G4Tubs(
      "BeamWindow",
      0.,
      beamRadius,
      beamWindowThickness,
      0*degree,
      360*degree);



   // Logical volume
   auto beamWindowLV = new G4LogicalVolume(
      beamWindowSolid,
      mat.titanium,
      "BeamWindowLV"
    );
    // place 1 at each end of the radiator volume
    // Beginning
    new G4PVPlacement(
      nullptr,                 // no rotation
      G4ThreeVector(0,0,-beamWindowThickness/2.),    // position
      beamWindowLV,           // logical volume
      "BeamWindow",          // name
      logicWorld,              // mother volume
      false,                   // no boolean operation
      0,                       // copy number
      true                     // check overlaps
    );
    // End
    new G4PVPlacement(
      nullptr,                 // no rotation
      G4ThreeVector(0,0,lenRadiator+beamWindowThickness/2.),    // position
      beamWindowLV,           // logical volume
      "BeamWindow",          // name
      logicWorld,              // mother volume
      false,                   // no boolean operation
      1,                       // copy number
      true                     // check overlaps
    );

  
    // Uncomment the code below to read in the mirror from an STL file
    // Import the .stl file
    /*
    auto mesh = CADMesh::TessellatedMesh::FromSTL("Mirror.stl");
    mesh->SetScale(1.0);  // mm

     G4VSolid* reflectorSolid = mesh->GetSolid();
    */
    // Otherwise, build the mirror by intersecting a partial sphere with a polycone
    // Origin or the center of the photons that get through
    //  Playing with different values
    G4double zOrigin = (lenRadiator-beamRadius/tan(ang))/2.;
    zOrigin = 0;
  
    // Construct a polycone that will be the envelope for  the mirror by projecting
    // the exit window
    G4double zEnv[2] = {lenRadiator,zOrigin+mirrorRadius+mirrorThickness};
    G4double rEnvInner[2] = {beamRadius-1*cm,beamRadius-1.*cm+(zEnv[1]-zEnv[0])*tan(ang)};
    G4double rEnvOuter[2] = {beamRadius+1*cm+lenRadiator*tan(ang),beamRadius+1.*cm+
         zEnv[1]*tan(ang)};
         
    // Determine the extent of the envelope based on the number of mirrors
    G4double envPhi0=1.*deg;
    G4double envDeltaPhi=178*deg;
    if(nMirrors==4) {
      envPhi0=46.*deg;
      envDeltaPhi=88.*deg;
    }
  
    auto envelope = new G4Polycone("Envelope",
         envPhi0,
         envDeltaPhi,
         2,
         zEnv,
         rEnvInner,
         rEnvOuter);
         
    // Now create the section of the spherical mirror
  
  
    auto sphere = new G4Sphere("Reflector",
                           mirrorRadius,
                           mirrorRadius+mirrorThickness,
                           0.*deg, 360.*degree,
                           0.*deg, 60.*degree);
                           
    // Now make the mirror from the union of the two.  Make the origine of the rays the 
    // middle of the part of the radiator that makes it out
    auto reflectorSolid = new G4IntersectionSolid(
       "Mirror",
       envelope,
       sphere,
       nullptr,      // <-- no rotation
       G4ThreeVector(0.,yDetector/2.,zOrigin)     // <-- translation only
    );
    
    auto reflectorLV = new G4LogicalVolume (
      reflectorSolid,
      mat.stainlessSteel,
      "ReflectorLV");
    
    // Add the reflective surface
    
    new G4LogicalSkinSurface(
     "MirrorSkin",
     reflectorLV,
     mat.mirrorSurface
    );    
    // Place nMirror of these, rotating by 90 or 180 degrees each time
    // Step through by 2 if there are only 2 mirrors
    double dPhi = 180.*deg;
    if(nMirrors==4) dPhi=90.*deg;
        
    for(int i = 0; i<nMirrors; i+=1) {
       auto refRot = new G4RotationMatrix();
      
       refRot->rotateZ(i*dPhi);


       new G4PVPlacement(
        refRot,                 // rotate
        G4ThreeVector(0.,0.,0.),    // position
        reflectorLV,           // logical volume
        "Reflector",          // name
        logicWorld,              // mother volume
        false,                   // no boolean operation
        i,                       // copy number
        false                     // check overlaps
      );
     }
    // Set up a bunch of virtual detectors near the focal plane
    auto detectorTube = new G4Tubs(
      "Detector",
      yDetector-30.*cm,
      yDetector+30.*cm,
      1*mm,
      0*degree,
      360*degree);
      
    auto detectorLV = new G4LogicalVolume(
      detectorTube,
      mat.air,
      "DetectorLV"
    );
    

    detectorLV->SetSensitiveDetector(surfaceSD);

    // The values RMSStudy should match these (yeah, I know I should put it in
    // a header file.)    
    const G4int NDET=24;
    G4double z0=-200*mm,deltaZ=20.*mm;
    
    for(int i=0;i<NDET;i++) {
          new G4PVPlacement(
          nullptr,                 // no rotation
          G4ThreeVector(0,0,z0+i*deltaZ),    // position
          detectorLV,           // logical volume
          "Detector",          // name
          logicWorld,              // mother volume
          false,                   // no boolean operation
          i,                       // copy number
          true                     // check overlaps
       );       
    }


    //
    // always return the physical World
    // This is a legacy from B1 that I haven't gotten rid of.
    fScoringVolume = rad.radiatorLV;
    
    return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B1
