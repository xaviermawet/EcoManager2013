/* ------------------------------------- *
 *            EcoManager 2013            *
 * ------------------------------------- */

Cette version de EcoMotion est compatible avec les anciens fichiers de données et les nouveaux.

Pour les anciens:
-------------------
Compiler le programme dans la branche master. Le programme contient un graphique Qwt pour les données mégasquirt
et les graphique de JB pour les autres.
Afin d'importer les anciennes données dans l'application, vous devez ajouter "_old" à la première méthode 
on_actionImport_triggered de la classe MainWindow et enlever ce même sufixe de la deuxième méthode
on_actionImport_triggered. Ensuite dans la classe GeoCoordinate, vous devez commenter la première partie du constructeur
et décommenté la deuxième partie.

Pour les nouvelles données :
---------------------------------
Vous devez compiler le programme tel qu'il est dans la branche master.

la branche Qwt:
------------------
Le programme dans cette branche ne contient plus que des graphiques Qwt, des problèmes lors de l'importation des données
dans la base ont été corrigés, les signaux de sélection dans les graphiques Qwt sont opérationnels etc ... mais
les courbes de vitesses par rapport au temps et à la distance ne sont pas affichées et les signaux de sélection(s) ne
sont pas connectés.
