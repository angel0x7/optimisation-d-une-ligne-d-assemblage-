#include <stdio.h>
#include <stdlib.h>

#define MAX_FILENAME_LENGTH 100

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

void afficherRepartition(Station* stations, int nombreStations, Choix choix) {
    printf("\n\n--------------------------------------------------------------------------------------\n\n");

    printf("Repartition des operations dans les stations en fonction de la %s:\n", choix.nom[choix.numero]);
    for (int i = 0; i < nombreStations; ++i) {
        printf("Station %d :", stations[i].numero);
        for (int j = 0; j < stations[i].nombreOperations; ++j) {
            printf(" %d", stations[i].operations[j].numero);
        }
        if ((choix.numero != 1) && (choix.numero != 3)){
            printf(" -- temps de cycle: %f", stations[i].tempsTotal);
        }
        printf("\n");
    }
    printf("\n--------------------------------------------------------------------------------------\n\n");
}

///-----------------------------------------------------------------------------------------------------------------------------///

Operation* InitialisationOperation(int nombreOperations){
    // Initialisation des opérations
    Operation* operations = malloc(nombreOperations * sizeof(Operation));

    // Vérifiez que l'allocation de mémoire a réussi
    if (operations == NULL) {
        printf("Erreur lors de l'allocation des operation\n");
        return NULL;
    }

    for (int i = 1; i < nombreOperations; ++i) {
        operations[i].numero = i;
        operations[i].tempsExecution = 0;
        operations[i].station = 0;  // Initialisation à 0

        operations[i].exclusion = malloc(nombreOperations * sizeof(int)); // Initialisation du tableau d'exclusion
        // Vérifiez que l'allocation de mémoire a réussi
        if (operations[i].exclusion == NULL) {
            printf("Erreur lors de l'allocation de l'exclusion de l'operation %d\n", i);
            return NULL;
        }
        operations[i].nombreExclusions = 0; // Initialisation du nombre d'exclusions

        operations[i].precedences = malloc(nombreOperations * sizeof(int)); // Initialisation du tableau des précédences
        // Vérifiez que l'allocation de mémoire a réussi
        if (operations[i].precedences == NULL) {
            printf("Erreur lors de l'allocation de la precedence de l'operation %d\n", i);
            return NULL;
        }
        operations[i].Toutprecedences = malloc(nombreOperations * sizeof(int)); // Initialisation du tableau d'antériorité
        // Vérifiez que l'allocation de mémoire a réussi
        if (operations[i].Toutprecedences == NULL) {
            printf("Erreur lors de l'allocation de l'exclusion de l'operation %d\n", i);
            exit(1);
        }
        operations[i].nombrePrecedences = 0; // Initialisation du nombre de précédences
        operations[i].datePlusTot = 0;
        operations[i].datePlusTard = 0;

        operations[i].anterieur = malloc(nombreOperations * sizeof(int)); // Initialisation du tableau des anterieurs
        // Vérifiez que l'allocation de mémoire a réussi
        if (operations[i].anterieur == NULL) {
            printf("Erreur lors de l'allocation de la anterieur de l'operation %d\n", i);
            return NULL;
        }
        operations[i].nombreAnterieur = 0;

        for (int j = 1; j < nombreOperations; ++j) {
            operations[i].exclusion[j] = 0;
            operations[i].precedences[j] = 0;
            operations[i].Toutprecedences[j] = 0;
            operations[i].anterieur[j] = 0;
        }
    }
    return operations;
}

Station* InitialisationStation(float T0, int nombreOperations ){
    Station* stations = malloc(nombreOperations * sizeof(Station));

    for (int i = 0; i < nombreOperations; ++i) {
        stations[i].nombreOperations = 1;
        stations[i].numero = 0;
        stations[i].tempsTotal = 0;
        stations[i].tempsCycle = T0;
        stations[i].operations = malloc(nombreOperations * sizeof(Operation));
    }
    return stations;
}

