#include "defs.h"

//Output parameters: list - an initialized list
void initEvidenceList(EvidenceListType *list) {
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

//Output parameters: list - after the evidence is added
//Input parameters: evidence - the evidence to add to the list
void addEvidence(EvidenceListType *list, int evidence){
    EvidenceNodeType *newNode = (EvidenceNodeType*)malloc(sizeof(EvidenceNodeType));
    newNode->data = evidence;
    newNode->next = NULL;
    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
    list->count++;
}

//Input-Output parameters: list - after the evidence is removed
//Input parameters: evidence - the evidence to remove from the list
void removeEvidence(EvidenceListType *list, int evidence){
    EvidenceNodeType *current = list->head;
    EvidenceNodeType *previous = NULL;
    while (current != NULL) {
        if (current->data == evidence) {
            if (previous == NULL) {
                list->head = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            list->count--;
            return;
        }
        previous = current;
        current = current->next;
    }
}


//Input parameters: list - the list to check
//                  evidence - the evidence to check for
//Output parameters: C_TRUE if the evidence exists in the list, C_FALSE otherwise
int evidenceExists(EvidenceListType *list, int evidence){
    EvidenceNodeType *current = list->head;
    while (current != NULL) {
        if (current->data == evidence) {
            return C_TRUE;
        }
        current = current->next;
    }
    return C_FALSE;
}

void printEvidenceList(EvidenceListType *list){
    EvidenceNodeType *current = list->head;
    for(int i = 0; i < list->count; i++){
        char ev[MAX_STR];
        evidenceToString(current->data, ev);
        printf("%s\n", ev);
        current = current->next;
    }
}

//Input-Output parameters: list - the list to clean up
void cleanUpEvidenceList(EvidenceListType *list) {
    EvidenceNodeType *current = list->head;
    EvidenceNodeType *previous = NULL;
    while (current != NULL) {
        previous = current;
        current = current->next;
        if(previous != NULL){
            free(previous);
        }
    }
}