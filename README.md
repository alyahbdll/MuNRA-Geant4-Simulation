# MuNRA Geant4 Simulation

Geant4 simulation of the MuNRA plastic scintillator detector developed during a research internship at the Laboratory for Research and Development of Radiation and Astroparticle Detectors (LIDeRA), Universidad Industrial de Santander (UIS), Colombia.

The simulation was developed to study the response of the MuNRA detector to atmospheric secondary particles and to investigate particle-flux attenuation through concrete layers.

## Overview

The detector model consists of:

- an 8 × 4 × 1 cm³ polystyrene scintillator;
- optical scintillation processes;
- aluminium reflective wrapping;
- a 6 × 6 mm² silicon photomultiplier (SiPM);
- wavelength-dependent SiPM photon detection efficiency.

The optical properties of the scintillator include its emission spectrum, absorption length, refractive index, scintillation yield and decay time.

## Atmospheric particle source

Atmospheric secondary particles are provided by an ARTI/CORSIKA `.shw` file.

For each particle, the simulation reads:

- CORSIKA particle ID;
- momentum components (`px`, `py`, `pz`);
- shower information;
- primary-particle information.

The CORSIKA particle ID is converted to the corresponding Geant4 particle definition, and the kinetic energy and direction are reconstructed from the momentum components.

Particles are generated uniformly over a circular source above the detector.

The ARTI `.shw` input files are not included in this repository because of their size.

## Concrete attenuation

Concrete slabs can be placed between the particle source and the detector to study the attenuation of the atmospheric particle flux.

Each slab has dimensions:

`50 × 50 × 20 cm³`

with a 1 cm air gap between successive slabs.

The number of concrete layers and the source height were varied between simulations in order to reproduce different material thicknesses while maintaining the appropriate source-detector geometry.

## Output

Two output files are generated during a simulation:

### `results.csv`

Contains event-level information:

- event ID;
- energy deposited in the scintillator;
- number of photons detected by the SiPM;
- first detected photon time;
- mean detected photon time;
- detection flag.

### `pulse.csv`

Contains the temporal distribution of detected optical photons for each event.

## Repository structure

```text
.
├── include/                # Header files
├── src/                    # Geant4 source files
├── CMakeLists.txt          # CMake configuration
├── exampleB1.cc            # Main application
├── runProduction.mac       # Production run macro
├── init_vis.mac            # Visualization initialization
└── vis.mac                 # Visualization macro

```


## Main simulation files

- `DetectorConstruction.cc`: detector geometry, materials, optical properties and concrete layers.
- `PrimaryGeneratorAction.cc`: ARTI/CORSIKA particle input and primary-particle generation.
- `EventAction.cc`: event-level quantities and output files.
- `RunAction.cc`: run-level initialization and accumulated quantities.
- `SteppingAction.cc`: particle tracking and optical-photon detection.
- `ActionInitialization.cc`: registration of the Geant4 user actions.

## Software

The simulation was developed using Geant4 and is based on the structure of the Geant4 B1 example.

## Author

Alyah Abdellaoui-Masquelier  
Université Paris Cité  
Research internship at LIDeRA – Universidad Industrial de Santander, 2026
