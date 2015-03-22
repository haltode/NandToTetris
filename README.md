# Hack

*Date du projet : Février 2014 à Avril 2014*

Projet Hack du livre *The Elements of Computing Systems : Building a Modern Computer from First Principles*.

Hack est le nom du projet qui consiste à :

  * La création d’un ordinateur 16 bits basé sur une architecture von Neumann, pouvant accueillir un clavier et un écran de 256×512 en noir et blanc.
  * La création d’un compilateur complet pour le langage Jack (le langage de base de l’ordinateur).
  * La création d’un mini-OS pour l’ordinateur.

## L'ordinateur

L'ordinateur est créé virtuellement dans des fichiers sources et peut être émulé par le CPU Emulator (fournit par le livre). Cet ordinateur est entièrement construit à partir d'une porte logique NAND (implémentée de base). Le but était de construire tout le reste à partir de cette porte logique en passant par différentes étapes :

  * La construction d'autres portes logiques : AND, OR, NOT, etc.
  * La construction de l'ALU
  * La construction de la mémoire : flip-flop, bit, registre, RAM
  * La construction du CPU

## Le compilateur

Le compilateur est codé en langage C (choix personnel), il est constitué d'une machine virtuelle.

Le compilateur est décomposé en trois programmes :

  * Jack Analyzer : Ce programme nettoie les fichiers .jack (enlève les commentaires, les espaces blancs inutiles etc.), puis il traduit pour chaque fichier .jack son équivalent en .vm (langage de la machine virtuelle).
  * VM Translator : Ce programme traduit tous les .vm en un seul fichier .asm (assembleur, langage de l’ordinateur).
  * Assembler : Ce programme traduit le fichier .asm en un seul fichier binaire .hack (exécutable par l’ordinateur).

Et voici comment il fonctionne :

```
      Fichier(s) .jack
	    
+------------------------+
|     Jack Analyzer      |
|------------------------|
|                        |
|     .jack -> .vm       |
|                        |
+------------------------+
	    
+------------------------+
|      VM translator     |
|------------------------|
|                        |
|      .vm -> .asm       |
|                        |
+------------------------+

+------------------------+
|        Assembler       |
|------------------------|
|                        |
|      .asm -> .hack     |
|                        |
+-----------+------------+

      Fichier binaire .hack

  (exécutable par l'ordinateur)
```

## Le mini-OS

Le mini-OS fournit tous les appels systèmes à l'ordinateur, les fonctions ont été codé en Jack (le langage créé et utilisé pour l'ordinateur). Le mini-OS comprend 8 fichiers :


  * Math : Fonctions mathématiques de base.
  * String : Crée la classe String ainsi que plusieurs fonctions permettant de manipuler les chaînes de caractères.
  * Array : Crée la classe Array.
  * Output : Permet d’afficher du texte, des nombres etc. sur l’écran.
  * Screen : Permet d’afficher des lignes, des rectangles, des cercles etc. sur l’écran.
  * Keyboard : Permet de lire sur l’entrée de l’ordinateur (le clavier) des nombres, des caractères, des chaînes etc.
  * Memory : Permet d’allouer et de dé-allouer des objets et définit quelques appels systèmes sur la mémoire.
  * Sys : Fournit quelques appels systèmes sur l’ordinateur comme une fonction arrêtant le programme en cours, une fonction en cas d’erreur, une fonction qui attends un certains nombres de millisecondes.

## Le langage Jack

Le langage de programmation Jack est un langage simple orienté objet dont vous pouvez trouver une description complète ici : http://www.nand2tetris.org/09.php
