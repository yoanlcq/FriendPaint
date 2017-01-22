#include <cstdlib>
#include <iostream>
#include <SFML/Graphics.hpp>

#include "Layer.hpp"

using namespace std;


Layer::Layer(String new_title) : title(new_title), image(0) {
}


void Layer::setTitle(sf::String set_new_title) {
	title = set_new_title;
}

String Layer::getTitle() {
	return title;
}

void applyBrush(sf::Vector2u from, sf::Vector2u to, sf::Image brush, bool erase_mode) {}