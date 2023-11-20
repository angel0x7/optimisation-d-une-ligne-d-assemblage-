
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

void afficherRepartition(Station* stations, int nombreStations) {
    printf("\n\n--------------------------------------------------------------------------------------\n\n");

    printf("Repartition des operations dans les stations :\n");
    for (int i = 0; i < nombreStations; ++i) {
        printf("Station %d :", stations[i].numero);
        for (int j = 0; j < stations[i].nombreOperations; ++j) {
            printf(" %d", stations[i].operations[j].numero);
        }
        printf(" -- temps de cycle: %f\n", stations[i].tempsTotal);
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
        if (operations[i].precedences == NULL) {
            printf("Erreur lors de l'allocation de la precedence de l'operation %d\n", i);
            return NULL;
        }
        operations[i].nombrePrecedences = 0; // Initialisation du nombre de précédences
        operations[i].datePlusTot = 0;
        operations[i].datePlusTard = 0;

        operations[i].anterieur = malloc(N * sizeof(int)); // Initialisation du tableau des anterieurs
        // Vérifiez que l'allocation de mémoire a réussi
        if (operations[i].anterieur == NULL) {
            printf("Erreur lors de l'allocation de la anterieur de l'operation %d\n", i);
            return NULL;
        }
        operations[i].nombreAnterieur = 0;
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
    while (fscanf(fichierOperations, "%d %f", &op1, &Optime) != EOF) {
        operations[op1].tempsExecution = Optime;
    }

    // Contrainte de temps de cycle
    float T0; // Temps de cycle
    fscanf(fichierTempsCycle, "%f", &T0);


    fclose(fichierExclusions);
    fclose(fichierPrecedences);
    fclose(fichierOperations);
    fclose(fichierTempsCycle);

    return T0;
}

// Fonction récursive pour afficher toutes les opérations antérieures
void AfficherOperationsAnterieures(Operation* operations, int indiceOperation, int* dejaAffiche, int* Tableu) {
    if (!dejaAffiche[indiceOperation]) {
        dejaAffiche[indiceOperation] = 1;
        Tableu[indiceOperation] = operations[indiceOperation].numero;

        // Afficher les opérations antérieures
        for (int i = 0; i < operations[indiceOperation].nombrePrecedences; ++i) {
            int precedente = operations[indiceOperation].precedences[i];
            AfficherOperationsAnterieures(operations, precedente, dejaAffiche, Tableu);
        }
    }
}

// Calcul des dates au plus tôt et au plus tard pour chaque opération
void CalculerDatesPERT(Operation* operations) {

    for (int i = 1; i < N; ++i) {
        int dejaAffiche[N] = {0};  // Initialiser le tableau des opérations déjà affichées à 0

        operations[i].anterieur = malloc(N * sizeof(int)); // Initialisation du tableau d'antérioriter
        // Vérifiez que l'allocation de mémoire a réussi
        if (operations[i].anterieur == NULL) {
            printf("Erreur lors de l'allocation de l'exclusion de l'operation %d\n", i);
            exit(1);
        }
        printf("Operation %d : ", operations[i].numero);

        // Calculer la date au plus tôt en fonction des précédences
        float datePlusTot = 0;
        for (int j = 0; j < operations[i].nombrePrecedences; ++j) {
            int precedente = operations[i].precedences[j];
            AfficherOperationsAnterieures(operations, precedente, dejaAffiche, operations[i].anterieur);

            if (operations[precedente].datePlusTard + operations[precedente].tempsExecution > datePlusTot) {
                datePlusTot = operations[precedente].datePlusTard + operations[precedente].tempsExecution;
            }
        }

        for (int j = 0; j < N; ++j) {
            if(operations[i].anterieur[j] != 0){
                printf("%d-", operations[i].anterieur[j]);
            }
        }

        operations[i].datePlusTot = datePlusTot;
        operations[i].datePlusTard = datePlusTot;

        printf("\n");
    }
}

// Fonction pour vérifier la compatibilité d'une opération avec une station
int VerifierCompatibilite(Operation* operation, Station* stations, int nombreStations) {
/*
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
*/
    /*
    // Vérifier les contraintes de précédence avec les opérations existantes dans toutes les stations
    for (int l = 0; l < operation->nombrePrecedences; ++l) {
        int precedente = operation->precedences[l];
        int precedenteTrouvee = 0;

        // Vérifier si l'opération précédente est déjà dans une station
        for (int k = 0; k < nombreStations; ++k) {
            for (int m = 0; m < stations[k].nombreOperations; ++m) {
                if (precedente == stations[k].operations[m].numero && stations[k].operations[m].datePlusTard > operation->datePlusTot) {
                    precedenteTrouvee = 1;
                    break;
                }
            }
            if (precedenteTrouvee) {
                break;
            }
        }

        // Si une opération précédente manque, l'opération en cours n'est pas compatible
        if (!precedenteTrouvee) {
            return 0; // Non compatible
        }
    }
    */

    // Vérifier la contrainte de temps de cycle pour chaque station
    for (int k = 0; k < nombreStations; ++k) {
        if (stations[k].tempsTotal + operation->tempsExecution > stations[k].tempsCycle) {
            return 0; // Non compatible
        }
    }

    return 1; // Compatible
}

// Fonction pour vérifier si toutes les opérations antérieures ont été utilisées dans l'ensemble des stations
int VerifierAnterieuresUtilisees(Operation* operations, int indiceOperation, Station* stations, int nombreStations) {
    for (int l = 0; l <= nombreStations; ++l) {

    }

    return 0;  // Toutes les antérieures n'ont pas été utilisées dans l'ensemble des stations
}

int main() {

    Operation *operations = InitialisationOperation();

    float T0 = LectureDesFichiers(operations);

    // Calcul des dates au plus tôt et au plus tard
    CalculerDatesPERT(operations);

    Station* stations = malloc(N * sizeof(Station));
    int nombreStations = 0;
    for (int i = 0; i < N; ++i) {
        stations[i].nombreOperations = 1;
        stations[i].numero = 0;
        stations[i].tempsTotal = 0;
        stations[i].tempsCycle = T0;
        stations[i].operations = malloc(N * sizeof(Operation));
    }

    // Répartition des opérations dans les stations en respectant les contraintes d'exclusion, de précédence et de temps de cycle
    for (int i = 1; i < N; ++i) {
        int placeTrouvee = 0;

        // Parcourir les stations existantes pour trouver une station disponible
        for (int j = 0; j < nombreStations && !placeTrouvee; ++j) {
           // int compatible = VerifierCompatibilite(&operations[i], stations, nombreStations);

            int compatible = 1, compatible2 = 1, compatible3 = 1;

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
/*
            // Vérifier si toutes les antérieures ont été utilisées dans l'ensemble des stations
            int toutesAnterieuresUtilisees = 1;  // Suppose que toutes les antérieures sont utilisées

            for (int k = 0; k < N && toutesAnterieuresUtilisees == 1; ++k) {
                if (operations[i].anterieur[k] != 0) {
                    //int anterieureATrouver = operations[i].anterieur[k];

                    int anterieureTrouvee = 0;

                    for (int l = 0; l <= nombreStations; ++l) {
                        for (int m = 0; m < stations[j].nombreOperations; ++m) {
                            if (stations[j].operations[m].numero == operations[i].anterieur[k]) {
                                anterieureTrouvee = 1;
                                break;
                            }
                        }
                    }


                    if (!anterieureTrouvee) {
                        toutesAnterieuresUtilisees = 0;
                        break;  // Si une antérieure n'est pas trouvée, arrêter la recherche
                    }
                }
            }

            if (toutesAnterieuresUtilisees) {
                compatible2 = 1;  // Toutes les antérieures ont été utilisées dans cette station
            }


*/
            // Vérifier la contrainte de temps de cycle pour chaque station
            if (stations[j].tempsTotal + operations[i].tempsExecution > stations[j].tempsCycle) {
                compatible = 0; // Non compatible
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
            stations[nombreStations].operations[0] = operations[i];
            stations[nombreStations].tempsTotal = operations[i].tempsExecution;
            //stations[nombreStations].nombreOperations = 1;
            //stations[nombreStations].tempsCycle = T0;  // Initialisation du temps de cycle pour chaque nouvelle station
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