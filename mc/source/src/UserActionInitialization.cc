#include "UserActionInitialization.hh"
#include "PrimaryGenerator.hh"

UserActionInitialization::UserActionInitialization(){}

UserActionInitialization::~UserActionInitialization(){}

void UserActionInitialization::Build() const
{
  SetUserAction(new PrimaryGenerator());
}
