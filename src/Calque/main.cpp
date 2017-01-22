#include <iostream>
#include <string>
#include <vector>
#include "Layer.hpp"
#include "ListLayers.hpp"

using namespace std;


int main() {

    // On declare la liste de calques
    ListLayers liste_layers;

    // On creer le calque "Premier calque" qui 
    // se place a la premiere position de la liste
    // puis on affiche les calques de la liste
    liste_layers.createLayer("Premier calque");
    liste_layers.afficherLesCalques();

    putchar('\n');

    // On creer le calque "Deuxieme calque" qui 
    // se place a la deuxieme position de la liste
    // puis on affiche les calques de la liste
    liste_layers.createLayer("Deuxieme calque");
    liste_layers.afficherLesCalques();

    putchar('\n');

 
    // On creer le calque "Troisieme calque" qui 
    // se place a la troisieme position de la liste
    // puis on affiche les calques de la liste
    liste_layers.createLayer("Troisieme calque");
    liste_layers.afficherLesCalques();

    putchar('\n');


    // On focus le deuxieme calque
    // qui se trouve a list[1]
    // (le premier se trouve a list[0])
    liste_layers.focusByPosition(1);

    putchar('\n');

    // On detruit le calque qui a ete
    // focus (le deuxieme)
    // puis on affiche les calques
    liste_layers.deleteLayer();
    liste_layers.afficherLesCalques();

    putchar('\n');

    // On creer le calque "Deuxieme calque" qui 
    // se place a la troisieme position de la liste
    // puis on affiche les calques de la liste
    liste_layers.createLayer("Deuxieme calque");
    liste_layers.afficherLesCalques();

    putchar('\n');

    // On focus le deuxieme calque ("Troisieme Calque")
    // qui se trouve a list[1]
    liste_layers.focusByPosition(1);

    putchar('\n');

    // On fait descendre le calque
    // qui a ete focus
    liste_layers.downLayer();
    liste_layers.afficherLesCalques(); // et la c'est le drame

    putchar('\n');

    // On focus le premier calque ("Premier Calque")
    // qui se trouve a list[0]
    liste_layers.focusByPosition(0);
    putchar('\n');

    // On renomme le calque qui
    // a ete focus en "calque Premier"
    liste_layers.rename("calque Premier");
    liste_layers.afficherLesCalques();


    return 0;
}
