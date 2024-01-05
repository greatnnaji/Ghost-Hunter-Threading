#include "defs.h"

int main()
{
    // Initialize the random number generator
    srand(time(NULL));
    // Create the house: You may change this, but it's here for demonstration purposes
    // Note: This code will not compile until you have implemented the house functions and structures
    HouseType house;
    initHouse(&house);
    //hunters names
    char    H1[MAX_STR]= "H1";
    char    H2[MAX_STR]= "H2";
    char    H3[MAX_STR]= "H3";
    char    H4[MAX_STR]= "H4";
    char    *names[NUM_HUNTERS] = {H1,H2,H3,H4};
    //pthread array of hunters
    pthread_t   hunterThreads[NUM_HUNTERS];
    //thread for ghost
    pthread_t   ghostThread;
    //array of hunters
    HunterType  *hunters[NUM_HUNTERS];
    //ghost
    GhostType   *ghost;

    //Get the names of the hunters from the user
    printf("Enter name for hunter 1: ");
    fgets(H1, MAX_STR, stdin);
    H1[strlen(H1)-1] = 0;
    printf("Enter name for hunter 2: ");
    fgets(H2, MAX_STR, stdin);
    H2[strlen(H2)-1] = 0;
    printf("Enter name for hunter 3: ");
    fgets(H3, MAX_STR, stdin);
    H3[strlen(H3)-1] = 0;
    printf("Enter name for hunter 4: ");
    fgets(H4, MAX_STR, stdin);
    H4[strlen(H4)-1] = 0;

    //populate the house
    populateRooms(&house);

    // Create the ghost
    createGhost(&ghost, &house);
    l_ghostInit(ghost->type, ghost->location->name);

    // Create the hunters
    createHunter(hunters, names, &house);
    for (int i = 0; i < NUM_HUNTERS ; i++)
    {
        l_hunterInit(hunters[i]->name, hunters[i]->equipment);
    }

    // Create the hunter threads
    for(int i = 0; i < NUM_HUNTERS; i++){
        pthread_create(&hunterThreads[i], NULL, hunterThreadFunc, hunters[i]);
    }

    // Create the ghost thread
    pthread_create(&ghostThread, NULL, ghostThreadFunc, ghost);

    // Join the ghost thread
    pthread_join(ghostThread, NULL);
    // Join the hunter threads
    for(int i = 0; i < NUM_HUNTERS; i++){
        pthread_join(hunterThreads[i], NULL);
    }

    // Print the final state of the house
    printEvidenceList(&house.sharedEvidence);

    if (house.sharedEvidence.count == 3)
    {
        determineGhostType(house.sharedEvidence);
    }

    // Clean up the house
    cleanUpHouse(&house);
    // Clean up the ghost
    cleanUpGhost(ghost);

    return 0;
}

