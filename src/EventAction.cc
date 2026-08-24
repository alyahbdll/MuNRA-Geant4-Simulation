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
//

#include "EventAction.hh"
#include "RunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include <fstream>

namespace B1
{


EventAction::EventAction(RunAction* runAction)
: fRunAction(runAction)
{}


void EventAction::BeginOfEventAction(const G4Event*)
{
// Reset all event-level quantities.
  fEdep = 0.;
  fMuonDetected = false;
  fPhotonsDetected = 0;
// Initialize the time bins used to reconstruct the optical pulse.
  fPulseBins.assign(200, 0);

  fFirstPhotonTime = 0.;
  fSumPhotonTime = 0.;
  fFirstPhotonRecorded = false;
}


void EventAction::EndOfEventAction(const G4Event* event)
{
// Add the energy deposited during this event to the run total.
  fRunAction->AddEdep(fEdep);
// Energy-deposition threshold used in the simulated detection criterion.
  G4double energyThreshold = 0.2*MeV;
  G4bool muonDetectedFinal = false;

// An event is considered detected if at least one detection flag was set
// and the deposited energy exceeds the selected threshold.

  if (fMuonDetected && fEdep > energyThreshold) {
    fRunAction->AddMuonDetected();
    muonDetectedFinal = true;
  }

// Add the number of SiPM-detected photons to the run total.
  fRunAction->AddPhotonsDetected(fPhotonsDetected);

  G4double meanPhotonTime = 0.;
  if (fPhotonsDetected > 0) {
    meanPhotonTime = fSumPhotonTime / fPhotonsDetected;
  }

// Append event-level quantities to results.csv.
  std::ofstream file("results.csv", std::ios::app);
  file << event->GetEventID() << ","
       << fEdep/MeV << ","
       << fPhotonsDetected << ","
       << fFirstPhotonTime/ns << ","
       << meanPhotonTime/ns << ","
       << muonDetectedFinal
       << "\n";
  file.close();
// Append the time distribution of detected photons to pulse.csv.
  std::ofstream pulseFile("pulse.csv", std::ios::app);
// Width of each optical-pulse time bin.
  G4double binWidth = 0.5*ns;

  for (G4int i = 0; i < (G4int)fPulseBins.size(); i++) {
    if (fPulseBins[i] > 0) {
      pulseFile << event->GetEventID() << ","
                << (i*binWidth)/ns << ","
                << fPulseBins[i]
                << "\n";
    }
  }

  pulseFile.close();
}

}



