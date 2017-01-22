#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include "GUI.hpp"
#include "PaintEngine.hpp"
#include "interface.hpp"


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




User::User() {
    color = sf::Color::White;
    name = "";
    is_host = false;
    tool_type = PEN;
    tool_diameter = 10;
    tool_color = sf::Color::Black;
    layer_index = 0;
    updateBrush();
}

User::~User() {}

sf::Vector2u User::getLastPositionOnCanvas() {return last_position_on_canvas;}
void User::setLastPositionOnCanvas(sf::Vector2u v) {last_position_on_canvas = v;}
sf::Uint32 User::getLayerIndex() {return layer_index;}
void User::setLayerIndex(sf::Uint32 i) {layer_index = i;}

sf::Color User::getColor() {return color;}
void User::setColor(sf::Color c) {color = c;}
sf::String User::getName() {return name;}
void User::setName(sf::String s) {name = s;}
sf::TcpSocket &User::getSocket() {return socket;}
bool User::getIsHost() {return is_host;}
void User::setIsHost(bool b) {is_host = b;}
User::ToolType User::getToolType() {return tool_type;}
void User::setToolType(User::ToolType t) {tool_type = t;}
void User::sendToolType(User::ToolType t) {
    sf::Packet packet;
    packet << FP_SET_TOOL_TYPE << color << t;
    socket.send(packet);
}
sf::Uint32 User::getToolDiameter() {return tool_diameter;}
void User::setToolDiameter(sf::Uint32 d) {tool_diameter = d;}
void User::sendToolDiameter(sf::Uint32 d) {
    sf::Packet packet;
    packet << FP_SET_TOOL_DIAMETER << color << d;
    socket.send(packet);
}
sf::Color User::getToolColor() {return tool_color;}
void User::setToolColor(sf::Color c) {tool_color = c;}
void User::sendToolColor(sf::Color c) {
    sf::Packet packet;
    packet << FP_SET_TOOL_COLOR << color << c;
    socket.send(packet);
}

sf::Image User::buildCircleImage(sf::Color color, sf::Uint32 diameter, bool fill) {
    sf::Image brush;
    sf::Uint32 *pixels;
    bool even=false;
    sf::Int32 x, y, m, center, i;
    
    if(diameter%2==0) {
	diameter++;
	even=true;
    }
    pixels = (sf::Uint32*) malloc(diameter*diameter*sizeof(sf::Uint32));
    memset(pixels, 0, diameter*diameter*sizeof(sf::Uint32));

    if(diameter==3 && even) {
	
	pixels[0] = pixels[1] = pixels[3] = pixels[4] = (color.a<<24)|(color.b<<16)|(color.g<<8)|(color.r);
	
    } else {
    
	x = 0;
	center=diameter/2 ;
	y=center ; 
	m=5-4*center ;
	while(x <= y) { 
	    pixels[(x+center)*diameter+(y+center)]=(color.a<<24)|(color.b<<16)|(color.g<<8)|(color.r);
	    pixels[(y+center)*diameter+(x+center)]=(color.a<<24)|(color.b<<16)|(color.g<<8)|(color.r);
	    pixels[(-x+center)*diameter+(y+center)]=(color.a<<24)|(color.b<<16)|(color.g<<8)|(color.r);
	    pixels[(-y+center)*diameter+(x+center)]=(color.a<<24)|(color.b<<16)|(color.g<<8)|(color.r);
	    pixels[(x+center)*diameter+(-y+center)]=(color.a<<24)|(color.b<<16)|(color.g<<8)|(color.r);
	    pixels[(y+center)*diameter+(-x+center)]=(color.a<<24)|(color.b<<16)|(color.g<<8)|(color.r);
	    pixels[(-x+center)*diameter+(-y+center)]=(color.a<<24)|(color.b<<16)|(color.g<<8)|(color.r);
	    pixels[(-y+center)*diameter+(-x+center)]=(color.a<<24)|(color.b<<16)|(color.g<<8)|(color.r);
	    if(m > 0) {
		y--;
		m=m-(8*y);
	    }
	    x++;
	    m=m+(8*x)+4;
	}

	//remplissage
	if(fill) {
	    for(i=diameter ;;) {
		for( ; pixels[i] == 0 ; i++);
		if(i > diameter*(diameter-1))
		    break;
		for( ; pixels[i] != 0 ; i++);

		do {
		    for( ; pixels[i] == 0 ; i++)
			pixels[i] = (color.a<<24)|(color.b<<16)|(color.g<<8)|(color.r);

		    for( ; pixels[i] != 0 ; i++);
		} while(i%diameter == 1);
	    }
	}
	//fin remplissage
    
	//Fix du diamètre pair.
	if(even) {
	    for(y=0 ; y<diameter ; y++) {
		for(x=diameter/2 ; x<diameter-1 ; x++) {
		    pixels[y*diameter + x] = pixels[y*diameter + x+1];
		}
		pixels[y*diameter + x] = 0;
	    }
	    for(x=0 ; x<diameter-1 ; x++) {
		for(y=diameter/2 ; y<diameter-1 ; y++) {
		    pixels[y*diameter + x] = pixels[(y+1)*diameter + x];
		}
		pixels[y*diameter + x] = 0;
	    }
	}
    }
    
    brush.create(diameter, diameter, (sf::Uint8*) pixels);
    if(even)
	diameter--;
    free(pixels);
    return brush;
}

void User::updateBrush() {
    brush = buildCircleImage(tool_color, tool_diameter, true);
}

sf::Image& User::getBrush() {return brush;}


sf::Packet& operator <<(sf::Packet& packet, const User& user) {
    return packet << user.color 
		  << user.name 
		  << (sf::Uint32) user.tool_type 
		  << user.tool_diameter
		  << user.tool_color 
		  << user.last_position_on_canvas.x
		  << user.last_position_on_canvas.y
		  << user.layer_index;
}
sf::Packet& operator >>(sf::Packet& packet, User& user) {
    sf::Uint32 tmp;
    packet >> user.color 
	   >> user.name 
	   >> tmp
	   >> user.tool_diameter
	   >> user.tool_color 
	   >> user.last_position_on_canvas.x
	   >> user.last_position_on_canvas.y
	   >> user.layer_index;
    user.tool_type = (User::ToolType) tmp;
    return packet;
}














Layer::Layer() {
    name = "Uninitialized";
    image.create(1,1,sf::Color::Transparent);
}
Layer::~Layer() {}

