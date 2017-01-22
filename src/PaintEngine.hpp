#ifndef PAINTENGINE_HPP
#define PAINTENGINE_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>



#define FP_VERSION "project"
#define FP_DEFAULT_PORT 63360
#define FP_SOLO_PORT 63310


/* Dans un enum, on n'est pas obligé d'initialiser les valeurs.
   Seulement, les valeurs par défaut varient selon le compilateurs.
   Ainsi, si un jour le client et le serveur sont compilés séparéments,
   ils risquent de ne pas se comprendre. */

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
	FP_DELETE_LAYER = 23,
	FP_LOCK_EDIT_LAYER = 24,
	FP_UNLOCK_EDIT_LAYER = 25,
	FP_LOCK_DELETE_LAYER = 26,
	FP_UNLOCK_DELETE_LAYER = 27,
      FP_KILL = 28, //Nouveau. Pour tuer le serveur local.
      FP_USERS = 29
	};



sf::Packet& operator <<(sf::Packet& packet, const sf::Image& img);
sf::Packet& operator >>(sf::Packet& packet, sf::Image& img);
sf::Packet& operator <<(sf::Packet& packet, const sf::Color& c);
sf::Packet& operator >>(sf::Packet& packet, sf::Color& c);




class User {
public:
    enum ToolType : sf::Uint32 {ERASER=0, PEN=1};
private:
    sf::Color color;
    sf::String name;
    sf::TcpSocket socket;
    bool is_host;
    ToolType tool_type;
    sf::Uint32 tool_diameter;
    sf::Color tool_color;
    sf::Image brush;
    sf::Vector2u last_position_on_canvas;
    sf::Uint32 layer_index;
public:
    User();
    ~User();
    sf::Color getColor();
    void setColor(sf::Color);
    sf::String getName();
    void setName(sf::String);
    sf::TcpSocket &getSocket();
    bool getIsHost();
    void setIsHost(bool);
    ToolType getToolType();
    void setToolType(ToolType);
    void sendToolType(User::ToolType t);
    sf::Uint32 getToolDiameter();
    void setToolDiameter(sf::Uint32);
    void sendToolDiameter(sf::Uint32 d);
    sf::Color getToolColor();
    void setToolColor(sf::Color);
    void sendToolColor(sf::Color c);
    sf::Vector2u getLastPositionOnCanvas();
    void setLastPositionOnCanvas(sf::Vector2u);
    sf::Uint32 getLayerIndex();
    void setLayerIndex(sf::Uint32 i);
    static sf::Image buildCircleImage(sf::Color color, sf::Uint32 diameter, bool fill);
    void updateBrush();
    sf::Image &getBrush();
    friend sf::Packet& operator <<(sf::Packet& packet, const User& user);
    friend sf::Packet& operator >>(sf::Packet& packet, User& user);
};



class Layer {
protected:
    sf::Image image;
    sf::String name;
    void copyImageFromCenter(sf::Image &dest, sf::Image &src, sf::Vector2u center, bool erase); //Fonction utilitaire.
public:
    Layer();
    ~Layer();
    void applyBrush(sf::Vector2u from, sf::Vector2u to, sf::Image brush, bool erase);
    sf::Image& getImage();
    void setImage(sf::Image);
    sf::String getName();
    void setName(sf::String);
    friend sf::Packet& operator <<(sf::Packet& packet, const Layer& layer);
    friend sf::Packet& operator >>(sf::Packet& packet, Layer& layer);
};

class ServerLayer : public Layer {
public:
    ServerLayer();
    ~ServerLayer();
};

class ClientLayer : public Layer, public sf::Drawable {
private:
    sf::Texture texture;
    sf::Sprite sprite;
    bool is_selected;
    bool is_visible;
    bool lock_edit;
    bool lock_delete;
public:
    ClientLayer();
    ~ClientLayer();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void setIsVisible(bool);
    bool getIsVisible();
    void setIsSelected(bool);
    bool getIsSelected();
    void setLockEdit(bool);    
    bool getLockEdit();
    void setLockDelete(bool);
    bool getLockDelete();
};



class Session {
protected:
    sf::String title;
    sf::Vector2u size;
    std::vector<User*> users;
public:
    Session();
    ~Session();
    sf::String getTitle();
    void setTitle(sf::String);
    sf::Vector2u getSize();
    void setSize(sf::Vector2u);
    std::vector<User*> &getUsers();
    static const sf::Vector2u A4;
    static const sf::Vector2u A6;
    virtual void createLayer(sf::Uint32 index, sf::String new_name="New Layer");
    virtual void moveLayer(sf::Uint32 index, sf::Int32 delta);
    virtual void renameLayer(sf::Uint32 index, sf::String new_name);
    virtual void deleteLayer(sf::Uint32 index);
    virtual void mergeDownLayer(sf::Uint32 index);
};

