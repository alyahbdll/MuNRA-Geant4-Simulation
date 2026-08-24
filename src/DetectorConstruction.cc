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
// --------------------------------------------------------------------
// MuNRA detector simulation
//
// Adapted and developed by: Alyah Abdellaoui-Masquelier
// Universidad Industrial de Santander (UIS) - LIDeRA
// Research internship, 2026
//
// Based on the Geant4 B1 example.
// --------------------------------------------------------------------

/// \file CosmicWatch/src/DetectorConstruction.cc
/// \brief Implementation of the MuNRA detector geometry and optical properties.

#include "DetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Acces the NIST materiel database.
  G4NistManager* nist = G4NistManager::Instance();

  // Envelope dimensions
  // we chose a small enveloppe to reduce the calcul calcul
  G4double env_sizeXY = 60*cm, env_sizeZ = 3.2*m;
  G4Material* env_mat = nist->FindOrBuildMaterial("G4_AIR");

  // Checking overlap for placed volumes.
  G4bool checkOverlaps = true;

  // World
  G4double world_sizeXY = 1.2*env_sizeXY;
  G4double world_sizeZ  = 1.2*env_sizeZ;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

  auto solidWorld = new G4Box("World",                           // its name
    0.5 * world_sizeXY, 0.5 * world_sizeXY, 0.5 * world_sizeZ);  // half length along X,Y,Z

  auto logicWorld = new G4LogicalVolume(solidWorld,  // Geometry
    world_mat,                                       // Materiel : Air
    "World");                                        // Logical volume name 

  auto physWorld = new G4PVPlacement(nullptr,  // No rotation
    G4ThreeVector(),                           // At (0,0,0)
    logicWorld,                                // Its logical volume
    "World",                                   // Its name
    nullptr,                                   // Its mother  volume
    false,                                     // No boolean operation
    0,                                         // Copy number
    checkOverlaps);                            // Overlaps checking

// --------------------------------------------------------------------
// Envelope
// --------------------------------------------------------------------

  // Creating the air-Filled enveloppe containing the detetctor geometry
  auto solidEnv = new G4Box("Envelope",                    //Name
    0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5 * env_sizeZ);  // Size

  auto logicEnv = new G4LogicalVolume(solidEnv,  // Geometry
    env_mat,                                     // Material: air
    "Envelope");                                 // Logical volume name


 //Place the envelope at the center of the world.
 auto physEnv = new G4PVPlacement(nullptr,
  G4ThreeVector(), //No rotation 
  logicEnv,	   //Position at the origin
  "Envelope", 	   //Logical volume to place
  logicWorld,	   //Mother volume : world
  false,	  //No boolean operation
  0,
  checkOverlaps);


// --------------------------------------------------------------------
// Scintillator material and optical properties
// --------------------------------------------------------------------

// Use polystyrene from the Geant4 NIST material database as the
// scintillator bulk material.
G4Material* scint_mat =
    nist->FindOrBuildMaterial("G4_POLYSTYRENE");



// Table containing the optical properties of the scintillator.

auto mptScint = new G4MaterialPropertiesTable();


// Photon-energy grid used to define the wavelength-dependent
// optical properties of the scintillator.
static G4double photonEnergy[] = {
    2.00*eV, 2.03*eV, 2.06*eV, 2.09*eV, 2.12*eV,
    2.15*eV, 2.18*eV, 2.21*eV, 2.24*eV, 2.27*eV,
    2.30*eV, 2.33*eV, 2.36*eV, 2.39*eV, 2.42*eV,
    2.45*eV, 2.48*eV, 2.51*eV, 2.54*eV, 2.57*eV,
    2.60*eV, 2.63*eV, 2.66*eV, 2.69*eV, 2.72*eV,
    2.75*eV, 2.78*eV, 2.81*eV, 2.84*eV, 2.87*eV,
    2.90*eV, 2.93*eV, 2.96*eV, 2.99*eV, 3.02*eV,
    3.05*eV, 3.08*eV, 3.11*eV, 3.14*eV, 3.17*eV,
    3.20*eV, 3.23*eV, 3.26*eV, 3.29*eV, 3.32*eV,
    3.35*eV, 3.38*eV, 3.41*eV, 3.44*eV, 3.47*eV
};



// Automatically determine the number of entries in the photon-energy array.
// This allows the optical-property arrays to be updated consistently.
G4int nEntries =
    sizeof(photonEnergy)/sizeof(G4double);




// --------------------------------------------------------------------
// Refractive index
// --------------------------------------------------------------------

// Refractive index of the polystyrene scintillator.
// A constant value of n = 1.58 is assumed over the full energy range.
static G4double refIndex[] = {
    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
    1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58
 };

mptScint->AddProperty( "RINDEX",photonEnergy,refIndex,nEntries);


// --------------------------------------------------------------------
// Absorption length
// --------------------------------------------------------------------

