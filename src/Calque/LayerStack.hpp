#ifndef DEF_LAYERSTACK
#define DEF_LAYERSTACK

#include <iostream>
#include <string>
#include <vector>
#include "Layer.hpp"

class LayerStack {
    public:
	    LayerStack();

	    void createLayer(sf::Uint32 index, sf::String name);
	    void moveLayer(Uint32 index, Uint32 delta);
	    void renameLayer(sf::Uint32 index, sf::String new_name);
	    void deleteLayer(sf::Uint32 index);
	    void mergeDownLayer(sf::Uint32 index);

    private:
    	std::vector<Layer*> list;
};

#endif
