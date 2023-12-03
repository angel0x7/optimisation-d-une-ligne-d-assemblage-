#include "Ecelecar.h"

#define MAX_FILENAME_LENGTH 100


int main() {
    // Allocation dynamique de mémoire pour les noms de fichiers
    char* filenameExclusions = malloc(sizeof(char));
    char* filenamePrecedences = malloc(sizeof(char));
    char* filenameOperations = malloc(sizeof(char));
    char* filenameTempsCycle = malloc(sizeof(char));

    char txt[] = ".txt";  // Extension des fichiers

    FILE* fichierExclusions;
    FILE* fichierPrecedences;
    FILE* fichierOperations;
    FILE* fichierTempsCycle;
    do {
        printf("Entrez le nom du fichier d'exclusions :");
        scanf("%s", filenameExclusions);

        // Ajouter l'extension ".txt"
        strcat(filenameExclusions, txt);

        fichierExclusions = fopen(filenameExclusions, "r");
    } while (fichierExclusions == NULL);

    do {
        printf("\nEntrez le nom du fichier de precedences :");
        scanf("%s", filenamePrecedences);

        // Ajouter l'extension ".txt"
        strcat(filenamePrecedences, txt);

        fichierPrecedences = fopen(filenamePrecedences, "r");
    } while (fichierPrecedences == NULL);

    do {
        printf("\nEntrez le nom du fichier d'operation :");
        scanf("%s", filenameOperations);

        // Ajouter l'extension ".txt"
        strcat(filenameOperations, txt);

        fichierOperations = fopen(filenameOperations, "r");
    } while (fichierOperations == NULL);

    do {
        printf("\nEntrez le nom du fichier de temps de cycle :");
        scanf("%s", filenameTempsCycle);

        // Ajouter l'extension ".txt"
        strcat(filenameTempsCycle, txt);

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
    // Libérer la mémoire à la fin du programme
    free(filenameExclusions);
    free(filenamePrecedences);
    free(filenameOperations);
    free(filenameTempsCycle);
}