Choix InitialisationDUchoix(){
    Choix choix;
    choix.numero = 0;
    choix.nom[1] = "contrainte d'exclusion seul";
    choix.nom[2] = "contrainte de precedence et de temps de cycle";
    choix.nom[3] = "contrainte d'exclusion et de precedence";
    choix.nom[4] = "contrainte d'exclusion et temps de cycle";
    choix.nom[5] = "contrainte d'exclusion de precedence et de temps de cycle";
    return choix;
}

///-----------------------------------------------------------------------------------------------------------------------------///

void lireNombreOperations(const char* filename, int* nombreOperations) {
    FILE* fichier = fopen(filename, "r");

    if (fichier == NULL) {
        printf("Erreur de lecture du fichier %s\n", filename);
        exit(1);
    }

    *nombreOperations = 0;
    int op1, op2;

    while (fscanf(fichier, "%d %d", &op1, &op2) != EOF) {
        if (op1 > *nombreOperations) {
            *nombreOperations = op1;
        }
        if (op2 > *nombreOperations) {
            *nombreOperations = op2;
        }
    }
    *nombreOperations += 1;

    fclose(fichier);
}

// Lecture des exclusions à partir du fichier
void LectureFichierExclusion(Operation* operations, FILE* fichierExclusions){
    int op1, op2;
    while (fscanf(fichierExclusions, "%d %d", &op1, &op2) != EOF) {
        operations[op1].exclusion[operations[op1].nombreExclusions++] = op2;
        operations[op2].exclusion[operations[op2].nombreExclusions++] = op1;
    }
}

// Lecture des précédences à partir du fichier
void LectureFichierPrecedence(Operation* operations, FILE* fichierPrecedences){
    int op1, op2;
    while (fscanf(fichierPrecedences, "%d %d", &op1, &op2) != EOF) {
        operations[op1].precedences[operations[op1].nombrePrecedences++] = op2;
    }
}

void LectureFichierTimeOperation(Operation* operations, FILE* fichierOperations){
    float Optime;
    int op1;
    while (fscanf(fichierOperations, "%d %f", &op1, &Optime) != EOF) {
        operations[op1].tempsExecution = Optime;
    }
}

// Contrainte de temps de cycle
float LectureFichierTempsCycle(FILE* fichierTempsCycle){
    float T0; // Temps de cycle
    if (fscanf(fichierTempsCycle, "%f", &T0) != 1) {
        printf("Erreur lors de la lecture du temps de cycle\n");
        exit(1);
    }
    return T0;
}

float LectureDesFichiers(Operation* operations, char* filenameExclusions, char* filenamePrecedences, char* filenameOperations, char* filenameTempsCycle){

    FILE* fichierExclusions = fopen(filenameExclusions, "r");
    FILE* fichierPrecedences = fopen(filenamePrecedences, "r");
    FILE* fichierOperations = fopen(filenameOperations, "r");
    FILE* fichierTempsCycle = fopen(filenameTempsCycle, "r");

    if (fichierExclusions == NULL || fichierPrecedences == NULL || fichierOperations == NULL || fichierTempsCycle == NULL) {
        printf("Erreur de lecture des fichiers\n");
        exit(1);
    }

    LectureFichierExclusion(operations, fichierExclusions);
    LectureFichierPrecedence(operations, fichierPrecedences);
    LectureFichierTimeOperation(operations, fichierOperations);
    float T0 = LectureFichierTempsCycle(fichierTempsCycle);

    fclose(fichierExclusions);
    fclose(fichierPrecedences);
    fclose(fichierOperations);
    fclose(fichierTempsCycle);

    return T0;
}

///-----------------------------------------------------------------------------------------------------------------------------///

// Fonction récursive pour afficher toutes les opérations antérieures
void CreationOperationsAnterieur(Operation* operations, int nombreOperations)  {
    for (int i = 1; i < nombreOperations; ++i) {
        for (int j = 1; j < nombreOperations; ++j) {
            int precedente = operations[i].Toutprecedences[j];
            if (precedente != 0) {
                operations[precedente].anterieur[operations[precedente].nombreAnterieur++] = operations[i].numero;
            }
        }
    }
}

