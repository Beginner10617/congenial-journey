// Brainfuck interpreter in C
// Checkout https://en.wikipedia.org/wiki/Brainfuck for more information
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Extension for Brainfuck files
#define BF_EXTENSION "bf"

// Commands
#define MOVE_FORWARD '>'
#define MOVE_BACKWARD '<'
#define INCREMENT_DATA '+'
#define DECREMENT_DATA '-'
#define OUTPUT_DATA '.'
#define INPUT_DATA ','
#define LOOP_START '['
#define LOOP_END ']'

// Stack block structure
struct StackBlock
{
    struct StackBlock *next;
    struct StackBlock *prev;
    char data;
};

// Functions
int checkBrackets(FILE *file) {
    int loopCounter = 0;
    char command;
    while ((command = fgetc(file)) != EOF) {
        if (command == LOOP_START) loopCounter++;
        if (command == LOOP_END) loopCounter--;
        if (loopCounter < 0) return 0; // Unmatched closing bracket
    }
    return loopCounter == 0;
}

int validate_file_extension(const char *filename, const char *extension) {
    // Find the last occurrence of '.' in the file name
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
        return 0; // No extension or the '.' is the first character (invalid)
    }

    // Compare the file extension with the expected extension
    return strcmp(dot + 1, extension) == 0;
}
void freeBlocks(struct StackBlock *block) {
    // Free all allocated memory
    struct StackBlock *currentBlock = block;
    while (currentBlock != NULL) {
        struct StackBlock *nextBlock = currentBlock->next;
        free(currentBlock);
        currentBlock = nextBlock;
    }
}

struct StackBlock *moveForward(struct StackBlock *blockPointer)
{
    // Move the block pointer forward and allocate memory if needed
    if (blockPointer->next == NULL)
    {
        struct StackBlock *newBlock = malloc(sizeof(struct StackBlock));
        if (!newBlock) {
            perror("Error: Memory allocation failed");
            exit(1);
        }
        newBlock->prev = blockPointer;
        newBlock->next = NULL;
        blockPointer->next = newBlock;
        newBlock->data = 0;
    }
    return blockPointer->next;
}

struct StackBlock *moveBackward(struct StackBlock *blockPointer)
{
    // Move the block pointer backward and allocate memory if needed
    if (blockPointer->prev == NULL)
    {
        struct StackBlock *newBlock = malloc(sizeof(struct StackBlock));
        if (!newBlock) {
            perror("Error: Memory allocation failed");
            exit(1);
        }
        newBlock->next = blockPointer;
        newBlock->prev = NULL;
        blockPointer->prev = newBlock;
        newBlock->data = 0;
    }
    return blockPointer->prev;
}

void incrementData(struct StackBlock *blockPointer)
{
    blockPointer->data++;
}

void decrementData(struct StackBlock *blockPointer)
{
    blockPointer->data--;
}

void outputData(struct StackBlock *blockPointer)
{
    printf("%c", blockPointer->data);
}

void inputData(struct StackBlock *blockPointer)
{
    blockPointer->data = getchar();
}

void parseInstructions(FILE *file, struct StackBlock *instructionHead) {
    // Parse the instructions and store them in a linked list
    struct StackBlock *currentBlock = instructionHead;
    char command;
    while ((command = fgetc(file)) != EOF) {
        if (command == MOVE_FORWARD || command == MOVE_BACKWARD || command == INCREMENT_DATA || command == DECREMENT_DATA || command == OUTPUT_DATA || command == INPUT_DATA || command == LOOP_START || command == LOOP_END) {
            currentBlock->data = command;
            currentBlock = moveForward(currentBlock);
        }
    }
}

void executeProgram(struct StackBlock *InstructionPointer, struct StackBlock *DataPointer)
{
    // Execute the program
    while (InstructionPointer->data != 0)
    {
        switch (InstructionPointer->data)
        {
        case MOVE_FORWARD:
            DataPointer = moveForward(DataPointer);
            break;
        case MOVE_BACKWARD:
            DataPointer = moveBackward(DataPointer);
            break;
        case INCREMENT_DATA:
            incrementData(DataPointer);
            break;
        case DECREMENT_DATA:
            decrementData(DataPointer);
            break;
        case OUTPUT_DATA:
            outputData(DataPointer);
            break;
        case INPUT_DATA:
            inputData(DataPointer);
            break;
        case LOOP_START:
            if (DataPointer->data == 0)
            {
                int loopCounter = 1;
                while (loopCounter != 0)
                {
                    InstructionPointer = moveForward(InstructionPointer);
                    if (InstructionPointer->data == LOOP_START)
                    {
                        loopCounter++;
                    }
                    else if (InstructionPointer->data == LOOP_END)
                    {
                        loopCounter--;
                    }
                }
            }
            break;
        case LOOP_END:
            if (DataPointer->data != 0)
            {
                int loopCounter = 1;
                while (loopCounter != 0)
                {
                    InstructionPointer = moveBackward(InstructionPointer);
                    if (InstructionPointer->data == LOOP_START)
                    {
                        loopCounter--;
                    }
                    else if (InstructionPointer->data == LOOP_END)
                    {
                        loopCounter++;
                    }
                }
            }
            break;
        default:
            break;
        }
        InstructionPointer = moveForward(InstructionPointer);
    }
}

// Main function
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
     // Validate the file extension (e.g., "fl")
    if (!validate_file_extension(argv[1], BF_EXTENSION)) {
        printf("Invalid file extension. Please provide a '%s' file.\n", BF_EXTENSION);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("Error opening file\n");
        fclose(file);
        return 1;
    }

    if (!checkBrackets(file)) {
        printf("Error: Unmatched brackets\n");
        fclose(file);
        return 1;
    }
    rewind(file);

    struct StackBlock *instructionHead = malloc(sizeof(struct StackBlock));
    struct StackBlock *dataHead = malloc(sizeof(struct StackBlock));
    if (!instructionHead || !dataHead) {
        printf("Memory allocation failed\n");
        fclose(file);
        return 1;
    }
    instructionHead->next = instructionHead->prev = NULL;
    dataHead->next = dataHead->prev = NULL;
    dataHead->data = 0;

    // Parse and store instructions in a linked list
    parseInstructions(file, instructionHead);
    fclose(file);

    // Execute the program
    executeProgram(instructionHead, dataHead);

    // Free allocated memory
    freeBlocks(instructionHead);
    freeBlocks(dataHead);

    return 0;
}
// End of brainfuck.c