void Layer::copyImageFromCenter(sf::Image &dest, sf::Image &src, sf::Vector2u center, bool erase) {
    sf::Color pixel;
    sf::Int32 tx, ty;
    for(sf::Uint32 y=0 ; y<src.getSize().y ; y++) {
	ty = center.y + y - (src.getSize().y/2);
	if(ty >= 0 && ty < dest.getSize().y) {
	    for(sf::Uint32 x=0 ; x<src.getSize().x ; x++) {
		tx = center.x + x - (src.getSize().x/2);
		if(tx >= 0 && tx < dest.getSize().x) {
		    pixel = src.getPixel(x, y);
		    if(pixel.a > 0) {
			if(erase)
			    pixel.a = 0;
			dest.setPixel(tx, ty, pixel);
		    }
		}
	    }
	}
    }
}


void Layer::applyBrush(sf::Vector2u from, sf::Vector2u to, sf::Image brush, bool erase) {
    sf::Uint32 w = image.getSize().x;
    sf::Uint32 h = image.getSize().y;
    sf::Int32 delta_x(to.x - from.x);
    // if from.x == to.x, then it does not matter what we set here
    sf::Int8 const ix((delta_x > 0) - (delta_x < 0));
    delta_x = std::abs(delta_x) << 1;
 
    sf::Int32 delta_y(to.y - from.y);
    // if from.y == to.y, then it does not matter what we set here
    sf::Int8 const iy((delta_y > 0) - (delta_y < 0));
    delta_y = std::abs(delta_y) << 1;
 
    if(from.x < w && from.y < h)
	copyImageFromCenter(image, brush, from, erase);
 
    if (delta_x >= delta_y)
	{
	    // error may go below zero
	    sf::Int32 error(delta_y - (delta_x >> 1));
 
	    while (from.x != to.x)
		{
		    if ((error >= 0) && (error || (ix > 0)))
			{
			    error -= delta_x;
			    from.y += iy;
			}
		    // else do nothing
 
		    error += delta_y;
		    from.x += ix;
 
		    if(from.x < w && from.y < h)
			copyImageFromCenter(image, brush, from, erase);
		}
	}
    else
	{
	    // error may go below zero
	    sf::Int32 error(delta_x - (delta_y >> 1));
 
	    while (from.y != to.y)
		{
		    if ((error >= 0) && (error || (iy > 0)))
			{
			    error -= delta_y;
			    from.x += ix;
			}
		    // else do nothing
 
		    error += delta_x;
		    from.y += iy;
 
		    if(from.x < w && from.y < h)
			copyImageFromCenter(image, brush, from, erase);
		}
	}
}

sf::Image &Layer::getImage() {return image;}
void Layer::setImage(sf::Image new_image) {image = new_image;}
sf::String Layer::getName() {return name;}
void Layer::setName(sf::String new_name) {name = new_name;}
//Les fonctions suivantes peuvent accéder aux attributs de Layer
//Car elles sont déclarées "friend" dans le .hpp
sf::Packet& operator <<(sf::Packet& packet, const Layer& layer) {
    return packet << layer.name << layer.image;
}
sf::Packet& operator >>(sf::Packet& packet, Layer& layer) {
    return packet >> layer.name >> layer.image;
}



ServerLayer::ServerLayer() : Layer() {}
ServerLayer::~ServerLayer() {}



ClientLayer::ClientLayer() : Layer() {
    image.create(Client::session.getSize().x,
		 Client::session.getSize().y, 
		 sf::Color::Transparent);
    texture.loadFromImage(image);
    sprite.setTexture(texture);
    is_selected=true;
    is_visible=true;
    lock_edit=false;
    lock_delete=false;
}
ClientLayer::~ClientLayer() {}

void ClientLayer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    //Fixme bouffeur de ressources.
    ((ClientLayer*)this)->texture.loadFromImage(image);
    ((ClientLayer*)this)->sprite.setTexture(((ClientLayer*)this)->texture, true);
    target.draw(sprite, states);
}

void ClientLayer::setIsVisible(bool new_state) {is_visible = new_state;}
bool ClientLayer::getIsVisible() {return is_visible;}
void ClientLayer::setIsSelected(bool new_state) {is_selected = new_state;}
bool ClientLayer::getIsSelected() {return is_selected;}
void ClientLayer::setLockEdit(bool new_state) {lock_edit = new_state;}
bool ClientLayer::getLockEdit() {return lock_edit;}
void ClientLayer::setLockDelete(bool new_state) {lock_delete = new_state;}
bool ClientLayer::getLockDelete() {return lock_delete;}










Session::Session() {
    title = "Untitled";
    users.push_back(new User());
}
Session::~Session() {}
sf::String Session::getTitle() {return title;}
void Session::setTitle(sf::String s) {title=s;}
sf::Vector2u Session::getSize() {return size;}
void Session::setSize(sf::Vector2u s) {size=s;}
std::vector<User*> &Session::getUsers() {return users;}
const sf::Vector2u Session::A4(2480,3508);
const sf::Vector2u Session::A6(1240,1748);
//Les fonctions suivantes doivent avoir un corps vide.
//Leur implémentation est réservée aux classes filles.
void Session::createLayer(sf::Uint32 index, sf::String new_name) {}
void Session::moveLayer(sf::Uint32 index, sf::Int32 delta) {}
void Session::renameLayer(sf::Uint32 index, sf::String new_name) {}
void Session::deleteLayer(sf::Uint32 index) {}
void Session::mergeDownLayer(sf::Uint32 index) {}




ServerSession::ServerSession() : Session() {}
ServerSession::~ServerSession() {}
sf::String ServerSession::getDescription() {return description;}
void ServerSession::setDescription(sf::String s) {description = s;}
sf::Uint8 ServerSession::getNumSlots() {return num_slots;}
void ServerSession::setNumSlots(sf::Uint8 n) {num_slots = n;}
sf::String ServerSession::getPassword() {return password;}
void ServerSession::setPassword(sf::String s) {password = s;}
bool ServerSession::getAskBeforeAccept() {return ask_before_accept;}
void ServerSession::setAskBeforeAccept(bool b) {ask_before_accept = b;}
std::vector<ServerLayer> &ServerSession::getLayers() {return layers;}
void ServerSession::createLayer(sf::Uint32 index, sf::String new_name) {
    //Si l'index est hors de bords, on crée le calque en haut de la pile.
    if(index >= layers.size())
	index = layers.size()-1;

    layers.insert(layers.begin()+index, ServerLayer());
    layers[index].setName(new_name);
}
void ServerSession::moveLayer(sf::Uint32 index, sf::Int32 delta) {
    //Pourquoi s'ennuyer avec un seul calque ?
    if(layers.size() > 1 && delta != 0) {
	//Si delta est hors de bords, on le réduit à l'extreme le plus proche.
	if(index+delta <= 0)
	    delta = -index;
	else if(index+delta >= layers.size())
	    delta = layers.size()-1-index;

	layers.insert(layers.begin()+index+delta, layers[index]);
//Si on a inséré en dessous (i.e delta négatif), l'index de notre ancien calque a été incrémenté.
	layers.erase(layers.begin()+index+(delta < 0 ? 1 : 0));
    }
}
void ServerSession::renameLayer(sf::Uint32 index, sf::String new_name) {
    layers[index].setName(new_name);
}
void ServerSession::deleteLayer(sf::Uint32 index) {
    layers.erase(layers.begin()+index);
}
void ServerSession::mergeDownLayer(sf::Uint32 index) {
    if(layers.size() > 1 && index > 0) {
	layers[index-1].getImage().copy(layers[index].getImage(), 0, 0, sf::IntRect(0, 0, 0, 0), true);
	layers.erase(layers.begin()+index);
    }
}




