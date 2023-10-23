# Synthèse d'image : Projet "DungeonMaster" - Thomas VALLENTIN

## Introduction 

Ce projet est réalisé au sein du Master 2 : Sciences de l'Image de l'IGM (Université Gustave eiffel). L'objectif est de créer un jeu vidéo inspiré du célèbre Dungeon Master en utilisant la librairie OpenGL. 

[Lien vers la démo vidéo](https://drive.google.com/file/d/1f00EFASyQ0NYUHK83t1ttETJDu4aWn0e/view?usp=share_link)

## Build

Voici les dépendances du projet :

- OpenGL 4.6
- GLFW 3.3
- glm
- stb -> lecture d'image
- assimp -> chargement des modèles 3D
- RapidJSON -> Parsing des fichier de niveau

Toutes ces dépendances sont déclarées en tant que submodules, Il suffit donc de cloner le projet avec ses dépendances et de lancer les commandes suivantes pour compiler l'executable ``DungeonMaster``.

```
git clone --recurse-submodules https://github.com/ThomasVallentin/DungeonMaster.git
cd DungeonMaster
mkdir build
cd build
cmake ..
make
```

## Données d'entrée

Le programme compilé attend un seul et unique argument : un chemin vers le niveau qu'il devra charger. Les niveaux sont au format json et suivent la structure suivante :

```
{
    "name": "Labyrinth",
    "floors": [
        {
            "name": "First floor",
            "map": "Labyrinth.ppm",
            "rewards": [
                ...
            ],
            "monsters": [
                ...
            ]
        }
    ]
}
```

On y trouve la liste des étages, chaque étage étant composé d'une map pointant vers une image ppm dont les pixels décrivent la structure de l'étage selon les règles suivantes :

```
- noir : mur
- blanc : couloir, pièce vide
- marron (#AA7722) : porte
- bleu (#0000FF) : eau
- rouge : L'entrée de l'étage 
- vert : La sortie de l'étage

L'entrée et la sortie doivent être uniques au sein d'un même étage et sont à placer contre un des bords de l'image mais pas dans un coin.
```

**Attention** Dans l'état actuel, le jeu ne supporte que la présence d'un seul étage. Une fois arrivé au bout de ce premier étage, le jeu prendra fin.

On trouve ensuite la description des bonus/rewards (des armes et des potions de soin) et des monstres. 
Les armes possèdent les caractéristiques suivantes : 

```
{
    "name": "Master Sword",
    "origin": [18, 11],
    "model": "Models/MasterSword.fbx",

    "type": "weapon",
    "damage": 2.0,
    "attackSpeed": 1.0  -> nombre d'attaque par seconde
}
```

Les potions celles-ci :

```
{
    "name": "Strange Mixture",
    "origin": [20, 13],
    "model": "Models/Potion.fbx",

    "type": "heal",
    "healing": 5.0
}
```

Et les monstres cette dernière :

```
{
    "name": "Khrryh",
    "origin": [22, 16],
    "model": "Models/Slime.fbx",

    "health": 3,
    "strength": 1.0,
    "attackSpeed": 1.0,  -> nombre d'attaque par seconde
    "speed": 1.5         -> nombre de déplacement par seconde
}
```

Un niveau d'exemple est compris dans le projet : "resources/Levels/Labyrinth.json", on peut donc commencer une première partie en lançant :

```
./DungeonMaster Levels/Labyrinth.json
```

## Les controles

- Flèche du haut : Avancer
- Flèche du bas : Reculer
- Flèche de gauche : Tourner à gauche
- Flèche de droite : Tourner à droite
- Enter : Ouvrir une porte
- Clic gauche : Attaquer


## Spécifications du système

J'ai tenté d'aborder ce projet en me mettant dans la situation d'un studio de jeu vidéo développant son moteur de jeu. Plutôt que de spécialiser le programme entièrement sur les critères du projet, j'ai tenté de développer un framework assez ouvert et modulaire à la manière d'un Unity ou d'un UnrealEngine (toute proportion gardée bien évidemment).

### Entity Component System

L'application est donc basée sur un "Entity Component System" comme cela se fait majoritairement dans ce genre de moteur. Une "Scene" détient un registre dans lequel sont stockées des Components (en l'occurrence de simples struct) et accessibles via des poignées : les Entities. Ces dernières ne détiennent que leur Id et un pointeur vers la Scène qui leur permettent d'accéder aux components qu'elles représentes, ce qui en fait des objets très simples à manipuler et élimine le risque de copier des données sans le vouloir.

### Gestion des resources

