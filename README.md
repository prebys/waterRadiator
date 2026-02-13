\page waterRadiator

 This is geant project for the Mu2e Cerenkov based on the B1 example in the Geant 
 distribution.
 
 It's designed with a water radiator with a quartz exit window, followed by 2 or 4
 mirrors, which focus the light on points away from the beam axis.  
 
 To use, make a parallel build directory, cd into it, and then type
- cmake ../waterRadiator

 You can then run it interactively with
 
- ./waterRadiator

 or in batch mode with
 
- ./waterRadiator waterRadiator.in (currently 1000 events)

It makes currently makes two ntuples
 
- windowhits: photons that go through the exit window
- hits: photons that hit a number of virtual detector planes near the focal region, to study the focusing.
 
 The key files in src and include directories are:
- DetectorConstruction: Builds the geometry based on parameters near the top.
- MyMaterials: Makes all the materials, which are complicated because of optical properties
- SurfaceSD:  Defines the sensitive detector and fills one of two ntuples based on which volume it's called from. stores only optical photons
- RunAction: Books the Ntuples

In addition, there are root analysis files in the main director:
- Analyze.C (.h): analyze the quartz window ntuple
- RMSStudy.C (.h): analyze the virtual detector Ntuple

The templates were made with the TTRee::MakeClass() method, and the usage from within ROOT is, eg.
- .L Analyze.C+
- Analyze a
- a.Loop()

Note!  These are both copied over from the main main directory to the build directory every time your run cmake, so be sure to edit them there!

# History
13-Feb-2026  E. Prebys   Lots of cleanup.  First version I'm willing to share.


```


