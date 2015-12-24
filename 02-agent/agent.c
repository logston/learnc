#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_FILE_LINE_LENGTH  128
#define MAX_CITY_NAME_LENGTH        64
#define INITIAL_HASHMAP_SIZE        256
#define EMPTY_HASHMAP_VALUE         -1

/* Structure declarations */
typedef struct Edge {
    char city1[MAX_CITY_NAME_LENGTH];
    char city2[MAX_CITY_NAME_LENGTH];
    int cost;
} Edge;

typedef struct ValueNode {
    char city[MAX_CITY_NAME_LENGTH];
    int cost;
    struct ValueNode* next;
} ValueNode;

typedef struct KeyNode {
    char city[MAX_CITY_NAME_LENGTH];
    ValueNode* nodes;
} KeyNode;

/* Forward declarations */
static void print_usage();
static void read_input_file(char *filename, KeyNode** map);
static Edge* parse_line(char *line);
static KeyNode** create_hashmap();
static KeyNode* create_keynode(char *city);
static ValueNode* create_valuenode(char *city, int cost);
static bool insert_hashmap_edge(KeyNode **map, char *city1, char *city2, int cost);
static void print_map(KeyNode **map);
unsigned long hash(unsigned char *str);

/* Global variables */
static int HASHMAP_SIZE = 0;

/*
 * MAIN
 */
int main(int argc, char *argv[]) {
    if (argc != 5) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    KeyNode** map = create_hashmap();

    read_input_file(argv[1], map);

    print_map(map);

    exit(EXIT_SUCCESS);
    return 0;
}

/*
 * Print usage of agent program
 */
static void
print_usage()
{
    fprintf(stderr, "Usage: ./agent input.dat [fare|route] DEPARTURE ARRIVAL\n");
}

/*
 * Read input.dat file into memory
 */
static void
read_input_file(char *filename, KeyNode **map)
{
    // Declare variables
    FILE *fp;
    char buffer[MAX_INPUT_FILE_LINE_LENGTH];
    Edge *edge;

    // Ensure file exists
    if (access(filename, F_OK) == -1) {
        fprintf(stderr, "Unable to locate file named \"%s\"\n", filename);
        exit(EXIT_FAILURE);
    }

    // Open file
    if (!(fp = fopen(filename, "r"))) {
        fprintf(stderr, "Unable to open file name \"%s\"\n", filename);
        exit(EXIT_FAILURE);
    }

    while(fgets(buffer, MAX_INPUT_FILE_LINE_LENGTH, fp)) {
        edge = parse_line(buffer);
        if (!insert_hashmap_edge(map, edge->city1, edge->city2, edge->cost)) {
            fprintf(stderr, "Unable to insert edge to map %s -> %s ($%i)\n",
                    edge->city1, edge->city2, edge->cost);
            exit(EXIT_FAILURE); 
        }
        if (!insert_hashmap_edge(map, edge->city2, edge->city1, edge->cost)) {
            fprintf(stderr, "Unable to insert edge to map %s -> %s ($%i)\n",
                    edge->city2, edge->city1, edge->cost);
            exit(EXIT_FAILURE); 
        }
    }
}

/*
 * Parse line for CITY CITY COST values.
 * Return an Edge
 */
static Edge*
parse_line(char *line)
{
    char *city_str1 = NULL;
    char *city_str2 = NULL;
    char *cost_str = NULL;
    int cost = 0;
    Edge *edge = malloc(sizeof(Edge));

    city_str1 = strtok(line, "\t");
    if (!city_str1) {
       fprintf(stderr, "Improperly formatted input file\n");
       exit(EXIT_FAILURE);
    }

    city_str2 = strtok(NULL, "\t");
    if (!city_str2) {
       fprintf(stderr, "Improperly formatted input file\n");
       exit(EXIT_FAILURE);
    }

    cost_str = strtok(NULL, "\n");
    if (!cost_str) {
       fprintf(stderr, "Improperly formatted input file\n");
       exit(EXIT_FAILURE);
    }

    cost = atoi(cost_str);
    if (!cost) {
       fprintf(stderr, "Invalid cost for cities %s and %s\n", city_str1, city_str2);
       exit(EXIT_FAILURE);
    }

    // compare city names so we store names alphabetically in Edge struct
    if (strcmp(city_str1, city_str2) < 0) {
        strncpy(edge->city1, city_str1, MAX_CITY_NAME_LENGTH);
        strncpy(edge->city2, city_str2, MAX_CITY_NAME_LENGTH);
    } else {
        strncpy(edge->city1, city_str2, MAX_CITY_NAME_LENGTH);
        strncpy(edge->city2, city_str1, MAX_CITY_NAME_LENGTH);
    }

    edge->cost = cost;

    return edge;
}

