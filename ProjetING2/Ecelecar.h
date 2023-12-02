//
// Created by gregm on 02/12/2023.
//

#ifndef PROJETING2_ECELECAR_H
#define PROJETING2_ECELECAR_H
typedef struct Operation {
    int numero; // Numéro de l'opération
    float tempsExecution; // Temps que mets l'opération à s'éffectuer
    int station; // Numéro de la station où se situe l'opération
    int* exclusion; // Tableau d'exclusion
    int nombreExclusions; // Nombre d'éléments dans le tableau d'exclusion
    int* precedences; // Tableau des opérations qui doivent précéder celle-ci
    int* Toutprecedences; // Tableau des opérations qui doivent précéder celle-ci
    int nombrePrecedences; // Nombre d'éléments dans le tableau des précédences
    float datePlusTot;  // Date au plus tôt
    float datePlusTard; // Date au plus tard
    int* anterieur;
    int nombreAnterieur;
} Operation;
typedef struct Station {
    int numero; // Numéro de la station
    Operation* operations; // Tableau de structure contenant les opérations
    int nombreOperations; // Nombre d'opérations dans la station
    float tempsTotal; // Temps total des opérations dans la station
    float tempsCycle; // Temps de cycle de la station
} Station;

typedef struct Choix{
    int numero;
    char* nom[6];
}Choix;

#endif //PROJETING2_ECELECAR_H
