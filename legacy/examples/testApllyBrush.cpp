#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Test image.copy()");
    sf::Event event;


    sf::Image brush, image;
    sf::Vector2u from(10,10);
    sf::Vector2u to(500,500);
    sf::Texture texture;
    sf::Sprite sprite;

    if(image.loadFromFile("./sw.jpg"))
        std::cout << "ok" << std::endl;
    brush.create(50, 50, sf::Color(255, 0, 0, 128));

    // function
    sf::Uint32 w = image.getSize().x;
    sf::Uint32 h = image.getSize().y;
    sf::Int32 delta_x(to.x - from.x);
    // if from.x == to.x, then it does not matter what we set here
    sf::Int8 const ix((delta_x > 0) - (delta_x < 0));
    delta_x = std::abs(delta_x) << 1;
 
    sf::Int32 delta_y(to.y - from.y);
    // if from.y == to.y, then it does not matter what we set here
    sf::Int8 const iy((delta_y > 0) - (delta_y < 0));
    delta_y = std::abs(delta_y) << 1;
 
    if(from.x < w && from.y < h)
    image.copy(brush, from.x, from.y, sf::IntRect(0, 0, 0, 0), true);
 
    if (delta_x >= delta_y) {
        // error may go below zero
        sf::Int32 error(delta_y - (delta_x >> 1));
 
        while (from.x != to.x) {
            if ((error >= 0) && (error || (ix > 0))) {
                error -= delta_x;
                from.y += iy;
            }
            // else do nothing
 
            error += delta_y;
            from.x += ix;
 
            if(from.x < w && from.y < h)
                image.copy(brush, from.x, from.y, sf::IntRect(0, 0, 0, 0), true);
        }
    } else {
        // error may go below zero
        sf::Int32 error(delta_x - (delta_y >> 1));
 
        while (from.y != to.y) {
            if ((error >= 0) && (error || (iy > 0))) {
                error -= delta_y;
                from.x += ix;
            }
            // else do nothing
 
            error += delta_x;
            from.y += iy;
 
            if(from.x < w && from.y < h)
                image.copy(brush, from.x, from.y, sf::IntRect(0, 0, 0, 0), true);
        }
    }

    texture.loadFromImage(image);
    sprite.setTexture(texture, true);

    // end function

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
