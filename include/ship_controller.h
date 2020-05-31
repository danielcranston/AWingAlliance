#pragma once

#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/behavior_tree.h>

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
