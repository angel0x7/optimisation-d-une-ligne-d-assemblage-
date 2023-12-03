//
// Created by gregm on 02/12/2023.
//

#ifndef PROJETING2_ECELECAR_H
#define PROJETING2_ECELECAR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

///-----------------------------------------------------------------------------------------------------------------------------///

void Menu();
void FaireSonChoix(Choix* choix);

///-----------------------------------------------------------------------------------------------------------------------------///

Operation* InitialisationOperation(int nombreOperations);
Station* InitialisationStation(float T0, int nombreOperations );
Choix InitialisationDUchoix();

///-----------------------------------------------------------------------------------------------------------------------------///

void lireNombreOperations(const char* filename, int* nombreOperations);

// Lecture des exclusions à partir du fichier
void LectureFichierExclusion(Operation* operations, FILE* fichierExclusions);

// Lecture des précédences à partir du fichier
void LectureFichierPrecedence(Operation* operations, FILE* fichierPrecedences);

// Lecture des temps des opérations à partir du fichier
void LectureFichierTimeOperation(Operation* operations, FILE* fichierOperations);

// Lecture du temps de cycle à partir du fichier
float LectureFichierTempsCycle(FILE* fichierTempsCycle);
float LectureDesFichiers(Operation* operations, char* filenameExclusions, char* filenamePrecedences, char* filenameOperations, char* filenameTempsCycle);

///-----------------------------------------------------------------------------------------------------------------------------///

// Fonction récursive pour afficher toutes les opérations antérieures
void CreationOperationsAnterieur(Operation* operations, int nombreOperations);
// Fonction récursive pour afficher toutes les opérations précédentes
void CreationOperationsPrecedente(Operation* operations, int indiceOperation, int* dejaAffiche, int* Tableu);
// Calcul des dates au plus tôt et au plus tard pour chaque opération
void CalculerDatesPERT(Operation* operations, int nombreOperations);
// Tri à bulles en fonction du nombre d'antérieurs
void trierParAnterieur(Operation* operations, int nombreOperations);
// Tri à bulles dans l'ordre croissant des numéros des opérations pour une meilleur lisibilité
void trierOperationsDansStations(Station* stations, int nombreStations);

///-----------------------------------------------------------------------------------------------------------------------------///
//Cette fonction vérifie si l'ajout d'une opération dans une station respecte les contraintes d'exclusion
int ContrainteExclusion(Station* stations, Operation* operations, int indiceJ, int IndiceI);
//Cette fonction ajoute une opération à une station
int AjoutOperationDansStation(Station* stations, Operation* operations, int indiceJ, int IndiceI);
//Cette fonction est responsable de l'ensemble de la tâche d'affectation des opérations aux stations
int RepartitionsDesOperations(Station* stations, Operation* operations, Choix choix, int nombreOperations, int nombreStations);

///-----------------------------------------------------------------------------------------------------------------------------///

void afficherRepartition(Station* stations, int nombreStations, Choix choix);

///-----------------------------------------------------------------------------------------------------------------------------///

// Libérer la mémoire
void libererMemoir(Station* stations, Operation* operations, int nombreStations, int nombreOperations);

///-----------------------------------------------------------------------------------------------------------------------------///

#endif //PROJETING2_ECELECAR_H
