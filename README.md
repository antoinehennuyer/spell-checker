# Compilation:

Afin de compiler correctement notre programme, il suffit de faire: "sh build.sh" ce qui va créer deux binaires différents: TextMiningCompiler et TextMiningApp

Ensuite afin de créer le dictionnaire qui va être utiliser il faut faire: "./TextMiningCompiler words.txt dict.bin"

Enfin pour utiliser le dictionnaire créé il faut faire: "./TextMiningApp dict.bin"

Vous allez pouvoir rechercher les mots désirés grâce à cette commande dans le terminal généré: ">approx 'distance' 'mot' "

distance: est un nombre qui désigne la distance de Damerau-Levenshtein pour notre recherche dans la dictionnaire.

Test Suite:
==========

Pour exécuter Test Suite : "./testSuite.py  'distance' 'mot' 'TextMiningApp' 'TextMiningAppRef' 'dict.bin' 'dictRef.bin' "


Questions:
==========

 1.     Décrivez les choix de design de votre programme
 Ce projet a été réalisé dans le langage C++.
 Nous produisons deux exécutables différents, l'un se nomme TextMiningCompiler et le second TextMiningApp.
 Nous avons un dossier "src" qui contient toute la partie code des deux binaires (TextMiningApp et TextMiningCompiler
)
Afin de faciliter l'utilisation de ce programme nous avons créé un script afin de compiler ce dernier (build.sh).

 Etapes:
    Compiler:
    1) on sort notre words.txt
    2) on crée notre partricia trie à partir de notre liste de mots triés
    3) on écrit le patricia trie dans un fichier binaire à l'aide de streams
    App:
    4) on recrée notre structure patricia trie à partir du fichier binaire à l'aide de streams
    5) on parcourt l'arbre en préfix, puis on calcule la distance entre le mot donné en argument et la valeur des noeuds
    6) on calcule la distance entre deux mots à l'aide d'une matrice de distance
    7) si la distance entre les mots est inférieure à la distance donnée en argument alors on crée un Json avec le mot, fréquence et distance et on l'ajoute à une liste
    8) on sort la liste en fonction de sa distance, fréquence et ordre lexicographique
puis on print la liste dans stdout.


 2.     Listez l’ensemble des tests effectués sur votre programme (en plus des units tests)
Nous avons réalisé une testSuite en python où l'on renseigne une distance et un mot, puis il vérifie et affiche les éléments présents dans la réf et qui ne sont pas présent dans notre TextMiningApp et vis versa.

 3.     Avez-vous détecté des cas où la correction par distance ne fonctionnait pas (même avec une distance élevée) ?
 La correction par distance peut ne pas fonctionner lorsque les mots sont éloignés grammaticalement mais proche phonétiquement.
 
 4.     Quelle est la structure de données que vous avez implémentée dans votre projet, pourquoi ?
En premier lieu, nous avions décidé d'utiliser un simple Trie afin de créer notre programme. Mais l'inconvénient c'est qu'il prend beaucoup de place en RAM. En effet, pour chaque caractére, il utilise un noeud. Pour des raisons de performances, nous avons finalement utiliser un Patricia Trie, c'est une implémentation qui permet d'optimiser certaines branches de l'arbre, c'est à dire stocker plus d'un caractéres par noeuds.

 5.     Proposez un réglage automatique de la distance pour un programme qui prend juste une chaîne de caractères en entrée, donner le processus d’évaluation ainsi que les résultats
Nous allons prendre le mot et trouver les mots dont la distance 0 de Damerau-Levenshtein. Si il à été trouver alors on arrête le programme. Sinon on passe à la distance 1 si il n'y a toujours pas de résultat on essaie avec la distance 2. Nous allons retourner les premiers résultats trouver.
 
 6.     Comment comptez vous améliorer les performances de votre programme
Afin d'améliorer les performances de notre programme, nous pouvons changer notre façon d'écrire notre arbre dans un fichier, qui est plutôt lente dû à l'appel successif d'appel de fonction  write. De plus nous pouvons réaliser un bloomfilter afin d'améliorer les performances de notre programme.

 7.     Que manque-t-il à votre correcteur orthographique pour qu’il soit à l’état de l’art ?

Il nous faudrait:
    - un dictionnaire plus grand pour augmenter la qualité des résultats
    - réduire le temps de calcul et la quantité de ram utilisée par notre correcteur orthographique
    - corrigé les cas où la correction par distance ne fonctionne pas cad les mots proches phonétiquement mais éloignés grammaticalement.


