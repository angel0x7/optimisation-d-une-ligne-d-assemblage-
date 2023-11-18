#include <stdio.h>
#include <stdlib.h>

#define N 36
#define MAX_FILENAME_LENGTH 100

typedef struct Operation {
    int numero; // Numéro de l'opération
    float tempsExecution; // Temps que mets l'opération à s'éffectuer
    int station; // Numéro de la station où se situe l'opération
    int* exclusion; // Tableau d'exclusion
    int nombreExclusions; // Nombre d'éléments dans le tableau d'exclusion
    int* precedences; // Tableau des opérations qui doivent précéder celle-ci
    int nombrePrecedences; // Nombre d'éléments dans le tableau des précédences
} Operation;

typedef struct Station {
    int numero; // Numéro de la station
    Operation* operations; // Tableau de structure contenant les opérations
    int nombreOperations; // Nombre d'opérations dans la station
    float tempsTotal; // Temps total des opérations dans la station
    float tempsCycle; // Temps de cycle de la station
} Station;

void afficherRepartition(Station* stations, int nombreStations) {
    printf("\n\n--------------------------------------------------------------------------------------\n\n");

    printf("Repartition des operations dans les stations :\n");
    for (int i = 0; i < nombreStations; ++i) {
        printf("Station %d :", stations[i].numero);
        for (int j = 0; j < stations[i].nombreOperations; ++j) {
            printf(" %d", stations[i].operations[j].numero);
        }
        printf("\n");
    }
    printf("\n--------------------------------------------------------------------------------------\n\n");
}

Operation* InitialisationOperation(){
    // Initialisation des opérations
    Operation* operations = malloc(N * sizeof(Operation));

    // Vérifiez que l'allocation de mémoire a réussi
    if (operations == NULL) {
        printf("Erreur lors de l'allocation des operation\n");
        return NULL;
    }

    for (int i = 1; i < N; ++i) {
        operations[i].numero = i;
        operations[i].tempsExecution = 0;
        operations[i].station = 0;  // Initialisation à 0
        operations[i].exclusion = malloc(N * sizeof(int)); // Initialisation du tableau d'exclusion
        // Vérifiez que l'allocation de mémoire a réussi
        if (operations[i].exclusion == NULL) {
            printf("Erreur lors de l'allocation de l'exclusion de l'operation %d\n", i);
            return NULL;
        }
        operations[i].nombreExclusions = 0; // Initialisation du nombre d'exclusions
        operations[i].precedences = malloc(N * sizeof(int)); // Initialisation du tableau des précédences
        // Vérifiez que l'allocation de mémoire a réussi
        if (operations[i].exclusion == NULL) {
            printf("Erreur lors de l'allocation de la precedence de l'operation %d\n", i);
            return NULL;
        }
        operations[i].nombrePrecedences = 0; // Initialisation du nombre de précédences
    }
    return operations;
}

float LectureDesFichiers(Operation* operations){

    char filenameExclusions[MAX_FILENAME_LENGTH];
    char filenamePrecedences[MAX_FILENAME_LENGTH];
    char filenameOperations[MAX_FILENAME_LENGTH];
    char filenameTempsCycle[MAX_FILENAME_LENGTH];

    printf("Entrez le nom du fichier d'exclusions :");
    scanf("%s", filenameExclusions);
    printf("\nEntrez le nom du fichier de precedences :");
    scanf("%s", filenamePrecedences);
    printf("\nEntrez le nom du fichier d'operation :");
    scanf("%s", filenameOperations);
    printf("\nEntrez le nom du fichier de temps de cycle :");
    scanf("%s", filenameTempsCycle);

    FILE* fichierExclusions = fopen("../exclusions.txt", "r");
    //FILE* fichierExclusions = fopen(filenameExclusions, "r");

    FILE* fichierPrecedences = fopen("../precedences.txt", "r");
    //FILE* fichierExclusions = fopen(filenamePrecedences, "r");

    FILE* fichierOperations = fopen("../operations.txt", "r");
    //FILE* fichierExclusions = fopen(filenameOperations, "r");

    FILE* fichierTempsCycle = fopen("../temps_cycle.txt", "r");
    //FILE* fichierExclusions = fopen(filenameTempsCycle, "r");

    if (fichierExclusions == NULL || fichierPrecedences == NULL || fichierOperations == NULL || fichierTempsCycle == NULL) {
        printf("Erreur de lecture des fichiers\n");
        exit(1);
    }

    // Lecture des exclusions à partir du fichier
    int op1, op2;
    while (fscanf(fichierExclusions, "%d %d", &op1, &op2) != EOF) {
        operations[op1].exclusion[operations[op1].nombreExclusions++] = op2;
        operations[op2].exclusion[operations[op2].nombreExclusions++] = op1;
    }

    // Lecture des précédences à partir du fichier
    while (fscanf(fichierPrecedences, "%d %d", &op1, &op2) != EOF) {
        operations[op2].precedences[operations[op2].nombrePrecedences++] = op1;
    }

    float Optime;
    while (fscanf(fichierOperations, "%d %fl", &op1, &Optime) != EOF) {
        operations[op1].tempsExecution = Optime;
    }

    // Contrainte de temps de cycle
    float T0; // Temps de cycle
    fscanf(fichierTempsCycle, "%fl", &T0);


    fclose(fichierExclusions);
    fclose(fichierPrecedences);
    fclose(fichierOperations);
    fclose(fichierTempsCycle);

    return T0;
}

