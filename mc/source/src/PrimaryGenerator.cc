#include "PrimaryGenerator.hh"
#include "G4ParticleGun.hh"

PrimaryGenerator::PrimaryGenerator()
: fpParticleGun{nullptr}
{
  fpParticleGun = new G4ParticleGun();
}

PrimaryGenerator::~PrimaryGenerator()
{
  delete fpParticleGun;
}

void PrimaryGenerator::GeneratePrimaries( G4Event* anEvent )
{
  fpParticleGun->GeneratePrimaryVertex( anEvent );
}