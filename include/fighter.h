#pragma once

#include <actor.h>
#include <spline.h>

namespace actor
{
    class Fighter : public Actor
    {
    public:
        Fighter(const glm::vec3& p,
                const glm::vec3& d,
                const std::vector<std::pair<Model*, glm::mat4>>& part_vector
                );
        void Draw(glm::mat4 camprojMat) override;

        void Update(const float speedChange, const float turnChange, const float dt);
        void Update(const std::bitset<8>& keyboardInfo, float dt);
        void Update_Roaming(float t);
        void Fire();

        bool bDrawBBox;
        bool bDrawSpline;
    private:
        Spline s;
        glm::vec3 color = {0.7, 0.0, 0.0};


        float currentSpeed;
        float currentTurnSpeed;
        const float maxSpeed;
        const float maxTurnSpeed;
    };

}