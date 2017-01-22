#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

#include "ListLayers.hpp"
#include "Layer.hpp"

using namespace std;


ListLayers::ListLayers() : lock_create(false) {
}



void ListLayers::setLockCreate() {
	if (lock_create == false)
		lock_create = true;
	else
		lock_create = false;
}

bool ListLayers::getLockCreate() {
	return lock_create;
}

void ListLayers::afficherLesCalques() { // juste pour les tests
	for(int i=0; i<list.size(); i++) {
        cout << list[i]->getNom() << endl;
    }
}

void ListLayers::focusByPosition(int pos) {
	for (int i=0; i<list.size(); i++) {
		if (list[i]->getFocus() == true)
			list[i]->setFocus();
	}
	list[pos]->setFocus();
	cout << list[pos]->getNom() << endl;
	cout << list[pos]->getFocus() << endl;
}


void ListLayers::createLayer(string name) {
	list.push_back(new Layer(name));
}

void ListLayers::deleteLayer() {
	for(int i=0; i<list.size(); i++) {
        if (list[i]->getFocus() == true)
        	list.erase(list.begin()+i);
    }
}

void ListLayers::upLayer() {
	if (list[0]->getFocus() == false) {
		for(int i=0; i<list.size(); i++) {
			if (list[i]->getFocus() == true) {
	        	std::iter_swap(list.begin()+i, list.begin()+i-1);
	        }
	    }
	}
}

void ListLayers::downLayer() {
	int endList = list.size()-1;
	if (list[endList]->getFocus() == false) {
		for(int i=0; i<list.size(); i++) {
	        if (list[i]->getFocus() == true) {
	        	std::iter_swap(list.begin()+i, list.begin()+i+1);
	        }
	    }
	}
}

void ListLayers::rename(string new_name) {
	for(int i=0; i<list.size(); i++) {
		if (list[i]->getFocus() == true) {
	       	list[i]->setNom(new_name);
	    }
	}
}