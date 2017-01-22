#include <iostream>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include "GUI.hpp"
#include "interface.hpp"

WidgetContainer dialogtest;
WidgetContainer gui;

sf::IntRect iface;
sf::RectangleShape theme;

/* Left panel */
WidgetContainer left_panel;
IconButton pen_btn("../../data/icons/pen.png"),
  eraser_btn("../../data/icons/eraser.png");
Slider tool_diameter_slider(false);

/* Top Panel */
WidgetContainer top_panel;

IconButton logo_btn("../../data/icons/friendpaint.png"),
  create_canvas_btn("../../data/icons/create_canvas.png"),
  host_btn("../../data/icons/host.png"),
  join_btn("../../data/icons/join.png"),
  save_btn("../../data/icons/save.png");

/* Right panel */
WidgetContainer right_panel;

/* Right panel : Chat */
TextContainer chat, chat_prompt;
TextField chat_input;

/* Right panel : Color selection*/
WidgetContainer color_selection_panel;
Widget color_widget;
Slider R_slider(true), G_slider(true), B_slider(true);
TextField R_input, G_input, B_input;

/* Right panel : Locks, etc*/
WidgetContainer locks_panel;
std::vector<Button> user_ctrl_buttons;
TextContainer user_ctrl_name;
IconButton lock_create_btn("../../data/icons/lock_create.png"),
  lock_rename_btn("../../data/icons/lock_rename.png"),
  lock_edit_btn("../../data/icons/lock_edit.png"),
  lock_delete_btn("../../data/icons/lock_delete.png"),
  lock_host_btn("../../data/icons/lock_host.png"),
  lock_admin_btn("../../data/icons/lock_admin.png"),
  exit_btn("../../data/icons/exit.png"),
  create_layer_btn("../../data/icons/create_layer.png"),
  rename_layer_btn("../../data/icons/rename_layer.png"),
  merge_down_btn("../../data/icons/merge_down.png"),
  delete_layer_btn("../../data/icons/delete_layer.png");

/* Right panel : Layer stack*/
WidgetContainer layer_stack_panel;
WidgetContainer layer_panels[256];
IconButton eye_btns[256];
TextButton layer_name_btns[256];


/* Bottom panel */
WidgetContainer bottom_panel;
TextContainer info_bar;

/* Center panel */
WidgetContainer center_panel;
CanvasWidget canvas_widget(NULL);



//create canvas
WidgetContainer create_canvas_dialog;
TextContainer title_text, width_text, height_text, predef_text;
TextField title_input, width_input, height_input;
TextButton A4_btn("A4"), A6_btn("A6"), create_btn("Create");

bool is_connected=false;

//join
WidgetContainer join_dialog;
TextContainer lobby_text,slots_text,slots_num_text1;
TextButton lobby_name_text1("<uninitialized>");

//connect
WidgetContainer connect_dialog;
TextContainer username_text, address_text, port_text;
TextField username_input, address_input, port_input;
TextButton connect_btn("Connect");


//Host
WidgetContainer host_dialog;
TextContainer password_text, description_text, num_slots_text;
TextField password_input, description_input, num_slots_input;
TextButton host_btn_dialog("Host");



/* Déclaration des fonctions callback */


void onPen() {
    Client::session.getUsers()[0]->sendToolType(User::ToolType::PEN);
}

void onEraser() {
    Client::session.getUsers()[0]->sendToolType(User::ToolType::ERASER);
}

void updateLayersPanel() {

    layer_stack_panel.removeAll();

    for(sf::Int32 i=Client::session.getLayers().size()-1 ; i>=0 ; i--) {
	if(i >= 256)
	    i = 255;
	layer_panels[i] = WidgetContainer();
	eye_btns[i] = IconButton("../../data/icons/visible.png");
	eye_btns[i].setRectangleShape(locks_panel.getRectangleShape());
	eye_btns[i].tellColor(sf::Color(36,36,36));

	layer_name_btns[i].setRectangleShape(locks_panel.getRectangleShape());
	layer_name_btns[i].tellColor(sf::Color(36,36,36));

	eye_btns[i].setInsets(sf::FloatRect(0, 0, -20, 0));
	layer_name_btns[i].setInsets(sf::FloatRect(20, 0, 0, -24));
	layer_panels[i].setInsets(sf::FloatRect(0, i*24, 0, -24));

	layer_panels[i].add(&eye_btns[i]);
	layer_panels[i].add(&layer_name_btns[i]);
	layer_stack_panel.add(&layer_panels[i]);

	layer_name_btns[i].update();
	layer_name_btns[i].clear(Client::session.getLayers()[i].getName());
	layer_name_btns[i].update();
    }
}