int main() {

    Operation *operations = InitialisationOperation();

    float T0 = LectureDesFichiers(operations);

    // Répartition des opérations dans les stations en respectant les contraintes d'exclusion, de précédence et de temps de cycle
    Station* stations = malloc(N * sizeof(Station));
    int nombreStations = 0;

    for (int i = 1; i < N; ++i) {
        int placeTrouvee = 0;

        // Parcourir les stations existantes pour trouver une station disponible
        for (int j = 0; j < nombreStations && !placeTrouvee; ++j) {
            int compatible = 1;

            // Vérifier les contraintes d'exclusion avec les opérations existantes dans la station
            for (int k = 0; k < stations[j].nombreOperations && compatible; ++k) {
                int opExistante = stations[j].operations[k].numero;

                // Vérifier si l'opération en cours est exclue de l'opération existante
                for (int l = 0; l < operations[i].nombreExclusions; ++l) {
                    if (opExistante == operations[i].exclusion[l]) {
                        compatible = 0;
                        break;
                    }
                }
            }

            // Vérifier les contraintes de précédence avec les opérations existantes dans la station
            for (int k = 0; k < stations[j].nombreOperations && compatible; ++k) {
                int opExistante = stations[j].operations[k].numero;

                // Vérifier si l'opération en cours doit être précédée par l'opération existante
                for (int l = 0; l < operations[i].nombrePrecedences; ++l) {
                    if (opExistante == operations[i].precedences[l]) {
                        compatible = 0;
                        break;
                    }
                }
            }

            // Vérifier la contrainte de temps de cycle pour chaque station
            if (stations[j].tempsTotal + operations[i].tempsExecution > stations[j].tempsCycle) {
                compatible = 0;
            }

            // Si compatible, ajouter l'opération à la station
            if (compatible) {
                stations[j].operations = realloc(stations[j].operations, (stations[j].nombreOperations + 1) * sizeof(Operation));
                stations[j].operations[stations[j].nombreOperations] = operations[i];
                stations[j].nombreOperations++;
                stations[j].tempsTotal += operations[i].tempsExecution;
                operations[i].station = stations[j].numero;
                placeTrouvee = 1;
            }
        }

        // Si aucune station n'est compatible, créer une nouvelle station
        if (!placeTrouvee) {
            stations[nombreStations].numero = nombreStations + 1;
            stations[nombreStations].operations = malloc(sizeof(Operation));
            stations[nombreStations].operations[0] = operations[i];
            stations[nombreStations].nombreOperations = 1;
            stations[nombreStations].tempsTotal = operations[i].tempsExecution;
            stations[nombreStations].tempsCycle = T0;  // Initialisation du temps de cycle pour chaque nouvelle station
            operations[i].station = stations[nombreStations].numero;
            nombreStations++;
        }
    }

    // Afficher la répartition des opérations dans les stations
    afficherRepartition(stations, nombreStations);

    // Libérer la mémoire
    for (int i = 0; i < N; ++i) {
        free(operations[i].exclusion);
        free(operations[i].precedences);
    }
    free(operations);

    for (int i = 0; i < nombreStations; ++i) {
        free(stations[i].operations);
    }
    free(stations);

    return 0;
}
