#include <iostream>
#include <memory>
#include "ship_controller.h"
#include <actor/ship.h>

class IsAlive : public BT::SyncActionNode
{
  public:
    IsAlive(const std::string& name, const BT::NodeConfiguration& config, const actor::Ship* ship)
      : BT::SyncActionNode(name, config), ship(ship)
    {
    }

    BT::NodeStatus tick()
    {
        if (ship->GetHealth() > 0)
            return BT::NodeStatus::SUCCESS;
        else
            return BT::NodeStatus::FAILURE;
    };

  private:
    const actor::Ship* ship;
};

class Tumble : public BT::LeafNode
{
  public:
    Tumble(const std::string& name, const BT::NodeConfiguration& config, actor::Ship* ship)
      : BT::LeafNode(name, config), ship(ship), dt(0.001666f)
    {
    }

    BT::NodeStatus tick()
    {
        if (time_since_death == 0.0f)
            dying_dir = ship->GetDirection();

        time_since_death += dt;
        float roll = 2.0f * time_since_death * glm::pi<float>();
        ship->SetRoll(roll);
        glm::mat4 anim = glm::rotate(glm::mat4(1.0f), 0.1f * roll, glm::vec3(0, 1, 0)) *
                         glm::rotate(glm::mat4(1.0f), 0.5f * roll, glm::vec3(1, 0, 0));
        ship->SetDirection(glm::mat3(anim) * glm::vec3(0, 0, -1));
        ship->SetPosition(ship->GetPosition() + ship->GetSpeed() * dying_dir * dt);

        return BT::NodeStatus::RUNNING;
    };

    virtual void halt() override final
    {
        setStatus(BT::NodeStatus::IDLE);
    }

    virtual BT::NodeType type() const override final
    {
        return BT::NodeType::ACTION;
    }

  private:
    actor::Ship* ship;
    glm::vec3 dying_dir;
    float time_since_death = 0.0f;
    float dt;
};

ShipController::ShipController(const actor::Ship* ship)
{
    // Avert your eyes while I cast away const. I want Ship to own its controller, but also for the
    // controller to be able to access the non-const API of Ship so it can update its pose etc..
    actor::Ship* non_const_ship = const_cast<actor::Ship*>(ship);
    BT::BehaviorTreeFactory factory;
    BT::NodeBuilder builder_IsAlive = [non_const_ship](const std::string& name,
                                                       const BT::NodeConfiguration& config) {
        return std::make_unique<IsAlive>(name, config, non_const_ship);
    };
    BT::NodeBuilder builder_Tumble = [non_const_ship](const std::string& name,
                                                      const BT::NodeConfiguration& config) {
        return std::make_unique<Tumble>(name, config, non_const_ship);
    };

    factory.registerBuilder<IsAlive>("IsAlive", builder_IsAlive);
    factory.registerBuilder<IsAlive>("Tumble", builder_Tumble);
    tree = factory.createTreeFromFile("./behavior1.xml");
}

void ShipController::Tick()
{
    tree.tickRoot();
}