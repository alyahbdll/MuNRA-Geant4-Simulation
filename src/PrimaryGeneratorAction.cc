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
// MuNRA detector simulation
//
// Adapted and developed by: Alyah Abdellaoui-Masquelier
// Universidad Industrial de Santander (UIS) - LIDeRA
// Research internship, 2026
//
// Based on the Geant4 B1 example.
// --------------------------------------------------------------------
/// \file CosmicWatch/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the primary particle generator used for the MuNRA simulations.
#include "PrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4Exception.hh"
#include "Randomize.hh"
#include "G4Event.hh"
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

namespace B1
{


PrimaryGeneratorAction::PrimaryGeneratorAction()
{

    // One primary particle is generated per event.
    G4int n_particle = 1;
    fParticleGun = new G4ParticleGun(n_particle);

    // Default particle definition.
    // These initial settings are overwritten in GeneratePrimaries()
    // by the particle properties read from the ARTI input file.
    G4ParticleTable* particleTable =
        G4ParticleTable::GetParticleTable();

    G4ParticleDefinition* particle =
        particleTable->FindParticle("mu-");

    fParticleGun->SetParticleDefinition(particle);

    fParticleGun->SetParticleMomentumDirection(
        G4ThreeVector(0., 0., -1.)
    );

    fParticleGun->SetParticleEnergy(1.*GeV);
}


PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
}


void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
// ============================================================
// ARTI input flux
//
// Expected file format:
//
// CORSIKA_ID  px  py  pz  x  y  z
// shower_id  primary_id  primary_energy  primary_theta  primary_phi
//
// Momentum components px, py and pz are given in GeV/c.
// ============================================================

static std::ifstream artiFile("bga_1h.shw");

if (!artiFile.is_open())
{
    G4ExceptionDescription msg;

    msg << "Unable to open ARTI input file: bga_1h.shw";

    G4Exception(
        "PrimaryGeneratorAction::GeneratePrimaries()",
        "ARTI_FILE_ERROR",
        FatalException,
        msg
    );
}

if (!artiFile.is_open())
{
    G4ExceptionDescription msg;

    msg << "Impossible d'ouvrir le fichier ARTI bga_1h.shw";

    G4Exception(
        "PrimaryGeneratorAction::GeneratePrimaries()",
        "MyCode0003",
        FatalException,
        msg
    );
}


// ============================================================
// Read the next valid ARTI particle
// ============================================================

    std::string line;

    while (true)
    {
        if (!std::getline(artiFile, line))
        {
            
        // End of file reached: restart from the beginning.
            artiFile.clear();
            artiFile.seekg(0);
            continue;
        }

       // Skip comments and empty lines.
        if (!line.empty() && line[0] != '#')
        {
            break;
        }
    }


// ============================================================
// Read the 12 ARTI columns
// ============================================================
    std::stringstream ss(line);

    G4int corsikaId;

    G4double px;
    G4double py;
    G4double pz;

    G4double artiX;
    G4double artiY;
    G4double artiZ;

    G4int showerId;
    G4int primaryId;

    G4double primaryEnergy;
    G4double primaryTheta;
    G4double primaryPhi;

    ss >> corsikaId
       >> px
       >> py
       >> pz
       >> artiX
       >> artiY
       >> artiZ
       >> showerId
       >> primaryId
       >> primaryEnergy
       >> primaryTheta
       >> primaryPhi;

    if (ss.fail())
    {
        return;
    }


// ============================================================
// CORSIKA ID to Geant4 particle conversion
// ============================================================
    G4ParticleTable* particleTable =
        G4ParticleTable::GetParticleTable();

    G4ParticleDefinition* particle = nullptr;

    switch (corsikaId)
    {
        case 1:
            particle = particleTable->FindParticle("gamma");
            break;

        case 2:
            particle = particleTable->FindParticle("e+");
            break;

        case 3:
            particle = particleTable->FindParticle("e-");
            break;

        case 5:
            particle = particleTable->FindParticle("mu+");
            break;

        case 6:
            particle = particleTable->FindParticle("mu-");
            break;

        case 8:
            particle = particleTable->FindParticle("pi+");
            break;

        case 9:
            particle = particleTable->FindParticle("pi-");
            break;

        case 13:
            particle = particleTable->FindParticle("neutron");
            break;

        case 14:
            particle = particleTable->FindParticle("proton");
            break;

        case 15:
            particle = particleTable->FindParticle("anti_proton");
            break;

        default:
              // Other CORSIKA particle IDs are not considered in this simulation.
            return;
    }

    if (!particle)
    {
        return;
    }

    fParticleGun->SetParticleDefinition(particle);


// ============================================================
// Kinetic energy calculation
//
// ARTI provides the momentum components px, py and pz in GeV/c.
//
// p     = sqrt(px^2 + py^2 + pz^2)
// E_tot = sqrt(p^2 + m^2)
// E_kin = E_tot - m
//
// Natural units (c = 1) are used in the energy-momentum relation.
// ============================================================
    G4double momentum =
        std::sqrt(
            px*px +
            py*py +
            pz*pz
        ) * GeV;

    G4double mass =
        particle->GetPDGMass();

    G4double totalEnergy =
        std::sqrt(
            momentum*momentum +
            mass*mass
        );

    G4double kineticEnergy =
        totalEnergy - mass;

    fParticleGun->SetParticleEnergy(
        kineticEnergy
    );


// ============================================================
// Particle direction
//
// The transverse momentum components px and py are preserved.
// The Z component is forced to be negative so that all
// particles propagate downward toward the detector.
// ============================================================
    G4ThreeVector direction(
        px,
        py,
        -std::abs(pz)
    );

// Reject particles with zero momentum.
    if (direction.mag2() == 0.)
    {
        return;
    }

// Normalize the momentum vector to obtain a unit direction vector.
    direction = direction.unit();

    fParticleGun->SetParticleMomentumDirection(
        direction
    );


// ============================================================
// Particle generation position
//
// Particles are generated uniformly over a circular source
// positioned above the detector.
//
// Disk radius: R = 3.75 cm
// Source height: z0 (varied between simulation configurations)
// ============================================================
    G4double R =
        3.75*cm;
// Source height for the current simulation configuration.
    G4double z0 =
        169.5*cm;


// Uniform sampling over the surface of the disk.
//
// The square root is required to obtain a uniform surface
// density over the disk.
    G4double u =
        G4UniformRand();

    G4double phi =
        2.*CLHEP::pi*G4UniformRand();

    G4double r =
        R*std::sqrt(u);

    G4double x0 =
        r*std::cos(phi);

    G4double y0 =
        r*std::sin(phi);


    fParticleGun->SetParticlePosition(
        G4ThreeVector(
            x0,
            y0,
            z0
        )
    );


// ============================================================
// Geant4 event generation
// ============================================================

// Print the event number every 100,000 events to monitor
// the progress of long simulation runs.
if (anEvent->GetEventID() % 100000 == 0)
{
    G4cout << "Event " << anEvent->GetEventID() << G4endl;
}


// Generate the primary particle for the current event.
fParticleGun->GeneratePrimaryVertex(anEvent);
}


}
