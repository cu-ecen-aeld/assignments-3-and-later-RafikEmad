#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#define MAX_LINE_LENGTH 1024

// Function to log messages using syslog
void log_message(int priority, const char *message) {
    syslog(priority, "%s", message);
}

int main(int argc, char *argv[]) {
    // Check if the required arguments are passed
    if (argc != 3) {
        fprintf(stderr, "Error: Both file path and text string must be provided.\n");
        log_message(LOG_ERR, "Error: Missing arguments (file path and text string).");
        exit(1);
    }

    // Get file path and text string from arguments
    const char *writefile = argv[1];
    const char *writestr = argv[2];

    // Open the file for writing (create/overwrite the file)
    FILE *file = fopen(writefile, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Failed to create or open the file '%s'.\n", writefile);
        log_message(LOG_ERR, "Error: Failed to open file for writing.");
        exit(1);
    }

    // Write the string to the file
    if (fprintf(file, "%s", writestr) < 0) {
        fprintf(stderr, "Error: Failed to write to the file '%s'.\n", writefile);
        log_message(LOG_ERR, "Error: Failed to write to file.");
        fclose(file);
        exit(1);
    }

    // Log the success message using syslog
    char log_msg[MAX_LINE_LENGTH];
    snprintf(log_msg, sizeof(log_msg), "Writing \"%s\" to %s", writestr, writefile);
    log_message(LOG_DEBUG, log_msg);

    // Close the file
    fclose(file);

    // Print success message
    printf("File '%s' created with content: %s\n", writefile, writestr);
    
    // Close syslog connection
    closelog();

    return 0;
}
