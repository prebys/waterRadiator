//  Radiator.cc
// This is the class that creates the radiator assembly for the Mu2e extinction
// water-based Cerenkov detector.  It currently creates logical volumes for the water 
// radiator and the quartz window, called:
//   - radiatorLV 
//   - windowLV
//  It also creates a logical assembly of the two
//   - radiatorAV
//

#include "Radiator.hh" 
#include <cmath>
#include "G4ios.hh"

// Constructor class for Radiator.  Basically does all the calculations
// Of the dimensions.
//
Radiator::Radiator(MyMaterials *mat,G4double beamRadius, G4double lenRadiator,
    G4double windowThickness) {
   // Cerenkov angle of 8 GeV protons in water
   const G4double lightAngle=0.713532378;
   // I'm putting all the z positions in one array, but they are not in order
   // The first three define the radiator polycone and the next three define
   // the window, which starts at the second point of the radiator
   // The dimensions of the radiator
   z[0]=0.;
   z[1]=lenRadiator*(1.-sin(lightAngle)*sin(lightAngle));
   z[2]=lenRadiator;
   rInner[0]=0.;
   rInner[1]=0.;
   rInner[2]=0.;
   rOuter[1]=beamRadius+(z[1]-z[0])*tan(lightAngle);
   rOuter[0]=rOuter[1];
   rOuter[2]=beamRadius;
   // Now add the beam window.  We'll start by creating a polycone and then
   // subtract the radiator from it
   z[3]=z[1];  // Begin at the top of the radiator
   z[4]=z[1]+windowThickness/cos(lightAngle);  
   z[5]=z[2]; // end of the radiator
   z[6]=z[2]+windowThickness/cos(lightAngle);
   
   // Project the beam window back to this point
   // First two points have zero inner radius
   rInner[3] = 0;
   rInner[4] = 0;
   rInner[5] = rOuter[2];  // Edge of beam window
   rInner[6] = rInner[5]+(z[6]-z[5])*tan(lightAngle);
   // Now project the outside
   rOuter[3] = rOuter[1];
   rOuter[4] = rOuter[3]+(z[4]-z[3])*tan(lightAngle);
   rOuter[5] = rOuter[3]-(z[5]-z[3])/tan(lightAngle);
   rOuter[6] = rInner[6];

   
   
   //Print these out to make sure they make sense
   for(int i=0;i<NSEG ;i++){
     G4cout <<"z="<<z[i]<<", rInner = "<<rInner[i]<<", rOuter="<<rOuter[i]<<G4endl;
      }

   // Full 360-degree polycone
   G4double startPhi = 0.0*deg;
   G4double deltaPhi = 360.0*deg;

   // Create the radiator solid
   // Make the radiator our of a polycone
   G4cout << "About to create the radiator" <<G4endl;
   // Create the solid
   auto radiatorSolid = new G4Polycone(
    "Radiator",     // name
    startPhi,         // start angle
    deltaPhi,         // opening angle
    3,       // number of z planes
    z,          // z coordinates
    rInner,           // inner radii
    rOuter            // outer radii
   );
   G4cout << "Created Radiator Solid."<<G4endl;
   
   
   radiatorLV = new G4LogicalVolume(
      radiatorSolid,
      mat->water,
      "RadiatorLV"
    );

   G4cout << "Created Radiator Logical Volume."<<G4endl;
   G4cout << "About to Create Window."<<G4endl;
   
    auto windowTmpSolid = new G4Polycone(
    "tmpWindow",     // name
    startPhi,         // start angle
    deltaPhi,         // opening angle
    4,       // number of z planes
    &z[3],                // coordinates, starting at 4th one
    &rInner[3],           // inner radii
    &rOuter[3]            // outer radii
   );
   
   // Now create a subtraction solid
   auto windowSolid = new G4SubtractionSolid(
      "Window",
      windowTmpSolid,
      radiatorSolid);
   
   G4cout << "Created Window Solid."<<G4endl;
   
   windowLV = new  G4LogicalVolume(
      windowSolid,
      mat->quartz,
      "WindowLV");
   
   
   G4cout << "Created Window Logical Volume."<<G4endl;
   // Now add the two together
    G4ThreeVector pos(0, 0, 0);
    G4RotationMatrix* rot = new G4RotationMatrix();

   radiatorAV = new G4AssemblyVolume();
   radiatorAV->AddPlacedVolume(radiatorLV,pos,rot);
   radiatorAV->AddPlacedVolume(windowLV,pos,rot);
    


}