ClientSession::ClientSession() : 
    Session(), 
    listening_thread(&listeningThreadFunc, this) 
{
    controlled_user_color = users[0]->getColor();
    users[0]->setName("Root");
    tool_dragged = false;
}
ClientSession::~ClientSession() {}




void ClientSession::listeningThreadFunc(ClientSession* session) {
    //TODO
    sf::SocketSelector selector;
    sf::Packet packet;
    User tmp_user;
    std::string pseudo, msg, contenu;
    sf::Uint32 msg_type;
    sf::Uint32 tmp_u32;
    bool there_is_host;
    sf::Int32 tmp;
    sf::Vector2u tmp_vec2u;
    sf::Vector2i tmp_vec2i;
    bool tmp_bool;
    sf::Color tmp_color, tmp_ucolor;
    sf::String tmp_string;
    sf::Socket::Status status;
    
    sf::sleep(sf::microseconds(600));

    selector.add(session->getUsers()[0]->getSocket());
    std::cout << "Listening thread launched." << std::endl;

    {
	sf::Lock lock(session->getGuiMutex());
	tmp_color = session->getUsers()[0]->getColor();
	user_ctrl_name.println(TextContainer::RGB(tmp_color.r, tmp_color.g, tmp_color.b)
			       + session->getUsers()[0]->getName()
			       + TextContainer::RESET
			       + "'s locks");
	updateLayersPanel();
	tmp_color = sf::Color(R_slider.getValue(), G_slider.getValue(), B_slider.getValue());
	session->getUsers()[0]->sendToolColor(tmp_color);
	session->getUsers()[0]->sendToolDiameter(tool_diameter_slider.getValue());
    }

    for(;;) {
	if(!selector.wait())
	    continue;
	if(!selector.isReady(session->getUsers()[0]->getSocket()))
	    continue;

	{
	    sf::Lock lock(session->getSocketMutex());
	    status = session->getUsers()[0]->getSocket().receive(packet);
	}
	if(status == sf::Socket::Disconnected)
	    break;
	if(status != sf::Socket::Done) {
	    std::cout << "Listening Thread : Unexpected recv() status. Restarting loop." << std::endl;
	    continue;
	}


	packet >> msg_type;

	switch(msg_type) {
	case FP_SAY: 
	    packet >> tmp_string;
	    {
		sf::Lock lock(session->getGuiMutex());
		chat.println(tmp_string);
	    }
	    break;
	case FP_USERS: 
	    tmp = session->getUsers().size();
	    do {
		session->getUsers().push_back(new User());
	    } while(packet >> (*session->getUsers().back()));
	    delete session->getUsers().back();
	    session->getUsers().pop_back();
	    {
		sf::Lock lock(session->getGuiMutex());
		for( ; tmp < session->getUsers().size() ; ++tmp) {
		    tmp_color = session->getUsers()[tmp]->getColor();
		    chat.println(TextContainer::RGB(tmp_color.r, tmp_color.g, tmp_color.b)
				 + session->getUsers()[tmp]->getName()
				 + TextContainer::RESET
				 + " is in the lobby.");
		}
	    } 
	    {
	      sf::Lock lock(session->getGuiMutex());
	      tmp_color = sf::Color(R_slider.getValue(), G_slider.getValue(), B_slider.getValue());
	      session->getUsers()[0]->sendToolColor(tmp_color);
	    }
	    break;
	case FP_EXIT: 
	    packet >> tmp_color;
	    for(sf::Uint32 i=1 ; i < session->getUsers().size() ; ++i) {
		if(session->getUsers()[i]->getColor() == tmp_color) {
		    {
			sf::Lock lock(session->getGuiMutex());
			chat.println(TextContainer::RGB(tmp_color.r, tmp_color.g, tmp_color.b)
				     + session->getUsers()[i]->getName()
				     + TextContainer::RESET
				     + " exited the lobby.");
		    }
	    
		    delete session->getUsers()[i];
		    session->getUsers().erase(session->getUsers().begin()+i);
		    break;
		}
	    }
	    break;
	case FP_SET_TOOL_TYPE: 
	case FP_SET_TOOL_DIAMETER: 
	    packet >> tmp_color >> tmp_u32;
	    for(sf::Uint32 i=0 ; i<session->getUsers().size() ; ++i) {
		if(session->getUsers()[i]->getColor() == tmp_color) {
		    if(msg_type == FP_SET_TOOL_TYPE)
			session->getUsers()[i]->setToolType((User::ToolType)tmp_u32);
		    else 
			session->getUsers()[i]->setToolDiameter(tmp_u32);
		    session->getUsers()[i]->updateBrush();
		    break;
		}
	    }
	    break;
	case FP_SET_TOOL_COLOR:
	    packet >> tmp_ucolor >> tmp_color;
	    packet.clear();
	    for(sf::Uint32 i=0 ; i<session->getUsers().size() ; i++) {
		if(session->getUsers()[i]->getColor() == tmp_ucolor) {
		    session->getUsers()[i]->setToolColor(tmp_color);
		    session->getUsers()[i]->updateBrush();
		    break;
		}
	    }
	    break;
	case FP_BEGIN_TOOL_DRAG: 
	case FP_STEP_TOOL_DRAG:
	case FP_END_TOOL_DRAG: 
	    packet >> tmp_ucolor >> tmp_vec2u.x >> tmp_vec2u.y;
	    for(sf::Uint32 i=0 ; i<session->getUsers().size() ; i++) {
		if(session->getUsers()[i]->getColor() == tmp_ucolor) {
		    {
			sf::Lock lock(session->getGuiMutex());
			session->getLayers()[session->getUsers()[i]->getLayerIndex()]
			    .applyBrush(msg_type==FP_BEGIN_TOOL_DRAG
					? tmp_vec2u
					: session->getUsers()[i]->getLastPositionOnCanvas(), 
					tmp_vec2u,
					session->getUsers()[i]->getBrush(),
					session->getUsers()[i]->getToolType()==User::ToolType::ERASER);
		    }
		    session->getUsers()[i]->setLastPositionOnCanvas(tmp_vec2u);
		    break;
		}
	    }
	    break;
	case FP_CREATE_LAYER: break;
	case FP_RENAME_LAYER: break;
	case FP_MOVE_LAYER: break;
	case FP_MERGE_DOWN_LAYER: break;
	case FP_DELETE_LAYER: break;
	case FP_LOCK_EDIT_LAYER: break;
	case FP_UNLOCK_EDIT_LAYER: break;
	case FP_LOCK_DELETE_LAYER: break;
	case FP_UNLOCK_DELETE_LAYER: break;
	case FP_GRANT_ADMIN: break;
	case FP_GRANT_HOST: break;
	case FP_LOCK_CREATE_LAYER: break;
	case FP_UNLOCK_CREATE_LAYER: break;
	}
    }
    session->getUsers()[0]->getSocket().disconnect();
    std::cout << "Listening thread ended." << std::endl;
}



