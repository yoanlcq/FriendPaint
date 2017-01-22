#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <SFML/Network.hpp>


#define GLOBAL_PORT 64646

class User {
private:

public:
    std::string name;
    sf::TcpSocket socket;    
};

enum P2P_message_type {P2P_INIT=0, P2P_MSG=1, P2P_END=2};

class P2PServer {
private:
    P2PServer() {};
    static sf::TcpListener listener;
    static std::vector<User*> users;
    static sf::SocketSelector selector;
    static sf::Packet packet;

public:
    static unsigned short port;
    
    static bool mainLoop() {
        listener.listen(port);
        selector.add(listener);
        while (true) {
            std::cout << "Attente de donnees..." << std::endl;
            if (selector.wait()) {
            /* Quoi de neuf ? */
                if (selector.isReady(listener)) {
                //std::cout << "Debug: Listener !" << std::endl;
                /* C'est une nouvelle connexion ! */
                    User* new_user = new User();
                    if (listener.accept(new_user->socket) == sf::Socket::Done) {
                        /* On l'ajoute à la liste des clients, pour pouvoir lui envoyer des données. */
                        users.push_back(new_user);
                        /* On l'ajoute au sélecteur, pour pouvoir recevoir ses données. */
                        selector.add(new_user->socket);
                        std::cout << "Nouvelle connexion entrante !" << std::endl;
                    } else {
                        /* On n'a pas pu accepter la connexion ! */
                        std::cerr << "N'a pas pu accepter une connexion entrante !" << std::endl;
                        delete new_user;
                    }
                } else { 
                //std::cout << "Debug: Message !" << std::endl;
                /* C'est un message d'un de nos clients ! Mais lequel ? */    
                    for (std::vector<User*>::iterator it = users.begin(); it != users.end(); ++it) {
                        User *user = *it;
                        if (selector.isReady(user->socket)) {
                            /* C'est ce client là ! Recevons ses données. */
                            packet.clear();
                            if (user->socket.receive(packet) == sf::Socket::Done) {
                                int msg_type;
                                std::string msg;
                                char answer;
                                packet >> msg_type >> msg;
                                switch(msg_type) {
                                case P2P_INIT: 
                                    user->name = msg;
				    std::cout << "Depuis " << user->socket.getRemoteAddress().toString() << ", ";
                                    std::cout << msg << " souhaite rejoindre le salon. Accepter ? (y/n) ";
                                    std::cin >> answer;
                                    std::getline(std::cin, msg);
                                    packet.clear();
                                    if (answer!='y') {
                                        std::cout << "Vous pouvez entrer un motif pour ce refus." << std::endl << "> ";
                                        std::getline(std::cin, msg);
                                        packet << P2P_END << msg;
                                        user->socket.send(packet);
                                        selector.remove(user->socket);
                                        user->socket.disconnect();
                                    } else {
                                        packet << P2P_INIT << "";
                                        user->socket.send(packet);
                                    }
                                    break;
                                case P2P_MSG: 
                                    if(msg != "") {
                                        msg = user->name + " a dit : " + msg;
                                        std::cout << msg << std::endl;
                                        packet.clear();
                                        packet << P2P_MSG << msg;
                                        for (std::vector<User*>::iterator it2 = users.begin(); it2 != users.end(); ++it2) {
                                            if(*it2 != user) {
                                                (*it2)->socket.send(packet);
                                            }
                                        }
                                    }
                                    break;
                                case P2P_END: 
                                    std::cout << user->name << " a quitte le salon." << std::endl;
                                    selector.remove(user->socket);
                                    user->socket.disconnect();
                                    break;
                                }
                                
                                /* Effacer puis sortir de la boucle. Il faut invoquer un break, mais pas dans le switch. */
                                /* Ne pas toucher. */
                                if((msg_type==P2P_INIT && answer!='y') || msg_type == P2P_END) {
                                    users.erase(it);
                                    delete user;
                                    break;
                                }
                            } else {
                                std::cout << user->name << " a tue son programme comme un gros sauvage." << std::endl;
                                selector.remove(user->socket);
                                user->socket.disconnect();
                                users.erase(it);
                                delete user;
                                break;
                            }
                        }
                    } /* FOR */
                } /* nouvelle connexion/message client */
            } else {
                std::cout << "Debug: Le selecteur retourne false." << std::endl;
            }
        } /* WHILE */
        return true;
    } /* FUNCTION */
};

