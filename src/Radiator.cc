//  Radiator.cc
// This is a static class that will create the dimensions needed to construct
// the water radiator for the Mu2e extinction radiator.
// It's in two section:
//   - The first is the radiator section, which is solid. The sides are angled
//     at the Cerenkov angle for water.
//   - The second is the transport section, which is is a hollow cone statarting
//     at the end of the first section.
//

#include "Radiator.hh" 
#include <cmath>
#include "G4ios.hh"

// Constructor class for Radiator.  Basically does all the calculations
// Of the dimensions.
//
Radiator::Radiator(G4double beamRadius, G4double lenRadiator, G4double lenTransport) {
   // Cerenkov angle of 8 GeV protons in water
   const G4double lightAngle=0.713532378;
   // get the radiator dimension
   z[0]=0.;
   z[1]=lenRadiator;
   rInner[0]=0.;
   rInner[1]=0.;
   rOuter[0]=beamRadius;
   rOuter[1]=beamRadius+lenRadiator*tan(lightAngle);
   
   // get the transport dimensions 
   z[2]=z[1];
   z[3]=z[2]+lenTransport - rOuter[0]*tan(lightAngle);
   z[4]=z[2]+lenTransport;
   rInner[2]=beamRadius;
   rOuter[2]=rOuter[1];
   rInner[3]=rInner[2]+(z[3]-z[2])*tan(lightAngle);
   rOuter[3]=rOuter[2]+(z[3]-z[2])*tan(lightAngle);
   rInner[4]=rInner[2]+lenRadiator*tan(lightAngle);
   rOuter[4]=rInner[4];
   
   for(int i=0;i<NSEG ;i++){
     G4cout <<"z="<<z[i]<<", rInner = "<<rInner[i]<<", rOuter="<<rOuter[i]<<G4endl;
      }

   // Create the radiator solid
   // Make the radiator our of a polycone
   // Full 360-degree polycone
   G4double startPhi = 0.0*deg;
   G4double deltaPhi = 360.0*deg;
   G4cout << "About to create the radiator" <<G4endl;
   // Create the solid
   radiatorSolid = new G4Polycone(
    "Radiator",     // name
    startPhi,         // start angle
    deltaPhi,         // opening angle
    2,       // number of z planes
    z,          // z coordinates
    rInner,           // inner radii
    rOuter            // outer radii
   );
   
   G4cout << "About to create the transport"<<G4endl;
   transportSolid = new G4Polycone(
    "Transport",     // name
    startPhi,         // start angle
    deltaPhi,         // opening angle
    3,       // number of z planes
    &z[2],          // z coordinates
    &rInner[2],           // inner radii
    &rOuter[2]            // outer radii
   );
}