sf::Thread &ClientSession::getListeningThread() {return listening_thread;}
sf::Mutex &ClientSession::getSocketMutex() {return socket_mutex;}
sf::Mutex &ClientSession::getGuiMutex() {return gui_mutex;}
void ClientSession::say(sf::String txt) {
    sf::Packet packet;
    packet << FP_SAY << txt;
    {
	sf::Lock lock(socket_mutex);
	users[0]->getSocket().send(packet);
    }
}
bool ClientSession::getIsAdmin() {return is_admin;}
void ClientSession::setIsAdmin(bool b) {is_admin = b;}
sf::Color ClientSession::getControlledUserColor() {return controlled_user_color;}
void ClientSession::setControlledUserColor(sf::Color c) {controlled_user_color = c;}
void ClientSession::grantAdminToControlledUser() {
    sf::Packet packet;
    packet << FP_GRANT_ADMIN << controlled_user_color;
    {
	sf::Lock lock(socket_mutex);
	users[0]->getSocket().send(packet);
    }
}
void ClientSession::grantHostToControlledUser() {
    sf::Packet packet;
    packet << FP_GRANT_HOST << controlled_user_color;
    {
	sf::Lock lock(socket_mutex);
	users[0]->getSocket().send(packet);
    }
}
void ClientSession::kickControlledUser() {
    sf::Packet packet;
    packet << FP_EXIT << controlled_user_color;
    {
	sf::Lock lock(socket_mutex);
	users[0]->getSocket().send(packet);
    }
}
bool ClientSession::getToolDragged() {return tool_dragged;}
void ClientSession::beginToolDrag(sf::Vector2u position_on_canvas) {
    sf::Packet packet;
    packet << FP_BEGIN_TOOL_DRAG << users[0]->getColor() 
	   << position_on_canvas.x << position_on_canvas.y;
    {
	sf::Lock lock(socket_mutex);
	users[0]->getSocket().send(packet);
    }
    tool_dragged = true;
}
void ClientSession::stepToolDrag(sf::Vector2u position_on_canvas) {
    sf::Packet packet;
    packet << FP_STEP_TOOL_DRAG << users[0]->getColor() 
	   << position_on_canvas.x << position_on_canvas.y;
    {
	sf::Lock lock(socket_mutex);
	users[0]->getSocket().send(packet);
    }
}
void ClientSession::endToolDrag(sf::Vector2u position_on_canvas) {
    sf::Packet packet;
    packet << FP_END_TOOL_DRAG << users[0]->getColor() 
	   << position_on_canvas.x << position_on_canvas.y;
    {
	sf::Lock lock(socket_mutex);
	users[0]->getSocket().send(packet);
    }
    tool_dragged = false;
}
bool ClientSession::getLockCreate() {return lock_create;}
void ClientSession::setLockCreate(bool b) {lock_create = b;}
void ClientSession::sendLockCreate(bool b) {
    sf::Packet packet;
    packet << (b ? FP_LOCK_CREATE_LAYER : FP_UNLOCK_CREATE_LAYER) 
	   << controlled_user_color;
    {
	sf::Lock lock(socket_mutex);
	users[0]->getSocket().send(packet);
    }
}
std::vector<ClientLayer> &ClientSession::getLayers() {
    return layers;
}
void ClientSession::sendCreateLayer(sf::Uint32 index, sf::String new_name) {
    sf::Packet packet;
    packet << FP_CREATE_LAYER << users[0]->getColor() << index << new_name;
    {
	sf::Lock lock(socket_mutex);
	users[0]->getSocket().send(packet);
    }
}
void ClientSession::sendMoveLayer(sf::Uint32 index, sf::Int32 delta) {
    sf::Packet packet;
    packet << FP_MOVE_LAYER << users[0]->getColor() << index << delta;
    {
	sf::Lock lock(socket_mutex);
	users[0]->getSocket().send(packet);
    }
}
void ClientSession::sendRenameLayer(sf::Uint32 index, sf::String new_name) {
    sf::Packet packet;
    packet << FP_RENAME_LAYER << users[0]->getColor() << index << new_name;
    {
	sf::Lock lock(socket_mutex);
	users[0]->getSocket().send(packet);
    }
}
void ClientSession::sendDeleteLayer(sf::Uint32 index) {
    sf::Packet packet;
    packet << FP_DELETE_LAYER << users[0]->getColor() << index;
    {
	sf::Lock lock(socket_mutex);
	users[0]->getSocket().send(packet);
    }
}
void ClientSession::sendMergeDownLayer(sf::Uint32 index) {
    sf::Packet packet;
    packet << FP_MERGE_DOWN_LAYER << users[0]->getColor() << index;
    {
	sf::Lock lock(socket_mutex);
	users[0]->getSocket().send(packet);
    }
}
void ClientSession::createLayer(sf::Uint32 index, sf::String new_name) {
    //Si l'index est hors de bords, on crée le calque en haut de la pile.
    if(layers.size()<=0)
	index = 0;
    else if(index >= layers.size())
	index = layers.size()-1;

    layers.insert(layers.begin()+index, ClientLayer());
    layers[index].setName(new_name);
}
void ClientSession::moveLayer(sf::Uint32 index, sf::Int32 delta) {
    //Pourquoi s'ennuyer avec un seul calque ?
    if(layers.size() > 1 && delta != 0) {
	//Si delta est hors de bords, on le réduit à l'extreme le plus proche.
	if(index+delta <= 0)
	    delta = -index;
	else if(index+delta >= layers.size())
	    delta = layers.size()-1-index;

	layers.insert(layers.begin()+index+delta, layers[index]);
//Si on a inséré en dessous (i.e delta négatif), l'index de notre ancien calque a été incrémenté.
	layers.erase(layers.begin()+index+(delta < 0 ? 1 : 0));
    }
}
void ClientSession::renameLayer(sf::Uint32 index, sf::String new_name) {
    layers[index].setName(new_name);
}
void ClientSession::deleteLayer(sf::Uint32 index) {
    layers.erase(layers.begin()+index);
}
void ClientSession::mergeDownLayer(sf::Uint32 index) {
    if(layers.size() > 1 && index > 0) {
	layers[index-1].getImage().copy(layers[index].getImage(), 0, 0, sf::IntRect(0, 0, 0, 0), true);
	layers.erase(layers.begin()+index);
    }
}
sf::Image ClientSession::getFlattenedImage() {
    //Renvoie un aplat de tous les calques visibles.
    //Cette fonction n'ajoute pas le damier gris pour la transparence.
    sf::Image res;
    res.create(getSize().x, getSize().y, sf::Color::Transparent);

    for(sf::Uint32 i=0 ; i<layers.size() ; i++)
	res.copy(layers[i].getImage(), 0, 0, sf::IntRect(0, 0, 0, 0), true);

    return res;
}






