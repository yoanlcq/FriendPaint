#include <SFML/Graphics.hpp>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sstream>
#define _USE_MATH_DEFINES
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif
#include "prisma.h"
#include "GUI.hpp"
#include "PaintEngine.hpp"
#include "interface.hpp"





//===< WidgetNode source >===







WidgetNode::WidgetNode() {
    has_focus = false;
    parent = NULL;
    rectangle_shape.setPosition(0, 0);
}

WidgetNode::~WidgetNode() {}
void WidgetNode::setRectangleShape(sf::RectangleShape rs) {
    rectangle_shape = rs;
}
sf::RectangleShape& WidgetNode::getRectangleShape() {
    return rectangle_shape;
}

void WidgetNode::setParent(WidgetNode* node) {
    parent = node;
}
WidgetNode* WidgetNode::getParent() {
    return parent;
}

void WidgetNode::recomputeRectangle() {

    sf::FloatRect parent_rect, my;

    if(parent != NULL) {

	parent_rect.left = parent->getRectangleShape().getPosition().x;
	parent_rect.top = parent->getRectangleShape().getPosition().y;
	parent_rect.width = parent->getRectangleShape().getSize().x;
	parent_rect.height = parent->getRectangleShape().getSize().y;
	
	my.left = parent_rect.left + insets.left + (insets.left<0 ? parent_rect.width : 0);
	my.top = parent_rect.top + insets.top + (insets.top<0 ? parent_rect.height : 0);
	my.width = insets.left<0 ? -insets.left : (insets.width<0 ? 0 : parent_rect.width - insets.left) - insets.width;
	my.height = insets.top<0 ? -insets.top : (insets.height<0 ? 0 : parent_rect.height - insets.top) - insets.height;
	
	rectangle_shape.setPosition(sf::Vector2f(my.left, my.top));
	rectangle_shape.setSize(sf::Vector2f(my.width, my.height));
    }
}

void WidgetNode::setInsets(sf::FloatRect left_top_right_bottom) {
    insets = left_top_right_bottom;
}
sf::FloatRect& WidgetNode::getInsets() {
    return insets;
}

void WidgetNode::tellWindowSize(sf::Vector2u size) {
    sf::Event event;
    event.type = sf::Event::Resized;
    event.size.width = size.x;
    event.size.height = size.y;
    handleEvent(event);
}

void WidgetNode::focus() {has_focus = true;}
void WidgetNode::unfocus() {has_focus = false;}
void WidgetNode::update() {recomputeRectangle();}
void WidgetNode::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(rectangle_shape);
}
void WidgetNode::handleEvent(sf::Event e) {}







//===< WidgetContainer source >===








WidgetContainer::WidgetContainer() : WidgetNode() {
    dialog = NULL;
}
WidgetContainer::~WidgetContainer() {}




void WidgetContainer::add(WidgetNode* node) {
    children.push_back(node);
    children.back()->setParent(this);
    update();
}

void WidgetContainer::remove(WidgetNode* node) {
    for(unsigned i=0; i < children.size() ; i++) 
	if(children[i] == node) {
	    children[i]->setParent(NULL);
	    children.erase(children.begin()+i);
	    break;
	}
    update();
}

void WidgetContainer::removeAll() {
    for(unsigned i=0; i < children.size() ; i++)
	children[i]->setParent(NULL);
    children.clear();
    update();
}

WidgetNode* WidgetContainer::getChild(sf::Uint32 index) {
    return children[index];
}

void WidgetContainer::openDialog(WidgetNode* dialg) {
    sf::Color veil;
    dialog = dialg;
    if(dialog != NULL) {
	dialog->setParent(this);
	dialog->update();

	veil = dialog->getRectangleShape().getFillColor();
	veil.a /= 1.2f;
	dialog_veil.setFillColor(veil);

	dialog_veil.setPosition(rectangle_shape.getPosition());
	dialog_veil.setSize(rectangle_shape.getSize());
    }
}

void WidgetContainer::closeDialog() {
    dialog = NULL;
}

