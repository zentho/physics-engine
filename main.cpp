#include <iostream>
#include <SFML/Graphics.hpp>
#include "solver.hpp"
#include "renderer.hpp"
#include "number_generator.hpp"
#include "math.hpp"

void initializeSolver(Solver& solver, int32_t window_width, int32_t window_height, uint32_t frame_rate)
{
    solver.setConstraint({static_cast<float>(window_width) * 0.5f, static_cast<float>(window_height) * 0.5f}, 450.0f);
    solver.setSubStepsCount(8);
    solver.setSimulationUpdateRate(frame_rate);
}

bool handleEvents(sf::RenderWindow& window)
{
    sf::Event event{};
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
            return false;
        }
    }
    return true;
}

void spawnObject(Solver& solver, sf::Clock& clock, float object_spawn_delay, sf::Vector2f position, float min_radius, float max_radius, float speed, float max_angle)
{
    if (solver.getObjectsCount() < 1000 && clock.getElapsedTime().asSeconds() >= object_spawn_delay) {
        clock.restart();
        auto& object = solver.addObject(position, RNGf::getRange(min_radius, max_radius));
        float t = solver.getTime();
        float angle = max_angle * sin(t) + Math::PI * 0.5f;
        solver.setObjectVelocity(object, speed * sf::Vector2f{cos(angle), sin(angle)});
        object.color = sf::Color::White; 
    }
}

int32_t main(int32_t, char*[])
{
    constexpr int32_t window_width  = 1000;
    constexpr int32_t window_height = 1000;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Verlet", sf::Style::Default, settings);
    const uint32_t frame_rate = 60;
    window.setFramerateLimit(frame_rate);

    Solver solver;
    Renderer renderer{window};
    initializeSolver(solver, window_width, window_height, frame_rate);

    const float        object_spawn_delay    = 0.025f;
    const float        object_spawn_speed    = 1200.0f;
    const sf::Vector2f object_spawn_position = {500.0f, 200.0f};
    const float        object_min_radius     = 1.0f;
    const float        object_max_radius     = 20.0f;
    const float        max_angle             = 1.0f;

    sf::Clock clock;
    while (window.isOpen()) {
        if (!handleEvents(window)) break;

        spawnObject(solver, clock, object_spawn_delay, object_spawn_position, object_min_radius, object_max_radius, object_spawn_speed, max_angle);

        solver.update();
        window.clear(sf::Color::Black);
        renderer.render(solver);
        window.display();
    }

    return 0;
}