// Mean distance travelled by an optical photon before absorption
// in the scintillator. A constant absorption length of 24 cm is
// assumed over the full photon-energy range.

static G4double absLen[] = {
   24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm,
   24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm,
    24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm,
    24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm,
    24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm
};

mptScint->AddProperty(
    "ABSLENGTH",
    photonEnergy,
    absLen,
    nEntries);

// Scale factor applied to the statistical fluctuations of the
// number of scintillation photons produced.

mptScint->AddConstProperty("RESOLUTIONSCALE", 1.0);



// --------------------------------------------------------------------
// Scintillation emission spectrum
// --------------------------------------------------------------------


// Approximate normalized spectrum digitized from Fig. 4 of
// Pla-Dalmau et al., FERMILAB-CONF-03-318-E.
//
// The measured scintillator contains:
//   - Dow Styron 663W polystyrene
//   - 1 wt.% PPO
//   - 0.03 wt.% POPOP
//
// The emission maximum is close to 420 nm
// (approximately 2.95 eV).
//
// The photon-energy array is ordered increasingly, while the
// corresponding wavelength is ordered decreasingly.
//

static G4double scintEmission[] = {
    0.000, 0.000, 0.000, 0.000, 0.000,
    0.000, 0.000, 0.000, 0.000, 0.004,
    0.011, 0.018, 0.025, 0.034, 0.053,
    0.072, 0.090, 0.123, 0.155, 0.187,
    0.228, 0.277, 0.325, 0.372, 0.430,
    0.490, 0.556, 0.623, 0.702, 0.784,
    0.864, 0.943, 0.989, 0.946, 0.892,
    0.840, 0.840, 0.871, 0.898, 0.853,
    0.809, 0.769, 0.733, 0.699, 0.702,
    0.749, 0.795, 0.772, 0.665, 0.560
};

mptScint->AddProperty(
    "SCINTILLATIONCOMPONENT1",
    photonEnergy,
    scintEmission,
    nEntries);


// --------------------------------------------------------------------
// Scintillation yield
// --------------------------------------------------------------------

// Mean number of scintillation photons produced per MeV of
// energy deposited in the scintillator.

mptScint->AddConstProperty(
    "SCINTILLATIONYIELD",
    7222./MeV);

// --------------------------------------------------------------------
// Scintillation decay time
// --------------------------------------------------------------------

// Characteristic decay time of the scintillation light emission.

mptScint->AddConstProperty(
    "SCINTILLATIONTIMECONSTANT1",
    7.06*ns);

// All scintillation light is assigned to the first scintillation component.

mptScint->AddConstProperty(
    "SCINTILLATIONYIELD1",
    1.0);

// Assign the optical properties to the scintillator material.

scint_mat->SetMaterialPropertiesTable(mptScint);


// --------------------------------------------------------------------
// Detector geometry
// --------------------------------------------------------------------

// SiPM material: silicon.

 G4Material* sipm_mat =
    nist->FindOrBuildMaterial("G4_Si");


// Scintillator position at the center of the envelope.
G4ThreeVector pos1 = G4ThreeVector(0,0,0);



// MuNRA plastic scintillator: 8 x 4 x 1 cm.
auto solidShape1 = new G4Box(
    "Scintillator",
    4*cm,
    2*cm,
    0.5*cm);

// SiPM position on the upper surface of the scintillator.
G4ThreeVector posSiPM = G4ThreeVector(0,0,5.325*mm);


// MicroFC-60035-SMT SiPM active volume: 6 x 6 x 0.65 mm.
auto solidSiPM = new G4Box(
    "SiPM",
    3.0*mm,
    3.0*mm,
    0.325*mm);




// Create the scintillator logical volume.
auto logicShape1 = new G4LogicalVolume(
    solidShape1,
    scint_mat,
    "Scintillator");

// --------------------------------------------------------------------
// Reflective aluminium surface
// --------------------------------------------------------------------


// Optical surface used to model the reflective aluminium wrapping
// around the scintillator.
auto scinOptSurf = new G4OpticalSurface("ScinOptSurf");

scinOptSurf->SetType(dielectric_metal);
scinOptSurf->SetModel(unified);
scinOptSurf->SetFinish(polished);


// Optical properties of the aluminium-like reflective surface.
auto mptAl = new G4MaterialPropertiesTable();



// Constant reflectivity of 0.92 over the full photon-energy range.
static G4double scinOptSurfRefIndex[] = {
        0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92,
        0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92,
        0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92,
        0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92,
        0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92, 0.92
 };


// The aluminium wrapping is not used as a photon detector,
// therefore its optical detection efficiency is set to zero.
static G4double scinOptSurfEff[] = {
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
 };



mptAl->AddProperty("REFLECTIVITY", photonEnergy, scinOptSurfRefIndex, nEntries);
mptAl->AddProperty("EFFICIENCY",   photonEnergy, scinOptSurfEff,      nEntries);

// --------------------------------------------------------------------
// Building simulation
// --------------------------------------------------------------------