bool onConnectToServer() {
    ServerInfo info;
    info = Client::connectToServer(sf::IpAddress(address_input.getLine()), 
				   (unsigned short) strtol(port_input.getLine().toAnsiString().c_str(), NULL, 0), 
				   username_input.getLine());
    if(!info.getAccepting()) {
	chat.println(TextContainer::RGB(255, 0, 0) 
			 + "Could not reach server."
			 + TextContainer::RESET);
    } else {
	chat.println(TextContainer::RGB(0, 255, 0) 
			 + "Server reached !");
	chat.println("You've been disconnected from your current session."
			 + TextContainer::RESET);
	host_btn.setAction(&display_host);
	join_btn.setAction(&display_join);
    }
    gui.closeDialog();
    return info.getAccepting();
}

void display_host() {
    gui.openDialog(&host_dialog);
}

void display_join() {
    gui.openDialog(&join_dialog);
}

void onConnectToServerFromHostButton() {
    if(onConnectToServer())
	display_host();
}

void onConnectToServerFromJoinButton() {
    if(onConnectToServer())
	display_join();
}

void onHost() {
    std::cout << "Asking to host lobby..." << std::endl;
    if(!Client::requestHostLobby(""/*password_input.getLine()*/,  //FTS
			 description_input.getLine(), 
			 (sf::Uint8) strtol(num_slots_input.getLine().toAnsiString().c_str(), NULL, 0), 
			 false)) {
	std::cout << "Server refused to host lobby." << std::endl;
    } else {
	std::cout << "It's painting time !" << std::endl;
    }
    gui.closeDialog();
}

void onJoin() {
    std::cout << "Asking to join lobby..." << std::endl;
    if(!Client::requestJoinLobby(0, "")) {
	std::cout << "Couldn't join lobby." << std::endl;
    } else {
	std::cout << "It's painting time !" << std::endl;
    }
    gui.closeDialog();
}


void display_create_canvas() {
    gui.openDialog(&create_canvas_dialog);
}

void display_connect_from_host_button() {
    connect_btn.setAction(&onConnectToServerFromHostButton);
    gui.openDialog(&connect_dialog);
}

void display_connect_from_join_button() {
    connect_btn.setAction(&onConnectToServerFromJoinButton);
    gui.openDialog(&connect_dialog);
}


void onA4() {
    width_input.clear( std::to_string(Session::A4.x));
    height_input.clear(std::to_string(Session::A4.y));
}
void onA6() {
    width_input.clear( std::to_string(Session::A6.x));
    height_input.clear(std::to_string(Session::A6.y));
}
void onValidateCreateCanvas() {
    Client::createOfflineSession(title_input.getLine(), sf::Vector2u(std::stoul((std::string)width_input.getLine()), std::stoul((std::string)height_input.getLine())));
    canvas_widget.setSession(&Client::session);
    gui.closeDialog();
}

void onChatInputValidated() {
    if(chat_input.getLine() != "") {
	sf::Color c = Client::session.getUsers()[0]->getColor();
	Client::session.say(TextContainer::RGB(c.r, c.g, c.b) 
			    + Client::session.getUsers()[0]->getName()
			    + TextContainer::RESET
			    + ": " + chat_input.getLine());
	chat_input.clear();
    }
}

void onColorInput() {
    R_slider.setPercentage((float)strtol(R_input.getLine().toAnsiString().c_str(), NULL, 0)/255.0f);
    G_slider.setPercentage((float)strtol(G_input.getLine().toAnsiString().c_str(), NULL, 0)/255.0f);
    B_slider.setPercentage((float)strtol(B_input.getLine().toAnsiString().c_str(), NULL, 0)/255.0f);
}

float onRedSliderAction(float in) {
    float res = roundf(in*255.0f);
    sf::Color col = color_widget.getRectangleShape().getFillColor();
    col.r = res;
    color_widget.getRectangleShape().setFillColor(col);
    R_input.clear(std::to_string((sf::Uint8)res));
    Client::session.getUsers()[0]->sendToolColor(col);
    return res;
}

