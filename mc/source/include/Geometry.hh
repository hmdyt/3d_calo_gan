#ifndef Geometry_h
#define Geometry_h 1

#include "G4VUserDetectorConstruction.hh"
class G4VPhysicalVolume;
class G4LogicalVolume;
class G4NistManager;


class Geometry : public G4VUserDetectorConstruction
{
  public:
    Geometry();
    ~Geometry();

    G4VPhysicalVolume* Construct();

    constexpr static G4double n_split = 25; // means 25*25*25 = 15625 pixels
    constexpr static G4int copyNum_Pb_offset = 1000;

  private:
    G4NistManager* materi_Man;

    G4LogicalVolume* constructLogWorld();
    G4LogicalVolume* constructPbPixel(G4double, G4double, G4double);

};
#endif
