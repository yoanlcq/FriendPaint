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
sf::TcpSocket socket;



int main(void) {
	sf::Packet packet;
	sf::Image imgE;

	imgE.loadFromFile("./sw.jpg");

	if (socket.connect(IP, PORT) != sf::Socket::Done) {
		std::cout << "La connexion a echoué\n";
		return -1;
	}
	packet << imgE;
	socket.send(packet);

	while(true) {
	}
}

