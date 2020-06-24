#include "behavior/nodes/runnable_action_node.h"

RunnableActionNode::RunnableActionNode(const std::string& name, const BT::NodeConfiguration& config)
  : BT::ActionNodeBase(name, config)
{
}

void RunnableActionNode::halt()
{
    setStatus(BT::NodeStatus::IDLE);
}
