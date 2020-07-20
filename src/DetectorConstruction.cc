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
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UniformMagField.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4UnitsTable.hh"
#include "G4NistManager.hh"
#include "G4RunManager.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4GlobalMagFieldMessenger.hh"
#include "DetectorMessenger.hh"
#include "G4AutoDelete.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
 : G4VUserDetectorConstruction(),
 fAbsorberPresent(true),fPhysiWorld(nullptr),
 fAbsAlAway(false)
{
  fDetectorMessenger = new DetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  
G4VPhysicalVolume* DetectorConstruction::Construct()
{ 
  // if(fPhysiWorld) { return fPhysiWorld; }

  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();


  G4double density;
  G4int ncomponents, natoms;

  G4NistManager* man = G4NistManager::Instance();

  G4Element* O  = man->FindOrBuildElement("O");
  G4Element* In = man->FindOrBuildElement("In");
  G4Element* Sn = man->FindOrBuildElement("Sn");

  G4Material* Be     = man->FindOrBuildMaterial("G4_Be");
  G4Material* Al     = man->FindOrBuildMaterial("G4_Al");
  G4Material* Si     = man->FindOrBuildMaterial("G4_Si");
  G4Material* Air    = man->FindOrBuildMaterial("G4_AIR");
  G4Material* Kapton = man->FindOrBuildMaterial("G4_KAPTON");

  // ITO
  //
  G4Material* ITO =
    new G4Material("ITO", density= 7.14*g/cm3, ncomponents=3);
  ITO->AddElement(O,  natoms=5);
  ITO->AddElement(Sn, natoms=1);
  ITO->AddElement(In, natoms=2);

  // World
  //
  fWorldSizeX  = 30*cm;
  fWorldSizeYZ = 30*cm;
  G4Box* fSolidWorld = new G4Box("World",                                //its name
                   fWorldSizeX/2,fWorldSizeYZ/2,fWorldSizeYZ/2);  //its size
                         
  G4LogicalVolume* fLogicWorld = new G4LogicalVolume(fSolidWorld,          //its solid
                                   Air,        //its material
                                   "World");              //its name
                                   
  fPhysiWorld = new G4PVPlacement(0,                      //no rotation
                                 G4ThreeVector(0.,0.,0.), //at (0,0,0)
                                 fLogicWorld,             //its logical volume
                                 "World",                 //its name
                                 0,                       //its mother  volume
                                 false,                   //no boolean operation
                                 0);                      //copy number
                                 

  if (fAbsorberPresent) {

	if (!fAbsAlAway) {
		// Absorber = ITO | Kapton | Al
		G4double fAbsorberXpos   = 1.*cm;
		G4double fITOthickness   = 100.*angstrom;
		fAbsorberSizeYZ = 2.5*cm;
		G4double fITOXpos        = fAbsorberXpos + fITOthickness/2;
		G4Box* fSolidITO = new G4Box("ITO",        
							fITOthickness/2,fAbsorberSizeYZ/2,fAbsorberSizeYZ/2);
						
		G4LogicalVolume* fLogicITO = new G4LogicalVolume(fSolidITO,    //its solid
											 ITO, //its material
											 "ITO");       //its name
											  
		G4VPhysicalVolume* fPhysiITO = new G4PVPlacement(0,                   //no rotation
							  G4ThreeVector(fITOXpos,0.,0.),    //its position
									  fLogicITO,     //its logical volume
									  "ITO",         //its name
									  fLogicWorld,        //its mother
									  false,              //no boulean operat
									  0);                 //copy number

		G4double fKaptonThickness = 0.127*mm;
		G4double fKaptonXpos      = fITOXpos + fITOthickness/2 + fKaptonThickness/2;
		G4Box* fSolidKapton = new G4Box("Kapton",
								fKaptonThickness/2,fAbsorberSizeYZ/2,fAbsorberSizeYZ/2);

		G4LogicalVolume* fLogicKapton = new G4LogicalVolume(fSolidKapton,
										  Kapton,
										  "Kapton");

		G4VPhysicalVolume* fPhysiKapton = new G4PVPlacement(0,
								G4ThreeVector(fKaptonXpos, 0., 0.),
										fLogicKapton,
										"Kapton",
										fLogicWorld,
										false,
										0,
										true);

		G4double fAlThickness = 1000.*angstrom;
		G4double fAlXpos      = fKaptonXpos + fKaptonThickness/2 + fAlThickness/2;
		G4Box* fSolidAl = new G4Box("Al",
								fAlThickness/2,fAbsorberSizeYZ/2,fAbsorberSizeYZ/2);

		G4LogicalVolume* fLogicAl = new G4LogicalVolume(fSolidAl,
										  Al,
										  "Al");

		G4VPhysicalVolume* fPhysiAl = new G4PVPlacement(0,
								G4ThreeVector(fAlXpos, 0., 0.),
										fLogicAl,
										"Al",
										fLogicWorld,
										false,
										0,
										true);
	} else {
		// Absorber = Al | Kapton | ITO
		G4double fAbsorberXpos   = 1.*cm;
		fAbsorberSizeYZ = 2.5*cm;
		
		G4double fAlThickness = 1000.*angstrom;
		G4double fAlXpos      = fAbsorberXpos + fAlThickness/2;
		G4Box* fSolidAl = new G4Box("Al",
								fAlThickness/2,fAbsorberSizeYZ/2,fAbsorberSizeYZ/2);

		G4LogicalVolume* fLogicAl = new G4LogicalVolume(fSolidAl,
										  Al,
										  "Al");

		G4VPhysicalVolume* fPhysiAl = new G4PVPlacement(0,
								G4ThreeVector(fAlXpos, 0., 0.),
										fLogicAl,
										"Al",
										fLogicWorld,
										false,
										0,
										true);
		
		G4double fKaptonThickness = 0.127*mm;
		G4double fKaptonXpos      = fAlXpos + fAlThickness/2 + fKaptonThickness/2;
		G4Box* fSolidKapton = new G4Box("Kapton",
								fKaptonThickness/2,fAbsorberSizeYZ/2,fAbsorberSizeYZ/2);

		G4LogicalVolume* fLogicKapton = new G4LogicalVolume(fSolidKapton,
										  Kapton,
										  "Kapton");

		G4VPhysicalVolume* fPhysiKapton = new G4PVPlacement(0,
								G4ThreeVector(fKaptonXpos, 0., 0.),
										fLogicKapton,
										"Kapton",
										fLogicWorld,
										false,
										0,
										true);
		
		
		G4double fITOthickness   = 100.*angstrom;
		G4double fITOXpos        = fKaptonXpos + fKaptonThickness/2 + fITOthickness/2;
		G4Box* fSolidITO = new G4Box("ITO",        
							fITOthickness/2,fAbsorberSizeYZ/2,fAbsorberSizeYZ/2);
						
		G4LogicalVolume* fLogicITO = new G4LogicalVolume(fSolidITO,    //its solid
											 ITO, //its material
											 "ITO");       //its name
											  
		G4VPhysicalVolume* fPhysiITO = new G4PVPlacement(0,                   //no rotation
							  G4ThreeVector(fITOXpos,0.,0.),    //its position
									  fLogicITO,     //its logical volume
									  "ITO",         //its name
									  fLogicWorld,        //its mother
									  false,              //no boulean operat
									  0);                 //copy number
	
	}

}

  // Detector = Be | Si
  //
  // Si
  //
  G4double fSiSizeY     = 2.8*mm;
  G4double fSiSizeZ     = 2.4*mm;
  G4double fSiThickness = 0.5*mm;
  G4double fArmLength   = 10.*cm;
  G4double fSiXpos      = fArmLength + fSiThickness/2;

  fDetectorThickness = fSiThickness;
  fDetectorMaterial  = Si;
  fXstartDet         = fSiXpos - fSiThickness/2;
  fXendDet           = fSiXpos + fSiThickness/2;
  fDetectorSizeYZ    = fSiSizeY;
  
  G4Box* fSolidSi = new G4Box("Si",
                          fSiThickness/2,fSiSizeY/2,fSiSizeZ/2);

  G4LogicalVolume* fLogicSi = new G4LogicalVolume(fSolidSi,
                                    Si,
                                    "Si");

  fPhysiSi = new G4PVPlacement(0,
                          G4ThreeVector(fSiXpos, 0., 0.),
                                  fLogicSi,
                                  "Si",
                                  fLogicWorld,
                                  false,
                                  0);

  // Be
  //
  G4double fBeRadius    = 2.5*mm;
  G4double fBeThickness = 0.0254*mm;
  G4double fBeSpacing	= 1.524*mm; // Spacing between window and front of detector
  G4double fBeXpos      = fSiXpos - fSiThickness/2 - fBeThickness/2 - fBeSpacing;

  G4RotationMatrix* rotMat = new G4RotationMatrix();
  rotMat->rotateY(90.*deg);

  G4Tubs* fSolidBe = new G4Tubs("Be",
                        0., // inner radius
                        fBeRadius, // outer radius
                        fBeThickness/2, // height
                        0.*deg, 360.*deg); // segment angles

  G4LogicalVolume* fLogicBe = new G4LogicalVolume(fSolidBe,
                                    Be,
                                    "Be");

  G4VPhysicalVolume* fPhysiBe = new G4PVPlacement(rotMat,
                          G4ThreeVector(fBeXpos, 0., 0.),
                                  fLogicBe,
                                  "Be",
                                  fLogicWorld,
                                  false,
                                  0,
                                  true);



  PrintGeomParameters();         
  
  //always return the physical World
  //
  return fPhysiWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::PrintGeomParameters()
{
  G4cout << "\n" << "USING CUSTOM GEOMETRY -- MILAN" << G4endl;
  G4cout << "\n" << "Absorber is ";
  if (fAbsorberPresent) {
  	G4cout << "PRESENT" << G4endl;
  	G4cout << "\n" << "The Al side of the absorber is facing ";
  	  if (fAbsAlAway)
	  	G4cout << "AWAY FROM";
	  else
	  	G4cout << "TOWARDS";
	  G4cout << " the detector.";
  } else {
  	G4cout << "NOT present";
  }
  G4cout << G4endl;
  /*
  G4cout << "\n" << fWorldMaterial    << G4endl;
  G4cout << "\n" << fDetectorMaterial << G4endl;
    
  G4cout << "\n The  WORLD   is made of "  << G4BestUnit(fWorldSizeX,"Length")
         << " of " << fWorldMaterial->GetName();
  G4cout << ". The transverse size (YZ) of the world is " 
         << G4BestUnit(fWorldSizeYZ,"Length") << G4endl;
  G4cout << " The DETECTOR is made of " 
         <<G4BestUnit(fDetectorThickness,"Length")
         << " of " << fDetectorMaterial->GetName();
  G4cout << ". The transverse size (YZ) is " 
         << G4BestUnit(fDetectorSizeYZ,"Length") << G4endl;
  G4cout << " X position of the middle of the absorber "
         << G4BestUnit(fXposAbs,"Length");
  G4cout << G4endl; */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAbsorberPresent(G4bool val)
{
  if (val != fAbsorberPresent) {
    fAbsorberPresent = val; 
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAbsorberDirection(G4bool val)
{
  if (val != fAbsAlAway) {
    fAbsAlAway = val; 
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void DetectorConstruction::ConstructSDandField()
{
  if ( fFieldMessenger.Get() == 0 ) {
    // Create global magnetic field messenger.
    // Uniform magnetic field is then created automatically if
    // the field value is not zero.
    G4ThreeVector fieldValue = G4ThreeVector();
    G4GlobalMagFieldMessenger* msg =
      new G4GlobalMagFieldMessenger(fieldValue);
    //msg->SetVerboseLevel(1);
    G4AutoDelete::Register(msg);
    fFieldMessenger.Put( msg );
  }
}