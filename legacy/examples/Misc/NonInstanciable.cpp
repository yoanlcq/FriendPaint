#include <cstdlib>
#include <iostream>

class NonInstanciable {
private:
  NonInstanciable() {};
  static int attribut;
public:
  static int const CONSTANTE = 42;
  static void setAttribut() {attribut = 13;}
  static int getAttribut() {return attribut;}
};

int NonInstanciable::attribut = 24;

int main(int argc, char *argv[]) {
  NonInstanciable::setAttribut();
  std::cout << NonInstanciable::getAttribut() << ' ' 
	    << NonInstanciable::CONSTANTE 
	    << std::endl;
  exit(EXIT_SUCCESS);
}
