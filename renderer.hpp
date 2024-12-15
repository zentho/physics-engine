#include <SFML/Graphics.hpp>
#include "solver.hpp"

class Renderer {
public:
    explicit Renderer(sf::RenderTarget& target)
        : m_target(target) {}

    void render(const Solver& solver) const {
        drawConstraint(solver.getConstraint());
        drawObjects(solver.getObjects());
    }

private:
    sf::RenderTarget& m_target;

    void drawConstraint(const sf::Vector3f& constraint) const {
        sf::RectangleShape boundary;
        boundary.setSize(sf::Vector2f(800.0f, 800.0f)); 
        boundary.setOrigin(boundary.getSize() / 2.0f);
        boundary.setPosition(constraint.x, constraint.y);
        boundary.setFillColor(sf::Color::Transparent);
        boundary.setOutlineColor(sf::Color::White);
        boundary.setOutlineThickness(2.0f);
        m_target.draw(boundary);
    }

    void drawObjects(const std::vector<VerletObject>& objects) const {
        sf::CircleShape objectShape(1.0f);
        objectShape.setPointCount(32);
        objectShape.setOrigin(1.0f, 1.0f);

        for (const auto& obj : objects) {
            objectShape.setPosition(obj.position);
            objectShape.setScale(obj.radius, obj.radius);
            objectShape.setFillColor(obj.color);
            m_target.draw(objectShape);
        }
    }
};