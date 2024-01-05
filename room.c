#include "defs.h"

//Output parameters: room - an initialized room
//Input parameters: name - the name of the room
void initRoom(char* name, RoomType **room) {
    *room =(RoomType*) malloc(sizeof(RoomType));
    strcpy((*room)->name, name);
    (*room)->ghost = NULL;
    initEvidenceList(&(*room)->evidenceList);
    initRoomList(&(*room)->neighbors);
    initHunterArray(&(*room)->hunters);
    sem_init(&(*room)->mutex, 0, 1);
}

//Output parameters: room - a pointer to the created room
//Input parameters: name - the name of the room
RoomType* createRoom(char* name) {
    RoomType *room;
    initRoom(name, &room);
    return room;
}

//Output parameters: roomList - an initialized room list
void initRoomList(RoomListType *roomList) {
    roomList->head = NULL;
    roomList->tail = NULL;
    roomList->count = 0;
}

//Output parameters: roomList - after the room is added
//Input parameters: room - the room to add to the list
void addRoom(RoomListType *roomList, RoomType *room) {
    RoomNodeType *newNode = (RoomNodeType*)malloc(sizeof(RoomNodeType));
    newNode->data = room;
    newNode->next = NULL;
    if (roomList->head == NULL) {
        roomList->head = newNode;
        roomList->tail = newNode;
    } else {
        roomList->tail->next = newNode;
        roomList->tail = newNode;
    }
    roomList->count++;
}
   
//Input-Output parameters: room1, room2 - after the rooms are connected
void connectRooms(RoomType *room1, RoomType *room2) {
    addRoom(&room1->neighbors, room2);
    addRoom(&room2->neighbors, room1);
}

//Input-Ouput parameters: roomList - returns the room list after the room data is cleaned up
void cleanUpRoomData(RoomListType *roomlist){
    RoomNodeType *current = roomlist->head;
    RoomNodeType *previous = NULL;
    while (current != NULL) {
        previous = current;
        current = current->next;
        if (previous->data != NULL)
        {
            cleanUpRoomList(&previous->data->neighbors);
            cleanUpEvidenceList(&previous->data->evidenceList);
            free(previous->data);
        }
    }
}

//Input-Ouput parameters: list - returns the room list after the room list is cleaned upf
void cleanUpRoomList(RoomListType *list) {
    RoomNodeType *current = list->head;
    RoomNodeType *previous = NULL;
    while (current != NULL) {
        previous = current;
        current = current->next;
        if (previous != NULL)
        {
            free(previous);
        }
    }
}