ServerInfo::ServerInfo() {
    version = FP_VERSION; //Définie dans PaintEngine.hpp
    port = FP_SOLO_PORT;
    max_lobby_description_length = 256;
    max_lobby_slots = 8;
    taken_slots = 0;
    total_slots = max_lobby_slots;
    max_title_length = 32;
    max_password_length = 32;
    max_username_length = 16;
    accepting = false;
}
ServerInfo::~ServerInfo() {}
sf::String ServerInfo::getSessionTitle() {return session_title;}
sf::Uint8 ServerInfo::getTakenSlots() {return taken_slots;}
sf::Uint8 ServerInfo::getTotalSlots() {return total_slots;}
sf::String ServerInfo::getVersion() {return version;}
sf::Uint16 ServerInfo::getMaxLobbyDescriptionLength() {return max_lobby_description_length;}
sf::Uint8 ServerInfo::getMaxLobbySlots() {return max_lobby_slots;}
sf::Uint8 ServerInfo::getMaxTitleLength() {return max_title_length;}
sf::Uint8 ServerInfo::getMaxPasswordLength() {return max_password_length;}
sf::Uint8 ServerInfo::getMaxUsernameLength() {return max_username_length;}
unsigned short ServerInfo::getPort() {return port;}
bool ServerInfo::getAccepting() {return accepting;}
void ServerInfo::setSessionTitle(sf::String s) {session_title = s;}
void ServerInfo::setTakenSlots(sf::Uint8 n) {taken_slots = n;}
void ServerInfo::setTotalSlots(sf::Uint8 n) {total_slots = n;}
void ServerInfo::setMaxLobbyDescriptionLength(sf::Uint16 max) {max_lobby_description_length = max;}
void ServerInfo::setMaxLobbySlots(sf::Uint8 max) {max_lobby_slots = max;}
void ServerInfo::setMaxTitleLength(sf::Uint8 max) {max_title_length = max;}
void ServerInfo::setMaxPasswordLength(sf::Uint8 max) {max_password_length = max;}
void ServerInfo::setMaxUsernameLength(sf::Uint8 max) {max_username_length = max;}
void ServerInfo::setPort(unsigned short p) {port = p;}
void ServerInfo::setAccepting(bool b) {accepting = b;}

sf::Packet& operator <<(sf::Packet& packet, const ServerInfo& info) {
    return packet << info.session_title 
		  << info.taken_slots 
		  << info.total_slots 
		  << info.version 
		  << info.max_lobby_description_length 
		  << info.max_lobby_slots 
		  << info.max_title_length 
		  << info.max_password_length 
		  << info.max_username_length 
		  << info.accepting;
}
sf::Packet& operator >>(sf::Packet& packet, ServerInfo& info) {
    return packet >> info.session_title 
		  >> info.taken_slots 
		  >> info.total_slots 
		  >> info.version 
		  >> info.max_lobby_description_length 
		  >> info.max_lobby_slots 
		  >> info.max_title_length 
		  >> info.max_password_length 
		  >> info.max_username_length 
		  >> info.accepting;
}




/*
Les classes Client et Server.
En réalité, ce ne sont pas des classes à instancier.
Elle servent surtout à "cacher" des variables globales,
et regrouper des méthodes propres au client et au serveur.
*/


ClientSession Client::session;
const sf::String Client::version = FP_VERSION;
ServerInfo Client::offline_server_info;
sf::Thread Client::offline_server_thread(&Server::serverLoop, &offline_server_info);

Client::Client() {}

void Client::endSession() {
    session.setControlledUserColor(Client::session.getUsers()[0]->getColor());
    session.kickControlledUser();
    std::cout << "Waiting for listening thread to terminate." << std::endl;
    session.getListeningThread().wait();
}

void Client::endOfflineServerThread() {
    sf::Packet packet;
    sf::TcpSocket socket;
    packet << FP_KILL;
    socket.connect("localhost", offline_server_info.getPort());
    socket.send(packet);
    socket.disconnect();
    std::cout << "Waiting for local server thread to terminate." << std::endl;
    offline_server_thread.wait();
}

void Client::createOfflineSession(sf::String title, sf::Vector2u size) {
    ServerInfo server_info;
    sf::Uint32 i;

    offline_server_info.setPort(FP_SOLO_PORT);

    endSession();
    endOfflineServerThread();

    session.setControlledUserColor(session.getUsers()[0]->getColor());
    session.kickControlledUser();
    session.setTitle(title);
    session.setSize(size);
    for(i=0 ; i<session.getUsers().size() ; i++)
	delete session.getUsers()[i];
    session.getUsers().clear();
    session.getUsers().push_back(new User());
    session.getLayers().clear();
    session.createLayer(0, "Layer 0");

    //Lancer un thread tournant un serveur.
    //offline_server_info.setMaxLobbySlots(1); On laisse à 8 pour débugger.
    offline_server_info.setAccepting(true);

    std::cout << "Launching local server thread..." << std::endl;
    offline_server_thread.launch();

    //Fixme. peut etre que le thread n'a pas démarré.
    for(i=0 ; i<30 ; i++) {
	sf::sleep(sf::milliseconds(200));
	server_info = connectToServer("localhost", offline_server_info.getPort(), "Root");
	if(server_info.getAccepting()) 
	    break;
    }
    if(i>=30) {
	std::cout << "No server at localhost:" << offline_server_info.getPort() << "." << std::endl;
	return;
    }
    std::cout << "Asking local server to host lobby..." << std::endl;
    if(!requestHostLobby("", "", 4, false)) {
	std::cout << "Local server refused to host lobby ??" << std::endl;
    } else {
	std::cout << "It's painting time !" << std::endl;
    }
}

