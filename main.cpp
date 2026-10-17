#define NOMINMAX
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <optional>

constexpr float PI = 3.14159265358979323846f;
constexpr unsigned int WINDOW_WIDTH = 1000;
constexpr unsigned int WINDOW_HEIGHT = 600;

int main()
{
    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "SFML - Particle Under Periodic Force (Variant 24)");
    window.setFramerateLimit(60);

    float mass = 1.0f;
    float forceAmplitude = 300.0f;
    float frequency = 0.5f;
    
    float positionX = 100.0f;
    float positionY = 300.0f;
    float velocityX = 0.0f;
    float totalTime = 0.0f;

    bool isPaused = false;

    sf::CircleShape particle(15.0f);
    particle.setFillColor(sf::Color::Cyan);
    particle.setOrigin({15.0f, 15.0f});
    particle.setPosition({positionX, positionY});

    sf::RectangleShape axis({static_cast<float>(WINDOW_WIDTH), 2.0f});
    axis.setFillColor(sf::Color(100, 100, 100));
    axis.setPosition({0.0f, positionY});

    std::vector<sf::Vector2f> trajectoryPoints;
    sf::VertexArray trajectory(sf::PrimitiveType::LineStrip);

    sf::Font font;
    bool isFontLoaded = font.openFromFile("../resources/fonts/arial.ttf");

    sf::Text infoText(font, "", 18);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition({20.f, 20.f});

    sf::Clock clock;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Space)
                {
                    isPaused = !isPaused;
                }
                if (key->code == sf::Keyboard::Key::R)
                {
                    positionX = 100.0f;
                    velocityX = 0.0f;
                    totalTime = 0.0f;
                    trajectoryPoints.clear();
                }
                if (key->code == sf::Keyboard::Key::Up)
                {
                    forceAmplitude += 50.0f;
                }
                if (key->code == sf::Keyboard::Key::Down)
                {
                    forceAmplitude -= 50.0f;
                }
                if (key->code == sf::Keyboard::Key::Right)
                {
                    frequency += 0.1f;
                }
                if (key->code == sf::Keyboard::Key::Left)
                {
                    frequency -= 0.1f;
                }
            }
        }

        // Обмеження значень без стандартних std::min/max
        if (forceAmplitude < 0.0f) forceAmplitude = 0.0f;
        if (forceAmplitude > 2000.0f) forceAmplitude = 2000.0f;
        if (frequency < 0.05f) frequency = 0.05f;
        if (frequency > 5.0f) frequency = 5.0f;

        if (!isPaused)
        {
            totalTime += dt;

            float omega = 2.0f * PI * frequency;
            float currentForce = forceAmplitude * std::sin(omega * totalTime);

            float acceleration = currentForce / mass;

            velocityX += acceleration * dt;
            positionX += velocityX * dt;

            if (positionX <= 15.0f)
            {
                positionX = 15.0f;
                velocityX = -velocityX;
            }
            else if (positionX >= WINDOW_WIDTH - 15.0f)
            {
                positionX = WINDOW_WIDTH - 15.0f;
                velocityX = -velocityX;
            }

            particle.setPosition({positionX, positionY});

            trajectoryPoints.push_back({positionX, positionY});
            if (trajectoryPoints.size() > 600)
            {
                trajectoryPoints.erase(trajectoryPoints.begin());
            }
        }

        trajectory.clear();
        for (const auto& pt : trajectoryPoints)
        {
            sf::Vertex vertex;
            vertex.position = pt;
            vertex.color = sf::Color(255, 255, 0, 180);
            trajectory.append(vertex);
        }

        if (isFontLoaded)
        {
            float omega = 2.0f * PI * frequency;
            float currentForce = forceAmplitude * std::sin(omega * totalTime);

            std::string statusStr = isPaused ? "PAUSED" : "RUNNING";
            infoText.setString(
                "Status: " + statusStr + " (Space - Pause/Play, R - Reset)\n" +
                "Amplitude F0: " + std::to_string(static_cast<int>(forceAmplitude)) + " N  [Up/Down]\n" +
                "Frequency f: " + std::to_string(frequency).substr(0, 4) + " Hz  [Left/Right]\n" +
                "Current Force F(t): " + std::to_string(currentForce).substr(0, 6) + " N\n" +
                "Velocity V: " + std::to_string(velocityX).substr(0, 6) + " px/s\n" +
                "Position X: " + std::to_string(positionX).substr(0, 6) + " px\n" +
                "Time t: " + std::to_string(totalTime).substr(0, 5) + " s"
            );
        }

        window.clear(sf::Color(15, 15, 25));

        window.draw(axis);
        window.draw(trajectory);
        window.draw(particle);
        if (isFontLoaded)
        {
            window.draw(infoText);
        }

        window.display();
    }

    return 0;
}