void WidgetContainer::focus() {
    WidgetNode::focus();
    for(unsigned i=0 ; i<children.size() ; i++)
	children[i]->focus();
}
void WidgetContainer::unfocus() {
    WidgetNode::unfocus();
    for(unsigned i=0 ; i<children.size() ; i++)
	children[i]->unfocus();
}
void WidgetContainer::update() {
    WidgetNode::update();
    for(unsigned i=0 ; i<children.size() ; i++)
	children[i]->update();
    if(dialog != NULL) {
	dialog_veil.setPosition(rectangle_shape.getPosition());
	dialog_veil.setSize(rectangle_shape.getSize());
	dialog->update();
    }
}
void WidgetContainer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    WidgetNode::draw(target, states);
    for(unsigned i=0 ; i<children.size() ; i++)
	target.draw(*(children[i]), states);
    if(dialog != NULL) {
	target.draw(dialog_veil, states);
	target.draw(*dialog, states);
    }
}

void WidgetContainer::handleEvent(sf::Event e) {

    sf::Vector2f mouse_pos;

    WidgetNode::handleEvent(e);

    if(dialog != NULL) {
	switch(e.type) {
	case sf::Event::Resized:
	case sf::Event::LostFocus:
	case sf::Event::GainedFocus:
	    break;
	case sf::Event::MouseButtonPressed:
	    if(!dialog->getRectangleShape().getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y)) {
		closeDialog();
		break;
	    }
	default: 
	    dialog->handleEvent(e);
	    return;
	    break;
	}
    }

    switch(e.type) {
    case sf::Event::Resized:
	if(parent == NULL) {
	    rectangle_shape.setSize(sf::Vector2f(e.size.width, e.size.height));
	    update();
	}
	break;
    case sf::Event::MouseWheelMoved:
    case sf::Event::MouseButtonPressed:
	mouse_pos = e.type == sf::Event::MouseButtonPressed 
	    ? sf::Vector2f(e.mouseButton.x, e.mouseButton.y) 
	    : sf::Vector2f(e.mouseWheel.x, e.mouseWheel.y);
	for(int i=children.size()-1 ; i>=0 ; i--)
	    children[i]->unfocus();
	for(int i=children.size()-1 ; i>=0 ; i--)
	    if(children[i]->getRectangleShape().getGlobalBounds().contains(mouse_pos)) {
		children[i]->focus();
		break;
	    }
	break;
    }
    for(unsigned i=0 ; i<children.size() ; i++)
	children[i]->handleEvent(e);
}









//===< Widget source >===
   










Widget::Widget() : WidgetNode() {}
Widget::~Widget() {}
void Widget::focus() {WidgetNode::focus();}
void Widget::unfocus() {WidgetNode::unfocus();}
void Widget::update() {WidgetNode::update();}
void Widget::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    WidgetNode::draw(target, states);
}
void Widget::handleEvent(sf::Event e) {
    WidgetNode::handleEvent(e);
}









//===< TextContainer source >===
   





TextContainer::TextContainer() : Widget() {
    default_text_color = sf::Color(200, 200, 200);
    default_font.loadFromFile("../../data/fonts/basis33.ttf");
    text_size = 16;
    history_length = 32;
}
TextContainer::~TextContainer() {}

void TextContainer::println(sf::String str) {
    lines.push_back(str);
    recomputeTexts();
}
void TextContainer::recomputeTexts() {
    sf::Text attributes;
    unsigned max_lines, char_height, total_char_width, padding;
    int j;
    sf::Vector2f con_pos, con_size;
    sf::String str;
    struct PrismaEscapeData data;

    padding = 2;
    char_height = default_font.getLineSpacing(text_size);
    con_pos = rectangle_shape.getPosition();
    con_size = rectangle_shape.getSize();


    max_lines = con_size.y/char_height;
    attributes = sf::Text("", default_font, text_size);
    attributes.setColor(default_text_color);
    texts.clear();
    for(int i = (lines.size()<max_lines ? 0 : lines.size()-max_lines) ; i<lines.size() ; i++) {
	str = lines[i];

	texts.push_back(std::vector<sf::Text>());
	texts.back().push_back(attributes);
	texts.back().back().setPosition(sf::Vector2f(con_pos.x + padding, 0));
	total_char_width = 0;
	for(j=0 ; j<str.getSize() ; j += (data.length==0 ? 1 : data.length)) {
	    data = prisma_etod(&(str.getData()[j]), sizeof(sf::Uint32));
	    if(data.value == PRISMA__UNSUPPORTED) {
		total_char_width += attributes.getFont()->getGlyph(str[j], text_size, false).advance;
		if(total_char_width >= (con_size.x-2*padding)) {
		    texts.push_back(std::vector<sf::Text>());
		    texts.back().push_back(attributes);
		    texts.back().back().setPosition(sf::Vector2f(con_pos.x + padding, 0));
		    total_char_width = 0;
		}
		texts.back().back().setString(texts.back().back().getString() + sf::String(str[j]));
	    } else {
		switch(data.value) {
		case PRISMA__FG_EXTENDED: 
		    attributes.setColor(sf::Color(data.r, data.g, data.b));
		    break;
		case PRISMA__RESET: 
		    attributes.setStyle(sf::Text::Regular);
		    //No break
		case PRISMA__FG_DEFAULT: 
		    attributes.setColor(default_text_color);
		    break;
		case PRISMA__ITALIC: 
		    attributes.setStyle(attributes.getStyle() | sf::Text::Italic);
		    break;
		case PRISMA__ITALIC_OFF: 
		    attributes.setStyle(attributes.getStyle() & ~sf::Text::Italic);
		    break;
		case PRISMA__CLEAR: 
		    lines.clear();
		    println(str.substring(j+data.length));
		    return;
		    break;
		}
		texts.back().push_back(attributes);
		texts.back().back().setPosition(sf::Vector2f(con_pos.x + padding + total_char_width, 0));
	    }
	}
    }

    if(texts.size() > max_lines) {
	texts.erase(texts.begin(), texts.end()-max_lines);
    }

    for(unsigned i=0 ; i<texts.size() ; i++) {
	for(unsigned j=0 ; j<texts[i].size() ; j++) {
	    texts[i][j].setPosition(sf::Vector2f(texts[i][j].getPosition().x, con_pos.y + i*char_height));
	}
    }
    if(lines.size() > history_length) {
	lines.erase(lines.begin(), lines.end()-history_length);
    }
}

