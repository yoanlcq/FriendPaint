#include <iostream>
#include <SFML/Network.hpp>

const int PORT = 55555;
const sf::IpAddress IP = sf::IpAddress::getLocalAddress();

enum P2P_message_type {P2P_INIT=0, P2P_MSG=1, P2P_END=2, MUTE=3, UNMUTE=4};

class User {
    private:

    public:
        std::string name;
        sf::TcpSocket socket;    
};

void serveur() {

    sf::TcpListener listener;
    std::vector<User*> users;
    sf::SocketSelector selector;
    sf::Packet packet;
    std::string pseudo, msg, contenu;
    int msg_type;

    listener.listen(PORT);
    selector.add(listener);

    while(true) {
        std::cout << "Attente de donnees..." << std::endl;

        if (selector.wait()) {
            if (selector.isReady(listener)) {
                User* new_user = new User();
                if (listener.accept(new_user->socket) == sf::Socket::Done) {
                    users.push_back(new_user);
                    selector.add(new_user->socket);
                    std::cout << "Nouvelle connexion entrante !" << std::endl;
                } else {
                    std::cerr << "N'a pas pu accepter une connexion entrante !" << std::endl;
                    delete new_user;
                }
            } else {
                for (std::vector<User*>::iterator it = users.begin(); it != users.end(); ++it) {
                    User *user = *it;
                    if (selector.isReady(user->socket)) {
                        packet.clear();
                        if (user->socket.receive(packet) == sf::Socket::Done) {
                            packet >> msg_type >> contenu;
                            packet.clear();
                            // AJOUTER LES VERROUX
                            if (msg_type == P2P_INIT) { // ENUM ERROR
                                user->name = contenu;
                                std::cout << user->name << " viens de rejoindre le salon" << std::endl;
                                packet << P2P_MSG << "SERVEUR>Bienvenue !";
                                user->socket.send(packet);
                            }
                            else if (msg_type == P2P_MSG) { // ENUM ERROR
                                std::cout << user->name << '>' << contenu << std::endl;
                                packet.clear();
                                packet << P2P_MSG << user->name+'>'+contenu;
                                for (std::vector<User*>::iterator it2 = users.begin(); it2 != users.end(); ++it2) {
                                    if(*it2 != user) {
                                        (*it2)->socket.send(packet);
                                    }
                                }
                            }
                            else if (msg_type == P2P_END) {
                                std::cout << user->name << " a quitte le salon." << std::endl;
                                selector.remove(user->socket);
                                user->socket.disconnect();
                                break;
                            }
                            else if (msg_type == MUTE) {
                                std::cout << user->name << " mute " << contenu << "\n";
                                packet.clear();
                                packet << MUTE << "";
                                for (int i=0; i<users.size(); i++) {
                                    if (users[i]->name == contenu) {
                                        users[i]->socket.send(packet);
                                    }
                                    std::cout << i <<users[i]->name << std::endl;
                                }
                                packet.clear();
                                packet << P2P_MSG << "SERVER>vous avez mute "+contenu;
                                user->socket.send(packet);
                            }
                            else if (msg_type == UNMUTE) {
                                std::cout << user->name << " unmute " << contenu << "\n";
                                packet.clear();
                                packet << UNMUTE << "";
                                for (int i=0; i<users.size(); i++) {
                                    if (users[i]->name == contenu) {
                                        users[i]->socket.send(packet);
                                    }
                                    std::cout << i <<users[i]->name << std::endl;
                                }
                                packet.clear();
                                packet << P2P_MSG << "SERVER>vous avez unmute "+contenu;
                                user->socket.send(packet);  
                            }
                        } else {
                            msg = user->name+" a quitte le salon de maniere imprevu";
                            std::cout << msg << std::endl;
                            packet.clear();
                            packet << P2P_MSG << "SERVEUR>"+msg;
                            for (std::vector<User*>::iterator it2 = users.begin(); it2 != users.end(); ++it2) {
                                if(*it2 != user) {
                                    (*it2)->socket.send(packet);
                                }
                            }
                            selector.remove(user->socket);
                            user->socket.disconnect();
                            users.erase(it);
                            delete user;
                            break;
                        }

                    }
                }
            }
        }
    }

    /*sf::TcpSocket socket;
    sf::TcpListener ecouteur;
    listener.listen(PORT);
    listener.accept(socket);
    std::cout << "Nouveau client connecté" << socket.getRemoteAddress() << std::endl;*/
}

int main(void) {
     serveur();
     return 0;
}

