#include "parse.c"

/////////////////////



int execute_command(Command *command){
    
    if(strcmp(command->name, "true") == 0) return 0;
    if(strcmp(command->name, "false") == 0) return 1;
    
    int status = 0;
    pid_t pid = fork();
    if(pid == 0){
        if(command->mod1 == 1){
            int fd = creat(command->output, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        if(command->mod2 == 1){
            int fd = open(command->output, O_CREAT | O_WRONLY | O_APPEND, 777);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        if(command->mod3 == 1){
            int fd = open(command->input, O_RDONLY);
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        int cap = execvp(command->name, command->args);
        if(cap < 0){
            ERRORC(command->name);
            status = 1;
        }
        exit(0);
    }
    else{
    	wait(NULL);
    }
    return status;
}

int execute(Command *command){
    if(strcmp(command->name, "history") == 0){
        show_history();
        return 0;
    }
    if(strcmp(command->name, "help") == 0){
        help(command);
        return 0;
    }
    
    if(strcmp(command->name, "exit") == 0){
        exit(0);
    }
    if(strcmp(command->name, "cd") == 0){
        if(command->length_args < 2){
            ERRORC("cd");
            return 1;
        }
        int success = chdir(command->args[1]);
        if(success != 0){
            printf("No such file or directory %s\n", command->args[1]);
            return 1;
        }
        return 0;
    }
    else{
        if(command->error == 1){
            ERRORC(command->name);
            return 1;
        }
        return execute_command(command);
    }
}

int String_Of_Commands(Commands_Split_Pipes *commands_pipes){
    Command *command = &(commands_pipes->command_by_pipes[0]);
    if(strcmp(command->name, "if") == 0){
        if(command->_if == NULL || command->_then == NULL){
            return 0;
        }
        Commands_Split_Cond split_cond;
        Constructor_Commands_Split_Cond(&split_cond);
        Parse_Input(command->_if, &split_cond);
        int status_if = 0; //IF
        for(int j = 0; j <= split_cond.length_cond; j++){ //iterating for conditions
            int status = String_Of_Commands(&split_cond.command_by_cond[j]);
            if(j == split_cond.length_cond){
                status_if = status;
                continue;
            }
            if(j == split_cond.length_cond){
                status_if = status;
                continue;
            }
            if(split_cond.Type_Cond[j] == 0){
                if(status == 1){
                    status_if = 1;
                    break;
                }
            }
            else{
                if(status == 1){
                    status_if = 1;
                    break;
                }
            }
        }
        if(status_if == 1){//ELSE
            if(command->_else != NULL){
                Constructor_Commands_Split_Cond(&split_cond);
                Parse_Input(command->_else, &split_cond);
                int status_else = 0;
                for(int j = 0; j <= split_cond.length_cond; j++){ //iterating for conditions
                    int status = String_Of_Commands(&split_cond.command_by_cond[j]);
                    if(j == split_cond.length_cond){
                        status_else = status;
                        continue;
                    }
                    if(j == split_cond.length_cond){
                        status_else = status;
                        continue;
                    }
                    if(split_cond.Type_Cond[j] == 0){
                        if(status == 1){
                            status_else = 1;
                            break;
                        }
                    }
                    else{
                        if(status == 1){
                            status_else = 1;
                            break;
                        }
                    }
                    return status_else;
                }
            }
            else{
                return 1;
            }
        }
        else{//THEN
            Constructor_Commands_Split_Cond(&split_cond);
            Parse_Input(command->_then, &split_cond);
            int status_then = 0;
            for(int j = 0; j <= split_cond.length_cond; j++){ //iterating for conditions
                int status = String_Of_Commands(&split_cond.command_by_cond[j]);
                if(j == split_cond.length_cond){
                    status_then = status;
                    continue;
                }
                if(j == split_cond.length_cond){
                    status_then = status;
                    continue;
                }
                if(split_cond.Type_Cond[j] == 0){
                    if(status == 1){
                        status_then = 1;
                        break;
                    }
                }
                else{
                    if(status == 1){
                        status_then = 1;
                        break;
                    }
                }
            }
            return status_then;
        }
    }
    else{
        int status = execute(command); //not pipes yet :(
        return status;
    }
}

int main(){
    Initial(); //Create file for history
    while(1){
        printf(Yellow "my-prompt " RESET "$ ");
        //initialize
        char *line_input = malloc(SIZE * sizeof(char));
        fgets(line_input, SIZE, stdin); //get line

        if(Is_Only_Spaces(line_input)) continue;

        //First we have to change every command 'again' for the right command on history
        char *new_line = malloc(SIZE);
        Change_Command_Again(line_input, new_line);
        
        Split_Lines_Dotcomma line_split;
        Constructor_Split_Lines_Dotcomma(&line_split);
        Split_Line(new_line, &line_split);
        Save_History(new_line);
        for(int i = 0; i < line_split.length_lines_splits; i++){ //lines independent for ;
            for(int j = 0; j <= line_split.commands_lines[i].length_cond; j++){ //iterating for conditions
                int status = String_Of_Commands(&line_split.commands_lines[i].command_by_cond[j]);
                if(j == line_split.commands_lines[i].length_cond) continue;
                if(line_split.commands_lines[i].Type_Cond[j] == 0){
                    if(status == 0) break;
                }
                else{
                    if(status == 1) break;
                }
            }
        }
    }

    return 0;
}
