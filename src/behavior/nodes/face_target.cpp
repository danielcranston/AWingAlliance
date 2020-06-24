#include "behavior/nodes/face_target.h"
#include "actor/ship.h"

FaceTarget::FaceTarget(const std::string& name,
                       const BT::NodeConfiguration& config,
                       actor::Ship* ship)
  : RunnableActionNode(name, config), ship(ship)
{
}

BT::PortsList FaceTarget::providedPorts()
{
    return { BT::InputPort<const actor::Ship*>("target") };
}

BT::NodeStatus FaceTarget::tick()
{
    BT::Optional<const actor::Ship*> msg = getInput<const actor::Ship*>("target");
    if (msg)
    {
        const auto target = msg.value();
        if (target != ship)
            ship->SetDirection(glm::normalize(target->GetPosition() - ship->GetPosition()));
    }
    else
    {
        throw BT::RuntimeError(msg.error());
    }

    return BT::NodeStatus::RUNNING;
}