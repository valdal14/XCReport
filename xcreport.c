#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char* command;
    char* command_description;
} ConsoleCommand;

int execute(char *command, char *command_description) {
    int result = system(command);
    
    if (result == 0) {
        printf("%s executed successfully.\n", command_description);
        return 0;
    } else {
        printf("Failed to execute %s. Error code: %d\n", command_description, result);
        return -1;
    }
}

int main(void) {
    ConsoleCommand createXML;
    createXML.command = "bash xml-converter.sh ci-test-results.xcresult/ > coverage.xml";
    createXML.command_description = "xml-converter";

    int res = execute(createXML.command, createXML.command_description);
    printf("%d\n", res);
    return 0;
}