void TextContainer::setDefaultTextColor(sf::Color color) {
    default_text_color = color;
}
sf::Color TextContainer::getDefaultTextColor() {
    return default_text_color;
}
void TextContainer::setDefaultFont(sf::Font font) {
    default_font = font;
}
sf::Font TextContainer::getDefaultFont() {
    return default_font;
}
void TextContainer::setTextSize(sf::Uint8 size) {
    text_size = size;
}
sf::Uint8 TextContainer::getTextSize() {
    return text_size;
}

void TextContainer::setHistoryLength(sf::Uint16 length) {
    history_length = length;
}
sf::Uint16 TextContainer::getHistoryLength() {
    return history_length;
}

std::vector<sf::String> TextContainer::getLines() {
    return lines;
}

sf::String TextContainer::ITALIC = ((const sf::Uint32*) prisma_dtoe({PRISMA__ITALIC}, sizeof(sf::Uint32)));
sf::String TextContainer::ITALIC_OFF = ((const sf::Uint32*) prisma_dtoe({PRISMA__ITALIC_OFF}, sizeof(sf::Uint32)));
sf::String TextContainer::RGB(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b) { return ((const sf::Uint32*) prisma_dtoe({PRISMA__FG_EXTENDED, r, g, b}, sizeof(sf::Uint32))); }
sf::String TextContainer::RESET = ((const sf::Uint32*) prisma_dtoe({PRISMA__RESET}, sizeof(sf::Uint32)));
sf::String TextContainer::CLEAR = ((const sf::Uint32*) prisma_dtoe({PRISMA__CLEAR}, sizeof(sf::Uint32)));

void TextContainer::focus() {
    //Nothing
}
void TextContainer::unfocus() {
    //Nothing
}
void TextContainer::update() {
    Widget::update();
    if(!lines.empty())
	recomputeTexts();
}
void TextContainer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    Widget::draw(target, states);
    for(unsigned i=0 ; i<texts.size() ; i++)
	for(unsigned j=0 ; j<texts[i].size() ; j++)
	    target.draw(texts[i][j]);
}
void TextContainer::handleEvent(sf::Event e) {
    //Nothing
}









TextCursor::TextCursor() : sf::RectangleShape() {}
TextCursor::~TextCursor() {}
sf::Time TextCursor::getBlinkTime() {return blink_time;}
void TextCursor::setBlinkTime(sf::Time bt) {blink_time = bt;}
sf::Clock& TextCursor::getBlinkClock() {return blink_clock;}












void TextField::defaultActionOnInput() {}
void TextField::defaultActionOnValidate() {}

