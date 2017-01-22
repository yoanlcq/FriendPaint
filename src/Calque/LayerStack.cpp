#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

#include "LayerStack.hpp"
#include "Layer.hpp"

using namespace std;


LayerStack::LayerStack() : {
}


void LayerStack::createLayer(Uint32 index, String name) {
	list.insert(list.begin()+index, new Layer(name));
}

void LayerStack::moveLayer(Uint32 index, Uint32 delta) {
	std::iter_swap(list.begin()+index, list.begin()+delta);
	// inversion entre l'index et le delta ?
}

void LayerStack::renameLayer(Uint32 index, String new_name) {
	list[index]->setTitle(new_name);
}

void LayerStack::deleteLayer() {
	list.erase(list.begin()+index);
}

void LayerStack::mergeDownLayer(Uint32 index) {
	// kcnbodfbo
}