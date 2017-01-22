#ifndef DEF_LAYER
#define DEF_LAYER

#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>

class Layer {
    public:
	    Layer(sf::String new_title);

	    void setTitle(sf::String set_new_title);
	    sf::String getTitle();

	    void applyBrush(sf::Vector2u from, sf::Vector2u to, sf::Image brush, bool erase_mode);

    private:
    	sf::String title;
    	sf::Image image;
};

#endif