unsigned long
hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

/*
 * Create and return pointer to new hashmap
 */
static KeyNode**
create_hashmap()
{
    // initialize map with a size of 8 pointers. Hat tip to cpython's dict
    KeyNode** map = (KeyNode**)malloc(INITIAL_HASHMAP_SIZE * sizeof(KeyNode*));
    if (!map) {
       fprintf(stderr, "Unable to create hashmap\n");
       exit(EXIT_FAILURE);
    }
    for (int i = 0; i < INITIAL_HASHMAP_SIZE; i++) {
        map[i] = (KeyNode *)-1;
    }
    HASHMAP_SIZE = INITIAL_HASHMAP_SIZE;
    return map;
}

static KeyNode*
create_keynode(char *city)
{
    KeyNode* keynode_ptr = (KeyNode*)malloc(sizeof(KeyNode));
    if (!keynode_ptr) {
       fprintf(stderr, "Unable to create KeyNode\n");
       exit(EXIT_FAILURE);
    }
    strcpy(keynode_ptr->city, city);
    keynode_ptr->nodes = NULL;
    return keynode_ptr;
}

static ValueNode*
create_valuenode(char *city, int cost)
{
    ValueNode* node_ptr = (ValueNode*)malloc(sizeof(ValueNode));
    if (!node_ptr) {
       fprintf(stderr, "Unable to create ValueNode\n");
       exit(EXIT_FAILURE);
    }
    strcpy(node_ptr->city, city);
    node_ptr->cost = cost;
    node_ptr->next = NULL;
    return node_ptr;
}

// delete a hashmap
// DON'T NEED TO IMPLEMENT THIS IF PROGRAM ENDS RIGHT AFTER USE OF MAP

// resize a hashmap
// DON'T NEED TO IMPLEMENT THIS IF INITIAL_HASHMAP_SIZE IS BIG ENOUGH


// insert into a hashmap
static bool
insert_hashmap_edge(KeyNode **map, char *city1, char *city2, int cost)
{
    KeyNode *key_ptr;
    ValueNode *valuenode_ptr;
    bool added_node = false;
    long i = 0;

    unsigned long hash_value = hash((unsigned char*)city1);
    // check for edge->city1 in hashmap starting at hash % HASHMAP_SIZE
    for (i = 0; i < HASHMAP_SIZE; i++) {
        key_ptr = map[(hash_value + i) % HASHMAP_SIZE];
        if ((int)key_ptr == -1) {
            // city has no key in hash map and space is available.
            // create keynode for city
            key_ptr = create_keynode(city1);
            // add value node for new city
            key_ptr->nodes = create_valuenode(city2, cost);
            map[(hash_value + i) % HASHMAP_SIZE] = key_ptr;
            added_node = true;
            break;
        } else if (!strcmp(key_ptr->city, city1)) {
            // city has key in hash map.
            // add value node for city IF one for corresponding value does not
            // exist with the same cost and name
            valuenode_ptr = key_ptr->nodes;
            // move down linked list
            while (strcmp(valuenode_ptr->city, city2) && valuenode_ptr->next) {
                valuenode_ptr = valuenode_ptr->next;
            }
            if (valuenode_ptr->next) {
                // there is more nodes in linked list,
                // thus city2 must be in linked list
                if (cost < valuenode_ptr->cost) {
                    // only store lowest cost between two cities
                    valuenode_ptr->cost = cost;
                }
            } else {
                // add new value node with city2 and cost
                valuenode_ptr->next = create_valuenode(city2, cost);
            }
            added_node = true;
            break;
        }
    }
    return added_node;
}

// remove from hashmap
// DON'T NEED TO IMPLEMENT THIS SINCE NOTHING IS REMOVED FROM MAP

static void
print_map(KeyNode **map)
{
    ValueNode *valuenode_ptr;

    for (int i = 0; i < HASHMAP_SIZE; i++) {
        if ((int)map[i] == -1) {
            continue;
        }
        // print key
        printf("%s\n", map[i]->city);

        // print value nodes
        valuenode_ptr = map[i]->nodes;
        while (valuenode_ptr) {
            printf("\t%s ($%i)\n", valuenode_ptr->city, valuenode_ptr->cost);
            valuenode_ptr = valuenode_ptr->next;
        }
    }
}

