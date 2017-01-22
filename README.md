# FriendPaint

![Interface](http://yoanlecoq.com/dl/dev/friendpaint/iface.png)

*French ahead!*  

FriendPaint est une application de peinture collaborative en réseau, réalisée
à l'occasion d'un projet tuteuré de DUT Info.  
L'équipe était composée de Sébastien Bourdin, Valentin Bocquel et moi-même.  
  
Pas mal de documents ont été rédigés lors du développement, et conservés dans
le dossier `legacy` pour des raisons de préservation.

A l'occasion de sa mise en ligne sur GitHub, son Makefile a été amélioré.

## Compiler le projet
Les pré-requis sont GNU Make, un G++ récent, et la SFML 2.3.  
Ensuite, simplement invoquer `make os=foo`, où `foo` doit être remplacé par `win32`, `win64`, `macosx`, `linux32`, `linux64`.  
  
Sous Windows, utiliser l'invite de commandes (et non un Bash).  
  
La compilation sous Mac OS X est censée bien se passer, mais elle n'a pas
été testée.
