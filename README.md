# NandToTetris

*Date du projet : février 2014 à avril 2014*

## Qu'est-ce que NandToTetris ?

NandToTetris signifie littéralement *"De la porte NAND (NON ET) à Tetris"*, on va donc partir d'une [porte logique](https://en.wikipedia.org/wiki/Logic_gate) pour construire un ordinateur fonctionnel et complet afin de pouvoir programmer dessus et éventuellement réaliser des jeux comme Tetris.

Le projet consiste donc à créer :

- Un **ordinateur** capable d'être émulé.
- Un **compilateur** complet pour le langage Jack (le langage de base de l’ordinateur).
- Un **mini-OS** nous permettant d'avoir des appels systèmes basiques.

J’ai réalisé ce projet à l’aide du livre *The Elements of Computing Systems : Building a Modern Computer from First Principles*. Ce livre fournit des émulateurs, et des fichiers de tests pour la création de ce projet, ainsi que les explications nécessaires.

### L'ordinateur

L'ordinateur est basé sur une architecture 16 bits de [von Neumann](https://en.wikipedia.org/wiki/Von_Neumann_architecture), et peut accueillir un clavier et un écran de 256×512 en noir et blanc.

Pour la création de l'ordinateur, on va donc partir de la porte logique [NAND](https://en.wikipedia.org/wiki/NAND_gate) qui est notre élément de base (elle est intégrée directement dans l'émulateur ce qui nous permet de l'utiliser avec les autres éléments sans problème). Grâce à cette porte logique, on peut en créer d'autres comme [AND](https://en.wikipedia.org/wiki/AND_gate), [OR](https://en.wikipedia.org/wiki/OR_gate), [NOT](https://en.wikipedia.org/wiki/Inverter_%28logic_gate%29), etc. et à partir de ces portes on peut construire l’[ALU](https://en.wikipedia.org/wiki/Arithmetic_logic_unit). Pour la [RAM](https://en.wikipedia.org/wiki/Random-access_memory) de l'ordinateur, il faut la construire étape par étape et augmenter progressivement en capacité de stockage. On commence par la construction d'un [flip-flop](https://en.wikipedia.org/wiki/Flip-flop_%28electronics%29) qui nous permet de réaliser un [bit](https://en.wikipedia.org/wiki/Bit), puis un registre et enfin la RAM complète. Le [CPU](https://en.wikipedia.org/wiki/Central_processing_unit) vient en dernier et achève la création de notre ordinateur.

Toutes les parties de l’ordinateur sont créées à partir de fichiers .hdl qui peuvent être simulés à l’aide du *CPU Emulator* fournit par le livre.

### Le compilateur

Ensuite on crée le compilateur (à partir de n’importe quel langage, j’ai choisi le C) qui est constitué d’une [machine virtuelle](https://en.wikipedia.org/wiki/Virtual_machine). Le livre décrit un nouvel langage, le Jack, et ce compilateur doit pouvoir convertir des fichiers .jack en fichier .hack exécutable par notre ordinateur émulé.

![Fonctionnement du compilateur](img/schema_compilateur.png)

Il est décomposé en 3 programmes :

- **Jack Analyzer** : Ce programme nettoie les fichiers .jack (enlève les commentaires, les espaces blancs inutiles, etc.), puis il traduit pour chaque fichier .jack son équivalent en .vm (langage de la machine virtuelle).
- **VM Translator** : Ce programme traduit tous les .vm en un seul fichier .asm (assembleur, langage de l’ordinateur).
- **Assembler** : Ce programme traduit le fichier .asm en un seul fichier binaire .hack (exécutable par l’ordinateur).

L’intérêt d’avoir une machine virtuelle est que si je veux par exemple ajouter un langage de programmation sur l’ordinateur, il me suffit de créer un programme qui traduit du nouveau langage en langage de la machine virtuelle et donc je ne recode pas inutilement la partie VM Translator et Assembler.

### Le mini-OS

Enfin on crée le mini-OS qui est plutôt une librairie qui fournit des fonctions de bases et des appels systèmes pour le langage Jack. Cette partie contient presque 50 appels systèmes répartis en 8 fichiers sources (le tout est codé en Jack) :

- **Math** : Fonctions mathématiques de base.
- **String** : Crée la classe `String` ainsi que plusieurs fonctions permettant de manipuler les chaînes de caractères.
- **Array** : Crée la classe `Array`.
- **Output** : Permet d’afficher du texte, des nombres, etc. sur l’écran.
- **Screen** : Permet d’afficher des lignes, des rectangles, des cercles, etc. sur l’écran.
- **Keyboard** : Permet de lire sur l’entrée de l’ordinateur (le clavier) des nombres, des caractères, des chaînes, etc.
- **Memory** : Permet d’allouer et de dé-allouer des objets et définit quelques appels systèmes sur la mémoire.
- **Sys** : Fournit quelques appels systèmes sur l’ordinateur comme une fonction arrêtant le programme en cours, une fonction en cas d’erreur, une fonction qui attends un certains nombres de millisecondes.


### Le langage Jack

Le langage de programmation Jack est un langage simple orienté objet dont vous pouvez trouver une description complète ici : <http://www.nand2tetris.org/09.php>

Un exemple de programme affichant "Hello world !" en Jack :

```java
class Main
{
   function void main()
   {
      do Output.printString("Hello world !");
      do Output.println();

      return ;
   }
}
```

## Lien

Le site du livre : <http://www.nand2tetris.org/>

D'autres liens utiles à propos du projet :

- L'intégralité du livre en format pdf : <http://www.nand2tetris.org/course.php>
- Le livre en format papier : <http://www.nand2tetris.org/book.php>
- Tous les fichiers du livre (émulateurs, fichiers tests, etc.) : <http://www.nand2tetris.org/software.php>
