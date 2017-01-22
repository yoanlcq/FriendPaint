#ifndef GUI_HPP
#define GUI_HPP

#include <SFML/Graphics.hpp>
#include "PaintEngine.hpp"





//COMPOSITE WIDGET STUFF







class WidgetNode : public sf::Drawable {
protected:
    WidgetNode *parent;
    bool has_focus;
    sf::RectangleShape rectangle_shape;
    sf::FloatRect insets;
    void recomputeRectangle();
public:
    WidgetNode();
    virtual ~WidgetNode();
    void setParent(WidgetNode* node);
    WidgetNode* getParent();
    void setRectangleShape(sf::RectangleShape);
    sf::RectangleShape& getRectangleShape();
    void setInsets(sf::FloatRect left_top_right_bottom);
    sf::FloatRect& getInsets();
    void tellWindowSize(sf::Vector2u size);
    virtual void focus();
    virtual void unfocus();
    virtual void update();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    virtual void handleEvent(sf::Event e);
};





class WidgetContainer : public WidgetNode {
private:
    std::vector<WidgetNode*> children;
    WidgetNode *dialog;
    sf::RectangleShape dialog_veil;
public:
    WidgetContainer(); /* default : stick to TOP */
    ~WidgetContainer();
    void add(WidgetNode* node);
    void remove(WidgetNode* node);
    void removeAll() ;
    WidgetNode* getChild(sf::Uint32 index);
    void openDialog(WidgetNode* dialg);
    void closeDialog();
    virtual void focus();
    virtual void unfocus();
    virtual void update();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    virtual void handleEvent(sf::Event e);
};





class Widget : public WidgetNode {
public:
    Widget();
    ~Widget();
    virtual void focus();
    virtual void unfocus();
    virtual void update();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    virtual void handleEvent(sf::Event e);
};






// TEXT STUFF







class TextContainer : public Widget {
protected:
    std::vector<sf::String> lines;
    std::vector<std::vector<sf::Text> > texts;
    sf::Color default_text_color;
    sf::Font default_font;
    sf::Uint8 text_size;
    sf::Uint16 history_length;
    void recomputeTexts();
public:
    TextContainer();
    ~TextContainer();
    void setDefaultTextColor(sf::Color);
    sf::Color getDefaultTextColor();
    void setDefaultFont(sf::Font);
    sf::Font getDefaultFont();
    void setTextSize(sf::Uint8);
    sf::Uint8 getTextSize();
    void setHistoryLength(sf::Uint16);
    sf::Uint16 getHistoryLength();
    void println(sf::String str);
    std::vector<sf::String> getLines();

    static sf::String ITALIC;
    static sf::String ITALIC_OFF;
    static sf::String RGB(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b);
    static sf::String RESET;
    static sf::String CLEAR;

    virtual void focus();
    virtual void unfocus();
    virtual void update();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    virtual void handleEvent(sf::Event e);
};

class TextCursor : public sf::RectangleShape {
private:
    sf::Time blink_time;
    sf::Clock blink_clock;
public:
    TextCursor();
    ~TextCursor();
    sf::Time getBlinkTime();
    void setBlinkTime(sf::Time);
    sf::Clock& getBlinkClock();
};


class TextField : public Widget {
private:
    sf::String placeholder;
    sf::String line;
    sf::Text text;
    sf::Color default_text_color;
    TextCursor text_cursor;
    sf::Uint32 cursor_position;
    sf::Uint32 text_offset;
    sf::Font default_font;
    sf::Uint8 text_size;
    void recompute();
    void (*actionOnInput)();
    void (*actionOnValidate)();
public:
    TextField();
    ~TextField();
    TextCursor& getTextCursor();
    void setTextCursor(TextCursor);
    static void defaultActionOnInput();
    static void defaultActionOnValidate();
    void setActionOnInput(void (*func)());
    void setActionOnValidate(void (*func)());
    void clear(sf::String s="");
    sf::String getLine();
    sf::String getPlaceholder();
    void setPlaceholder(sf::String);
    void setDefaultTextColor(sf::Color);
    sf::Color getDefaultTextColor();
    void setDefaultFont(sf::Font);
    sf::Font getDefaultFont();
    void setTextSize(sf::Uint8);
    sf::Uint8 getTextSize();
    virtual void focus();
    virtual void unfocus();
    virtual void update();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    virtual void handleEvent(sf::Event e);
};






//BUTTON STUFF






class Picture {
protected:
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;
public:
    Picture();
    ~Picture();
    sf::Image& getImage();
    void setImage(sf::Image);
    sf::Texture& getTexture();
    void setTexture(sf::Texture);
    sf::Sprite& getSprite();
    void setSprite(sf::Sprite);
};

class Button : public Widget {
protected:
    sf::Color color;
    bool is_pressed;
    void (*action)();
public:
    Button();
    ~Button();
    static void defaultAction();
    void setAction(void (*func)());
    void tellColor(sf::Color);
    virtual void focus();
    virtual void unfocus();
    virtual void update();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    virtual void handleEvent(sf::Event e);
};





class IconButton : public Button, public Picture {
public:
    IconButton();
    IconButton(sf::String img_path);
    ~IconButton();
    virtual void focus();
    virtual void unfocus();
    virtual void update();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    virtual void handleEvent(sf::Event e);
  
};







class TextButton : public Button {
private:
    sf::String str;
    sf::Font font;
    sf::Text text;
public:
    TextButton(sf::String txt="");
    ~TextButton();
    void clear(sf::String txt="");
    virtual void focus();
    virtual void unfocus();
    virtual void update();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    virtual void handleEvent(sf::Event e);
};





//SLIDER





class Slider : public Widget {
private:
    sf::CircleShape handle;
    bool is_horizontal;
    bool is_dragged;
    float percentage;
    float value;
    float (*calculator)(float);
    void recomputeHandle();
public:
    Slider(bool is_horizontal_);
    ~Slider();
    static float defaultCalculator(float in);
    void setValueCalculator(float (*func)(float));
    void setPercentage(float);
    float getValue();
    virtual void focus();
    virtual void unfocus();
    virtual void update();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    virtual void handleEvent(sf::Event e);
};






//CANVAS WIDGET






class CanvasWidget : public Widget {
private:
    ClientSession* session;
    sf::RenderWindow* parent_window; //Parce que le Viewport se définit relativement à la fenetre...
    sf::View canvas_view;
    Picture flattened_canvas;
    Picture alpha_grid;
    void fillAlphaChecker(sf::Uint32 side, sf::Color first, sf::Color second);
    float zoom_ratio;
    float zoom_delta;
    //Pour déplacer le canevas.
    bool is_moving;
    sf::Vector2i moving_pos;
public:
    CanvasWidget(sf::RenderWindow *window);
    ~CanvasWidget();
    void setSession(ClientSession *sess);
    sf::View& getCanvasView();
    void setCanvasView(sf::View);
    float getZoomRatio();
    void setZoomRatio(float);
    float getZoomDelta();
    void setZoomDelta(float);
    void zoomIn();
    void zoomOut();
    virtual void focus();
    virtual void unfocus();
    virtual void update();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    virtual void handleEvent(sf::Event e);
};




#endif // GUI_HPP
