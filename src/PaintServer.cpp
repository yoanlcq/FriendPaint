#include <cstring>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "PaintEngine.hpp"
#include "GUI.hpp"	                            

int main(void) {
    std::string info_srv, dialog_srv, new_value;
    ServerInfo info;
    sf::Int32 key, value;

    info_srv = "FriendPaintServer version " + ((std::string) (info.getVersion())) + "\n"
	+ "1) Description length   : " + std::to_string(info.getMaxLobbyDescriptionLength()) + "\n"
	+ "2) Max. number of slots : " + std::to_string(info.getMaxLobbySlots()) + "\n"
	+ "3) Max. title length    : " + std::to_string(info.getMaxTitleLength()) + "\n"
	+ "4) Max. password length : " + std::to_string(info.getMaxPasswordLength()) + "\n"
	+ "5) Max. username length : " + std::to_string(info.getMaxUsernameLength());
    std::cout << info_srv << "\nKeep these settings ? (y/n) ";
    std::getline(std::cin, dialog_srv);
    	                        		
    if (strncmp(dialog_srv.c_str(), "n", 1) == 0) {
        do {
	    std::cout << "Which parameter do you wish to change ?" << "\n" << "(Give its number, -1 to finish)" << "\n";
	    std::getline(std::cin,dialog_srv);
	    key = strtol(dialog_srv.c_str(), NULL, 0);
	    if (key>0 && key<=5) {
		std::cout << "Give the new value : " << "\n";
		std::getline(std::cin, new_value);
		value = strtol(new_value.c_str(), NULL, 0);
		switch(key) {
		case 1: info.setMaxLobbyDescriptionLength(value); break;
		case 2: info.setMaxLobbySlots(value); break;
		case 3: info.setMaxTitleLength(value); break;
		case 4: info.setMaxPasswordLength(value); break;
		case 5: info.setMaxUsernameLength(value); break;
		}
		info_srv = "FriendPaintServer version " + ((std::string) (info.getVersion())) + "\n"
		    + "1) Description length   : " + std::to_string(info.getMaxLobbyDescriptionLength()) + "\n"
		    + "2) Max. number of slots : " + std::to_string(info.getMaxLobbySlots()) + "\n"
		    + "3) Max. title length    : " + std::to_string(info.getMaxTitleLength()) + "\n"
		    + "4) Max. password length : " + std::to_string(info.getMaxPasswordLength()) + "\n"
		    + "5) Max. username length : " + std::to_string(info.getMaxUsernameLength());
		std::cout << info_srv << "\n";
	    }
	} while(key>0 && key<=5);
    }
    info.setPort(FP_DEFAULT_PORT);
    Server::serverLoop(&info);
    return 0;
}
