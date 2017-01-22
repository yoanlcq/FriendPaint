#include <SFML/Graphics.hpp>
#include "prisma.h"
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <string>
#include "PaintEngine.hpp"
#include "GUI.hpp"
#include "interface.hpp"


int main(int argc, char *argv[]) {
    sf::RenderWindow window;
    sf::Image icon;
    sf::Event event;
    sf::Vector2u win_size(740, 500);
    bool fullscreen;
   
    buildFriendPaintInterface(&window, &gui, win_size);

    fullscreen = false;
    icon.loadFromFile("../../data/icons/friendpaint.png");
    
    window.create(sf::VideoMode(win_size.x, win_size.y), "FriendPaint");
    window.setPosition(sf::Vector2i(0, 0));
    window.setIcon(32, 32, icon.getPixelsPtr());
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    
    while (window.isOpen()) {
	{
	    sf::Lock lock(Client::session.getGuiMutex());
	    window.clear(sf::Color::Black);
	    window.draw(gui);
	    window.display();
	}
	while (window.pollEvent(event)) {
	    {
		sf::Lock lock(Client::session.getGuiMutex());
		gui.handleEvent(event);
		
		switch(event.type) {
		case sf::Event::Resized:
		    window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
		    break;
		case sf::Event::Closed:
		    Client::endSession();
		    Client::endOfflineServerThread();
		    std::cout << "FriendPaint closed properly." << std::endl;
		    window.close();
		    break;
		case sf::Event::KeyPressed:
		    /* Fullscreen control */
		    if(!fullscreen && event.key.code == sf::Keyboard::F11) {
			window.create(sf::VideoMode::getFullscreenModes()[0], "FriendPaint", sf::Style::Fullscreen);
			window.setIcon(32, 32, icon.getPixelsPtr());
			window.setFramerateLimit(60);
			window.setVerticalSyncEnabled(true);
			window.setView(sf::View(sf::FloatRect(0, 0, sf::VideoMode::getFullscreenModes()[0].width, sf::VideoMode::getFullscreenModes()[0].height)));
			event.type = sf::Event::Resized;
			event.size.width = sf::VideoMode::getFullscreenModes()[0].width;
			event.size.height = sf::VideoMode::getFullscreenModes()[0].height;
			gui.handleEvent(event);
			fullscreen = true;
		    } else if(fullscreen && event.key.code == sf::Keyboard::F11) {
			window.create(sf::VideoMode(win_size.x, win_size.y), "FriendPaint");
			window.setIcon(32, 32, icon.getPixelsPtr());
			window.setFramerateLimit(60);
			window.setVerticalSyncEnabled(true);
			window.setView(sf::View(sf::FloatRect(0, 0, win_size.x, win_size.y)));
			event.type = sf::Event::Resized;
			event.size.width = win_size.x;
			event.size.height = win_size.y;
			gui.handleEvent(event);
			fullscreen = false;
		    }
		    break;
		}
	    }
	}
    }
    exit(EXIT_SUCCESS);
}
