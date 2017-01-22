#include <cstring>
#include <cstdio>
#include <SFML/Graphics.hpp>


sf::Image image;
sf::Texture texture;
int diameter;
sf::Color color(255,0,0);


void /*Tool::*/updateBrush() {
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
    
    image.create(diameter, diameter, (sf::Uint8*) pixels);
    if(even)
	diameter--;
    texture.loadFromImage(image);
    free(pixels);
}


int main(){
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(100, 100), "SFML window");
    sf::Sprite sprite;
    // Load a sprite to display
    diameter = 42;
    updateBrush();
    texture.loadFromImage(image);
    sprite.setTexture(texture, true);
    while(window.isOpen()) {
	// Process events
	sf::Event event;
	while (window.pollEvent(event))
	    {
		// Close window: exit
		if (event.type == sf::Event::Closed)
		    window.close();
		
		if (event.type == sf::Event::KeyPressed) {
		    if(event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right) {
			diameter += (event.key.code == sf::Keyboard::Left ? -1 : 1);
			updateBrush();
			printf("Diameter : %u\n", diameter);
			texture.loadFromImage(image);
			sprite.setTexture(texture, true);
		    }
		}
	    }
	window.clear(sf::Color::Black);
	window.draw(sprite);
	window.display();
    }
    return EXIT_SUCCESS;
}



