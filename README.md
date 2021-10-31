# Project-Emul-ARM-V7

*Grenoble INP - Projet 2A SICOM*

L’objectif de ce projet informatique est de réaliser, en langage C, un émulateur de microprocesseur ARM Cortex-M permettant d’exécuter et de mettre au point des programmes écrits dans le langage binaire cible. 

Le rôle d’un tel émulateur est de lire un programme donné en entrée et d’exécuter chacune des instructions avec le comportement attendu de la machine cible. Les émulateurs permettent notamment de prototyper et déboguer des programmes sans la contrainte de posséder le matériel cible
(ici il s’agit d’une machine avec un microprocesseur ARMV7 Cortex-M).
Plus précisément, l’émulateur prend en entrée un fichier objet binaire au format ELF et permet:

 - de le charger en mémoire (virtuelle) ;
 - de l’exécuter, entièrement ou pas à pas ;
 - de modifier et/ou afficher son code assembleur ou la mémoire qu’il utilise pour le mettre au point.



Cette implémentation a été élaboré par:
- Ali Saghiran et Damien Chabannes (2A Sicom - Grenoble-INP Phelma - 2015/2016)



## Comment utiliser ?

L'interpréteur utilise la librarie `libreadline`. Sur Linux, il faut donc installer le package de développement qui convient. Il apparaît que l'appel à `lreadline` n'est pas toujours suffisant pour compiler et nécessite parfois d'ajouter `-lcurses` selon les versions et l'état de votre OS.

pour compiler la version debuggable : 
```$ make debug``` 

pour générer la documentation :
```$ make doc```

pour nettoyer :
```$ make clean```

pour créer l'archive à envoyer à votre tuteur
```$ make archive```


pour exécuter
```emul-arm [fichier de scripts] ```

les sources se trouvent dans `./src/`, les includes dans `./include/`, les fichiers de script de test se trouvent dans `./tests/` et les rapports à nous envoyer se trouvent dans  `./rapports/`.