float onGreenSliderAction(float in) {
    float res = roundf(in*255.0f);
    sf::Color col = color_widget.getRectangleShape().getFillColor();
    col.g = res;
    color_widget.getRectangleShape().setFillColor(col);
    G_input.clear(std::to_string((sf::Uint8)res));
    Client::session.getUsers()[0]->sendToolColor(col);
    return res;
}

float onBlueSliderAction(float in) {
    float res = roundf(in*255.0f);
    sf::Color col = color_widget.getRectangleShape().getFillColor();
    col.b = res;
    color_widget.getRectangleShape().setFillColor(col);
    B_input.clear(std::to_string((sf::Uint8)res));
    Client::session.getUsers()[0]->sendToolColor(col);
    return res;
}

float onToolDiameterAction(float in) {
    float res = roundf((1.0f-in)*100.0f);
    if(res <= 0)
	res = 1;
    Client::session.getUsers()[0]->sendToolDiameter(res);
    return res;
}

void onSave() {
    sf::String name = "../../" + Client::session.getTitle() + ".png";
    chat.println("Saving canvas to \"" + name + "\"...");
    Client::session.getFlattenedImage().saveToFile(name);
    chat.println("Done !");
}

void onExit() {
    Client::endSession();
    canvas_widget.setSession(NULL);
}



/* Interface initialization */