// Concrete material used to model the building storeys.
G4Material* concrete_mat =
    nist->FindOrBuildMaterial("G4_CONCRETE");

// --------------------------------------------------------------------
// Concrete layers
// --------------------------------------------------------------------

// Concrete slab dimensions: 50 x 50 x 20 cm.
auto solidConcrete = new G4Box( "Concrete", 25*cm, 25*cm, 10*cm);




// --------------------------------------------------------------------
// SiPM logical volume
// --------------------------------------------------------------------

auto logicSiPM = new G4LogicalVolume(
    solidSiPM,
    sipm_mat,
    "SiPM");


// Visualization settings for the SiPM.
auto sipmVis = new G4VisAttributes(G4Colour::Black());
sipmVis->SetVisibility(true);
logicSiPM->SetVisAttributes(sipmVis);


// --------------------------------------------------------------------
// Scintillator placement
// --------------------------------------------------------------------

// Place the scintillator at the center of the envelope.
auto physScint = new G4PVPlacement(
    nullptr,
    pos1,
    logicShape1,
    "Scintillator",
    logicEnv,
    false,
    0,
    checkOverlaps);



// Concrete logical volume
auto logicConcrete = new G4LogicalVolume(
    solidConcrete,
    concrete_mat,
    "Concrete");

// --------------------------------------------------------------------
// Concrete slab placement
// --------------------------------------------------------------------

// Place successive concrete slabs along the Z axis.
// Each slab is 20 cm thick and separated from the next one by
// a 1 cm air gap.
//
// The upper limit of the loop was manually changed between runs
// to simulate different numbers of concrete layers.
for (G4int i = 0; i <0; i++) {

    G4double zConcrete = 11.5*cm + i*21.*cm;

    new G4PVPlacement(
        nullptr,
        G4ThreeVector(0,0,zConcrete),
        logicConcrete,
        "Concrete",
        logicEnv,
        false,
        i,
        checkOverlaps);
}




// --------------------------------------------------------------------
// Reflective wrapping applied to the scintillator
// --------------------------------------------------------------------

// Assign the aluminium-like optical properties to the reflective surface.
scinOptSurf->SetMaterialPropertiesTable(mptAl);

// Apply the reflective optical surface to the outer boundary
// of the scintillator volume.

new G4LogicalSkinSurface(
   "AlSkin",
    logicShape1,
    scinOptSurf);


// --------------------------------------------------------------------
// SiPM placement
// --------------------------------------------------------------------
auto physSiPM = new G4PVPlacement(
    nullptr,
    posSiPM,
    logicSiPM,
    "SiPM",
    logicEnv,
    false,
    0,
    checkOverlaps);

// --------------------------------------------------------------------
// SiPM optical surface
// --------------------------------------------------------------------
auto sipmSurf = new G4OpticalSurface("SiPMSurface");

sipmSurf->SetType(dielectric_metal);
sipmSurf->SetModel(unified);
sipmSurf->SetFinish(polished);

auto mptSiPM = new G4MaterialPropertiesTable();

// --------------------------------------------------------------------
// SiPM photon detection efficiency
// --------------------------------------------------------------------

// Photon detection efficiency (PDE) of the MicroFC-60035-SMT SiPM,
// digitized from the manufacturer datasheet (PDE versus wavelength).
// 3.47 eV ≈ 357 nm tab made from  of Figure 3. PDE versus Wavelength(MicroFC−30035−SMT)


static G4double sipmEff[] = { 
    0.01,   
    0.01, 0.01, 0.01, 0.02, 0.02,
    0.03, 0.04, 0.06, 0.08, 0.10,
    0.12, 0.15, 0.18, 0.21, 0.24,
    0.27, 0.30, 0.32, 0.34, 0.36,
    0.38, 0.39, 0.40, 0.41, 0.41,
    0.40, 0.39, 0.38, 0.36, 0.34,
    0.32, 0.30, 0.28, 0.26, 0.24,
    0.22, 0.20, 0.18, 0.16, 0.14,
    0.12, 0.10, 0.08, 0.06, 0.05,
    0.04, 0.03, 0.02, 0.02
};

// The manufacturer does not provide the wavelength-dependent
// reflectivity of the SiPM surface. Reflectivity is therefore
// neglected in this model.
static G4double sipmReflect[] = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};

mptSiPM->AddProperty("EFFICIENCY", photonEnergy, sipmEff, nEntries);
mptSiPM->AddProperty("REFLECTIVITY", photonEnergy, sipmReflect, nEntries);

sipmSurf->SetMaterialPropertiesTable(mptSiPM);

// Apply the SiPM optical surface at the scintillator-SiPM interface.

new G4LogicalBorderSurface(
    "ScintToSiPM",
    physScint,
    physSiPM,
    sipmSurf);


// Define the scintillator as the scoring volume.
  fScoringVolume = logicShape1;

  //always return the physical World
  return physWorld;
}

}
