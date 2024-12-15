#pragma once
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "math.hpp"


struct VerletObject
{
    sf::Vector2f position;
    sf::Vector2f position_last;
    sf::Vector2f acceleration;
    float        radius        = 10.0f;
    sf::Color    color         = sf::Color::White;

    VerletObject() = default;
    VerletObject(sf::Vector2f position_, float radius_)
        : position{position_}
        , position_last{position_}
        , acceleration{0.0f, 0.0f}
        , radius{radius_}
    {}

    void addVelocity(sf::Vector2f v, float dt)
    {
        position_last -= v * dt;
    }

    void setVelocity(sf::Vector2f v, float dt)
    {
        position_last = position - (v * dt);
    }

    void update(float dt)
    {
        const sf::Vector2f displacement = position - position_last;
        position_last = position;
        position      = position + displacement + acceleration * (dt * dt);

        acceleration  = {};
    }

    void accelerate(sf::Vector2f a)
    {
        acceleration += a;
    }

    [[nodiscard]]
    sf::Vector2f getVelocity(float dt) const
    {
        return (position - position_last) / dt;
    }
};


class Solver
{
public:
    Solver() = default;

    VerletObject& addObject(sf::Vector2f position, float radius)
    {
        return m_objects.emplace_back(position, radius);
    }

    void setSubStepsCount(uint32_t sub_steps)
    {
        m_sub_steps = sub_steps;
    }

    void setObjectVelocity(VerletObject& object, sf::Vector2f v)
    {
        object.setVelocity(v, getStepDt());
    }

    void setSimulationUpdateRate(uint32_t rate)
    {
        m_frame_dt = 1.0f / static_cast<float>(rate);
    }

    void setConstraint(sf::Vector2f position, float radius)
    {
        m_constraint_center = position;
        m_constraint_radius = radius;
    }

    void update()
    {
        m_time += m_frame_dt;
        const float step_dt = getStepDt();
        for (uint32_t i{m_sub_steps}; i--;) {
            applyGravity();
            checkCollisions(step_dt);
            applyConstraint();
            updateObjects(step_dt);
        }
    }

    [[nodiscard]]
    const std::vector<VerletObject>& getObjects() const
    {
        return m_objects;
    }

    [[nodiscard]]
    sf::Vector3f getConstraint() const
    {
        return {m_constraint_center.x, m_constraint_center.y, m_constraint_radius};
    }

    [[nodiscard]]
    uint64_t getObjectsCount() const
    {
        return m_objects.size();
    }

    [[nodiscard]]
    float getTime() const
    {
        return m_time;
    }

    [[nodiscard]]
    float getStepDt() const
    {
        return m_frame_dt / static_cast<float>(m_sub_steps);
    }

private:
    uint32_t                  m_sub_steps          = 1;
    sf::Vector2f              m_gravity            = {0.0f, 1000.0f};
    sf::Vector2f              m_constraint_center;
    float                     m_constraint_radius  = 100.0f;
    std::vector<VerletObject> m_objects;
    float                     m_time               = 0.0f;
    float                     m_frame_dt           = 0.0f;
    sf::FloatRect            m_boundary           = {100.0f, 100.0f, 800.0f, 800.0f};

    void applyConstraint()
    {
        for (auto& obj : m_objects) {
        if (obj.position.x - obj.radius < m_boundary.left) {
            obj.position.x = m_boundary.left + obj.radius;
        }

        if (obj.position.x + obj.radius > m_boundary.left + m_boundary.width) {
            obj.position.x = m_boundary.left + m_boundary.width - obj.radius;
        }

        if (obj.position.y - obj.radius < m_boundary.top) {
            obj.position.y = m_boundary.top + obj.radius;
        }

        if (obj.position.y + obj.radius > m_boundary.top + m_boundary.height) {
            obj.position.y = m_boundary.top + m_boundary.height - obj.radius;
        }
    }
    }

    void updateObjects(float dt)
    {
        for (auto& obj : m_objects) {
            obj.update(dt);
        }
    }

    void applyGravity()
    {
        for (auto& obj : m_objects) {
            obj.accelerate(m_gravity);
        }
    }

    void checkCollisions(float dt)
    {
        const float    response_coef = 0.75f;
        const uint64_t objects_count = m_objects.size();

        for (uint64_t i{0}; i < objects_count; ++i) {
            VerletObject& object_1 = m_objects[i];

            for (uint64_t k{i + 1}; k < objects_count; ++k) {
                VerletObject&      object_2 = m_objects[k];
                const sf::Vector2f v        = object_1.position - object_2.position;
                const float        dist2    = v.x * v.x + v.y * v.y;
                const float        min_dist = object_1.radius + object_2.radius;
                if (dist2 < min_dist * min_dist) {
                    const float        dist  = sqrt(dist2);
                    const sf::Vector2f n     = v / dist;
                    const float mass_ratio_1 = object_1.radius / (object_1.radius + object_2.radius);
                    const float mass_ratio_2 = object_2.radius / (object_1.radius + object_2.radius);
                    const float delta        = 0.5f * response_coef * (dist - min_dist);
                    object_1.position -= n * (mass_ratio_2 * delta);
                    object_2.position += n * (mass_ratio_1 * delta);
                }
            }
        }
    }
};