void init_dialogs() {
    sf::RectangleShape rs_input, rs_text, rs_btn;

    rs_btn = gui.getRectangleShape();
    rs_input = rs_btn;
    rs_input.setFillColor(rs_input.getFillColor()-sf::Color(12,12,12));
    rs_text = rs_btn;
    rs_text.setOutlineThickness(0);

	
	
    //CREATE CANVAS
    create_canvas_dialog.setRectangleShape(rs_btn);
    create_canvas_dialog.setInsets(sf::FloatRect(iface.left, iface.top, -320, -110));
    
    title_text.setRectangleShape(rs_text);
    width_text.setRectangleShape(rs_text);
    height_text.setRectangleShape(rs_text);
    predef_text.setRectangleShape(rs_text);
    title_input.setRectangleShape(rs_input);
    width_input.setRectangleShape(rs_input);
    height_input.setRectangleShape(rs_input);
    A4_btn.setRectangleShape(rs_btn);
    A6_btn.setRectangleShape(rs_btn);
    create_btn.setRectangleShape(rs_btn);
    A4_btn.tellColor(sf::Color(42,42,42));
    A6_btn.tellColor(sf::Color(42,42,42));
    create_btn.tellColor(sf::Color(42,42,42));
    
    title_text.setInsets(sf::FloatRect(0, 0, -160, -20));
    width_text.setInsets(sf::FloatRect(0, 20, -160, -20));
    height_text.setInsets(sf::FloatRect(0, 40, -160, -20));
    predef_text.setInsets(sf::FloatRect(0, 60, -160, -20));

    title_input.setInsets(sf::FloatRect(160, 0, -160, -20));
    width_input.setInsets(sf::FloatRect(160, 20, -160, -20));
    height_input.setInsets(sf::FloatRect(160, 40, -160, -20));

    A4_btn.setInsets(sf::FloatRect(160, 60, -80, -20));
    A6_btn.setInsets(sf::FloatRect(240, 60, -80, -20));

    create_btn.setInsets(sf::FloatRect(0, 80, 0, -30));

    create_canvas_dialog.add(&title_text);
    create_canvas_dialog.add(&width_text);
    create_canvas_dialog.add(&height_text);
    create_canvas_dialog.add(&predef_text);
    create_canvas_dialog.add(&title_input);
    create_canvas_dialog.add(&width_input);
    create_canvas_dialog.add(&height_input);
    create_canvas_dialog.add(&A4_btn);
    create_canvas_dialog.add(&A6_btn);
    create_canvas_dialog.add(&create_btn);


    title_text.println("Title :");
    width_text.println("width :");
    height_text.println("height :");
    predef_text.println("Default formats :");
    title_input.clear("Untitled");
    width_input.clear("200");
    height_input.clear("160");

    create_canvas_btn.setAction(&display_create_canvas);
    A4_btn.setAction(&onA4);
    A6_btn.setAction(&onA6);
    create_btn.setAction(&onValidateCreateCanvas);


    //LOBBY_CONNECT_DIALOG
    join_dialog.setRectangleShape(rs_btn);
    join_dialog.setInsets(sf::FloatRect(iface.left, iface.top, -320, -110));
	
    lobby_text.setRectangleShape(rs_text);
    slots_text.setRectangleShape(rs_text);
    lobby_name_text1.setRectangleShape(rs_text);
    slots_num_text1.setRectangleShape(rs_text);
	
    lobby_text.setInsets(sf::FloatRect(0, 0, 50, -20));
    slots_text.setInsets(sf::FloatRect(-50, 0, 0, -20));
    lobby_name_text1.setInsets(sf::FloatRect(0, 20, 50, -20));
    lobby_name_text1.tellColor(sf::Color(42,42,42));
    slots_num_text1.setInsets(sf::FloatRect(-50, 20, 0, -20));
	
    join_btn.setAction(&display_connect_from_join_button);
	
    lobby_text.println("Lobby");
    slots_text.println("Users");
    slots_num_text1.println("---");
    lobby_name_text1.setAction(&onJoin);

    join_dialog.add(&lobby_text);
    join_dialog.add(&slots_text);
    join_dialog.add(&lobby_name_text1);
    join_dialog.add(&slots_num_text1);

	
	
    //CONNECT_DIALOG
    connect_dialog.setRectangleShape(rs_btn);
    connect_dialog.setInsets(sf::FloatRect(iface.left, iface.top, -320, -110));

	
	
    username_text.setRectangleShape(rs_text);
    port_text.setRectangleShape(rs_text);
    address_text.setRectangleShape(rs_text);
    username_input.setRectangleShape(rs_input);
    address_input.setRectangleShape(rs_input);
    port_input.setRectangleShape(rs_input);
	
	
    username_text.setInsets(sf::FloatRect(0, 0, -160, -20));
    address_text.setInsets(sf::FloatRect(0, 20, -160, -20));
    port_text.setInsets(sf::FloatRect(0, 40, -160, -20));
	
    username_input.setInsets(sf::FloatRect(160, 0, -160, -20));
    address_input.setInsets(sf::FloatRect(160, 20, -160, -20));
    port_input.setInsets(sf::FloatRect(160, 40, -160, -20));
	
    connect_btn.tellColor(sf::Color(42,42,42));
    connect_btn.setInsets(sf::FloatRect(0, 80, 0, -30));
	
    username_text.println("Username :");
    address_text.println("Address :");
    port_text.println("Port :");

    username_input.clear("Bob");
    address_input.clear("localhost");	
    port_input.clear((sf::String) std::to_string(FP_DEFAULT_PORT));	
	
    connect_dialog.add(&username_text);
    connect_dialog.add(&address_text);
    connect_dialog.add(&port_text);
    connect_dialog.add(&username_input);
    connect_dialog.add(&address_input);
    connect_dialog.add(&port_input);
    connect_dialog.add(&connect_btn);
	


    //HOST DIALOG
    host_dialog.setRectangleShape(rs_btn);
    host_dialog.setInsets(sf::FloatRect(iface.left, iface.top, -320, -110));

	
    password_text.setRectangleShape(rs_text);
    description_text.setRectangleShape(rs_text);
    num_slots_text.setRectangleShape(rs_text);
    password_input.setRectangleShape(rs_input);
    description_input.setRectangleShape(rs_input);
    num_slots_input.setRectangleShape(rs_input);
	
    password_text.setInsets(sf::FloatRect(0, 0, -160, -20));
    description_text.setInsets(sf::FloatRect(0, 20, -160, -20));
    num_slots_text.setInsets(sf::FloatRect(0, 40, -160, -20));
    password_input.setInsets(sf::FloatRect(160, 0, -160, -20));
    description_input.setInsets(sf::FloatRect(160, 20, -160, -20));
    num_slots_input.setInsets(sf::FloatRect(160, 40, -160, -20));
	
    password_text.println("Password :");
    num_slots_text.println("Num Slots (max 8) :");
    description_text.println("Description :");
    num_slots_input.clear("6");
    description_input.clear("My lobby");
	
    host_btn_dialog.tellColor(sf::Color(42,42,42));
    host_btn_dialog.setInsets(sf::FloatRect(0, 80, 0, -30));
    host_btn_dialog.setAction(&onHost);
	
	
    host_dialog.add(&password_text);
    host_dialog.add(&description_text);
    host_dialog.add(&num_slots_text);
    host_dialog.add(&password_input);
    host_dialog.add(&description_input);
    host_dialog.add(&num_slots_input);
    host_dialog.add(&host_btn_dialog);

    host_btn.setAction(&display_connect_from_host_button);

}