TextField::TextField() : Widget() {
    default_text_color = sf::Color(200, 200, 200);
    default_font.loadFromFile("../../data/fonts/basis33.ttf");
    text_size = 16;
    text_cursor.setFillColor(default_text_color);
    text_cursor.setSize(sf::Vector2f(1, default_font.getLineSpacing(text_size)));
    text_cursor.setBlinkTime(sf::milliseconds(500));
    cursor_position = text_offset = 0;
    setActionOnInput(TextField::defaultActionOnInput);
    setActionOnValidate(TextField::defaultActionOnValidate);
}
TextField::~TextField() {}
TextCursor& TextField::getTextCursor() {return text_cursor;}
void TextField::setTextCursor(TextCursor tc) {text_cursor = tc;}
void TextField::setActionOnInput(void (*func)()) {
    actionOnInput = func;
}
void TextField::setActionOnValidate(void (*func)()) {
    actionOnValidate = func;
}
void TextField::focus() {
    Widget::focus();
}
void TextField::unfocus() {
    Widget::unfocus();
}

sf::String TextField::getPlaceholder() {return placeholder;}
void TextField::setPlaceholder(sf::String p) {placeholder = p;}

void TextField::recompute() {
    //TODO le texte dépasse à droite !!
    sf::Uint32 max_width, total_char_width;
    sf::Int32 i; //ne pas changer
    sf::String str;

    max_width = rectangle_shape.getSize().x-4;
    text = sf::Text(line.substring(cursor_position < text_offset ? cursor_position : text_offset), default_font, text_size);
    text.setColor(default_text_color);

    if(cursor_position <= text_offset) {
	text_offset = cursor_position;
	total_char_width = max_width;
    } else {
	//On cherche la position graphique du curseur.
	total_char_width = 0;
	for(i=text_offset ; i<cursor_position ; i++) {
	    total_char_width += default_font.getGlyph(line[i], text_size, false).advance;
	}
    }
    if(total_char_width >= max_width) {
	total_char_width = 0;
	for(i=cursor_position-1 ; i>0 ; i--) {
	    total_char_width += default_font.getGlyph(line[i], text_size, false).advance;
	    if(total_char_width+1 >= max_width) {
		text_offset = i+1;
		break;
	    }
	}
	if(i==0) text_offset = 0;
    }

    text.setString(line=="" ? placeholder : line.substring(text_offset));
    text.setStyle(line=="" ? sf::Text::Italic : sf::Text::Regular);
    if(line=="") {
	text.setColor(default_text_color-sf::Color(0,0,0,0.7));
    }

    //Cleanup : Le texte peut dépasser à droite du conteneur.
    for(i=0 ; i<text.getString().getSize() ; i++)
	if(text.findCharacterPos(i).x >= max_width)
	    text.setString(line.substring(text_offset, i)); 

    text.setPosition(rectangle_shape.getPosition());
    text_cursor.setPosition(text.findCharacterPos(cursor_position-text_offset));
}

void TextField::update() {
    Widget::update();
    recompute();
}
void TextField::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    Widget::draw(target, states);
    target.draw(text, states);
    if(has_focus) {
	if(((TextField*) this)->text_cursor.getBlinkClock().getElapsedTime() >= 2.0f*((TextField*) this)->text_cursor.getBlinkTime()) {
	    ((TextField*) this)->text_cursor.getBlinkClock().restart();
	    target.draw(text_cursor, states);
	} else if(((TextField*) this)->text_cursor.getBlinkClock().getElapsedTime() < ((TextField*) this)->text_cursor.getBlinkTime()) {
	    target.draw(text_cursor, states);
	}
    }
}

void TextField::handleEvent(sf::Event e) {
    sf::String str;
    sf::Uint32 offset;
    Widget::handleEvent(e);
    if(has_focus) {
	switch(e.type) {
	case sf::Event::MouseButtonPressed:
	    if(line != "") { //On ne joue pas avec le placeholder.
		str = text.getString();
		offset = rectangle_shape.getPosition().x;
		for(sf::Uint32 i=0 ; i<str.getSize() ; i++) {
		    offset += default_font.getGlyph(str[i], text_size, false).advance;
		    if(e.mouseButton.x <= offset) {
			cursor_position = i;
			break;
		    }
		}
	    }
	    text_cursor.getBlinkClock().restart();
	    recompute(); //Ca fait des calculs redondants ici, mais tant pis.
	    break;
	case sf::Event::TextEntered:
	    switch(e.text.unicode) {
	    case 8:  //Backspace
	    case 27: //Esc.
	    case 13: //Return.	
	    case 127: //Del.
		break;
	    default: 
		line.insert(cursor_position, e.text.unicode);
		cursor_position++;
		break;
	    }
	    text_cursor.getBlinkClock().restart();
	    recompute();
	    actionOnInput();
	    break;
	case sf::Event::KeyPressed:
	    switch(e.key.code) {
	    case sf::Keyboard::Right:
		if(cursor_position < line.getSize())
		    cursor_position++;
		break;
	    case sf::Keyboard::Left:
		if(cursor_position > 0)
		    cursor_position--;
		break;
	    case sf::Keyboard::Delete: 
		line.erase(cursor_position, 1);
		break;
	    case sf::Keyboard::BackSpace:
		if(cursor_position > 0) {
		    cursor_position--;
		    line.erase(cursor_position, 1);
		}
		break;
	    case sf::Keyboard::Return: 
		actionOnValidate();
		break;
	    case sf::Keyboard::Escape: 
		unfocus();
		break;
	    case sf::Keyboard::Home:
		cursor_position = 0;
		break;
	    case sf::Keyboard::End: 
		cursor_position = line.getSize();
		break;
	    }
	    text_cursor.getBlinkClock().restart();
	    recompute();
	    break;
	}
    }
}

