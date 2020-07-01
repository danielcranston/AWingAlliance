#include "behavior/nodes/face_target.h"
#include "geometry.h"
#include "actor/ship.h"

glm::mat3 rotation_between_two_unit_vecs(const glm::vec3& a, const glm::vec3& b)
{
    // https://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d
    glm::vec3 cross = glm::cross(a, b);
    float c = glm::dot(a, b);
    glm::mat3 cross_mat(0, -cross.z, cross.y, cross.z, 0, -cross.x, -cross.y, cross.x, 0);
    // cross_mat << 0, -cross(2), cross(1), cross(2), 0, -cross(0), -cross(1), cross(0), 0;
    glm::mat3 R = glm::mat3(1.0f) + cross_mat + (cross_mat * cross_mat) / (1 + c);
    return R;
}

FaceTarget::FaceTarget(const std::string& name,
                       const BT::NodeConfiguration& config,
                       actor::Ship* ship,
                       const std::function<const actor::Ship*()> get_target_func)
  : RunnableActionNode(name, config), ship(ship), dt(0.001666f), get_target_func(get_target_func)
{
}

BT::NodeStatus FaceTarget::tick()
{
    if (start_time == 0.0f)
        target = get_target_func();

    time_now += dt;
    if (time_now > max_time)
        return BT::NodeStatus::SUCCESS;

    if (target)
    {
        const auto& pos = ship->GetPosition();
        const auto& dir = ship->GetDirection();

        glm::quat quat = geometry::RotationBetweenVectors(dir, target->GetPosition() - pos);
        glm::mat3 R(quat);
        ship->SetDirection(glm::normalize((30 * dt * quat) * dir));
        ship->SetPosition(pos + dir * ship->GetSpeed() * dt);

        float distance = glm::l2Norm(pos - target->GetPosition());
        if (distance < 20.0f)
            return BT::NodeStatus::SUCCESS;
        else
            return BT::NodeStatus::RUNNING;
    }
    else
    {
        throw BT::RuntimeError("FaceTarget: Accessing ShipController::GetTarget() return nullptr");
    }
}

BT::NodeBuilder FaceTarget::Builder(actor::Ship* ship,
                                    const std::function<const actor::Ship*()> get_target_func)
{
    return [ship, get_target_func](const std::string& name, const BT::NodeConfiguration& config) {
        return std::make_unique<FaceTarget>(name, config, ship, get_target_func);
    };
}