ServerInfo Client::connectToServer(sf::IpAddress remote_address, unsigned short remote_port, sf::String username) {
    sf::Packet packet;
    ServerInfo info;
    sf::Uint32 indicator;
    sf::TcpSocket tmp_socket;

    if(tmp_socket.connect(remote_address, remote_port) != sf::Socket::Done) {
	return info;
    }
    tmp_socket.disconnect();

    sf::sleep(sf::milliseconds(500));

    session.setControlledUserColor(Client::session.getUsers()[0]->getColor());
    session.kickControlledUser();

    sf::sleep(sf::milliseconds(500));

    if(session.getUsers()[0]->getSocket().connect(remote_address, remote_port) != sf::Socket::Done) {
	std::cout << "Fixme: Socket race occured." << std::endl;
	return info;
    }
    
    packet << FP_INIT << username << version;
    session.getUsers()[0]->getSocket().send(packet);
    packet.clear();
    if(session.getUsers()[0]->getSocket().receive(packet) == sf::Socket::Done) {
	packet >> indicator;
	if(indicator==FP_DENIED) {
	    std::cout << "Fixme: Connection denied. Missing details." << std::endl;
	} else {
	    packet >> info;
	}
    }
    session.getUsers()[0]->setName(username);
    lobby_name_text1.clear(info.getSessionTitle());
    slots_num_text1.println(std::to_string(info.getTakenSlots()-1) 
			    + "/"  + std::to_string(info.getTotalSlots()));
    return info;
}






bool Client::requestHostLobby(sf::String password, sf::String description, sf::Uint8 num_slots, bool ask_before_accept) {
    sf::Packet packet;
    sf::Uint32 indicator;
    sf::Color ucolor;
    packet << FP_CREATE_LOBBY << (*session.getUsers()[0]) << password << session.getTitle() << session.getSize().x << session.getSize().y << description << num_slots << ask_before_accept;
    for(sf::Uint32 i=0 ; i<session.getLayers().size() ; i++) {
	packet << session.getLayers()[i];
    }
    session.getUsers()[0]->getSocket().send(packet);
    packet.clear();
    if(session.getUsers()[0]->getSocket().receive(packet) != sf::Socket::Done)
	return false;
    packet >> indicator;
    if(indicator == FP_DENIED)
	return false;
    packet >> ucolor;
    session.getUsers()[0]->setColor(ucolor);

    //Lancer le listening thread.
    session.getListeningThread().launch();
    return true;
}







bool Client::requestJoinLobby(sf::Uint32 index, sf::String password) {
    sf::Packet packet;
    sf::Uint32 indicator;
    sf::Color ucolor;
    ClientLayer *tmp_layer;

    //TODO: quitter en rejoignant est bloquant.
    //Quand un utilisateur exit, le listening thread doit savoir.
    //Quand un utilisateur quitte comme un sauvage, ca apparait dans le tchat.
    endOfflineServerThread();

    packet << FP_JOIN_LOBBY << (*session.getUsers()[0]) << index << password;
    session.getUsers()[0]->getSocket().send(packet);
    packet.clear();
    if(session.getUsers()[0]->getSocket().receive(packet) != sf::Socket::Done)
	return false;
    packet >> indicator;
    if(indicator == FP_DENIED)
	return false;
    packet >> ucolor;
    session.getUsers()[0]->setColor(ucolor);
    session.getLayers().clear();

    tmp_layer = new ClientLayer();
    while(packet >> *tmp_layer) {
	session.getLayers().push_back(*tmp_layer);
	delete tmp_layer;
	tmp_layer = new ClientLayer();
    }
    delete tmp_layer;
    session.setSize(session.getLayers().back().getImage().getSize());

    canvas_widget.setSession(&session);
    
    //Lancer le listening thread.
    session.getListeningThread().launch();
    return true;
}






















