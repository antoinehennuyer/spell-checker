Questions:
==========

A VOIR 1 / 4 / 5 / 6 

 1.     Décrivez les choix de design de votre programme
 Projet en C++
 Deux fichiers exécutables: TextMiningCompiler et TextMiningApp
 
 Etapes:
    Compiler:
    1) on sort notre words.txt
    2) on crée notre partricia tree à partir de notre liste de mots triés
    3) on écrit le patricia tree dans un fichier binaire à l'aide de streams
    App:
    4) on recrée notre structure patricia tree à partir du fichier binaire à l'aide de streams
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
Nous avons décider d'utiliser un PatriciaTrie, car c'est un moyen d'utiliser moins de RAM qu'un trie normal. 
Cela nous as permit d'améliorer considérablement les performances de notre programme.

 5.     Proposez un réglage automatique de la distance pour un programme qui prend juste une chaîne de caractères en entrée, donner le processus d’évaluation ainsi que les résultats
 
 6.     Comment comptez vous améliorer les performances de votre programme
testesuite devient plus lente comme elle est écrite en python.
implémenter bloomfilter

 7.     Que manque-t-il à votre correcteur orthographique pour qu’il soit à l’état de l’art ?

Il nous faudrait:
    - un dictionnaire plus grand pour augmenter la qualité des résultats
    - réduire le temps de calcul et la quantité de ram utilisée par notre correcteur orthographique
    - corrigé les cas où la correction par distance ne fonctionne pas cad les mots proches phonétiquement mais éloignés grammaticalement.


