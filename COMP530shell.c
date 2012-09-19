#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
const int SIZE=200;

//Parser
char** parse(char *line, char **args)
{
	char *temp;
	int argIndex = 0;
	char delims[4] = {'\t',' ', '\n'};
	//Tokenize the input at whitespace characters	
	temp = strtok(line, delims);	
	while(temp != NULL)
	{
		args[argIndex] = temp;
		argIndex++;
		temp = strtok(NULL, delims);
	}		

	args[argIndex] = (char*) 0;
	return args;	
}

void cd(char *dir) 
{
	int ret = 0;
	
	if(dir == '\0')
		dir = getenv("HOME");

	else if(dir[0] == '~')
	{
		char* temp = malloc(SIZE);
		strcpy(temp, dir+1);
		sprintf(dir, "%s%s", getenv("HOME"), temp);
		free(temp);
	}

	ret = chdir(dir);

	if(ret != 0)
		fprintf(stderr, "Failed to enter directory: %s\n", dir);
	
}

int main(int argc, char *argv[])
{
	char *line;
	char *user;
	char *dir;
	char **args;
	int status;
	size_t nbytes = 100;
	int bytes_read;
	pid_t pid;
	int i = 0;	
	
	while(1)
	{
		user = (char*) getenv("USER");
		dir = getcwd(0,0);	
		line = (char *) malloc (nbytes+1);
			
		printf("%s@%s%% ", user, dir);
		bytes_read = getline(&line, &nbytes, stdin); 
		args = malloc(bytes_read);
		free(dir);
	
		//if user presses enter, output prompt again
		if (strcmp(line, "\n") == 0) {
			continue;
		}		
		args = parse(line, args);
		
		if(strcmp(args[0], "quit") == 0 || strcmp(args[0], "exit") == 0) {
			break;
		}
	
		else if ((strcmp(args[0], "cd") == 0)) {
			cd(args[1]);
		}
	
		else {
			pid = fork();
			if(pid == 0) {
				execvp(args[0], args);
				fprintf(stderr, "Unknown command\n");
				exit(0);
			}
					
			else if(pid < 0)
			{
				fprintf(stderr, "Failed to fork(): %s\n", args[0]);
				status = -1;
			}

			else
				while(wait(&status) != pid)
					continue;			
		}
		free(line);
	}
	return 0;
}