class ServerSession : public Session {
private:
    sf::String description;
    sf::Uint8 num_slots;
    sf::String password;
    bool ask_before_accept;
    std::vector<ServerLayer> layers;
public:
    ServerSession();
    ~ServerSession();
    sf::String getDescription();
    void setDescription(sf::String);
    sf::Uint8 getNumSlots();
    void setNumSlots(sf::Uint8);
    sf::String getPassword();
    void setPassword(sf::String);
    bool getAskBeforeAccept();
    void setAskBeforeAccept(bool);
    std::vector<ServerLayer> &getLayers();
    virtual void createLayer(sf::Uint32 index, sf::String new_name="New Layer");
    virtual void moveLayer(sf::Uint32 index, sf::Int32 delta);
    virtual void renameLayer(sf::Uint32 index, sf::String new_name);
    virtual void deleteLayer(sf::Uint32 index);
    virtual void mergeDownLayer(sf::Uint32 index);
};


class ClientSession : public Session {
private:
    sf::Thread listening_thread;
    sf::Mutex socket_mutex, gui_mutex;
    bool is_admin;
    sf::Color controlled_user_color;
    bool tool_dragged;
    bool lock_create;
    std::vector<ClientLayer> layers;
public:
    ClientSession();
    ~ClientSession();
    static void listeningThreadFunc(ClientSession* session);
    sf::Thread &getListeningThread();
    sf::Mutex &getSocketMutex();
    sf::Mutex &getGuiMutex();
    void say(sf::String txt);
    bool getIsAdmin();
    void setIsAdmin(bool);
    sf::Color getControlledUserColor();
    void setControlledUserColor(sf::Color);
    void grantAdminToControlledUser();
    void grantHostToControlledUser();
    void kickControlledUser();
    bool getToolDragged();
    void beginToolDrag(sf::Vector2u position_on_canvas);
    void stepToolDrag(sf::Vector2u position_on_canvas);
    void endToolDrag(sf::Vector2u position_on_canvas);
    bool getLockCreate();
    void setLockCreate(bool);
    void sendLockCreate(bool b);
    std::vector<ClientLayer> &getLayers();
    void sendCreateLayer(sf::Uint32 index, sf::String new_name="New Layer");
    void sendMoveLayer(sf::Uint32 index, sf::Int32 delta);
    void sendRenameLayer(sf::Uint32 index, sf::String new_name);
    void sendDeleteLayer(sf::Uint32 index);
    void sendMergeDownLayer(sf::Uint32 index);
    virtual void createLayer(sf::Uint32 index, sf::String new_name="New Layer");
    virtual void moveLayer(sf::Uint32 index, sf::Int32 delta);
    virtual void renameLayer(sf::Uint32 index, sf::String new_name);
    virtual void deleteLayer(sf::Uint32 index);
    virtual void mergeDownLayer(sf::Uint32 index);
    sf::Image getFlattenedImage();
};








class ServerInfo {
private:
    sf::String session_title;
    sf::Uint8 taken_slots;
    sf::Uint8 total_slots;
    sf::String version;
    sf::Uint16 max_lobby_description_length;
    sf::Uint8 max_lobby_slots;
    sf::Uint8 max_title_length;
    sf::Uint8 max_password_length;
    sf::Uint8 max_username_length;
    unsigned short port;
    bool accepting;
public:
    ServerInfo();
    ~ServerInfo();
    sf::String getSessionTitle();
    sf::Uint8 getTakenSlots();
    sf::Uint8 getTotalSlots();
    sf::String getVersion();
    sf::Uint16 getMaxLobbyDescriptionLength();
    sf::Uint8 getMaxLobbySlots();
    sf::Uint8 getMaxTitleLength();
    sf::Uint8 getMaxPasswordLength();
    sf::Uint8 getMaxUsernameLength();
    unsigned short getPort();
    bool getAccepting();
    void setSessionTitle(sf::String);
    void setTakenSlots(sf::Uint8);
    void setTotalSlots(sf::Uint8);
    void setMaxLobbyDescriptionLength(sf::Uint16);
    void setMaxLobbySlots(sf::Uint8);
    void setMaxTitleLength(sf::Uint8);
    void setMaxPasswordLength(sf::Uint8);
    void setMaxUsernameLength(sf::Uint8);
    void setPort(unsigned short);
    void setAccepting(bool);
    friend sf::Packet& operator <<(sf::Packet& packet, const ServerInfo& info);
    friend sf::Packet& operator >>(sf::Packet& packet, ServerInfo& info);
};

class Client {
private:
    Client();
public:
    static ClientSession session;
    static const sf::String version; //ADDED
    static sf::Thread offline_server_thread;
    static ServerInfo offline_server_info;
    static void endSession();
    static void endOfflineServerThread();
    static void createOfflineSession(sf::String title, sf::Vector2u size);
    static ServerInfo connectToServer(sf::IpAddress remote_adress, unsigned short remote_port, sf::String username);
    static bool requestHostLobby(sf::String password, sf::String description, sf::Uint8 num_slots, bool ask_before_accept);
    static bool requestJoinLobby(sf::Uint32 index, sf::String password);
};



class Server {
private:
    Server();
public:
    static ServerInfo info;
    static void serverLoop(ServerInfo* info);
/*
    static sf::SocketSelector socket_selector;
    static sf::TcpListener tcp_listener;
    static ServerInfo& getServerInfo();
    static void setServerInfo(ServerInfo);
    static sf::SocketSelector& getSocketSelector();
    static sf::TcpListener& getTcpListener();
*/
};
#endif //PAINTENGINE_HPP
