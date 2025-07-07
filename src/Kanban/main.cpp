#include <SFML/Graphics.hpp>

int main() {

    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event{};

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (clock.getElapsedTime().asSeconds() >= 5) {
            window.close();
        }

        window.clear(sf::Color::Black);

        window.display();
    }

    return 0;
}