sf::TcpListener P2PServer::listener;
std::vector<User*> P2PServer::users;
sf::SocketSelector P2PServer::selector;
sf::Packet P2PServer::packet;

unsigned short P2PServer::port = GLOBAL_PORT;



class P2PClient {
private:
    P2PClient() {};
    static sf::TcpSocket socket;
    static sf::Packet packet;

public:
    static std::string address;
    static unsigned short port;
    static std::string name;
    
    static bool mainLoop() {
        int msg_type;
        std::string msg;
        bool exitting;
        
        std::cout << "Tentative de connexion..." << std::endl;
        if(socket.connect(address, port) != sf::Socket::Done) {
            std::cerr << "N'a pas pu se connecter a " << address << ':' << port << '.' << std::endl;
            return false;
        }
        std::cout << "Demande de rejoindre le salon..." << std::endl;
        msg = name;
        packet << P2P_INIT << msg;
        if(socket.send(packet) != sf::Socket::Done) {
            std::cerr << "Le serveur est mort." << std::endl;
            socket.disconnect();
            return false;
        }
        std::cout << "Attente de la decision de l'hote..." << std::endl;
        packet.clear();
        if(socket.receive(packet) != sf::Socket::Done) {
            std::cerr << "Le serveur est mort." << std::endl;
            socket.disconnect();
            return false;
        }
        packet >> msg_type >> msg;
        if(msg_type == P2P_END) {
            std::cout << "L'entree au salon vous a ete refusee." << std::endl
                    << "Motif : \"" << msg << "\"." << std::endl;
            socket.disconnect();
            return false;
        }
        
        std::cout << std::endl << std::endl 
                << "Vous etes a present dans le salon. Commencez a discuter !" << std::endl
                << "(Entrez 'x', 'exit' ou 'quit' pour quitter)" << std::endl
                << "(NOTER : Vous devez appuyer sur Entree pour actualiser les messages)" << std::endl
                << "(Ce probleme sera resolu avec les threads)" << std::endl
                << std::endl;
        
        do {
            socket.setBlocking(false);
            while(socket.receive(packet) == sf::Socket::Done) {
                packet >> msg_type >> msg;
                switch(msg_type) {
                case P2P_MSG: 
                    std::cout << msg << std::endl;
                    break;
                case P2P_END: 
                    std::cout << "Le salon vous a kick." << std::endl
                        << "Motif : \"" << msg << "\"." << std::endl;
                    break;
                }
            }
            socket.setBlocking(true);
            
            msg = name;
            std::cout << msg << ": ";
            std::getline(std::cin, msg);
            if(msg != "") {
                exitting = (msg=="x"||msg=="x"||msg=="exit"||msg=="quit");
                packet.clear();
                packet << (exitting ? P2P_END : P2P_MSG) << msg;
                if(socket.send(packet) != sf::Socket::Done) {
                    std::cerr << "Le serveur est mort." << std::endl;
                    exitting = true;
                }
            }
        } while(!exitting);
        
        socket.disconnect();
        return true;
    } /* FUNCTION */
};

sf::TcpSocket P2PClient::socket;
sf::Packet P2PClient::packet;

std::string P2PClient::address, P2PClient::name;
unsigned short P2PClient::port = GLOBAL_PORT;

int main(int argc, char *argv[]) {
    if(argc <= 1) {
        std::cout << "Mode serveur actif." << std::endl
                << "(Mode client : entrer l'adresse et le pseudo sur la ligne de commande)." << std::endl;
	std::cout << "Votre adresse locale est " << sf::IpAddress::getLocalAddress().toString() << "." << std::endl;
	std::cout << "Votre adresse publique est " << sf::IpAddress::getPublicAddress().toString() << " (Note : Ce n'est pas forcement celle de votre machine)." << std::endl;
	std::cout << "Le serveur ecoute le port " << GLOBAL_PORT << "." << std::endl;
        P2PServer::mainLoop();
    } else {
        P2PClient::address = argv[1];
        P2PClient::name = (argc > 2 ? argv[2] : "Anonymous");
        P2PClient::mainLoop();
    }
    
    exit(EXIT_SUCCESS);
}
