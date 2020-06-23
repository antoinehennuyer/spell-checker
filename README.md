Questions:
==========
 1.     Décrivez les choix de design de votre programme
Tout d'abord, nous avons décider de trier le fichier en input pour faciliter la création de l'arbre. Aprés l'avoir trier
nous le parcourons afin de créer l'arbre. Ensuite nous écrivons l'arbre dans un fichier binaire. Ensuite, TextMiningApp
va ouvrir le binaire afin de l'utiliser pour les différents requêtes.
 2.     Listez l’ensemble des tests effectués sur votre programme (en plus des units tests)
Pour chaque requêtes demandé nous comparons le stdout de la référence et de notre programme. De plus, comparons si 
tous les mots de la ref sont bien présent dans notre sortie.
 3.     Avez-vous détecté des cas où la correction par distance ne fonctionnait pas (même avec une distance élevée) ?
 4.     Quelle est la structure de données que vous avez implémentée dans votre projet, pourquoi ?
Nous avons décider d'utiliser un PatriciaTrie, car c'est un moyen d'utiliser moins de RAM qu'un trie normal. 
Cela nous as permit d'améliorer considérablement les performances de notre programme.
 5.     Proposez un réglage automatique de la distance pour un programme qui prend juste une chaîne de caractères en entrée, donner le processus d’évaluation ainsi que les résultats
 6.     Comment comptez vous améliorer les performances de votre programme
testesuite devient plus lente comme elle est écrite en python.
 7.     Que manque-t-il à votre correcteur orthographique pour qu’il soit à l’état de l’art ?
Pour que le notre soit à l'état de l'art il faudra que le programme propose à l'utilisateur seulement le premier élément de la liste retourner.

