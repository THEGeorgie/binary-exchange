#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.csv>\n", argv[0]);
        return 1;
    }

    const char *inputFilename = argv[1];
    FILE *inputFile = fopen(inputFilename, "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return 1;
    }

    char line[LINE_SIZE];
    float sum = 0.0f;
    int count = 0;

    while (fgets(line, sizeof(line), inputFile)) {
        // Tokenize each line by comma
        char *token = strtok(line, ",");
        while (token != NULL) {
            float value = atof(token);  // Convert token to float
            sum += value;
            count++;
            token = strtok(NULL, ",");
        }
    }

    fclose(inputFile);

    if (count == 0) {
        fprintf(stderr, "No numbers found in input file\n");
        return 1;
    }

    float average = sum / count;

    FILE *outputFile = fopen("output.csv", "w");
    if (outputFile == NULL) {
        perror("Error creating output file");
        return 1;
    }

    fprintf(outputFile, "average\n%.3f\n", average);
    fclose(outputFile);

    printf("Average: %.3f written to output.csv\n", average);
    return 0;
}
