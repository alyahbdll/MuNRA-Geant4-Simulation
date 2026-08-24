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
// MuNRA detector simulation
//
// Adapted and developed by: Alyah Abdellaoui-Masquelier
// Universidad Industrial de Santander (UIS) - LIDeRA
// Research internship, 2026
//
// Based on the Geant4 B1 example.
// --------------------------------------------------------------------



#include <fstream>
#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

namespace B1
{


RunAction::RunAction()
{
// --------------------------------------------------------------------
// Dose units
// --------------------------------------------------------------------
// Additional dose units used for the run summary.
  const G4double milligray = 1.e-3*gray;
  const G4double microgray = 1.e-6*gray;
  const G4double nanogray  = 1.e-9*gray;
  const G4double picogray  = 1.e-12*gray;

  new G4UnitDefinition("milligray", "milliGy" , "Dose", milligray);
  new G4UnitDefinition("microgray", "microGy" , "Dose", microgray);
  new G4UnitDefinition("nanogray" , "nanoGy"  , "Dose", nanogray);
  new G4UnitDefinition("picogray" , "picoGy"  , "Dose", picogray);

// --------------------------------------------------------------------
// Run accumulables
// --------------------------------------------------------------------

// Register quantities accumulated over all events of the run.
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->RegisterAccumulable(fEdep);
  accumulableManager->RegisterAccumulable(fEdep2);
  accumulableManager->RegisterAccumulable(fMuonDetected);
 accumulableManager->RegisterAccumulable(fPhotonsDetected);
}


void RunAction::BeginOfRunAction(const G4Run*)
{
// inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

// reset all accumulables to their initial values
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Reset();
  std::ofstream file("results.csv");
  file << "eventID,edep_MeV,photons_SiPM,firstPhotonTime_ns,meanPhotonTime_ns,muon_detected\n";
  file.close();
    // Create the photon-arrival-time file and write its header.
	std::ofstream pulseFile("pulse.csv");
pulseFile << "eventID,time_ns,photons\n";
pulseFile.close();
}


void RunAction::EndOfRunAction(const G4Run* run)
{

// Number of simulated events in the current run.
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

 // Merge the accumulated quantities from all worker threads.
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Merge();

  // Compute dose = total energy deposit in a run and its variance
  //
  G4double edep  = fEdep.GetValue();
  G4double edep2 = fEdep2.GetValue();
  G4int muonDetected = fMuonDetected.GetValue();
 G4int photonsDetected = fPhotonsDetected.GetValue();

  G4double rms = edep2 - edep*edep/nofEvents;
  if (rms > 0.) rms = std::sqrt(rms); else rms = 0.;

  const auto detConstruction = static_cast<const DetectorConstruction*>(
    G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  G4double mass = detConstruction->GetScoringVolume()->GetMass();
  G4double dose = edep/mass;
  G4double rmsDose = rms/mass;

  // Run conditions
  //  note: There is no primary generator action object for "master"
  // run manager for multi-threaded mode.
  const auto generatorAction = static_cast<const PrimaryGeneratorAction*>(
    G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  G4String runCondition;
  if (generatorAction)
  {
    const G4ParticleGun* particleGun = generatorAction->GetParticleGun();
    runCondition += particleGun->GetParticleDefinition()->GetParticleName();
    runCondition += " of ";
    G4double particleEnergy = particleGun->GetParticleEnergy();
    runCondition += G4BestUnit(particleEnergy,"Energy");
  }

// --------------------------------------------------------------------
// Run summary
// --------------------------------------------------------------------
  if (IsMaster()) {
    G4cout
     << G4endl
     << "--------------------End of Global Run-----------------------";
  }
  else {
    G4cout
     << G4endl
     << "--------------------End of Local Run------------------------";
  }

  G4cout
     << G4endl
     << " The run consists of " << nofEvents << " "<< runCondition
     << G4endl
     << " Cumulated dose per run, in scoring volume : "
     << G4BestUnit(dose,"Dose") << " rms = " << G4BestUnit(rmsDose,"Dose")
     << G4endl
     << "Detected event in scintillator : "
    << muonDetected
    << G4endl
   << " Photons detected by SiPM : "
   << photonsDetected
   << G4endl
     << "------------------------------------------------------------"
     << G4endl
     << G4endl;
}


void RunAction::AddEdep(G4double edep)
{
  fEdep  += edep;
  fEdep2 += edep*edep;
}

void RunAction::AddMuonDetected()
{
  fMuonDetected += 1;
}

void RunAction::AddPhotonsDetected(G4int n)
{
  fPhotonsDetected += n;
}


}