// Fonction récursive pour afficher toutes les opérations précédentes
void CreationOperationsPrecedente(Operation* operations, int indiceOperation, int* dejaAffiche, int* Tableu) {
    if (!dejaAffiche[indiceOperation]) {
        dejaAffiche[indiceOperation] = 1;
        Tableu[indiceOperation] = operations[indiceOperation].numero;

        // Afficher les opérations antérieures
        for (int i = 0; i < operations[indiceOperation].nombrePrecedences; ++i) {
            int precedente = operations[indiceOperation].precedences[i];
            CreationOperationsPrecedente(operations, precedente, dejaAffiche, Tableu);
        }
    }
}

// Calcul des dates au plus tôt et au plus tard pour chaque opération
void CalculerDatesPERT(Operation* operations, int nombreOperations) {

    for (int i = 1; i < nombreOperations; ++i) {

        // Calculer la date au plus tôt en fonction des précédences
        float datePlusTot = 0;
        for (int j = 0; j < operations[i].nombrePrecedences; ++j) {
            int precedente = operations[i].precedences[j];
            int *dejaAffichePrecedences = malloc(nombreOperations * sizeof(int));

            // Vérifiez que l'allocation de mémoire a réussi
            if (dejaAffichePrecedences == NULL) {
                printf("Erreur lors de l'allocation du tableau dejaAffichePrecedences\n");
                exit(1);
            }

            // Initialiser le tableau des opérations déjà affichées à 0
            for (int k = 0; k < nombreOperations; ++k) {
                dejaAffichePrecedences[k] = 0;
            }

            CreationOperationsPrecedente(operations, precedente, dejaAffichePrecedences, operations[i].Toutprecedences);

            if (operations[precedente].datePlusTard + operations[precedente].tempsExecution > datePlusTot) {
                datePlusTot = operations[precedente].datePlusTard + operations[precedente].tempsExecution;
            }
            free(dejaAffichePrecedences);
        }

        operations[i].datePlusTot = datePlusTot;
        operations[i].datePlusTard = datePlusTot;
    }
}

// Tri à bulles en fonction du nombre d'antérieurs
void trierParAnterieur(Operation* operations, int nombreOperations) {
    for (int i = 1; i < nombreOperations - 1; ++i) {
        for (int j = 1; j < nombreOperations - i; ++j) {
            if (operations[j].nombreAnterieur > operations[j+1].nombreAnterieur) {
                Operation temp = operations[j];
                operations[j] = operations[j+1];
                operations[j+1] = temp;
            }
        }
    }
}

// Tri à bulles dans l'ordre croissant des numéros des opérations pour une meilleur lisibilité
void trierOperationsDansStations(Station* stations, int nombreStations) {
    for (int i = 0; i < nombreStations; ++i) {
        for (int j = 0; j < stations[i].nombreOperations - 1; ++j) {
            for (int k = 0; k < stations[i].nombreOperations - j - 1; ++k) {
                if (stations[i].operations[k].numero > stations[i].operations[k + 1].numero) {
                    // Échangez les opérations si elles ne sont pas dans l'ordre croissant
                    Operation temp = stations[i].operations[k];
                    stations[i].operations[k] = stations[i].operations[k + 1];
                    stations[i].operations[k + 1] = temp;
                }
            }
        }
    }
}

///-----------------------------------------------------------------------------------------------------------------------------///

void Menu(){
    printf("\n----------------------------Choissisez les contraintes que vous voulez----------------------------------\n");
    printf("|                   1- Contrainte d'exclusion seul                                                     |\n");
    printf("|                   2- Contrainte de precedence et de temps de cycle                                   |\n");
    printf("|                   3- Contrainte d'exclusion et de precedence                                         |\n");
    printf("|                   4- Contrainte d'exclusion et temps de cycle                                        |\n");
    printf("|                   5- Contrainte d'exclusion de precedence et de temps de cycle                       |\n");
    printf("|                   6- Quitter                                                                         |\n");
    printf("--------------------------------------------------------------------------------------------------------\n");
}

