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
//
// --------------------------------------------------------------------
// MuNRA detector simulation
//
// Adapted and developed by: Alyah Abdellaoui-Masquelier
// Universidad Industrial de Santander (UIS) - LIDeRA
// Research internship, 2026
//
// Based on the Geant4 B1 example.
// --------------------------------------------------------------------

/// \file CosmicWatch/src/ActionInitialization.cc
/// \brief Initialization of the user actions used in the MuNRA simulation.
#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

namespace B1
{


void ActionInitialization::BuildForMaster() const
{

// In multi-threaded mode, the master thread only needs the run action.
  auto runAction = new RunAction;
  SetUserAction(runAction);
}


void ActionInitialization::Build() const
{
// Primary particle generator.
  SetUserAction(new PrimaryGeneratorAction);
// Run-level accumulation and output handling.
  auto runAction = new RunAction;
  SetUserAction(runAction);
// Event-level accumulation and output writing.
  auto eventAction = new EventAction(runAction);
  SetUserAction(eventAction);
// Step-by-step tracking of energy deposition and optical photons.
  SetUserAction(new SteppingAction(eventAction));
}


}
