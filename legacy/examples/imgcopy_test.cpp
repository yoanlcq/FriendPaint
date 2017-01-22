#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(100, 100), "Test image.copy()");
    sf::Event event;
    // Load a sprite to display
    sf::Image above, below;
    sf::Texture texture;
    sf::Sprite sprite;

    above.create(100, 100, sf::Color(255, 0, 0, 128));
    below.create(100, 100, sf::Color(0, 255, 0, 128));
    below.copy(above, 0, 0, sf::IntRect(0, 0, 0, 0), true);
    texture.loadFromImage(below);
    sprite.setTexture(texture, true);

    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color::White);
        window.draw(sprite);
        window.display();
    }
    return EXIT_SUCCESS;
}