void TextField::clear(sf::String s) {
    line = s;
    if(cursor_position > line.getSize())
	cursor_position = line.getSize();
    recompute();
}

sf::String TextField::getLine() {return line;}

void TextField::setDefaultTextColor(sf::Color color) {
    default_text_color = color;
}
sf::Color TextField::getDefaultTextColor() {
    return default_text_color;
}
void TextField::setDefaultFont(sf::Font font) {
    default_font = font;
}
sf::Font TextField::getDefaultFont() {
    return default_font;
}
void TextField::setTextSize(sf::Uint8 size) {
    text_size = size;
}
sf::Uint8 TextField::getTextSize() {
    return text_size;
}














float Slider::defaultCalculator(float in) {
    return in;
}

Slider::Slider(bool is_horizontal_) : Widget() {
    is_horizontal = is_horizontal_;
    is_dragged = false;	
    setValueCalculator(Slider::defaultCalculator);
    setPercentage(0.5f);
}

Slider::~Slider() {}

void Slider::setValueCalculator(float (*func)(float)) {
    calculator = func;
}

void Slider::setPercentage(float percent) {
    percentage = percent;
    if(percentage > 1.0) 
	percentage = 1.0;
    else if(percentage < 0.0) 
	percentage = 0.0;

    value = calculator(percentage);
    recomputeHandle();
}

float Slider::getValue() {
    return value;
}

void Slider::recomputeHandle() {
    if(is_horizontal) {
	handle.setRadius(rectangle_shape.getSize().y/2.0f);
	handle.setPosition(
	    rectangle_shape.getPosition().x + percentage*(rectangle_shape.getSize().x - handle.getRadius()*2.0f),
	    rectangle_shape.getPosition().y
	    );
    }
    else {
	handle.setRadius(rectangle_shape.getSize().x/2.0f);
	handle.setPosition(
	    rectangle_shape.getPosition().x,
	    rectangle_shape.getPosition().y + percentage*(rectangle_shape.getSize().y - handle.getRadius()*2.0f)
	    ); 
    }
}

void Slider::focus() {
    Widget::focus();
}

void Slider::unfocus() {
    Widget::unfocus();
}

void Slider::update() {
    Widget::update();
    handle.setFillColor(rectangle_shape.getFillColor() + sf::Color(64, 64, 64));
    recomputeHandle();
}

void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    Widget::draw(target, states);
    target.draw(handle, states);
}

void Slider::handleEvent(sf::Event e) {

    Widget::handleEvent(e);

    switch(e.type) {
    case sf::Event::MouseWheelMoved: 
	if(has_focus || rectangle_shape.getGlobalBounds().contains(e.mouseWheel.x, e.mouseWheel.y))
	    setPercentage(percentage + ((float)(e.mouseWheel.delta)/100.0f));
	break;
	
    case sf::Event::MouseButtonPressed:
	if(rectangle_shape.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y)) {
	    is_dragged = true;
	    handle.setFillColor(rectangle_shape.getFillColor() + sf::Color(32, 32, 32));
	    e.type = sf::Event::MouseMoved;
	    e.mouseMove.x = e.mouseButton.x;
	    e.mouseMove.y = e.mouseButton.y;
	}
    case sf::Event::MouseMoved:
	if(is_dragged) {
	    if(is_horizontal)
		setPercentage((e.mouseMove.x-rectangle_shape.getPosition().x)/rectangle_shape.getSize().x);
	    else
		setPercentage((e.mouseMove.y-rectangle_shape.getPosition().y)/rectangle_shape.getSize().y);
	}
	break;
	
    case sf::Event::MouseButtonReleased:
	is_dragged = false;
	handle.setFillColor(rectangle_shape.getFillColor() + sf::Color(64, 64, 64));
	break;
    }
}




