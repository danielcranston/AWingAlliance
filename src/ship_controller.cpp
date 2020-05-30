#include <iostream>
#include <memory>
#include "ship_controller.h"
#include <actor/ship.h>

class SayHello : public BT::SyncActionNode
{
  public:
    SayHello(const std::string& name, const BT::NodeConfiguration& config, const std::string& msg)
      : BT::SyncActionNode(name, {}), msg(msg)
    {
    }

    // You must override the virtual function tick()
    BT::NodeStatus tick()
    {
        std::cout << "SayHello (" << this->name() << "): " << msg << std::endl;
        return BT::NodeStatus::SUCCESS;
    };

  private:
    std::string msg;
};

ShipController::ShipController()
{
    BT::BehaviorTreeFactory factory;
    BT::NodeBuilder builder_A = [](const std::string& name, const BT::NodeConfiguration& config) {
        return std::make_unique<SayHello>(name, config, "This went msg through the constructor");
    };

    factory.registerBuilder<SayHello>("SayHello", builder_A);
    tree = factory.createTreeFromFile("./behavior1.xml");
}

void ShipController::Tick(const actor::Ship* ship)
{
    tree.tickRoot();
}