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
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "globals.hh"
#include "G4Cache.hh"

class G4Box;
class G4VPhysicalVolume;
class G4Material;
class G4MaterialCutsCouple;
class G4UniformMagField;
class G4GlobalMagFieldMessenger;
class DetectorMessenger;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:

  DetectorConstruction();
  ~DetectorConstruction();

  void SetAbsorberPresent  (G4bool);
  void SetAbsorberDirection(G4bool val);
  void UpdateGeometry();

  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();

  void PrintGeomParameters();

  const G4Material* GetDetectorMaterial() const {return fDetectorMaterial;};
  G4double GetDetectorThickness() const         {return fDetectorThickness;};

  G4double GetxstartDet() const                 {return fXstartDet;};
  G4double GetxendDet() const                   {return fXendDet;};

  G4double GetWorldSizeX() const                {return fWorldSizeX;};

  const G4VPhysicalVolume* GetDetector() const  {return fPhysiSi;};

private:

  G4Material*        fDetectorMaterial;
  G4double           fDetectorThickness;
  G4double           fDetectorSizeYZ, fAbsorberSizeYZ;

  G4double           fXposAbs;
  G4double           fXstartDet, fXendDet;

  G4Material*        fWorldMaterial;
  G4double           fWorldSizeX;
  G4double           fWorldSizeYZ;

  G4bool             fDefaultWorld;
  G4bool             fAbsorberPresent;
  G4bool			 fAbsAlAway;

  G4VPhysicalVolume* fPhysiWorld;
  G4VPhysicalVolume* fPhysiSi;

     
  DetectorMessenger* fDetectorMessenger;
  G4Cache<G4GlobalMagFieldMessenger*> fFieldMessenger;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

