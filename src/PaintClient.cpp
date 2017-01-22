#include <iostream>
#include <SFML/Network.hpp>

const int PORT = 55555;
const sf::IpAddress IP = sf::IpAddress::getLocalAddress();
sf::Mutex mutex;
sf::TcpSocket socket;
bool isMute;

enum Indicator : sf::Uint32 {
    FP_INIT = 0,
	FP_OK = 1,
	FP_DENIED = 2,
	FP_CREATE_LOBBY = 3,
	FP_JOIN_LOBBY = 4,
	FP_CANVAS = 5,
	FP_DONE = 6,
	FP_SAY = 7,
	FP_GRANT_ADMIN = 8,
	FP_GRANT_HOST = 9,
	FP_EXIT = 10,
	FP_LOCK_CREATE_LAYER = 11,
	FP_UNLOCK_CREATE_LAYER = 12,
	FP_SET_TOOL_TYPE = 13,
	FP_SET_TOOL_COLOR = 14,
	FP_SET_TOOL_DIAMETER = 15,
	FP_BEGIN_TOOL_DRAG = 16,
	FP_STEP_TOOL_DRAG = 17,
	FP_END_TOOL_DRAG = 18,
	FP_CREATE_LAYER = 19,
	FP_RENAME_LAYER = 20,
	FP_MOVE_LAYER = 21,
	FP_MERGE_DOWN_LAYER = 22,
	FP_LOCK_EDIT_LAYER = 23,
	FP_UNLOCK_EDIT_LAYER = 24,
	FP_LOCK_DELETE_LAYER = 25,
	FP_UNLOCK_DELETE_LAYER = 26,
	FP_MUTE = 27,   //  PUT OR  
	FP_UNMUTE = 28  //  NOT ?
};


void userAction() {
	while (true) {
		std::string msg;
		std::cout << '>';
		std::getline(std::cin,msg);

		sf::Packet packet;
		packet.clear();
		if (!isMute) {
			if (msg.compare(0,4, "mute") == 0) {
				msg.erase(0,5);
				packet << MUTE << msg;
			} else if (msg.compare(0,6, "unmute") == 0) {
				msg.erase(0,7);
				packet << FP_UNMUTE << msg;
			} else {
				packet << FP_SAY << msg;
			}
			if(socket.send(packet) != sf::Socket::Done) {
            	std::cerr << "Le serveur est mort." << std::endl;
            	socket.disconnect();
        	}
		} else {
			std::cout << "Vous etes mute" << std::endl;
		}
	}

}

void serverAction() {
	std::string msg, pseudo;
	int type;
	sf::Packet packet;

	while(socket.receive(packet) == sf::Socket::Done) {
		if (packet >> type >> msg ) {
			if (type == FP_SAY) {
				std::cout << "\n" << msg << "\n";
			}
			else if (type == MUTE) {
				isMute = true;
				std::cout << "vous etes mute" << "\n";
			}
			else if (type == FP_UNMUTE) {
				isMute = false;
				std::cout << "vous etes unmute" << "\n";
			} else if (type == FP_EXIT) {
				std::cout << "Vous avez ete deco du serveur" << std::endl;
				socket.disconnect();
			}
		}
		packet.clear();
	}
}

int main(void) {
	sf::Packet packet;
	std::string pseudo;
	std::cout << "Entrez votre pseudo : ";
	std::getline (std::cin,pseudo);
	isMute = false;

	if (socket.connect(IP, PORT) != sf::Socket::Done) {
		std::cout << "La connexion a echoué\n";
		return -1;
	}
	std::cout << "Connecté en tant que " << pseudo << "\n";
	packet << P2P_INIT << pseudo;
	socket.send(packet);

	sf::Thread threadUser(&userAction);
	sf::Thread threadServer(&serverAction);

	threadUser.launch();
	threadServer.launch();

	while(true) {
	}
}

