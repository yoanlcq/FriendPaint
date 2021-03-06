#include <cstdlib>
#include <iostream>
#include <cstring>
#include <cmath>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>


//Cette fonction renvoie l'image d'un cercle.
//Si fill est à true, alors il est rempli de la couleur.
sf::Image buildCircleImage(sf::Color color, sf::Uint32 diameter, bool fill) {
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


void copyImageFromCenter(sf::Image &dest, sf::Image &src, sf::Vector2u center, bool erase) {
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


void applyBrush(sf::Image &image, sf::Vector2u from, sf::Vector2u to, sf::Image brush, bool erase) {
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




 
void fillAlphaChecker(sf::Image &image, sf::Uint32 side, sf::Color first, sf::Color second) {
    sf::Uint32 x, y;
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
}

int main(int argc, char *argv[]) {
    sf::Uint32 canvas_width = 120, canvas_height = 100; // 120 100
    const sf::Uint32 CANVAS_VIEW_MIN_WIDTH = 200;
    const sf::Uint32 CANVAS_VIEW_MIN_HEIGHT = 100;
    sf::RenderWindow window;
    sf::Image layer_img, alpha_img;
    sf::Texture layer_texture, alpha_texture;
    sf::Sprite layer_sprite, alpha_sprite;
    sf::View canvas_view;
    bool stroking = false;
    sf::Int32 old_x, old_y;
    sf::Vector2i current_pos;
    float zoom_level = 1.0f, zoom_delta = sqrtf(2.0f);

    sf::Vector2i moving_pos;
    bool moving = false;

    sf::Image brush;
    sf::Uint32 diameter = 6;
    bool erase;
    brush = buildCircleImage(sf::Color::Black, diameter, true);

    layer_img.create(canvas_width, canvas_height, sf::Color::Transparent);
    layer_texture.loadFromImage(layer_img);
    layer_sprite.setTexture(layer_texture);
  
    alpha_img.create(10, 10, sf::Color::Transparent);
    fillAlphaChecker(alpha_img, 5, sf::Color(255*1/3, 255*1/3, 255*1/3), sf::Color(255*2/3, 255*2/3, 255*2/3));
    alpha_texture.loadFromImage(alpha_img);
    alpha_texture.setRepeated(true);
    alpha_sprite.setTexture(alpha_texture);
    alpha_sprite.setTextureRect(sf::IntRect(0, 0, canvas_width, canvas_height));
  
    window.setVerticalSyncEnabled(true);
    window.create(sf::VideoMode(800, 600), "Minipaint");
    sf::Uint32 win_w = window.getSize().x, win_h = window.getSize().y;

    sf::IntRect iface(40, 2*40, 260, 3*20);

    sf::RectangleShape iface_top;
    iface_top.setPosition(sf::Vector2f(0, 0));
    iface_top.setSize(sf::Vector2f(win_w, iface.top));
    iface_top.setFillColor(sf::Color(0, 0, 0));

    sf::RectangleShape iface_right;
    iface_right.setPosition(sf::Vector2f(win_w-iface.width, iface.top));
    iface_right.setSize(sf::Vector2f(iface.width, win_h-iface.top-iface.height));
    iface_right.setFillColor(sf::Color(10, 10, 10));

    sf::RectangleShape iface_bot;
    iface_bot.setPosition(sf::Vector2f(0, win_h-iface.height));
    iface_bot.setSize(sf::Vector2f(win_w, iface.height));
    iface_bot.setFillColor(sf::Color(0, 0, 0));

    sf::RectangleShape iface_left;
    iface_left.setPosition(sf::Vector2f(0, iface.top));
    iface_left.setSize(sf::Vector2f(iface.left, win_h-iface.top-iface.height));
    iface_left.setFillColor(sf::Color(20, 20, 20));
    

    canvas_view.setViewport(sf::FloatRect((float)iface.left/(float)win_w, (float)iface.top/(float)win_h, ((float)win_w-(float)iface.width-(float)iface.left)/(float)win_w, ((float)win_h-(float)iface.height-(float)iface.top)/(float)win_h));
    canvas_view.setSize((1.0f/zoom_level)*sf::Vector2f(win_w-iface.left-iface.width, win_h-iface.top-iface.height));
    canvas_view.setCenter(sf::Vector2f(canvas_width/2, canvas_height/2));
/*
    canvas_view.setViewport(sf::FloatRect(((((win_w-iface.left-iface.width)-(float)canvas_width*zoom_level)/2.0f)+iface.left)/win_w, 
					      ((((win_h-iface.top-iface.height)-(float)canvas_height*zoom_level)/2.0f)+iface.top)/win_h, 
                          (float)canvas_width*zoom_level/win_w, 
					      (float)canvas_height*zoom_level/win_h));
*/

    while(window.isOpen()) {

	if(win_w < iface.left+iface.width+CANVAS_VIEW_MIN_WIDTH || win_h < iface.top+iface.height+CANVAS_VIEW_MIN_HEIGHT) {
	    window.setSize(sf::Vector2u(win_w < iface.left+iface.width ? iface.left+iface.width+CANVAS_VIEW_MIN_WIDTH : win_w,
					win_h < iface.top+iface.height ? iface.top+iface.height+CANVAS_VIEW_MIN_HEIGHT : win_h));
	}
    
	window.clear(sf::Color(28,28,28));
    
	layer_texture.update(layer_img);
    
	window.setView(sf::View(sf::FloatRect(0, 0, win_w, win_h)));//window.setView(window.getDefaultView());
	window.draw(iface_left);
	window.draw(iface_right);
	window.draw(iface_bot);
	window.draw(iface_top);

	window.setView(canvas_view);
	window.draw(alpha_sprite);
	window.draw(layer_sprite);
    
	window.display();

	sf::Event event;
	while(window.pollEvent(event)) {
	    switch(event.type) {
	    case sf::Event::Closed:
		window.close();
		break;
	    case sf::Event::Resized:
		win_w = event.size.width;
		win_h = event.size.height;
		canvas_view.setViewport(sf::FloatRect((float)iface.left/(float)win_w, (float)iface.top/(float)win_h, ((float)win_w-(float)iface.width-(float)iface.left)/(float)win_w, ((float)win_h-(float)iface.height-(float)iface.top)/(float)win_h));
		canvas_view.setSize((1.0f/zoom_level)*sf::Vector2f(win_w-iface.left-iface.width, win_h-iface.top-iface.height));
		canvas_view.setCenter(sf::Vector2f(canvas_width/2, canvas_height/2));
/*
		canvas_view.setViewport(sf::FloatRect(((((win_w-iface.left-iface.width)-(float)canvas_width*zoom_level)/2.0f)+iface.left)/win_w, 
						      ((((win_h-iface.top-iface.height)-(float)canvas_height*zoom_level)/2.0f)+iface.top)/win_h, 
						      (float)canvas_width*zoom_level/win_w, 
						      (float)canvas_height*zoom_level/win_h));
*/

		iface_top.setPosition(sf::Vector2f(0, 0));
		iface_top.setSize(sf::Vector2f(win_w, iface.top));

		iface_right.setPosition(sf::Vector2f(win_w-iface.width, iface.top));
		iface_right.setSize(sf::Vector2f(iface.width, win_h-iface.top-iface.height));

		iface_bot.setPosition(sf::Vector2f(0, win_h-iface.height));
		iface_bot.setSize(sf::Vector2f(win_w, iface.height));

		iface_left.setPosition(sf::Vector2f(0, iface.top));
		iface_left.setSize(sf::Vector2f(iface.left, win_h-iface.top-iface.height));

		break;
	    case sf::Event::MouseButtonPressed:
		if(!stroking && (event.mouseButton.button == sf::Mouse::Left || event.mouseButton.button == sf::Mouse::Right)) {
		    if(canvas_view.getViewport().contains((float)event.mouseButton.x/(float) window.getSize().x, (float) event.mouseButton.y/ (float) window.getSize().y)) {
			erase = (event.mouseButton.button == sf::Mouse::Right);
			stroking = true;
			current_pos = sf::Vector2i(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), canvas_view));
			applyBrush(layer_img, (sf::Vector2u) current_pos, (sf::Vector2u) current_pos, brush, erase);
			old_x = current_pos.x;
			old_y = current_pos.y;
		    }
		}
		if(!moving && event.mouseButton.button == sf::Mouse::Middle) {
		    moving = true;
		    moving_pos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
		}
		break;
	    case sf::Event::MouseMoved:
		if(stroking) {
		    // if(canvas_view.getViewport().contains((float)event.mouseMove.x/(float) window.getSize().x, (float) event.mouseMove.y/ (float) window.getSize().y)) {
		    current_pos = sf::Vector2i(window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), canvas_view));
		    applyBrush(layer_img, sf::Vector2u(old_x==-1 ? current_pos.x : old_x, old_y==-1 ? current_pos.y : old_y), sf::Vector2u(current_pos.x,current_pos.y), brush, erase);
		    old_x = current_pos.x;
		    old_y = current_pos.y;
		    /*} else if(!(old_x == -1 && old_y == -1)) {
		      current_pos = sf::Vector2i(window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), canvas_view));
		      drawLine(layer_img,  old_x,  old_y, current_pos.x,  current_pos.y, sf::Color::Black);
		      old_x = old_y = -1;
		      }*/
		}

		if(moving) {
		    canvas_view.move((1/zoom_level)*moving_pos.x - (1/zoom_level)*event.mouseMove.x, 
				     (1/zoom_level)*moving_pos.y - (1/zoom_level)*event.mouseMove.y);
		    moving_pos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
		}
		break;
	    case sf::Event::MouseButtonReleased:
		if(stroking && (event.mouseButton.button == sf::Mouse::Left || event.mouseButton.button == sf::Mouse::Right))
		    stroking = false;
		if(moving && event.mouseButton.button == sf::Mouse::Middle)
		    moving = false;
		break;
	    case sf::Event::KeyPressed:
		if(event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right) {
		    canvas_view.rotate(4 * (event.key.code == sf::Keyboard::Left ? 1 : -1));
		} else if(event.key.code == sf::Keyboard::Subtract && diameter>1) {
		    diameter--;
		    brush = buildCircleImage(sf::Color::Black, diameter, true);
		    std::cout << "Brush size : " << diameter << "px" << std::endl;
		} else if(event.key.code == sf::Keyboard::Add) {
		    diameter++;
		    brush = buildCircleImage(sf::Color::Black, diameter, true);
		    std::cout << "Brush size : " << diameter << "px" << std::endl;
		}
		break;
	    case sf::Event::MouseWheelMoved:
		float zoom_gain;
		if(event.mouseWheel.delta > 0) {
		    zoom_gain = powf(zoom_delta, event.mouseWheel.delta); //zoom in
		    if(zoom_level*zoom_gain > 4.0f) {
			std::cout << "FixMe: Maximum zoom level is arbitrary. High zoom level on a large canvas may corrupt mouse-to-canvas coordinates translations." << std::endl;
			break;
		    }
		}
		else {
		    zoom_gain = powf(1.0f/zoom_delta, -event.mouseWheel.delta); //zoom out
		    if(zoom_level*zoom_gain < 0.003f) break;
		}
		zoom_level *= zoom_gain;
		std::cout << "Zoom level : " << zoom_level*100.0f << '%' << std::endl;

		//canvas_view.zoom(1.0f/zoom_gain);
		canvas_view.setSize((1.0f/zoom_level)*sf::Vector2f(win_w-iface.left-iface.width, win_h-iface.top-iface.height));
/*
		sf::FloatRect vp = canvas_view.getViewport();
		vp.width  *= zoom_gain;
		vp.height *= zoom_gain;
		win_w = window.getSize().x;
		win_h = window.getSize().y;
		vp.left = ((1.0f-((float)(iface.left+iface.width)/(float)win_w)-vp.width)/2.0f)+((float)iface.left/(float)win_w);
		vp.top = ((1.0f-((float)(iface.top+iface.height)/(float)win_h)-vp.height)/2.0f)+((float)iface.top/(float)win_h);
		canvas_view.setViewport(vp);
*/
		break;
	    }
	}
    }
    exit(0);
}