Server::Server() {}
ServerInfo Server::info;
void Server::serverLoop(ServerInfo *info) {

    ServerSession session;
    sf::TcpListener listener;
    sf::SocketSelector selector;
    sf::Packet packet;
    std::string pseudo, msg, contenu;
    sf::Uint32 msg_type;
    bool there_is_host;
    sf::Int32 tmp;
    sf::Uint32 tmp_u32;
    sf::Uint8 tmp_uint8;
    sf::Vector2u tmp_vec2u;
    sf::Vector2i tmp_vec2i;
    bool tmp_bool;
    sf::Color tmp_color, tmp_ucolor;
    sf::String tmp_string;
    sf::String username, version; //case FP_INIT
    std::string logname;
    std::ostringstream logname_os;
    ServerLayer tmp_serverlayer;
    bool already_hosting = false;
    sf::Color available_colors[7] = {
	sf::Color::Yellow,
	sf::Color::Green,
	sf::Color::Red,
	sf::Color::Cyan,
	sf::Color::Blue,
	sf::Color::Magenta,
	sf::Color(127, 127, 127)
    };

    for(tmp=0 ; tmp<session.getUsers().size() ; ++tmp) {
	delete session.getUsers()[tmp];
    }
    session.getUsers().clear();

    session.setTitle("<No lobby>");

    while(listener.listen(info->getPort()) != sf::Socket::Done) 
	info->setPort(info->getPort() + 1);
    selector.add(listener);
    
    logname_os << "FPSRV " << sf::IpAddress::getPublicAddress().toString() << ':' << std::to_string(info->getPort()) << "> ";
    logname = logname_os.str();
    
    std::cout << logname << "Demarrage..." << std::endl;

    for(;;) {
        std::cout << logname << "..." << std::endl;
	if(info->getTotalSlots()<=0) {
	    info->setTotalSlots(8);
	    std::cout << logname << "Le nombre de slots a ete reinitilaise." << std::endl;
	}
        if (selector.wait()) {
            if (selector.isReady(listener)) {
		session.getUsers().push_back(new User());
		if (listener.accept(session.getUsers().back()->getSocket()) == sf::Socket::Done) {
		    if(info->getTakenSlots() >= info->getTotalSlots()) {
			std::cout << logname << "Pas de place pour la nouvelle connexion." << std::endl;
			session.getUsers().back()->getSocket().disconnect();
			delete session.getUsers().back();
			session.getUsers().pop_back();
		    } else {
			info->setTakenSlots(info->getTakenSlots()+1);
			selector.add(session.getUsers().back()->getSocket());
			std::cout << logname << "Nouvelle connexion entrante."
				  << "(" << std::to_string(info->getTakenSlots())
				  << "/" << std::to_string(info->getTotalSlots()) << ")" 
				  << std::endl;
		    }
		} else {
		    std::cout << logname  << "N'a pas pu accepter une connexion entrante." << std::endl;
		    delete session.getUsers().back();
		    session.getUsers().pop_back();
		}
            } else {
                for (sf::Uint32 it=0 ; it < session.getUsers().size(); ++it) {
                    User& user = *session.getUsers()[it];
                    if (selector.isReady(user.getSocket())) {
                        packet.clear();
                        if (user.getSocket().receive(packet) == sf::Socket::Done) {
                            packet >> msg_type;
                            // INIT
                            switch(msg_type) {
                            case FP_INIT:
                                packet >> username >> version;
                                if(username.getSize() > info->getMaxUsernameLength() 
                                   || version != info->getVersion()) {
                                    packet.clear();
                                    packet << FP_DENIED << info->getMaxUsernameLength() << info->getVersion();
                                    user.getSocket().send(packet);
                                    break;
                                }
				user.setName(username);
				info->setAccepting(true);
                                packet.clear();
                                packet << FP_OK << (*info);
                                user.getSocket().send(packet);  
                                std::cout << logname << (std::string) user.getName() 
					  << " prepare une requete. (FP_INIT)" 
					  << std::endl;
                                break;
                            case FP_CREATE_LOBBY:
				if(already_hosting) {
                                std::cout << logname << (std::string) user.getName() 
					  << " ne peut pas ecraser le salon actuel. (FP_CREATE_LOBBY)" 
					  << std::endl;
				    packet.clear();
				    packet << FP_DENIED;
				    user.getSocket().send(packet);  
				    break;
				}
				packet >> user;
				user.updateBrush();
                                packet >> tmp_string;
				session.setPassword(tmp_string);
                                packet >> tmp_string;
				session.setTitle(tmp_string);
				info->setSessionTitle(tmp_string);
				packet >> tmp_vec2u.x >> tmp_vec2u.y;
				session.setSize(tmp_vec2u);
                                packet >> tmp_string;
				session.setDescription(tmp_string);
                                packet >> tmp_uint8;
				session.setNumSlots(tmp_uint8);
				info->setTotalSlots(tmp_uint8);
                                packet >> tmp_bool;
				session.setAskBeforeAccept(tmp_bool);
				session.getLayers().clear();
                                while(packet >> tmp_serverlayer) {
                                    session.getLayers().push_back(tmp_serverlayer);
				}
				user.setIsHost(true);
				packet.clear();
				packet << FP_OK << sf::Color::White;
				user.setColor(sf::Color::White);
				user.getSocket().send(packet);
                                std::cout << logname << (std::string) user.getName() 
					  << " a cree un salon. (FP_CREATE_LOBBY)" 
					  << std::endl;
				already_hosting = true;
                                break;
				// JOIN SALON
                            case FP_JOIN_LOBBY:
				tmp_bool = false;
                            	packet >> user >> tmp >> tmp_string;
				user.updateBrush();
                            	packet.clear();
				if(info->getTakenSlots() >= info->getTotalSlots()) {
				    packet << FP_DENIED;
				    std::cout << logname << "Pas de place pour " 
					      << (std::string) user.getName() 
					      << ". (FP_JOIN_LOBBY)" 
					      << std::endl;
				} else if(tmp_string != session.getPassword()) {
                            	    packet << FP_DENIED;
				    std::cout << logname << "Mot de passe incorrect pour " 
					      << (std::string) user.getName() 
					      << ". (FP_JOIN_LOBBY)" 
					      << std::endl;
                            	} else {
                            	    /* Choix de la couleur */
				    for(sf::Uint32 i=0 ; i<7 ; i++) {
					tmp_bool = true;
					tmp_color = available_colors[i];
					for(sf::Uint32 j=0 ; j<session.getUsers().size() ; j++) {
					    if(session.getUsers()[j]->getColor() == tmp_color) {
						tmp_bool = false;
						break;
					    }
					}
					if(tmp_bool)
					    break;
				    }
				    std::cout << logname << "Envoi du canevas a " 
					      << (std::string) user.getName() 
					      << "... (FP_JOIN_LOBBY)" 
					      << std::endl;
				    user.setColor(tmp_color);
                            	    packet << FP_OK << tmp_color;
				    for(sf::Uint32 i=0 ; i<session.getLayers().size() ; i++) {
					packet << session.getLayers()[i];
				    }
				    tmp_bool = true;
                            	}
                            	user.getSocket().send(packet);

				if(tmp_bool) {
				    //Prévenir ceux qui étaient déjà là
				    packet.clear();
				    packet << FP_USERS << user;
				    for(sf::Uint32 i=0 ; i<session.getUsers().size() ; i++) {
					if(session.getUsers()[i]->getColor() != user.getColor())
					    session.getUsers()[i]->getSocket().send(packet);
				    }
				    //Informer le nouveau
				    packet.clear();
				    packet << FP_USERS;
				    for(sf::Uint32 i=0 ; i<session.getUsers().size() ; i++) {
					if(session.getUsers()[i]->getColor() != user.getColor())
					    packet << (*session.getUsers()[i]);
				    }
				    user.getSocket().send(packet);
				}
                                break;
				// SEND MESSAGE
                            case FP_SAY:
				//Aucun problème ici.
                                packet >> tmp_string;
                                std::cout << logname << (std::string) tmp_string << " (FP_SAY)" << std::endl;
                                packet.clear();
                                packet << FP_SAY << tmp_string; // On le restaure
                                for (std::vector<User*>::iterator it2 = session.getUsers().begin(); it2 != session.getUsers().end(); ++it2) {
                                    (*it2)->getSocket().send(packet);
                                }
                                break;
			    case FP_SET_TOOL_TYPE: 
			    case FP_SET_TOOL_DIAMETER:
				packet >> tmp_color >> tmp_u32;
				packet.clear();
				packet << msg_type << tmp_color << tmp_u32;
				for(sf::Uint32 i=0 ; i<session.getUsers().size() ; i++) {
				    if(session.getUsers()[i]->getColor() == tmp_color) {
					if(msg_type == FP_SET_TOOL_TYPE)
					    session.getUsers()[i]->setToolType((User::ToolType)tmp_u32);
					else 
					    session.getUsers()[i]->setToolDiameter(tmp_u32);
				    }
				    session.getUsers()[i]->getSocket().send(packet);
				}
				break;
			    case FP_SET_TOOL_COLOR:
				packet >> tmp_ucolor >> tmp_color;
				packet.clear();
				packet << FP_SET_TOOL_COLOR << tmp_ucolor << tmp_color;
				for(sf::Uint32 i=0 ; i<session.getUsers().size() ; i++) {
				    if(session.getUsers()[i]->getColor() == tmp_ucolor)
					session.getUsers()[i]->setToolColor(tmp_color);
				    session.getUsers()[i]->getSocket().send(packet);
				}
				break;
	case FP_BEGIN_TOOL_DRAG: 
	case FP_STEP_TOOL_DRAG:
	case FP_END_TOOL_DRAG: 
	    packet >> tmp_ucolor >> tmp_vec2u.x >> tmp_vec2u.y;
	    packet.clear();
	    packet << msg_type << tmp_ucolor << tmp_vec2u.x << tmp_vec2u.y;
	    for(sf::Uint32 i=0 ; i<session.getUsers().size() ; i++) {
		session.getUsers()[i]->getSocket().send(packet);
		if(session.getUsers()[i]->getColor() == tmp_ucolor) {
			session.getLayers()[session.getUsers()[i]->getLayerIndex()]
			    .applyBrush(msg_type==FP_BEGIN_TOOL_DRAG 
					? tmp_vec2u 
					: session.getUsers()[i]->getLastPositionOnCanvas(), 
					tmp_vec2u,
					session.getUsers()[i]->getBrush(),
					session.getUsers()[i]->getToolType()==User::ToolType::ERASER);
		    session.getUsers()[i]->setLastPositionOnCanvas(tmp_vec2u);
		}
	    }
	    break;
				// EXIT
                            case FP_EXIT:
				//Nous donne la couleur de l'utilisateur à virer.
				//Un utilisateur fait ça quand il quitte le salon,
				//Ou bien, s'il est admin, quand il kick quelqu'un.
				if(info->getTakenSlots() > 0)
				    info->setTakenSlots(info->getTakenSlots() - 1);
                                packet >> tmp_color;
				packet.clear();
                                packet << FP_EXIT << tmp_color;
                                for(sf::Uint32 i=0 ; i<session.getUsers().size() ; i++) {
                                    session.getUsers()[i]->getSocket().send(packet);
                                    if(session.getUsers()[i]->getColor() == tmp_color)
                                        tmp = i;
                                }
                                std::cout << logname << (std::string) session.getUsers()[tmp]->getName()
					  << " a quitte le salon. (FP_EXIT)" 
					  << "(" << std::to_string(info->getTakenSlots())
					  << "/" << std::to_string(info->getTotalSlots()) << ")" 
					  << std::endl;
				if(session.getUsers()[tmp]->getIsHost()) {
				    already_hosting = false;
				    std::cout << logname 
					      << "Le salon n'a plus d'hote et a ete detruit."
					      << std::endl;
				    session.setTitle("<No lobby>");
				    for(sf::Uint32 i=0 ; i<session.getUsers().size() ; i++) {
					selector.remove(session.getUsers()[i]->getSocket());
					session.getUsers()[i]->getSocket().disconnect(); 
					delete session.getUsers()[i];
				    }
				    session.getUsers().clear();
				} else {
				    selector.remove(session.getUsers()[tmp]->getSocket());
				    session.getUsers()[tmp]->getSocket().disconnect();
				    delete session.getUsers()[tmp];
				    session.getUsers().erase(session.getUsers().begin()+tmp);
				}
                                break;
			    case FP_KILL:
				std::cout << logname << "Deconnection des clients. Arret immediat. (FP_KILL)" << std::endl;
				listener.close();
				for(sf::Uint32 i=0 ; i<session.getUsers().size() ; i++)
				    session.getUsers()[i]->getSocket().disconnect();
				return;
				break;
                            }//FIN SWITCH
			    // UNEXPECTED EXIT
                        } else {
			    if(info->getTakenSlots() > 0)
				info->setTakenSlots(info->getTakenSlots() - 1);
			    if(user.getName() == "") {
				tmp_string = "Une connexion s'est fermee.";
				for(sf::Uint32 i=0 ; i<session.getUsers().size() ; i++)
				    if(session.getUsers()[i]->getColor() == user.getColor())
					tmp = i;
			    } else {
				tmp_string = user.getName()+" exited unexpectedly.";
				packet.clear();
				packet << FP_SAY << (sf::String) (TextContainer::ITALIC+tmp_string+TextContainer::RESET);
				for(sf::Uint32 i=0 ; i<session.getUsers().size() ; i++) {
				    if(session.getUsers()[i]->getColor() == user.getColor())
					tmp = i;
				    //else
				    session.getUsers()[i]->getSocket().send(packet);
				}
                            }
			    std::cout << logname << (std::string) tmp_string 
				      << "(" << std::to_string(info->getTakenSlots())
				      << "/" << std::to_string(info->getTotalSlots()) << ")" 
				      << std::endl;
			    if(user.getIsHost()) {
				already_hosting = false;
				std::cout << logname 
					  << "Le salon n'a plus d'hote et a ete detruit."
					  << std::endl;
				for(sf::Uint32 i=0 ; i<session.getUsers().size() ; i++) {
				    selector.remove(session.getUsers()[i]->getSocket());
				    session.getUsers()[i]->getSocket().disconnect(); 
				    delete session.getUsers()[i];
				}
				session.getUsers().clear();
			    } else {
				selector.remove(user.getSocket());
				user.getSocket().disconnect();
				delete session.getUsers()[tmp];
				session.getUsers().erase(session.getUsers().begin()+tmp);
			    }
                            break;
                        }
                    }
                }
            }
        }
    }
}
/*
sf::SocketSelector Server::socket_selector;
sf::TcpListener Server::tcp_listener;

ServerInfo& Server::getServerInfo() {return info;}
void Server::setServerInfo(ServerInfo i) {info = i;}
sf::SocketSelector& Server::getSocketSelector() {return socket_selector;}
sf::TcpListener& Server::getTcpListener() {return tcp_listener;}
*/
