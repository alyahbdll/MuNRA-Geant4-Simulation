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
/// \file B1/include/EventAction.hh
/// \brief Definition of the B1::EventAction class

#ifndef B1EventAction_h
#define B1EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>
#include "G4SystemOfUnits.hh"


namespace B1
{

class RunAction;

/// Event action class

class EventAction : public G4UserEventAction
{
  public:
    EventAction(RunAction* runAction);
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

    void AddEdep(G4double edep) { fEdep += edep; }
    void SetMuonDetected() { fMuonDetected = true; }

    void AddPhotonDetected(G4double time)
{
  fPhotonsDetected++;
  fSumPhotonTime += time;

  if (!fFirstPhotonRecorded || time < fFirstPhotonTime) {
    fFirstPhotonTime = time;
    fFirstPhotonRecorded = true;
  }

  G4double binWidth = 0.5*ns;
  G4int bin = (G4int)(time/binWidth);

  if (bin >= 0 && bin < (G4int)fPulseBins.size()) {
    fPulseBins[bin]++;
  }
}

    G4int GetPhotonsDetected() const { return fPhotonsDetected; }

const std::vector<G4int>& GetPulseBins() const { return fPulseBins; }

  private:
    RunAction* fRunAction = nullptr;

    G4double fEdep = 0.;
    G4bool fMuonDetected = false;

    G4int fPhotonsDetected = 0;

    G4double fFirstPhotonTime = 0.;
    G4double fSumPhotonTime = 0.;
    G4bool fFirstPhotonRecorded = false;
std::vector<G4int> fPulseBins;

};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif



