#include "defs.h"

//Output parameters: hunter - an initialized hunter
void initHunter(char* name, HunterType **hunter, HouseType *house) {
    *hunter =(HunterType*) malloc(sizeof(HunterType));
    strcpy((*hunter)->name, name);
    (*hunter)->sharedEvidence = &house->sharedEvidence;
    (*hunter)->count = 0;
    (*hunter)->fear = 0;
    (*hunter)->boredom = 0;
    (*hunter)->location = NULL;
    sem_init(&(*hunter)->mutex,0,1);
}

//Output parameters: hunterArray - an initialized hunter array
void initHunterArray(HunterArrayType *hunterArray) {
    for (int i = 0; i < NUM_HUNTERS; i++)
    {
        hunterArray->hunters[i] = NULL;
    }
    hunterArray->count = 0;
}


//Output parameters: hunterArray - after the hunter is added
//Input parameters: hunter - the hunter to add to the array
void addHunter(HunterArrayType *hunterArray, HunterType *hunter) {
    hunterArray->hunters[hunterArray->count] = hunter;
    hunterArray->count++;
}

//Input-Output parameters: hunterArray - after the hunter is removed
//Input parameters: hunter - the hunter to remove from the array
void removeHunter(HunterArrayType *array, HunterType *hunter){
    int index = 0;
    for(int i = 0; i < array->count ; i++){
        if(array->hunters[i] == hunter){
            index = i;
            break;
        }
    }
    for(int i = index; i < array->count; i++){
        array->hunters[i] = array->hunters[i+1];
    }
    array->count--;
}


//Output parameters: hunter - hunter after the equipment is selected
//Input parameters: index - the index of the equipment to select
void selectEquipment(HunterType *hunter, int index) {
    switch (index) {
        case 0:
            hunter->equipment = EMF;
            break;
        case 1:
            hunter->equipment = TEMPERATURE;
            break;
        case 2:
            hunter->equipment = FINGERPRINTS;
            break;
        case 3:
            hunter->equipment = SOUND;
            break;
    }
}


//Output parameters: hunter - hunter after the starting room is selected
//Input parameters: room - the room to start in
void startingRoom(HunterType *hunter, RoomType *room){
    hunter->location = room;
    addHunter(&room->hunters, hunter);
}


//Output parameters: hunters - an array of initialized hunters
//Input parameters: names - an array of hunter names
//Input-Output parameters: house - after the hunters are added
void createHunter(HunterType **hunters, char **names, HouseType *house) {
    for(int i = 0; i < NUM_HUNTERS; i++)
    {
        initHunter(names[i], &hunters[i], house);
        selectEquipment(hunters[i], i);
        startingRoom(hunters[i], house->rooms.head->data);
        addHunter(&house->hunters, hunters[i]);
    }
}


//Input-Ouput parameters: array - returns the hunter array after the hunter data is cleaned up
void cleanUpHunterArray(HunterArrayType *array){
    for(int i = 0; i < array->count; i++){
        free(array->hunters[i]);
    }
}


void printAllHouseHunterArrays(HouseType *house){
    RoomNodeType *current = house->rooms.head;
    for(int i = 0; i < house->rooms.count; i++){
        for(int j = 0; j < current->data->hunters.count; j++){
            printf("Hunter: %s\n", current->data->hunters.hunters[j]->name);
        }
        current = current->next;
    }
}


//Input-Ouput parameters: arg - the hunter to run the thread for
void *hunterThreadFunc(void *arg) {
    HunterType *hunter = (HunterType*)arg;
    while (1)
    {
        if (hunter->location->ghost != NULL && hunter->location->ghost->boredom <BOREDOM_MAX)
        {
            hunter->fear++;
            hunter->boredom = 0;
        }
        else
        {
            hunter->boredom++;
        }
        int random = randInt(0,3);
        //collect evidence
        if (random == 0)
        {
            if (hunter->location->evidenceList.count > 0)
            {
                sem_wait(&hunter->mutex);
                int true = C_FALSE;
                true = evidenceExists(&hunter->location->evidenceList, hunter->equipment);
                if(true == C_TRUE){
                    removeEvidence(&hunter->location->evidenceList, hunter->equipment);
                    true = evidenceExists(hunter->sharedEvidence, hunter->equipment);
                    if(true == C_FALSE){
                        addEvidence(hunter->sharedEvidence, hunter->equipment);
                        l_hunterCollect(hunter->name, hunter->equipment, hunter->location->name);
                    }
                }
                sem_post(&hunter->mutex);
            }
        }
        //move to a random room
        if (random == 1)
        {
            RoomType *temp = hunter->location;
            int random = randInt(0, hunter->location->neighbors.count);
            RoomNodeType *current = hunter->location->neighbors.head;
            for(int i = 0; i < random; i++){
                current = current->next;
            }
            l_hunterMove(hunter->name, current->data->name);
            sem_wait(&temp->mutex);
            sem_wait(&current->data->mutex);
            removeHunter(&hunter->location->hunters, hunter);
            hunter->location = current->data;
            addHunter(&hunter->location->hunters, hunter);
            sem_post(&current->data->mutex);
            sem_post(&temp->mutex);
        }
        //review evidence
        if (random == 2)
        {
            if(hunter->sharedEvidence->count == 3){
                l_hunterReview(hunter->name, LOG_SUFFICIENT);
                l_hunterExit(hunter->name, LOG_EVIDENCE);
                break;
            }
            else{
                l_hunterReview(hunter->name, LOG_INSUFFICIENT);
            }
        }
        if (hunter->fear >= FEAR_MAX || hunter->boredom >= BOREDOM_MAX)
        {
            if (hunter->fear >= FEAR_MAX)
            {
                l_hunterExit(hunter->name, LOG_FEAR);
            }
            else
            {
                l_hunterExit(hunter->name, LOG_BORED);
            }

            
            break;
        }      
        usleep(HUNTER_WAIT);
    }
    return NULL;
}