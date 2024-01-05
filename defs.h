#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_STR         64
#define MAX_RUNS        50
#define BOREDOM_MAX     100
#define C_TRUE          1
#define C_FALSE         0
#define HUNTER_WAIT     5000
#define GHOST_WAIT      600
#define NUM_HUNTERS     4
#define FEAR_MAX        10
#define LOGGING         C_TRUE

typedef enum EvidenceType EvidenceType;
typedef enum GhostClass GhostClass;

enum EvidenceType { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
enum GhostClass { POLTERGEIST, BANSHEE, BULLIES, PHANTOM, GHOST_COUNT, GH_UNKNOWN };
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };

typedef     struct Hunter       HunterType;
typedef     struct Ghost        GhostType;
typedef     struct Room         RoomType;
typedef     struct RoomList     RoomListType;
typedef     struct EvidenceList EvidenceListType;
typedef     struct House        HouseType;
typedef     struct HunterArray  HunterArrayType;
typedef     struct RoomNode     RoomNodeType;
typedef     struct EvidenceNode EvidenceNodeType;

struct EvidenceList{
    EvidenceNodeType *head;
    EvidenceNodeType *tail;
    int count;
};

struct EvidenceNode {
    int   data;
    EvidenceNodeType    *next;
};

struct RoomList{
    RoomNodeType *head;
    RoomNodeType *tail;
    int count;
};

struct RoomNode{
    RoomType *data;
    RoomNodeType *next;
};

struct Hunter{
    char name[MAX_STR];
    int equipment;
    int count;
    int boredom;
    int fear;
    RoomType *location;
    EvidenceListType *sharedEvidence;
    sem_t mutex;
};

struct HunterArray{
    HunterType *hunters[NUM_HUNTERS];
    int count;
};

struct Room{
    char name[MAX_STR];
    GhostType *ghost;
    EvidenceListType evidenceList;
    RoomListType neighbors;
    HunterArrayType hunters;
    sem_t mutex;
};

struct House{
    RoomListType rooms;
    EvidenceListType sharedEvidence;
    HunterArrayType hunters;
};

struct Ghost{
    enum GhostClass type;
    RoomType *location;
    EvidenceListType evidenceList;
    int boredom;
    HouseType *house;
    HunterArrayType hunters;
};





// Helper Utilies
int randInt(int,int);        // Pseudo-random number generator function
float randFloat(float, float);  // Pseudo-random float generator function
enum GhostClass randomGhost();  // Return a randomly selected a ghost type
void ghostToString(enum GhostClass ghost, char* buffer); // Convert a ghost type to a string, stored in output paremeter
void evidenceToString(enum EvidenceType type, char* str); // Convert an evidence type to a string, stored in output parameter
//room.c
void initRoom(char* name,RoomType **room); // Initialize a room
RoomType* createRoom(char* name); // Create a room  and return a pointer to it
void initRoomList(RoomListType *list); // Initialize a room list
void addRoom(RoomListType *list, RoomType *room); // Add a room to a room list
void connectRooms(RoomType *room1, RoomType *room2); // Connect two rooms together
void cleanUpRoomData(RoomListType *roomlist); // Free all memory in the data field of a room list
void cleanUpRoomList(RoomListType *list); // Free all memory associated with a room list
//hunter.c
void initHunter(char* name, HunterType **hunter, HouseType *House); // Initialize a hunter
void initHunterArray(HunterArrayType *array); // Initialize a hunter array
void addHunter(HunterArrayType *array, HunterType *hunter); // Add a hunter to a hunter array
void removeHunter(HunterArrayType *array, HunterType *hunter); // Remove a hunter from a hunter array
void startingRoom(HunterType *hunter, RoomType *room); // Set the starting room for a hunter
void selectEquipment(HunterType *hunter,int INDEX); // Randomly select equipment for a hunter
void createHunter(HunterType **hunters, char **names, HouseType *house); // Create a hunter
void *hunterThreadFunc(void *arg); // The hunter thread function
void printAllHouseHunterArrays(HouseType *house); // Print all hunter arrays in a house
void cleanUpHunterArray(HunterArrayType *array); // Free all memory associated with a hunter array
//ghost.c
void initGhost(GhostType **ghost, HouseType *house); // Initialize a ghost
void randomGhostRoom(HouseType *house, GhostType *ghost); // Randomly select a room for a ghost
void createGhost(GhostType **ghost, HouseType *house); // Create a ghost
void ghostEvidence(GhostType *ghost); // Add evidences to a the ghost's evidence list based on the ghost type
void *ghostThreadFunc(void *arg); // The ghost thread function
void cleanUpGhost(GhostType *ghost); // Free all memory associated with a ghost
void checkIfHunterHasExit(GhostType *ghost, int *count); // Check if the ghost should exit
void determineGhostType(EvidenceListType list); // Determine the ghost type based on the shared evidence list
//evidence.c
void initEvidenceList(EvidenceListType *list); // Initialize an evidence list
void addEvidence(EvidenceListType *list, int evidence); // Add evidence to an evidence list
void removeEvidence(EvidenceListType *list, int evidence); // Remove evidence from an evidence list
int evidenceExists(EvidenceListType *list, int evidence); // Check if evidence exists in an evidence list
void printEvidenceList(EvidenceListType *list); // Print an evidence list
void cleanUpEvidenceList(EvidenceListType *list); // Free all memory associated with an evidence list
//house.c
void initHouse(HouseType *house); // Initialize a house
void cleanUpHouse(HouseType *house); // Free all memory associated with a house
void populateRooms(HouseType* house); // Populate the house with rooms


// Logging Utilities
void l_hunterInit(char* name, enum EvidenceType equipment);
void l_hunterMove(char* name, char* room);
void l_hunterReview(char* name, enum LoggerDetails reviewResult);
void l_hunterCollect(char* name, enum EvidenceType evidence, char* room);
void l_hunterExit(char* name, enum LoggerDetails reason);
void l_ghostInit(enum GhostClass type, char* room);
void l_ghostMove(char* room);
void l_ghostEvidence(enum EvidenceType evidence, char* room);
void l_ghostExit(enum LoggerDetails reason);