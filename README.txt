sshell: Simple Unix Shell

--Description--

    sshell is a simple unix shell implemented in c. It has the ability to 
    read commands and arguments provided in the command line interface, 
    as well as files. 

--Error handling--

    - Empty input: 
         If the input is empty of blank, the user is prompted again in the shell.

    - End of file (EOF): 
         If the input is at the end of a file, the program "exit" is entered as a command.

    - Exit command: 
         If the program "exit" is entered, shell terminates.

    - Library errors: 
         If fork() or malloc() fails, shell terminates.

    - Invalid commands: 
         When an invalid command is entered, "Error, command not found" is displayed.

--File structure--
    
    - cmd.c, cmd.h: 
        The logic used to perform actions on cmd objects, which hold data on user input.
    
    - sshell.c, sshell.h: 
        These files contain logic for the main shell routine.

    - utils.c, utils.h: 
        Miscallaneous library functions such as iswhitespace(char*).

--Compiling the Program: Commands--
    
    make 
        -this will create the executable "sshell".

    make clean

        -discards object files and the executable

--Running the shell--
    ./sshell
        -This command will run the shell

