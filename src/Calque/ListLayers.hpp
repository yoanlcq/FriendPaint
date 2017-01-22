#ifndef DEF_LISTLAYERS
#define DEF_LISTLAYERS

#include <iostream>
#include <string>
#include <vector>
#include "Layer.hpp"

class ListLayers {
    public:
	    ListLayers();

	    void setLockCreate();
	    bool getLockCreate();
	    void afficherLesCalques(); // juste pour les tests
	    void focusByPosition(int pos); // juste pour les tests
	    void createLayer(std::string name);
	    void deleteLayer();
	    void upLayer();
	    void downLayer();
	    //void mergeDown();
	    void rename(std::string new_name);

    private:
    	std::vector<Layer*> list;
		bool lock_create;
};

#endif