void buildFriendPaintInterface(sf::RenderWindow* win, WidgetContainer *gui, sf::Vector2u winsize) {

    iface = sf::IntRect(40, 40, 256, 20);

    /* theme est un sf::RectangleShape dont le style sera utilisé par la plupart des éléments de l'interface. */
    theme.setPosition(sf::Vector2f(0, 0));
    theme.setFillColor(sf::Color(32, 32, 32));
    theme.setOutlineColor(sf::Color::Black);
    theme.setOutlineThickness(-0.5f);

    gui->setRectangleShape(theme);

    /* Initialisation de left_panel */

    left_panel.setRectangleShape(gui->getRectangleShape());
    left_panel.setInsets(sf::FloatRect(0, iface.top, -iface.left, 0));
    

    pen_btn.setRectangleShape(gui->getRectangleShape());
    pen_btn.tellColor(sf::Color(36,36,36));
    pen_btn.setInsets(sf::FloatRect(0, 0, 0, -iface.left));
    pen_btn.setAction(&onPen);

    eraser_btn.setRectangleShape(gui->getRectangleShape());
    eraser_btn.tellColor(sf::Color(36,36,36));
    eraser_btn.setInsets(sf::FloatRect(0, iface.left, 0, -iface.left));
    eraser_btn.setAction(&onEraser);

    tool_diameter_slider.setRectangleShape(gui->getRectangleShape());
    tool_diameter_slider.setInsets(sf::FloatRect(iface.left/3, 3*iface.left, iface.left/3, -200));
    tool_diameter_slider.setValueCalculator(&onToolDiameterAction);
    tool_diameter_slider.setPercentage(1);

    left_panel.add(&pen_btn);
    left_panel.add(&eraser_btn);
    left_panel.add(&tool_diameter_slider);


    /* Initialisation de top_panel */


    top_panel.setRectangleShape(gui->getRectangleShape());
    top_panel.setInsets(sf::FloatRect(0, 0, 0, -iface.top));

    logo_btn.setRectangleShape(gui->getRectangleShape());
    logo_btn.tellColor(sf::Color(36,36,36));
    logo_btn.setInsets(sf::FloatRect(0, 0, -iface.left, 0));

    create_canvas_btn.setRectangleShape(gui->getRectangleShape());
    create_canvas_btn.tellColor(sf::Color(36,36,36));
    create_canvas_btn.setInsets(sf::FloatRect(iface.left, 0, -50, 0));

    host_btn.setRectangleShape(gui->getRectangleShape());
    host_btn.tellColor(sf::Color(36,36,36));
    host_btn.setInsets(sf::FloatRect(iface.left+50, 0, -50, 0));

    join_btn.setRectangleShape(gui->getRectangleShape());
    join_btn.tellColor(sf::Color(36,36,36));
    join_btn.setInsets(sf::FloatRect(iface.left+100, 0, -50, 0));

    save_btn.setRectangleShape(gui->getRectangleShape());
    save_btn.tellColor(sf::Color(36,36,36));
    save_btn.setInsets(sf::FloatRect(iface.left+150, 0, -50, 0));
    save_btn.setAction(&onSave);
    
    top_panel.add(&logo_btn);
    top_panel.add(&create_canvas_btn);
    top_panel.add(&host_btn);
    top_panel.add(&join_btn);
    top_panel.add(&save_btn);




    /* Initialisation de right_panel : C'est le plus chargé. */




    right_panel.setRectangleShape(gui->getRectangleShape());
    right_panel.setInsets(sf::FloatRect(-iface.width, iface.top, 0, 0));

    /* Ce qui concerne le Tchat */

    chat.setRectangleShape(right_panel.getRectangleShape());
    chat.getRectangleShape().setFillColor(sf::Color(16, 16, 16));
    chat.setInsets(sf::FloatRect(0,0,0,-100));

    chat_input.setRectangleShape(right_panel.getRectangleShape());
    chat_input.getRectangleShape().setFillColor(sf::Color(16, 16, 16));
    chat_input.getRectangleShape().setOutlineThickness(0.5f);
    chat_input.setActionOnValidate(&(onChatInputValidated));
    chat_input.setInsets(sf::FloatRect(20,100,0,-20));

    chat_prompt.setRectangleShape(chat_input.getRectangleShape());
    chat_prompt.setInsets(sf::FloatRect(0,100,-20,-20));

    /* Ce qui concerne le sélécteur de couleur */

    color_selection_panel.setRectangleShape(right_panel.getRectangleShape());
    color_selection_panel.setInsets(sf::FloatRect(0,120,0,-48));

    color_widget.getRectangleShape().setFillColor(sf::Color::Blue);
    color_widget.setInsets(sf::FloatRect(4, 4, -40, 4));

    R_slider = Slider(true);
    R_slider.getRectangleShape().setFillColor(sf::Color(70, 0, 0));
    R_slider.setInsets(sf::FloatRect(48, 2, 40, -12));
    R_input.getRectangleShape().setFillColor(sf::Color(16, 16, 16));
    R_input.setInsets(sf::FloatRect(-40, 0, 0, -16));

    G_slider = Slider(true);
    G_slider.getRectangleShape().setFillColor(sf::Color(0, 50, 0));
    G_slider.setInsets(sf::FloatRect(48, 18, 40, -12));
    G_input.getRectangleShape().setFillColor(sf::Color(16, 16, 16));
    G_input.setInsets(sf::FloatRect(-40, 16, 0, -16));

    B_slider = Slider(true);
    B_slider.getRectangleShape().setFillColor(sf::Color(0, 0, 70));
    B_slider.setInsets(sf::FloatRect(48, 34, 40, -12));
    B_input.getRectangleShape().setFillColor(sf::Color(16, 16, 16));
    B_input.setInsets(sf::FloatRect(-40, 32, 0, -16));

    R_slider.setValueCalculator(&(onRedSliderAction));
    G_slider.setValueCalculator(&(onGreenSliderAction));
    B_slider.setValueCalculator(&(onBlueSliderAction));

    R_input.setActionOnInput(&(onColorInput));
    G_input.setActionOnInput(&(onColorInput));
    B_input.setActionOnInput(&(onColorInput));

    color_selection_panel.add(&R_slider);
    color_selection_panel.add(&G_slider);
    color_selection_panel.add(&B_slider);
    color_selection_panel.add(&R_input);
    color_selection_panel.add(&G_input);
    color_selection_panel.add(&B_input);
    color_selection_panel.add(&color_widget);


    /* Le panneau des verrous et boutons sympas */

    locks_panel.setRectangleShape(right_panel.getRectangleShape());
    locks_panel.setInsets(sf::FloatRect(0,168,0,0));

    user_ctrl_buttons.push_back(Button());
    user_ctrl_buttons.back().tellColor(sf::Color::White);
    
    for(sf::Uint32 i=0 ; i<user_ctrl_buttons.size() ; i++) {
	user_ctrl_buttons[i].setInsets(sf::FloatRect(i*20, 0, -20, -20));
	locks_panel.add(&user_ctrl_buttons[i]);
    }

    user_ctrl_name.setHistoryLength(1);
    user_ctrl_name.setRectangleShape(locks_panel.getRectangleShape());
    user_ctrl_name.setInsets(sf::FloatRect(0, 20, 0, -20));

    lock_create_btn.setRectangleShape(locks_panel.getRectangleShape());
    lock_create_btn.tellColor(sf::Color(36,36,36));
    lock_create_btn.setInsets(sf::FloatRect(1,   40, -35, -26));
    lock_rename_btn.setRectangleShape(locks_panel.getRectangleShape());
    lock_rename_btn.tellColor(sf::Color(36,36,36));
    lock_rename_btn.setInsets(sf::FloatRect(37,  40, -35, -26));
    lock_edit_btn.setRectangleShape(locks_panel.getRectangleShape());
    lock_edit_btn.tellColor(sf::Color(36,36,36));
    lock_edit_btn.setInsets(  sf::FloatRect(73,  40, -35, -26));
    lock_delete_btn.setRectangleShape(locks_panel.getRectangleShape());
    lock_delete_btn.tellColor(sf::Color(36,36,36));
    lock_delete_btn.setInsets(sf::FloatRect(109, 40, -35, -26));
    lock_host_btn.setRectangleShape(locks_panel.getRectangleShape());
    lock_host_btn.tellColor(sf::Color(36,36,36));
    lock_host_btn.setInsets(  sf::FloatRect(145, 40, -35, -26));
    lock_admin_btn.setRectangleShape(locks_panel.getRectangleShape());
    lock_admin_btn.tellColor(sf::Color(36,36,36));
    lock_admin_btn.setInsets( sf::FloatRect(181, 40, -35, -26));
    exit_btn.setRectangleShape(locks_panel.getRectangleShape());
    exit_btn.tellColor(sf::Color(36,36,36));
    exit_btn.setInsets(       sf::FloatRect(217, 40, -35, -26));
    create_layer_btn.setRectangleShape(locks_panel.getRectangleShape());
    create_layer_btn.tellColor(sf::Color(36,36,36));
    create_layer_btn.setInsets(sf::FloatRect(1,   66, -35, -26));
    rename_layer_btn.setRectangleShape(locks_panel.getRectangleShape());
    rename_layer_btn.tellColor(sf::Color(36,36,36));
    rename_layer_btn.setInsets(sf::FloatRect(37,  66, -35, -26));
    merge_down_btn.setRectangleShape(locks_panel.getRectangleShape());
    merge_down_btn.tellColor(sf::Color(36,36,36));
    merge_down_btn.setInsets(  sf::FloatRect(73,  66, -35, -26));
    delete_layer_btn.setRectangleShape(locks_panel.getRectangleShape());
    delete_layer_btn.tellColor(sf::Color(36,36,36));
    delete_layer_btn.setInsets(sf::FloatRect(109, 66, -35, -26));

    exit_btn.setAction(&onExit);


    locks_panel.add(&user_ctrl_name);
    locks_panel.add(&lock_create_btn);
    locks_panel.add(&lock_rename_btn);
    locks_panel.add(&lock_edit_btn);
    locks_panel.add(&lock_delete_btn);
    locks_panel.add(&lock_host_btn);
    locks_panel.add(&lock_admin_btn);
    locks_panel.add(&exit_btn);
    locks_panel.add(&create_layer_btn);
    locks_panel.add(&rename_layer_btn);
    locks_panel.add(&merge_down_btn);
    locks_panel.add(&delete_layer_btn);

    /* La pile de calques */

    layer_stack_panel.setRectangleShape(right_panel.getRectangleShape());
    layer_stack_panel.getRectangleShape().setOutlineThickness(1.0f);
    layer_stack_panel.setInsets(sf::FloatRect(0,262,0,0));


    /* Ajouter tout ça */

    right_panel.add(&locks_panel);
    right_panel.add(&color_selection_panel);
    right_panel.add(&chat);
    right_panel.add(&chat_prompt);
    right_panel.add(&chat_input);
    right_panel.add(&layer_stack_panel);


    /* Initialisation de bottom_panel. */

    bottom_panel.setRectangleShape(gui->getRectangleShape());
    bottom_panel.setInsets(sf::FloatRect(iface.left, -iface.height, iface.width, 0));

    info_bar.setRectangleShape(right_panel.getRectangleShape());
    info_bar.getRectangleShape().setFillColor(sf::Color(16, 16, 16));
    info_bar.setInsets(sf::FloatRect(0,0,0,0));

    bottom_panel.add(&info_bar);


    /* Initialisation de center_panel. */

    center_panel.setRectangleShape(gui->getRectangleShape());
    center_panel.getRectangleShape().setFillColor(sf::Color(12, 12, 12));
    center_panel.setInsets(sf::FloatRect(iface.left, iface.top, iface.width, iface.height));

    canvas_widget = CanvasWidget(win);
    canvas_widget.setSession(NULL);
    canvas_widget.setRectangleShape(center_panel.getRectangleShape());
    canvas_widget.setInsets(sf::FloatRect(0,0,0,0));

    center_panel.add(&canvas_widget);


    gui->add(&center_panel);
    gui->add(&left_panel);
    gui->add(&top_panel);
    gui->add(&right_panel);
    gui->add(&bottom_panel);


    /* Actions à faire une fois l'interface mise en place */

    chat.println(TextContainer::ITALIC + "Welcome, " + Client::session.getUsers()[0]->getName() + "!" + TextContainer::RESET);
    chat.println(TextContainer::ITALIC + "You can get started with the top-left buttons." + TextContainer::RESET);
    chat_prompt.println(">");
    chat_input.setPlaceholder("Chat here !");

    R_slider.setPercentage(0);
    G_slider.setPercentage(0);
    B_slider.setPercentage(0);

    user_ctrl_name.println(TextContainer::RGB(255, 255, 255) + Client::session.getUsers()[0]->getName() + TextContainer::RESET + "'s locks");

    /* Ne pas oublier de faire ça à la fin */
    gui->tellWindowSize(winsize);
    init_dialogs();
}
