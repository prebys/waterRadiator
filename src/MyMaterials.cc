// MyMaterials
// This builds all the materials needed by the Cerenkov program, including the optical
// properties for those that need them
//

#include "MyMaterials.hh"

MyMaterials::MyMaterials() {

   // Get nist material manager
   // Create a bunch of materials, including optical properties.
   G4NistManager* nist = G4NistManager::Instance();
   //****************************************************************
   // First the simple one. The rest all have optical properties
   titanium = nist->FindOrBuildMaterial("G4_Ti");

   //****************************************************************
   // quarts, including index of refraction and absorption length
   // Create fused Silica (straight from ChatGPT)
   // Elements
   auto* elSi = new G4Element("Silicon",  "Si", 14., 28.0855*g/mole);
   auto* elO  = new G4Element("Oxygen",   "O",  8., 15.999*g/mole);

   quartz = new G4Material("Quartz", 2.200*g/cm3, 2);
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
	
	mptQuartz = new G4MaterialPropertiesTable();
	mptQuartz->AddProperty("RINDEX",    photonEnergy, refractiveIndex,  NUM);
	mptQuartz->AddProperty("ABSLENGTH", photonEnergy, absorptionLength, NUM);
	
	// Optional: Rayleigh scattering (rough order-of-magnitude)
	G4double rayleighLength[NUM] = {
		40*m, 40*m, 35*m, 30*m, 25*m,
		20*m, 15*m, 10*m,  7*m,  5*m
	};
	mptQuartz->AddProperty("RAYLEIGH", photonEnergy, rayleighLength, NUM);

	quartz->SetMaterialPropertiesTable(mptQuartz);
	
	//************************************************************************
	// Air, including index of refraction
  
    air = nist->FindOrBuildMaterial("G4_AIR");
  
    G4double airRindex[NUM] = { 1.0003, 1.0003,1.0003, 1.0003,1.0003, 1.0003,1.0003, 1.0003,
     1.0003, 1.0003  };

    mptAir = new G4MaterialPropertiesTable();
    mptAir->AddProperty("RINDEX", photonEnergy, airRindex, NUM);
    
    // Uncomment this line to add optical properties.  Comment out if you don't want
    // internal reflection
  
    air->SetMaterialPropertiesTable(mptAir);
    //************************************************************************
    // Water, including index of refraction and absorption length
    water = nist->FindOrBuildMaterial("G4_WATER");
    
    G4double H2OrefractiveIndex[NUM] = {
		1.33, 1.33, 1.33, 1.33, 1.33,
		1.33, 1.33, 1.33, 1.33, 1.33
    };
	
	// Absorption length (very long in visible; drops in UV)
	G4double H2OabsorptionLength[NUM] = {
		70*m, 60*m, 55*m, 45*m, 35*m,
		20*m,  15*m,  10*m,  8*m,   4*m
	};
	
	mptH2O= new G4MaterialPropertiesTable();
	mptH2O->AddProperty("RINDEX",    photonEnergy, H2OrefractiveIndex,  NUM);
	mptH2O->AddProperty("ABSLENGTH", photonEnergy, H2OabsorptionLength, NUM);
	
	// Add the index of refraction and absorber length to the material
	water->SetMaterialPropertiesTable(mptH2O);

    //***********************************************************************
    // Stainless steel, including reflectivity
    stainlessSteel = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");

    // Reflective surface (must be added to logical volume)
    mirrorSurface = new G4OpticalSurface("MirrorSurface");

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

    mptSurface = new G4MaterialPropertiesTable();
    mptSurface->AddProperty("REFLECTIVITY", energy, reflectivity, nEntries);

    mirrorSurface->SetMaterialPropertiesTable(mptSurface);



}