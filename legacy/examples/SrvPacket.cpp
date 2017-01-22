#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>


//TODO Tester ces opérateurs ABSOLUMENT
sf::Packet& operator <<(sf::Packet& packet, const sf::Image& img) {
    packet << img.getSize().x << img.getSize().y;
    for(sf::Uint32 i=0 ; i<4*img.getSize().x*img.getSize().y ; i++)
    packet << img.getPixelsPtr()[i];
    return packet;
}
sf::Packet& operator >>(sf::Packet& packet, sf::Image& img) {
    sf::Vector2u size;
    sf::Uint8* pixels;
    packet >> size.x >> size.y;
    pixels = (sf::Uint8*) malloc(4*size.x*size.y);
    for(sf::Uint32 i=0 ; i<4*size.x*size.y ; i++)
    packet >> pixels[i];
    img.create(size.x, size.y, pixels);
    free(pixels);
    return packet;
}

sf::Packet& operator <<(sf::Packet& packet, const sf::Color& c) {
    return packet << c.r << c.g << c.b << c.a;
}
sf::Packet& operator >>(sf::Packet& packet, sf::Color& c) {
    return packet >> c.r >> c.g >> c.b >> c.a;
}






const int PORT = 55555;
const sf::IpAddress IP = sf::IpAddress::getLocalAddress();

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
    sf::Image imgR;

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
                            packet >> imgR;
                            packet.clear();
                            if (!imgR.saveToFile("./imageRecu.jpg"))
                                std::cout << "erreur de creation de l'image" << std::endl;
                        }
                    }
                }
            }
        }
    }
}

int main(void) {
     serveur();
     return 0;
}

