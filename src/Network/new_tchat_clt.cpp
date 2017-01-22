#include <iostream>
#include <SFML/Network.hpp>

const int PORT = 55555;
const sf::IpAddress IP = sf::IpAddress::getLocalAddress();
sf::Mutex mutex;
sf::TcpSocket socket;
bool isMute;

enum P2P_message_type {P2P_INIT=0, P2P_MSG=1, P2P_END=2, MUTE=3, UNMUTE=4};

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
				packet << UNMUTE << msg;
			} else {
				packet << P2P_MSG << msg;
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

	//socket.setBlocking(false);
	while(socket.receive(packet) == sf::Socket::Done) {
		if (packet >> type >> msg ) {
			//std::cout << "on est dedans !\n";
			if (type == P2P_MSG) {
				std::cout << "\n" << msg << "\n";
			}
			else if (type == MUTE) {
				isMute = true;
				std::cout << "vous etes mute" << "\n";
			}
			else if (type == UNMUTE) {
				isMute = false;
				std::cout << "vous etes unmute" << "\n";
			} else if (type == P2P_END) {
				std::cout << "Vous avez ete deco du serveur" << std::endl;
				socket.disconnect();
			}
		}
		packet.clear();
	}
	//socket.setBlocking(true);
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