void FaireSonChoix(Choix* choix){
    do {
        scanf("%d", &choix->numero);
    } while (choix->numero < 1 || choix->numero > 6);
    if(choix->numero == 6){
        exit(1);
    }
}

///-----------------------------------------------------------------------------------------------------------------------------///

// Libérer la mémoire
void libererMemoir(Station* stations, Operation* operations, int nombreStations, int nombreOperations){

    // Libérer la mémoire des opérations
    for (int i = 1; i < nombreOperations; ++i) {
        free(operations[i].exclusion);
        free(operations[i].precedences);
        free(operations[i].anterieur); // Libére la mémoire des antérieurs
        free(operations[i].Toutprecedences); // Libére la mémoire de Toutprecedences
    }
    free(operations);

    // Libérer la mémoire des stations
    for (int i = 0; i < nombreStations; ++i) {
        free(stations[i].operations);
    }
    free(stations);
}

///-----------------------------------------------------------------------------------------------------------------------------///

int ContrainteExclusion(Station* stations, Operation* operations, int indiceJ, int IndiceI){
    // Vérifier les contraintes d'exclusion avec les opérations existantes dans la station
    for (int k = 0; k < stations[indiceJ].nombreOperations; ++k) {
        int opExistante = stations[indiceJ].operations[k].numero;

        // Vérifier si l'opération en cours est exclue de l'opération existante
        for (int l = 0; l < operations[IndiceI].nombreExclusions; ++l) {
            if (opExistante == operations[IndiceI].exclusion[l]) {
                return 0;
            }
        }
    }
    return 1;
}

int AjoutOperationDansStation(Station* stations, Operation* operations, int indiceJ, int IndiceI){
    stations[indiceJ].operations = realloc(stations[indiceJ].operations, (stations[indiceJ].nombreOperations + 1) * sizeof(Operation));
    stations[indiceJ].operations[stations[indiceJ].nombreOperations] = operations[IndiceI];
    stations[indiceJ].nombreOperations++;
    stations[indiceJ].tempsTotal += operations[IndiceI].tempsExecution;
    operations[IndiceI].station = stations[indiceJ].numero;
    return 1;
}

int RepartitionsDesOperations(Station* stations, Operation* operations, Choix choix, int nombreOperations, int nombreStations){
    for (int i = 1; i < nombreOperations; ++i) {
        if(operations[i].nombreAnterieur != 0 || operations[i].nombrePrecedences != 0 || operations[i].nombreExclusions != 0){

            int placeTrouvee = 0;
            int toutesAnterieuresUtilisees = 0;  // Suppose que toutes les antérieures sont utilisées

            // Parcourir les stations existantes pour trouver une station disponible
            for (int j = 0; j < nombreStations && !placeTrouvee; ++j) {

                int Exclue = ContrainteExclusion(stations, operations, j, i);

                // Vérifier si toutes les antérieures ont été utilisées dans l'ensemble des stations

                for (int k = 0; k < operations[i].nombreAnterieur; ++k) {
                    int anterieureATrouver = operations[i].anterieur[k];
                    // Vérifier si l'opération antérieure est déjà dans une autre station
                    for (int m = 0; m < stations[j].nombreOperations; ++m) {
                        if (stations[j].operations[m].numero == anterieureATrouver) {
                            toutesAnterieuresUtilisees += 1;
                        }
                    }
                }

                if (choix.numero == 1){
                    if (Exclue) {
                        placeTrouvee = AjoutOperationDansStation(stations, operations, j, i);
                    }
                } else if (choix.numero == 2){
                    if (toutesAnterieuresUtilisees == operations[i].nombreAnterieur && (stations[j].tempsTotal + operations[i].tempsExecution) < stations[j].tempsCycle) {
                        placeTrouvee = AjoutOperationDansStation(stations, operations, j, i);

                    }
                } else if (choix.numero == 3){
                    if (Exclue && toutesAnterieuresUtilisees == operations[i].nombreAnterieur) {
                        placeTrouvee = AjoutOperationDansStation(stations, operations, j, i);
                    }
                } else if (choix.numero == 4){
                    if (Exclue && (stations[j].tempsTotal + operations[i].tempsExecution) < stations[j].tempsCycle) {
                        placeTrouvee = AjoutOperationDansStation(stations, operations, j, i);
                    }
                } else if (choix.numero == 5){
                    if (Exclue && toutesAnterieuresUtilisees == operations[i].nombreAnterieur && (stations[j].tempsTotal + operations[i].tempsExecution) < stations[j].tempsCycle){
                        placeTrouvee = AjoutOperationDansStation(stations, operations, j, i);
                    }
                }
            }

            // Si aucune station n'est compatible, créer une nouvelle station
            if (!placeTrouvee) {
                stations[nombreStations].numero = nombreStations + 1;
                stations[nombreStations].operations[0] = operations[i];
                stations[nombreStations].tempsTotal = operations[i].tempsExecution;
                operations[i].station = stations[nombreStations].numero;
                nombreStations++;
            }
        }
    }
    return nombreStations;
}