/* Button */



void Button::defaultAction() {
/* une foction de base qui ne fait rien */
}

Button::Button() : Widget() {
    setAction(&Button::defaultAction);
    is_pressed = false;
}

Button::~Button() {
}

void Button::tellColor(sf::Color col) {
    color = col;
    rectangle_shape.setFillColor(color);
}

void Button::focus() {
 
}


void Button::unfocus() {
  
}

void Button::update()	{
    Widget::update();
}
void Button::setAction(void (*func)(void)) {
    action = func;
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    Widget::draw(target, states);
	
}
void Button::handleEvent(sf::Event e) {
    Widget::handleEvent(e); 
    switch(e.type) {
    case sf::Event::MouseButtonPressed:
	if(rectangle_shape.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y)) {
	    rectangle_shape.setFillColor(sf::Color(color.r>20?color.r-20:0, 
						   color.g>20?color.g-20:0,
						   color.b>20?color.b-20:0));
	    is_pressed = true;
	}	
	break;
    case sf::Event::MouseButtonReleased:
	if(is_pressed) {
	    if(rectangle_shape.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y)) {
		action();
		rectangle_shape.setFillColor(color + sf::Color(20,20,20));
	    }
	    is_pressed = false;
	}
	break;
    case sf::Event::MouseMoved:
	if(rectangle_shape.getGlobalBounds().contains(e.mouseMove.x, e.mouseMove.y)) {
	    if(is_pressed) {
		rectangle_shape.setFillColor(sf::Color(color.r>20?color.r-20:0, 
						       color.g>20?color.g-20:0,
						       color.b>20?color.b-20:0));
	    } else
		rectangle_shape.setFillColor(color + sf::Color(20,20,20));
	} else {
	    rectangle_shape.setFillColor(color);
	}
	break;
    }
}












   
IconButton::IconButton() : Button(), Picture() {}
IconButton::IconButton(sf::String img_path) : Button(), Picture() {
    image.loadFromFile(img_path);
    update();
}
IconButton::~IconButton(){}
void IconButton::focus() {}
void IconButton::unfocus() {}
void IconButton::handleEvent(sf::Event e) {
    Button::handleEvent(e);
}
void IconButton::update() {
    Button::update();
    if(rectangle_shape.getSize().x > 0 && rectangle_shape.getSize().y > 0) {
	texture.create((unsigned)rectangle_shape.getSize().x, 
		       (unsigned)rectangle_shape.getSize().y);
	texture.loadFromImage(image);
	sprite.setTexture(texture, true);
	sprite.setOrigin((sf::Vector2f)(image.getSize()/2u));
	sprite.setPosition(rectangle_shape.getPosition()+(sf::Vector2f)(((sf::Vector2u)rectangle_shape.getSize())/2u));
    }
}

void IconButton::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    Button::draw(target, states);
    target.draw(sprite, states);
}



/**/




TextButton::TextButton(sf::String txt) : Button() {
    str = txt;
    text.setString(str);
    text.setColor(sf::Color(200, 200, 200));
    font.loadFromFile("../../data/fonts/basis33.ttf");
    text.setFont(font);
    text.setCharacterSize(16);
}
TextButton::~TextButton(){}
void TextButton::clear(sf::String s) {
    str = s;
    text.setString(str);
}
void TextButton::focus() {}
void TextButton::unfocus() {}
void TextButton::handleEvent(sf::Event e) {
    Button::handleEvent(e);
}
void TextButton::update() {
    sf::Uint32 tcw;
    Button::update();
    text.setPosition(rectangle_shape.getPosition());
    tcw=0;
    for(sf::Uint32 i=0 ; i<str.getSize() ; i++) {
	tcw += font.getGlyph(str[i], 16, false).advance;
	if(tcw >= rectangle_shape.getSize().x) {
	    text.setString(str.substring(0, i));
	    return;
	}
    }

    text.setPosition(sf::Vector2f(
			 (sf::Uint32)rectangle_shape.getPosition().x+((sf::Uint32)rectangle_shape.getSize().x/2)-(tcw/2), 
			 (sf::Uint32)rectangle_shape.getPosition().y+((sf::Uint32)rectangle_shape.getSize().y/2)-12
			 ));


/*
  for(sf::Uint32 i=0 ; i<str.getSize() ; i++) {
  offset += default_font.getGlyph(str[i], text_size, false).advance;
  if(e.mouseButton.x <= offset) {
  cursor_position = i;
  break;
  }
*/
}

