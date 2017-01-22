Bienvenue dans le README de FriendPaint, le projet tuteuré de Valentin BOCQUEL, Sébastien BOURDIN et Yoan LECOQ.
Vous trouverez ici comment construire le projet pour votre plateforme et comment le lancer correctement.

Ce README considère que vous êtes familiers avec la ligne de commande et les outils de compilation.

Si vous rencontrez des problèmes que ce README ne peut résoudre, vous pouvez m'envoyer un mail à yoanlecoq.io@gmail.com.


-- CONSTRUIRE DEPUIS LES SOURCES --

Ouvrir dans un éditeur de texte le Makefile.
Vers la 9ème ligne, repérez la variable OS.
Changez sa valeur en fonction du système sur lequel vous êtes; elle doit être une des valeurs suivantes :
win32, win64, macosx, linux32, linux64
Notez que seules win32, win64 et linux64 ont été testées.

Une fois cela fait, vous pouvez enregistrer vos changements et invoquer 'make'.
Sous Windows, vous aurez certainement besoin d'installer une suite d'outils comme MinGW (Minimal GNU For Windows).


-- TROUBLESHOOTING --

Nous avons fait en sorte qu'il n'y aie pas besoin d'installer les bibliothèques pour compiler le projet; tout le nécéssaire doit se trouver dans ce dossier.
Cependant, si vous rencontrez des problèmes, voici ce qui pourrait vous dépanner :

Sous Windows :
-> S'assurer qu'un compilateur g++ est installé (Nous avons utilisé TDM-GCC).

Sous Linux :
-> La SFML se trouve déjà dans le répertoire FriendPaint. Le seul problème qui pourrait se poser serait celui des dépendances (notamment libGL, etc).
   Dans ce cas, il n'y a pas de meilleur moyen que se rendre sur le site de la SFML (sfml-dev.org) et suivre les instructions d'installation de la SFML.
   Si vous utilisez le gestionnaire de paquets apt, alors 
   $ sudo apt-get install -y libsfml-dev 
   devrait faire l'affaire.

   
-- LANCEMENT --

Vous devez toujours lancer les exécutables en vous assurant que votre répertoire courant est 'bin/<votre_OS>'.
Si vous ne le faites pas, les polices et les icônes ne pourront pas être chargées et le comportement du programme sera indéterminé.

Vous pouvez alors vous référer à 'Guide_utilisateur.pdf'.