///-----------------------------------------------------------------------------------------------------------------------------///

int main() {

    char filenameExclusions[MAX_FILENAME_LENGTH];
    char filenamePrecedences[MAX_FILENAME_LENGTH];
    char filenameOperations[MAX_FILENAME_LENGTH];
    char filenameTempsCycle[MAX_FILENAME_LENGTH];
    FILE* fichierExclusions;
    FILE* fichierPrecedences;
    FILE* fichierOperations;
    FILE* fichierTempsCycle;
    do {
        printf("Entrez le nom du fichier d'exclusions :");
        scanf("%s", filenameExclusions);
        fichierExclusions = fopen(filenameExclusions, "r");
    } while (fichierExclusions == NULL);

    do {
        printf("\nEntrez le nom du fichier de precedences :");
        scanf("%s", filenamePrecedences);
        fichierPrecedences = fopen(filenamePrecedences, "r");
    } while (fichierPrecedences == NULL);
    
    do {
        printf("\nEntrez le nom du fichier d'operation :");
        scanf("%s", filenameOperations);
        fichierOperations = fopen(filenameOperations, "r");
    } while (fichierOperations == NULL);

    do {
        printf("\nEntrez le nom du fichier de temps de cycle :");
        scanf("%s", filenameTempsCycle);
        fichierTempsCycle = fopen(filenameTempsCycle, "r");
    } while (fichierTempsCycle == NULL);
    
    while (1){

        Menu();
        Choix choix = InitialisationDUchoix();
        FaireSonChoix(&choix);

        // Lire le nombre d'opérations depuis les fichiers
        int nombreOperations = 0;
        lireNombreOperations(filenameExclusions, &nombreOperations);
        lireNombreOperations(filenamePrecedences, &nombreOperations);

        Operation *operations = InitialisationOperation(nombreOperations);

        float T0 = LectureDesFichiers(operations, filenameExclusions, filenamePrecedences, filenameOperations, filenameTempsCycle);

        // Calcul des dates au plus tôt et au plus tard
        CalculerDatesPERT(operations, nombreOperations);

        CreationOperationsAnterieur(operations, nombreOperations);

        // Tri des opérations par ordre croissant du nombre d'antérieurs
        trierParAnterieur(operations, nombreOperations);

        Station* stations = InitialisationStation(T0, nombreOperations);
        int nombreStations = 0;

        // Répartition des opérations dans les stations en respectant les contraintes d'exclusion, de précédence et de temps de cycle
        nombreStations = RepartitionsDesOperations(stations, operations, choix, nombreOperations, nombreStations);

        // Triez les opérations à l'intérieur de chaque station
        trierOperationsDansStations(stations, nombreStations);

        // Afficher la répartition des opérations dans les stations
        afficherRepartition(stations, nombreStations, choix);

        // Libérer la mémoire
        libererMemoir(stations, operations, nombreStations, nombreOperations);
    }
}
