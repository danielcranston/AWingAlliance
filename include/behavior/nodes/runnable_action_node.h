#pragma once

#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/behavior_tree.h>

class RunnableActionNode : public BT::ActionNodeBase
{
  public:
    RunnableActionNode(const std::string& name, const BT::NodeConfiguration& config);
    virtual ~RunnableActionNode() = default;
    virtual void halt() override final;
};