Au dela d'éviter de copier des données par erreur, on cherche également à partager les données souhaitées au maximum. Les modèles, les scenes et les textures (qui contiennent les données les plus lourdes) sont gérés par un ResourceManager. Ce singleton se charge du chargement de ces données et y donne accès via d'autres objets plus légers : les ResourcesHandle. Une ResourceHandle consiste principalement en un weak_ptr pointant vers la donnée stockée dans le Manager et qui permet donc d'accéder aux données sans que les Components ne la détiennent. On évite ainsi de charger plusieurs fois un même fichier, de stocker plusieurs fois une resource et d'accéder à une donnée invalide lorsqu'on libère des Resources. 

Les chemins pour accéder aux resources sont exprimées en local par rapport au dossier ``resources`` présent à la racine du projet.

### Fonctionnement du moteur

Le moteur possède plusieurs satellites qui prennent en charge des aspects qu'on pourrait qualifier de communs : le Scripting (ici implémenté sous une forme très simplifiée), la Navigation et le Rendu.

#### Le Scripting::Engine

Un Script est un Component dans lequel on peut déclarer de la logique qui sera executée à différents moment de la boucle de l'application. Ils sont en charge de mettre à jour les données de la scène, de les faire réagir aux inputs du joueur, etc... Le Scripting::Engine répertorie tous les Scripts et execute leur contenu avant que l'application ne fasse le rendu du contenu de la scène.

Le Script le plus commun est le Component ``Scriptable`` auquel on peut passer plusieurs fonctions : ``OnCreate`` appélée à la création du Component, ``OnUpdate`` appelée à chaque frame, ``OnEvent`` appelée lorsqu'un évenement du jeu survient et ``OnDestroy`` appelée lorsque le Component est détruit.

On peut également noter le Trigger qui détecte la présence d'une entité ``target`` dans un certain rayon et emet un TriggerEvent lorsque le target entre, reste ou sors de l'espace qu'il occupe.

NB: Ce genre de systeme devrait sans doute être géré par un systeme de physique et de collision plus avancé, cependant le projet n'ayant pas cette vocation, j'ai volontairement laissé cet aspect de côté.

#### Le Navigation::Engine

