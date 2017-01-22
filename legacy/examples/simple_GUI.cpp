#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <iostream>
#include "GUI.hpp"

int main(int argc, char *argv[]) {
    sf::RenderWindow window;
    sf::Event event;
    sf::Vector2u win_size(400, 400);
    sf::RectangleShape theme;
    WidgetContainer gui;
    TextContainer chat;

    //On change la couleur du RectangleShape de gui (Conteneur racine)
    theme.setPosition(sf::Vector2f(0, 0));
    theme.setFillColor(sf::Color(32, 32, 32));
    gui.setRectangleShape(theme);

    //Lui aussi, on lui change sa couleur
    chat.getRectangleShape().setFillColor(sf::Color(16, 16, 16));
    //On précise les Insets de notre widget.
    chat.setInsets(sf::FloatRect(0,-100,0,0));
    
    //On n'oublie pas de l'ajouter
    gui.add(&chat);
    //On doit dire au conteneur racine la taille de la fenêtre (ça va générer un faux évènement "resized")
    gui.tellWindowSize(win_size);

    window.create(sf::VideoMode(win_size.x, win_size.y), "FriendPaint");

    chat.println(TextContainer::ITALIC + "Welcome to FriendPaint!" + TextContainer::RESET);
    chat.println(TextContainer::RGB(255, 0, 0)     + "This" 
		 + TextContainer::RGB(255, 255, 0) + " text" 
		 + TextContainer::RGB(0, 255, 0)   + " is" 
		 + TextContainer::RGB(0, 255, 255) + " so" 
		 + TextContainer::RGB(0, 0, 255)   + " pretty"
		 + TextContainer::RESET);

    while (window.isOpen()) {
	window.clear(sf::Color::Black);
	//En dessinant le conteneur racine, on dessine tous ses fils, et donc toute l'interface.
	window.draw(gui);
	window.display();
        while (window.pollEvent(event)) {
  	    //On donne l'évènement au conteneur racine, qu va le transmettre à ses fils.
	    gui.handleEvent(event);
	    switch(event.type) {
	    case sf::Event::Resized:
		window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
		break;
	    case sf::Event::Closed:
                window.close();
		break;
	    }
        }
    }
    exit(EXIT_SUCCESS);
}
