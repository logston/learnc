#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_FILE_LINE_LENGTH  128
#define MAX_CITY_NAME_LENGTH        64

/* Structure delcarations */
typedef struct Edge {
    char city1[MAX_CITY_NAME_LENGTH];
    char city2[MAX_CITY_NAME_LENGTH];
    int cost;
} Edge;

/* Forward declarations */
static void print_usage();
static void read_input_file(char *filename);
static Edge* parse_line(char *line);

/*
 * MAIN
 */
int main(int argc, char *argv[]) {
    if (argc != 5) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    read_input_file(argv[1]);

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
read_input_file(char *filename)
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
        printf("C1: %s, C2: %s, $%d\n", edge->city1, edge->city2, edge->cost);
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