Les monstres présent dans le jeu (et le joueur dans une moindre mesure) se basent sur un systeme de navigation externe pour se deplacer dans le niveau. Le Navigation::Engine détient une ``navMap`` (sorte de version 2D d'un NavMesh) générée à partir de la map du niveau. Les couleurs des cellules sont remplacées par des valeurs binaires (des "flags") correspondant aux surfaces représentées (mur, eau, sol).

L'Engine détient une liste d'Agent et se charge de les mettre à jour à chaque frame. Chaque Agent détient un filtre de navigation qui lui permet de demander un chemin à l'Engine (calculé via un algorithm A*) correspondant à ses contraintes. Par exemple un oiseau pourrait se déplacer à la fois sur le sol et sur l'eau mais pas sur les murs, un humain uniquement sur le sol et un fantome partout. Le Navigation::Engine permet également de connaitre la position des autres agents de manière à interdire les cases déjà occupées par un autre Agent. Une fois qu'un chemin adequat à été trouvé, l'Engine le donne à l'Agent et va mettre à jour ses transformations à chaque frame pour qu'il avance le long de ce chemin. 
De manière à optimiser le temps de calcul, la recherche de chemin n'est effectuée que lorsque l'Agent ne se déplace plus (lorsqu'il est sur au centre d'une cellule), même si ce dernier à demandé un nouveau chemin à l'Engine entre temps.

Les Entities souhaitant se déplacer uniquement via la Navigation ont seulement besoin d'avoir un Component NavAgent et un Scriptable appelant ``navAgent.SetDestination()``. Le NavAgent fait simplement la liaison entre le Transform de l'Entity et l'Engine.

De son coté, le joueur ne se déplace pas directement via la Navigation, mais il interroge la navMap au moment de faire chaque déplacement. C'est de cette manière qu'il ne passe pas à travers les murs et les ennemis.

#### Le Pipeline de Rendu

La classe au coeur du rendu s'appelle le Renderer. C'est lui qui va traverser la scène et chercher les différents Components qui représentent des données à rendre. Actuellement deux Components sont pris en compte : Les Meshs/RenderMeshs et les RenderImage.

##### Les Meshs

Le Mesh représente un maillage de triangles (stocké dans le ResourceManager). Plus précisément, un Mesh est un VertexArray composé d'un ou plusieurs VertexBuffer et d'un ElementBuffer.

De son côté, le RenderMesh indique au Renderer qu'il faut afficher un Mesh avec un certain Material. Les deux concepts sont séparés ici pour se baser sur ce qui se fait dans les moteur grand public, dans lesquels on pourrait utiliser un Mesh pour faire autre chose que du rendu comme de la collision par exemple. On trouve également des informations au rendu du mesh qui n'ont leur place ni dans les informations du mesh, ni dans celles du material par exemple doubleSided ou d'autres comme cast/receiveShadows (non implémenté ici).

##### Les Materials

Les Materials sont l'union de deux choses : un Shader (ou program) défini par un vertexShader, un fragmentShader et potentiellement un geometryShader et des tessellationShaders (non utilisés ici) et des uniforms qui représentent les inputs du Material et qui seront passés lorsqu'on souhaite Render un maillage avec le Material.

Les Uniforms des Materials suivent la normes suivantes : ils sont stockés dans un UniformBuffer simple à binder lorsqu'on effectue le rendu. Grace à lui, on peut partager un même shader entre plusieurs Materials tout en stockant des inputs différents. 

Il y a quelques règles à suivre cependant : L'UniformBuffer d'un shader utilisé dans un Material doit s'appeler "MaterialInputs". De plus comme les sampler2Ds sont des types "complexes" pour glsl, ils ne peuvent être contenus dans des UniformBuffers. On stocke donc les textures dans une liste en dehors du block ainsi que l'index de la texture par input. A l'inverse, dans le buffer, on défini les valeurs "scalaires", ainsi qu'un booléen indiquant s'il faut utiliser la valeur scalaire ou sampler la texture. 

Par exemple : 

```
[...]

layout(std140, binding = 0) uniform MaterialInputs
{
    vec3  baseColor;
    bool  baseColorUseTexture;

    float metallic;
    bool  metallicUseTexture;

    float roughness;
    bool  roughnessUseTexture;
};

const int baseColorTexture = 0;
const int metallicTexture = 1;
const int roughnessTexture = 2;

uniform sampler2D uTextures[3];

vec3 SampleBaseColor()
{
    // Using mix instead of an if statement to limit branching
    return mix(baseColor, 
               texture(uTextures[baseColorTexture], vTexCoords).rgb,
               float(baseColorUseTexture));
}

[...]
```

##### Le Rendu d'image

Le Component RenderImage permet d'indiquer au Renderer de rendre une texture sur tout l'écran. Cela permet notamment d'afficher les pseudo menus du jeu.

##### Multisampling, rendu linéaire et matériaux PBR

Le Renderer rend la scène dans un FrameBuffer généré par l'application. Par défault, celle-ci lui force un multisampling à 8 samples de manière à obtenir un rendu moins crénelé. 

L'avantage de générer nous même le frameBuffer de rendu est qu'on peut également changer son type et ainsi faire tous les calculs de lumière en linéaire et avec des matériaux PBR de manière à obtenir un résultat plus "physique".

Toutes les constantes de couleurs sont exprimées en linéaire dans l'espace de couleur du rendu. Une pseudo gestion des espace colorimétriques des textures est également mise en place. Celle-ci permet actuellement de lire une image en Raw, sans modifier ses valeurs, ou d'appliquer un gamma(1/2.2) lors de la lecture pour ramener les couleurs dans un espace linéaire.

Le matériau PBR se base sur la BRDF à microfacettes de Cook-Torrance, une des plus utilisée pour le rendu en temps réel. Celle-ci repose sur trois composants : une distribution de normales (ici Trowbridge-Reitz / GGX), une estimation de l'ombrage geométrique (ici Smith, basé sur Schlick-Beckmann GGX) et un facteur de fresnel (ici l'approximation de Schlick).

Ce modèle permet de rendre des types de matériaux très variés, qu'ils soient métaliques ou non, rugueux ou lisses, etc... Un paramètre d'émission permet aux matériaux d'émettre de la lumière (localement, celle-ci n'impactera par les autres éléments de la scène) et un paramètre de transmission permet de gérer l'opacité du matériau (cependant aucune réfraction n'est effectuée).

##### Post-Processing

Une fois le rendu effectué en linéaire mutli-samplé, celui-ci est copié vers un autre FrameBuffer non-multisamplé de manière à pouvoir l'utiliser en tant que texture dans une une passe de post-process. Cette passe applique un tone-mapping ACESFilmic (Narkowicz 2015) et vient repasser les couleurs dans un espace sRGB en appliquant un gamma 2.2 sur les couleurs.

Cette passe de post-process n'est pas seulement technique, c'est également ici qu'on pourrait ajouter toute sorte d'effet réagissant à des évenements de la scène. Pour le moment, un effet de halo indique lorsque le joueur passe sur une potion (il sera alors vert) ou lorsqu'il se fait toucher par un ennemi (il sera alors rouge).
