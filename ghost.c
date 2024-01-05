#include "defs.h"

//Output parameters: ghost - an initialized ghost
void initGhost(GhostType **ghost, HouseType *house){
    *ghost = (GhostType*)malloc(sizeof(GhostType));
    (*ghost)->type = randomGhost();
    initEvidenceList(&(*ghost)->evidenceList);
    (*ghost)->location = NULL;
    (*ghost)->boredom = 0;
    (*ghost)->house = house;
    initHunterArray(&(*ghost)->hunters);
}

//Output parameters: ghost - after the evidence is added
void ghostEvidence(GhostType *ghost){
    if(ghost->type == POLTERGEIST){
        addEvidence(&ghost->evidenceList, EMF);
        addEvidence(&ghost->evidenceList, TEMPERATURE);
        addEvidence(&ghost->evidenceList, FINGERPRINTS);
    }
    else if (ghost->type == BANSHEE)
    {
        addEvidence(&ghost->evidenceList, EMF);
        addEvidence(&ghost->evidenceList, TEMPERATURE);
        addEvidence(&ghost->evidenceList, SOUND);
    }
    else if (ghost->type == BULLIES)
    {
        addEvidence(&ghost->evidenceList, EMF);
        addEvidence(&ghost->evidenceList, FINGERPRINTS);
        addEvidence(&ghost->evidenceList,SOUND);
    }
    else if (ghost->type == PHANTOM)
    {
        addEvidence(&ghost->evidenceList, TEMPERATURE);
        addEvidence(&ghost->evidenceList, FINGERPRINTS);
        addEvidence(&ghost->evidenceList,SOUND);
    }
}


//Output parameters: ghost - after the room is selected
//Input parameters: house - the house to select the room from
void randomGhostRoom(HouseType *house, GhostType *ghost){
    int random = randInt(1, house->rooms.count);
    RoomNodeType *current = house->rooms.head;
    for(int i = 0; i < random; i++){
        current = current->next;
    }
    current->data->ghost = ghost;
    ghost->location = current->data;
}


//Output parameters: ghost - after the ghost is created
//Input parameters: house - the house to create the ghost in
void createGhost(GhostType **ghost, HouseType *house){
    initGhost(ghost, house);
    ghostEvidence(*ghost);
    randomGhostRoom(house, *ghost);
}

//Input-Ouput parameters: ghost - returns the ghost after the ghost data is cleaned up
void cleanUpGhost(GhostType *ghost){
    cleanUpEvidenceList(&ghost->evidenceList);
    free(ghost);
}


void checkIfHunterHasExit(GhostType *ghost, int *count){
    (*count) = 0;
    for(int i=0; i < NUM_HUNTERS; i++){
        if(ghost->house->hunters.hunters[i]->boredom == BOREDOM_MAX || ghost->house->hunters.hunters[i]->fear == FEAR_MAX){
            (*count)++;
        }
    }
}

void determineGhostType(EvidenceListType list){
    if(evidenceExists(&list, EMF)==C_TRUE
     && evidenceExists(&list, TEMPERATURE) == C_TRUE
      && evidenceExists(&list, FINGERPRINTS) == C_TRUE){
        printf("Ghost type is Poltergeist\n");
    }
    else if (evidenceExists(&list, EMF)==C_TRUE
     && evidenceExists(&list, TEMPERATURE) == C_TRUE
      && evidenceExists(&list, SOUND) == C_TRUE)
    {
        printf("Ghost type is Banshee\n");
    }
    else if (evidenceExists(&list, EMF)==C_TRUE
     && evidenceExists(&list, FINGERPRINTS) == C_TRUE
      && evidenceExists(&list, SOUND) == C_TRUE)
    {
        printf("Ghost type is Bullies\n");
    }
    else if (evidenceExists(&list, TEMPERATURE)==C_TRUE
     && evidenceExists(&list, FINGERPRINTS) == C_TRUE
      && evidenceExists(&list, SOUND) == C_TRUE)
    {
        printf("Ghost type is Phantom\n");
    }
}

//Output parameters: ghost - the ghost to run the thread
void *ghostThreadFunc(void *arg){
    GhostType *ghost = (GhostType*)arg; 
    int count = 0;
    while (1)
    {
        if (ghost->location->hunters.count > 0)
        {
            ghost->boredom = 0;

            if(ghost->location->hunters.hunters[0]->sharedEvidence->count == 3){
                l_ghostExit(LOG_EVIDENCE);
                break;
            }
        }
        else 
        {
            ghost->boredom++;
        }
        int random = randInt(0,3);
        if (random == 1 && ghost->location->hunters.count == 0) {
            RoomType *temp = ghost->location;
            int random = randInt(0, ghost->location->neighbors.count-1);
            RoomNodeType *current = ghost->location->neighbors.head;
            for(int i = 0; i < random; i++){
                current = current->next;
            }
            sem_wait(&temp->mutex);
            sem_wait(&current->data->mutex);
            ghost->location->ghost = NULL;
            ghost->location = current->data;
            l_ghostMove(ghost->location->name);
            ghost->location->ghost = ghost;
            sem_post(&current->data->mutex);
            sem_post(&temp->mutex);
        }
        if(random == 2)
        {
            int random = randInt(0, EV_COUNT -1);
            EvidenceNodeType *current = ghost->evidenceList.head;
            for(int i = 0; i < random; i++){
                current = current->next;
            }
            sem_wait(&ghost->location->mutex);
            addEvidence(&ghost->location->evidenceList, current->data);
            sem_post(&ghost->location->mutex);
            l_ghostEvidence(current->data, ghost->location->name);
        }
        checkIfHunterHasExit(ghost, &count);
        if (count == NUM_HUNTERS)
        {
            printf("Ghost won\n");
            break;
        }
        if(ghost->boredom == BOREDOM_MAX){
            l_ghostExit(LOG_BORED);
            break;
        }
        usleep(GHOST_WAIT);
    } 
    return NULL;
}