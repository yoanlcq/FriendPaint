#ifndef INTERFACE_HPP
#define INTERFACE_HPP


extern WidgetContainer dialogtest;
extern WidgetContainer gui;

extern sf::IntRect iface;
extern sf::RectangleShape theme;

/* Left panel */
extern WidgetContainer left_panel;
extern IconButton pen_btn, eraser_btn;
extern Slider tool_diameter_slider;

/* Top Panel */
extern WidgetContainer top_panel;

extern IconButton logo_btn, create_canvas_btn, host_btn, join_btn, save_btn;

/* Right panel */
extern WidgetContainer right_panel;

/* Right panel : Chat */
extern TextContainer chat, chat_prompt;
extern TextField chat_input;

/* Right panel : Color selection*/
extern WidgetContainer color_selection_panel;
extern Widget color_widget;
extern Slider R_slider, G_slider, B_slider;
extern TextField R_input, G_input, B_input;

/* Right panel : Locks, etc*/
extern WidgetContainer locks_panel;
extern std::vector<Button> user_ctrl_buttons;
extern TextContainer user_ctrl_name;
extern IconButton lock_create_btn,
  lock_rename_btn,
  lock_edit_btn,
  lock_delete_btn,
  lock_host_btn,
  lock_admin_btn,
  exit_btn,
  create_layer_btn,
  rename_layer_btn,
  merge_down_btn,
  delete_layer_btn;

/* Right panel : Layer stack*/
extern WidgetContainer layer_stack_panel;
extern WidgetContainer layer_panels[256];
extern IconButton eye_btns[256];
extern TextButton layer_name_btns[256];

void updateLayersPanel();

/* Bottom panel */
extern WidgetContainer bottom_panel;
extern TextContainer info_bar;

/* Center panel */
extern WidgetContainer center_panel;
extern CanvasWidget canvas_widget;



//create canvas
extern WidgetContainer create_canvas_dialog;
extern TextContainer title_text, width_text, height_text, predef_text;
extern TextField title_input, width_input, height_input;
extern TextButton A4_btn, A6_btn, create_btn;

extern bool is_connected;

//join
extern WidgetContainer join_dialog;
extern TextContainer lobby_text,slots_text,slots_num_text1, slots_num_text2, slots_num_text3, slots_num_text4;
extern TextButton lobby_name_text1, lobby_name_text2, lobby_name_text3, lobby_name_text4;

//connect
extern WidgetContainer connect_dialog;
extern TextContainer username_text, address_text, port_text;
extern TextField username_input, address_input, port_input;
extern TextButton connect_btn;


//Host
extern WidgetContainer host_dialog;
extern TextContainer password_text, description_text, num_slots_text;
extern TextField password_input, description_input, num_slots_input;
extern TextButton host_btn_dialog;



void display_create_canvas();
void display_connect();
void display_host();
void display_join();
bool onConnectToServer();
void onA4();
void onA6();
void onValidateCreateCanvas();
void onChatInputValidated();
void onColorInput();
float onRedSliderAction(float in);
float onGreenSliderAction(float in);
float onBlueSliderAction(float in);

void init_dialogs();
void buildFriendPaintInterface(sf::RenderWindow* win, WidgetContainer *gui, sf::Vector2u winsize);

#endif // INTERFACE_HPP
