#include <stdio.h>
#include <stdlib.h>

char *CMD_CREATE_XML = "bash xml-converter.sh ci-test-results.xcresult/ > ./coverage-report/coverage.xml";
char *CMD_STRIP_ABSOLUTE_PATH = "sed -i '' 's|<file path=\".*/ios-mobile-app/\\(.*\\)\"|<file path=\"\\1\"|g' ./coverage-report/coverage.xml";
char *CMD_XCODE_BUILD = "xcodebuild test -scheme CI-iOS-PROD -project canda.xcodeproj -destination 'platform=iOS Simulator,name=iPhone SE (3rd generation)' -enableCodeCoverage YES -resultBundlePath ci-test-results.xcresult";

typedef struct {
    char* command;
    char* command_description;
} ConsoleCommand;

// execute commmand
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

// run xcode tests and make .xcresult file
int run_tests() {
    ConsoleCommand execute_tests;
    execute_tests.command = CMD_XCODE_BUILD;
    execute_tests.command_description = "running xcode tests";
    printf("Executing xcodebuild test, please wait...\n");
    int xcode_result = execute(execute_tests.command, execute_tests.command_description);
    return xcode_result;
}

// make coverage-report using makedir.sh
int make_dir() {
    ConsoleCommand make_dir;
    make_dir.command = "bash makedir.sh";
    make_dir.command_description = "make coverage-report dir if does not exist!";
    int make_dir_result = execute(make_dir.command, make_dir.command_description);
    return make_dir_result;
}

// convert xcode .xcresult to .xml
int convert_to_xml() {
    ConsoleCommand create_XML;
    create_XML.command = CMD_CREATE_XML;
    create_XML.command_description = "xml-converter";
    int convert_xml_result = execute(create_XML.command, create_XML.command_description);
    return convert_xml_result;
}

 // stript the absolute path
int strip_absolute_path() {
    ConsoleCommand make_relative_path;
    make_relative_path.command = CMD_STRIP_ABSOLUTE_PATH;
    make_relative_path.command_description = "create relative path";
    int strip_command_result = execute(make_relative_path.command, make_relative_path.command_description);
    return strip_command_result;
}

int main(void) {
    // run tests and create a .xcresult file
    int tests_result = run_tests();

    if (tests_result == 0) {
        // make a dir if it does not exist
        int make_dir_result = make_dir();

        if (make_dir_result != 0) {
            return -1;
        }

        // convert xcode .xcresult to .xml
        int convert_to_xml_result = convert_to_xml();
        if (convert_to_xml_result == 0) {
            // stript the absolute path
            int strip_absolute_path_result = strip_absolute_path();
            if (strip_absolute_path_result == 0) {
                return 0;
            } else {
                return -1;
            }

        } else {
            return -1;
        }

    } else {
        return -1;
    }
}