void TextButton::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    Button::draw(target, states);
    target.draw(text, states);
}





/**/







Picture::Picture() {
    sprite.setTexture(texture, true);
}
Picture::~Picture() {}
sf::Image& Picture::getImage() {return image;}
void Picture::setImage(sf::Image img) {image = img;}
sf::Texture& Picture::getTexture() {return texture;}
void Picture::setTexture(sf::Texture tex) {texture = tex;}
sf::Sprite& Picture::getSprite() {return sprite;}
void Picture::setSprite(sf::Sprite spr) {sprite = spr;}










//






CanvasWidget::CanvasWidget(sf::RenderWindow *window) {
    session = NULL;
    zoom_ratio = 1.0f;
    zoom_delta = sqrtf(2.0f);
    parent_window = window;
    is_moving = false;
}
CanvasWidget::~CanvasWidget() {}
void CanvasWidget::fillAlphaChecker(sf::Uint32 side, sf::Color first, sf::Color second) {
    sf::Image image;
    sf::Uint32 x, y;
    image.create(2*side, 2*side, sf::Color::Transparent);
    for(y=0 ; y<side ; y++) {
        for(x=0 ; x<side ; x++)
            image.setPixel(x, y, first);
        for(    ; x<side*2 ; x++)
            image.setPixel(x, y, second);
    }
    for(    ; y<side*2 ; y++) {
        for(x=0 ; x<side ; x++)
            image.setPixel(x, y, second);
        for(    ; x<side*2 ; x++)
            image.setPixel(x, y, first);
    }
    alpha_grid.setImage(image);
    alpha_grid.getTexture().loadFromImage(image);
    alpha_grid.getTexture().setRepeated(true);
    alpha_grid.getSprite().setTexture(alpha_grid.getTexture(), true);
    alpha_grid.getSprite().setTextureRect(sf::IntRect(0, 0, session->getSize().x, session->getSize().y));
}

