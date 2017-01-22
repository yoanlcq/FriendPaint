#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <SFML/System.hpp>


int getOneLetter(void) {
  char c, first;
  first = c = getchar();
  while(c != '\n' &&  c!= EOF)
    c = getchar();
  return first;
}

class ThreadArgument {
public:
  char data;
  sf::Thread *thread_pointer;
};

void useValue(void *arg) {
  ThreadArgument *thread_arg = (ThreadArgument*) arg;
  /* On sauvegarde le pointeur dans une variable car on va le supprimer en dernier; */
  /* Parce que sous Windows, libérer ce pointeur tue le thread et ignore le reste de la fonction ! */
  sf::Thread* thread_pointer = thread_arg->thread_pointer;
  sf::sleep(sf::seconds(2));
  std::cout << "Dynamic Thread " 
	    << thread_pointer
	    << " processed '" << thread_arg->data << "'";
  delete thread_arg;
  std::cout << " (thread freed)." << std::endl;
  delete thread_pointer;
}

int main(int argc, char *argv[]) {
  char c;
  sf::Thread *thread;
  struct ThreadArgument *thread_arg;
  while(1) {
    std::cout << "Momma : Ready! Please enter a single character." << std::endl;
    c = getOneLetter();
    thread_arg = new ThreadArgument();
    thread = new sf::Thread(&useValue, thread_arg);
    thread_arg->thread_pointer = thread;
    thread_arg->data = c;
    thread->launch();
  }
  exit(EXIT_SUCCESS);
}
