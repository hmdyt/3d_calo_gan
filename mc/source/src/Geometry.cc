#include "Geometry.hh"
#include "SensitiveDetector.hh"
#include "G4SDManager.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"


Geometry::Geometry(){
   materi_Man = G4NistManager::Instance();
}

Geometry::~Geometry() {}

G4LogicalVolume* Geometry::constructLogWorld()
{
   G4double leng_X_World = 10.0 * m;
   G4double leng_Y_World = 10.0 * m;
   G4double leng_Z_World = 10.0 * m;
   G4Box* solid_World = new G4Box(
      "Solid_World",
      leng_X_World / 2.0,
      leng_Y_World / 2.0,
      leng_Z_World / 2.0
      );
   G4Material* materi_World = materi_Man->FindOrBuildMaterial("G4_AIR");
   G4LogicalVolume* logVol_World = new G4LogicalVolume(
      solid_World,
      materi_World,
      "LogVol_World");
   logVol_World->SetVisAttributes(G4VisAttributes::GetInvisible());

   return logVol_World;
}

G4LogicalVolume* Geometry::constructPbPixel(G4double leng_X_Pixel, G4double leng_Y_Pixel, G4double leng_Z_Pixel){
   G4VSolid* solid_PbGlassScinti = new G4Box(
      "solid_PbGlassScinti",
      leng_X_Pixel / 2.0,
      leng_Y_Pixel / 2.0,
      leng_Z_Pixel / 2.0
   );
   G4Material* materi_PbGlassScinti = materi_Man->FindOrBuildMaterial("G4_Pb");
   G4LogicalVolume* logVol_PbGlassScinti = new G4LogicalVolume(
      solid_PbGlassScinti,
      materi_PbGlassScinti,
      "LogVol_PbGlassScinti"
   );
   // G4VisAttributes* attr_PbGlassScinti = new G4VisAttributes(true);
   // attr_PbGlassScinti->SetColor(G4Color::Magenta());
   // logVol_PbGlassScinti->SetVisAttributes(attr_PbGlassScinti);

   return logVol_PbGlassScinti;
}

G4VPhysicalVolume* Geometry::Construct()
{
   G4LogicalVolume* logVol_World = constructLogWorld();
   G4double outer_box_length = 10.0 * cm;
   G4double leng_X_Pixel = outer_box_length/ n_split;
   G4double leng_Y_Pixel = outer_box_length/ n_split;
   G4double leng_Z_Pixel = outer_box_length/ n_split;
   G4LogicalVolume* logVol_PbGlassScinti = constructPbPixel(leng_X_Pixel, leng_Y_Pixel, leng_Z_Pixel);

   // sensitive detector set
   SensitiveDetector* sensitiveDetector = new SensitiveDetector("SensitiveDetector");
   logVol_PbGlassScinti->SetSensitiveDetector(sensitiveDetector);
   G4SDManager* SDManager = G4SDManager::GetSDMpointer();
   SDManager->AddNewDetector(sensitiveDetector);

   // placement world
   G4int copyNum_World = 0;
   G4PVPlacement* physVol_World  = new G4PVPlacement(
      G4Transform3D(),
      "PhysVol_World",
      logVol_World,
      0,
      false,
      copyNum_World,
      true
      );

   // placement Pb
   for (G4int ix = 0; ix < n_split; ix++) {
      for (G4int iy = 0; iy < n_split; iy++) {
         for (G4int iz = 0; iz < n_split; iz++) {
            G4double pos_X_Pixel = - outer_box_length / 2.0 + ix * leng_X_Pixel + leng_X_Pixel / 2.0;
            G4double pos_Y_Pixel = - outer_box_length / 2.0 + iy * leng_Y_Pixel + leng_Y_Pixel / 2.0;
            G4double pos_Z_Pixel = - outer_box_length / 2.0 + iz * leng_Z_Pixel + leng_Z_Pixel / 2.0;
            G4int copyNum_Pb = ix * n_split * n_split + iy * n_split + iz + copyNum_Pb_offset;
            new G4PVPlacement(
               G4Transform3D(
                  G4RotationMatrix(),
                  G4ThreeVector(pos_X_Pixel, pos_Y_Pixel, pos_Z_Pixel)
               ),
               "PhysVol_Pb",
               logVol_PbGlassScinti,
               physVol_World,
               false,
               copyNum_Pb,
               true
            );
         }
      }
   }
   
   return physVol_World;
}
