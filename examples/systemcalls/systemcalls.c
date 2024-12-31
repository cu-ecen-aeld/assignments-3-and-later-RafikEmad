#include "systemcalls.h"
#include <unistd.h>    // for fork(), execv()
#include <sys/wait.h>  // for waitpid()
#include <stdlib.h>  // For system()

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

// Call system() to execute the command and check the return value
    int result = system(cmd);

    // If system() returns 0, the command executed successfully
    if (result == 0) {
        return true;
    }

    // Otherwise, the command failed
    return false;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    // command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/

    va_end(args);

    // Check if the command path is absolute
    if (command[0][0] != '/') {
        // If the command doesn't start with '/', return false
        return false;
    }


    pid_t pid = fork();  // Create a new child process

    if (pid == -1) {
        // If fork() fails
        return false;
    } else if (pid == 0) {
        // In child process
        // Execute the command with execv()
        if (execv(command[0], command) == -1) {
            // execv() failed
            return false;
        }
    } else {
        // In parent process
        int status;
        // Wait for the child process to complete
        if (waitpid(pid, &status, 0) == -1) {
            // waitpid() failed
            return false;
        }

        // Check the child process exit status
        if (WIFEXITED(status)) {
            // If child process exited normally, check the exit status
            return WEXITSTATUS(status) == 0;
        } else {
            // If the child process terminated abnormally
            return false;
        }
    }
    return false;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    // command[count] = command[count];

    
/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/

    va_end(args);

    // Check if the command path is absolute
    if (command[0][0] != '/') {
        // If the command doesn't start with '/', return false
        return false;
    }

    pid_t pid = fork();  // Create a new child process

    if (pid == -1) {
        // If fork() fails
        return false;
    } else if (pid == 0) {
        // In child process
        // Redirect stdout to the file specified by outputfile
        FILE *file = freopen(outputfile, "w", stdout);
        if (file == NULL) {
            // If freopen fails to open the file
            return false;
        }

        // Execute the command with execv()
        if (execv(command[0], command) == -1) {
            // execv() failed
            return false;
        }
    } else {
        // In parent process
        int status;
        // Wait for the child process to complete
        if (waitpid(pid, &status, 0) == -1) {
            // waitpid() failed
            return false;
        }

        // Check the child process exit status
        if (WIFEXITED(status)) {
            // If child process exited normally, check the exit status
            return WEXITSTATUS(status) == 0;
        } else {
            // If the child process terminated abnormally
            return false;
        }
    }
    return false;
}
