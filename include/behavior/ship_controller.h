#pragma once

#include "behavior/nodes.h"

// Forward declaration of actor::Ship. Is there really no other way ?
namespace actor
{
class Ship;
}

class Behavior;

class ShipController
{
  public:
    explicit ShipController(const actor::Ship* ship);
    void Tick();

  private:
    BT::Tree tree;
};
