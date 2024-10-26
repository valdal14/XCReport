#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#define GREEN   "\033[32m"
#define RESET   "\033[0m"
#define RED     "\033[31m" 

char *APP_VERSION = "1.0";
char *REPORT_DIR_NAME = "coverage-report";
char *XCODE_REPORT_FILENAME = "ci-test-results.xcresult";
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
        printf(GREEN "%s executed successfully.\n" RESET, command_description);
        return 0;
    } else {
        printf(RED "Failed to execute %s. Error code: %d\n" RESET, command_description, result);
        return -1;
    }
}

// run xcode tests and make .xcresult file
int run_tests() {
    ConsoleCommand execute_tests;
    execute_tests.command = CMD_XCODE_BUILD;
    execute_tests.command_description = "running xcode tests";
    printf(GREEN "Executing xcodebuild test, please wait...\n" RESET);
    printf(RESET "\n");
    int xcode_result = execute(execute_tests.command, execute_tests.command_description);
    return xcode_result;
}

int make_dir() {
    printf(GREEN "Creating %s directory.\n" RESET, REPORT_DIR_NAME);
    struct stat st = {0};
    if (stat(REPORT_DIR_NAME, &st) == -1) {
        // Directory does not exist, so create it
        if (mkdir(REPORT_DIR_NAME, 0755) == 0) {
            printf(GREEN "Directory %s successfully created.\n" RESET, REPORT_DIR_NAME);
            return 0;
        } else {
            printf(RED "Error creating directory" RESET);
            return -1;
        }
    } else {
        return 1;
    }
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

// delete previous .xcresult if exists
int delete_report() {
    printf(GREEN "Deleting previous %s report if exists...\n" RESET, XCODE_REPORT_FILENAME);

    char command[256];
    snprintf(command, sizeof(command), "rm -rf %s", XCODE_REPORT_FILENAME);
    // Run the command
    int result = system(command);

    if (result == 0) {
        printf(GREEN "Previous report %s was successfully deleted.\n" RESET, XCODE_REPORT_FILENAME);
        return 0;    
    } else {
       printf(RESET "Error deleting the report!" RESET);
       return -1;
    }
}

// Launch Screen
void print_lanuch_screen() {
    printf("\n");
    printf("██╗  ██╗ ██████╗ ██████╗ ███████╗██████╗  ██████╗ ██████╗ ████████╗\n");
    printf("██║ ██╔╝██╔═══██╗██╔══██╗██╔════╝██╔══██╗██╔═══██╗██╔══██╗╚══██╔══╝\n");
    printf("█████╔╝ ██║   ██║██████╔╝█████╗  ██████╔╝██║   ██║██████╔╝   ██║   \n");
    printf("██╔═██╗ ██║   ██║██╔═══╝ ██╔══╝  ██╔═══╝ ██║   ██║██╔═══╝    ██║   \n");
    printf("██║  ██╗╚██████╔╝██║     ███████╗██║     ╚██████╔╝██║        ██║   \n");
    printf("╚═╝  ╚═╝ ╚═════╝ ╚═╝     ╚══════╝╚═╝      ╚═════╝ ╚═╝        ╚═╝   \n");
    printf("                                                                   \n");
    printf("                   Welcome to XCReport - Version %s\n", APP_VERSION);
    printf("                   Developed by Valerio D'Alessio\n");
    printf("\n");
}

void print_completion_message() {
    printf("\n");
    printf(GREEN "========= Process Completed =========\n");
    printf("                ✔ Success! 🎉\n");
    printf("===========================================\n");
}

void print_error_message() {
    printf("\n");
    printf(RED "=========== Process Failed ============\n");
    printf("                ✖ Error! ❌\n");
    printf("===========================================\n");
}

int main(void) {
    print_lanuch_screen();

    // delete previous .xcresult if exists
    int delete_report_result = delete_report();

    if (delete_report_result != 0) {
        print_error_message();
        return -1;
    }

    // run tests and create a .xcresult file
    int tests_result = run_tests();

    if (tests_result == 0) {
        // make a dir if it does not exist
        int make_dir_result = make_dir();

        if (make_dir_result == 0 || make_dir_result == 1) {

            // convert xcode .xcresult to .xml
            int convert_to_xml_result = convert_to_xml();
            if (convert_to_xml_result == 0) {
                // stript the absolute path
                int strip_absolute_path_result = strip_absolute_path();
                if (strip_absolute_path_result == 0) {
                    print_completion_message();
                    return 0;
                } else {
                     print_error_message();
                    return -1;
                }

            } else {
                 print_error_message();
                return -1;
            }  

        } else {
             print_error_message();
            return -1;
        }

    } else {
         print_error_message();
        return -1;
    }
}