#pragma once

#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/behavior_tree.h>

#include "spline.h"

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

    static std::function<const actor::Ship*(const actor::Ship& requester)> GetNewTargetFunc;
    static std::function<glm::vec3(const glm::vec3& center, const glm::vec3& area_size)>
        RandomVecFunc;

    static std::function<float()> GetTimeStepFunc;

    const actor::Ship* GetTarget();
    void SetTarget(const actor::Ship* new_target);

  private:
    BT::Tree tree;
    std::unique_ptr<Spline> spline;
    const actor::Ship* target;
};