void CanvasWidget::setSession(ClientSession *sess) {
    session = sess;
    if(session != NULL)
	fillAlphaChecker(4, sf::Color(255*1/3, 255*1/3, 255*1/3), sf::Color(255*2/3, 255*2/3, 255*2/3));
    focus();
    update();
}
sf::View& CanvasWidget::getCanvasView() {return canvas_view;}
void CanvasWidget::setCanvasView(sf::View v) {canvas_view = v;}
float CanvasWidget::getZoomRatio() {return zoom_ratio;}
void CanvasWidget::setZoomRatio(float ratio) {
    zoom_ratio = ratio;
    update();
}
float CanvasWidget::getZoomDelta() { return zoom_delta;}
void CanvasWidget::setZoomDelta(float delta) {zoom_delta = delta;}
void CanvasWidget::zoomIn() {setZoomRatio(zoom_ratio*zoom_delta);}
void CanvasWidget::zoomOut() {setZoomRatio(zoom_ratio/zoom_delta);}
void CanvasWidget::focus() {Widget::focus();}
void CanvasWidget::unfocus() {Widget::unfocus();}
void CanvasWidget::update() {
    sf::Vector2f my_pos, my_siz, p_siz;
    Widget::update();
    if(session != NULL) {
	my_pos = rectangle_shape.getPosition();
	my_siz = rectangle_shape.getSize();
	p_siz  = (sf::Vector2f) parent_window->getSize();
	canvas_view.setViewport(sf::FloatRect(my_pos.x/p_siz.x, my_pos.y/p_siz.y, 
					      my_siz.x/p_siz.x, my_siz.y/p_siz.y));
	canvas_view.setSize(my_siz/zoom_ratio);
	canvas_view.setCenter(((sf::Vector2f) session->getSize())/2.0f);
    }
}
void CanvasWidget::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    sf::View old_view;
    Widget::draw(target, states);
    if(session != NULL) {
	old_view = target.getView();
	target.setView(canvas_view);
	target.draw(((CanvasWidget*)this)->alpha_grid.getSprite(), states);
	for(sf::Uint32 i=0 ; i<session->getLayers().size() ; i++)
	    target.draw(session->getLayers()[i], states);
	target.setView(old_view);
    }
}
void CanvasWidget::handleEvent(sf::Event e) {
    sf::Vector2f my_pos, my_siz, p_siz;
    sf::Vector2i mpos;
    std::wostringstream os;

    Widget::handleEvent(e);
    if(session != NULL) {
	switch(e.type) {
	case sf::Event::MouseWheelMoved:
	    if(has_focus) {
		if(e.mouseWheel.delta > 0)
		    for(unsigned i=0 ; i<e.mouseWheel.delta ; i++)
			zoomIn();
		else
		    for(unsigned i=0 ; i<-e.mouseWheel.delta ; i++)
			zoomOut();
	    }
	    break;
	case sf::Event::MouseButtonPressed:
	    if(has_focus) {
		if(!session->getToolDragged() && e.mouseButton.button == sf::Mouse::Left) {
		    mpos = (sf::Vector2i) parent_window->mapPixelToCoords(
			sf::Vector2i(e.mouseButton.x, e.mouseButton.y), 
			canvas_view);
		    if(mpos.x < 0)
			mpos.x = 0;
		    else if(mpos.x >= session->getSize().x)
			mpos.x = session->getSize().x-1;
		    if(mpos.y < 0)
			mpos.y = 0;
		    else if(mpos.y >= session->getSize().y)
			mpos.y = session->getSize().y-1;
		    session->beginToolDrag((sf::Vector2u) mpos);
		}
	    }
	    break;
	case sf::Event::KeyPressed:
	    if(has_focus) {
		if(e.key.code == sf::Keyboard::Left || e.key.code == sf::Keyboard::Right)
		    canvas_view.rotate(e.key.code == sf::Keyboard::Left ? 5 : -5);
		if(!is_moving && e.key.code == sf::Keyboard::Space) {
		    is_moving = true;
		    moving_pos = (sf::Vector2i) sf::Mouse::getPosition(*parent_window);
		}
	    }
	    break;
	case sf::Event::MouseMoved:
	    if(has_focus) {
		if(is_moving) {
		    sf::Vector2f v = sf::Vector2f((moving_pos.x-e.mouseMove.x)/zoom_ratio,
						  (moving_pos.y-e.mouseMove.y)/zoom_ratio);
		    float ro = canvas_view.getRotation()*M_PI/180.f;
		    sf::Vector2f a = sf::Vector2f(cosf(ro), sinf(ro));
		    canvas_view.move(v.x*a.x - v.y*a.y, v.x*a.y + v.y*a.x); //Miam miam
		    moving_pos = sf::Vector2i(e.mouseMove.x, e.mouseMove.y);
		}
		if(session->getToolDragged()) {
		    mpos = (sf::Vector2i) parent_window->mapPixelToCoords(
			sf::Vector2i(e.mouseMove.x, e.mouseMove.y), 
			canvas_view);
		    if(mpos.x < 0)
			mpos.x = 0;
		    else if(mpos.x >= session->getSize().x)
			mpos.x = session->getSize().x-1;
		    if(mpos.y < 0)
			mpos.y = 0;
		    else if(mpos.y >= session->getSize().y)
			mpos.y = session->getSize().y-1;
		    session->stepToolDrag((sf::Vector2u) mpos);
		}
	    }
	    break;
	case sf::Event::KeyReleased:
	    if(is_moving && e.key.code == sf::Keyboard::Space)
		is_moving = false;
	    break;
	case sf::Event::MouseButtonReleased:
	    if(session->getToolDragged() && e.mouseButton.button == sf::Mouse::Left) {
		mpos = (sf::Vector2i) parent_window->mapPixelToCoords(
		    sf::Vector2i(e.mouseButton.x, e.mouseButton.y), 
		    canvas_view);
		    if(mpos.x < 0)
			mpos.x = 0;
		    else if(mpos.x >= session->getSize().x)
			mpos.x = session->getSize().x-1;
		    if(mpos.y < 0)
			mpos.y = 0;
		    else if(mpos.y >= session->getSize().y)
			mpos.y = session->getSize().y-1;
		    session->endToolDrag((sf::Vector2u) mpos);
	    }
	    break;
	} 
	sf::Vector2i mtmp = sf::Mouse::getPosition(*parent_window);
	mtmp = (sf::Vector2i) parent_window->mapPixelToCoords(mtmp, canvas_view);
	os << " X: " << mtmp.x
	   << " Y: " << mtmp.y 
	   << " (" << zoom_ratio*100 << "%, "
	   << canvas_view.getRotation() << L"°)";
	info_bar.println(os.str());
    }
}
