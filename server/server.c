#include <stdio.h>
#include <stdlib.h>

/*
 * Serves the NodeMCU's requests
 */

void get_conditions();

int main(int argc, char *argv[])
{
    // No arguments
    if (argc < 2)
    {
        printf("No arguments.\n");
        exit(1);
    }

    // Switch commands
    switch(argv[1][0])
    {
        case 'G':
            get_conditions();
            break;
        default:
            fprintf(stderr, "ERR: I don't know that command.\n");
            break;
    }
}

void get_conditions()
{
    // Open file
    FILE *f = fopen("current_conditions", "r");
    if (!f)
    {
        fprintf(stderr, "Can't open current_conditions for reading.\n");
        exit(2);
    }

    // Read file line by line
    char line[1000];
    while (fgets(line, 1000, f) != NULL)
    {
        fprintf(stdout, "%s", line);
    }

    // Close
    fclose(f);
}
