/**********************************************************************
 * Copyright (c) 2019
 *  Sang-Hoon Kim <sanghoonkim@ajou.ac.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>


#include "types.h"
#include "parser.h"

/*====================================================================*/
/*          ****** DO NOT MODIFY ANYTHING FROM THIS LINE ******       */
/**
 * String used as the prompt (see @main()). You may change this to
 * change the prompt */
static char __prompt[MAX_TOKEN_LEN] = "$";

/**
 * Time out value. It's OK to read this value, but ** DO NOT CHANGE
 * DIRECTLY **. Instead, use @set_timeout() function below.
 */
static unsigned int __timeout = 2;


static void set_timeout(unsigned int timeout)
{

	__timeout = timeout;

	if (__timeout == 0) {
		fprintf(stderr, "Timeout is disabled\n");
	} else {
		fprintf(stderr, "Timeout is set to %d second%s\n",
				__timeout,
				__timeout >= 2 ? "s" : "");
	}
}
/*          ****** DO NOT MODIFY ANYTHING UP TO THIS LINE ******      */
/*====================================================================*/


/***********************************************************************
 * run_command()
 *
 * DESCRIPTION
 *   Implement the specified shell features here using the parsed
 *   command tokens.
 *
 * RETURN VALUE
 *   Return 1 on successful command execution
 *   Return 0 when user inputs "exit"
 *   Return <0 on error
 */

char *name;
pid_t child_pid;
int time_ini = 0;

void timeover(int sigaction) { 
	fprintf(stderr, "%s is timed out\n", name);
	time_ini = 0;
	if (kill(child_pid, SIGTERM) == -1) printf("failed\n");
}

int recursion = 1;  //¿Á±Õ∏¶ ¿ß«— ∫Øºˆ(recursion : π›∫π«“ »Ωºˆ  , count :  Ω««‡«“ ∏Ì∑…æÓ¿« ¿ßƒ°) 
int count = 0;

static int run_command(int nr_tokens, char *tokens[])
{
	/* This function is all yours. Good luck! */
	int i = 0,j=0;
	char what[1000];
	int complete = 0;
	name = tokens[0];

	while (strncmp(tokens[count], "for", strlen("for")) == 0) {  //countøÕ recursion¿ª ¡§«—¥Ÿ.
		i = atoi(tokens[count + 1]);
		recursion *= i;
		count += 2;
	}

	

	if (strncmp(tokens[count], "timeout", strlen("timeout")) == 0) {
		unsigned int num = 0;
		if (!tokens[1]) {
			if(time_ini ==0) printf("Current timeout is 0 second\n");
			else printf("Current timeout is %d second\n", __timeout);
		}
		else {
			num = atoi(tokens[1]);
			set_timeout(num);
			time_ini = 1;
		}
	count = 0;
	return 1;
	}

	child_pid = fork();
	if (child_pid > 0) {
		signal(SIGALRM, timeover);
		if (time_ini != 0) {
			alarm(__timeout);
		}
		while (wait(0) != -1);
	}
	else if (child_pid == 0) {
		close(0);
		if (execvp(tokens[count], &tokens[count]) == -1) {
			if(strncmp(tokens[0], "exit", strlen("exit")) != 0
				&& strncmp(tokens[count], "cd", strlen("cd")) != 0
				&& strncmp(tokens[count], "prompt", strlen("prompt")) !=0
				&& strncmp(tokens[count], "timeout", strlen("timeout")!=0)) {
				fprintf(stderr, "No such file or directory\n");
			}
		}
		complete = 1;
		exit(0);
	}
	if (complete = 1) {
		signal(SIGALRM,SIG_IGN);
	}

	if (strncmp(tokens[0], "exit", strlen("exit")) == 0) {
		return 0;
	}
		
	if(strncmp(tokens[count],"cd",strlen("cd"))==0){
		if (strncmp(tokens[count+1], "~", strlen("~")) == 0) {
		if (chdir(getenv("HOME")) != 0) printf("x");
		}
		else if (chdir(tokens[count+1]) == 0) {
			getcwd(what, 1000);
		}
	}

	else if (strncmp(tokens[count], "prompt", strlen("prompt")) == 0) {
		strncpy(__prompt, tokens[1], sizeof(__prompt) - 1);
	}

	

	if (recursion != 1) {  // π›∫π«ÿæﬂ «—¥Ÿ∏È ¿Á±Õ»£√‚«—¥Ÿ.aZ
		recursion--;
		run_command(nr_tokens, tokens);

	}
	else count = 0;  // ¥Ÿ¿Ω ¿‘∑¬¿ª ¿ß«ÿ ∏Ì∑…æÓ¿« ¿ßƒ°∏¶ 0¿∏∑Œ «—¥Ÿ.
	return 1;
}


/***********************************************************************
 * initialize()
 *
 * DESCRIPTION
 *   Call-back function for your own initialization code. It is OK to
 *   leave blank if you don't need any initialization.
 *
 * RETURN VALUE
 *   Return 0 on successful initialization.
 *   Return other value on error, which leads the program to exit.
 */
static int initialize(int argc, char * const argv[])
{
	return 0;
}


/***********************************************************************
 * finalize()
 *
 * DESCRIPTION
 *   Callback function for finalizing your code. Like @initialize(),
 *   you may leave this function blank.
 */
static void finalize(int argc, char * const argv[])
{

}


/*====================================================================*/
/*          ****** DO NOT MODIFY ANYTHING BELOW THIS LINE ******      */

static bool __verbose = true;
static char *__color_start = "[0;31;40m";
static char *__color_end = "[0m";

/***********************************************************************
 * main() of this program.
 */
int main(int argc, char * const argv[])
{
	char command[MAX_COMMAND_LEN] = { '\0' };
	int ret = 0;
	int opt;

	while ((opt = getopt(argc, argv, "qm")) != -1) {
		switch (opt) {
		case 'q':
			__verbose = false;
			break;
		case 'm':
			__color_start = __color_end = "\0";
			break;
		}
	}

	if ((ret = initialize(argc, argv))) return EXIT_FAILURE;

	if (__verbose)
		fprintf(stderr, "%s%s%s ", __color_start, __prompt, __color_end);

	while (fgets(command, sizeof(command), stdin)) {	
		char *tokens[MAX_NR_TOKENS] = { NULL };
		int nr_tokens = 0;

		if (parse_command(command, &nr_tokens, tokens) == 0)
			goto more; /* You may use nested if-than-else, however .. */

		ret = run_command(nr_tokens, tokens);
		if (ret == 0) {
			break;
		} else if (ret < 0) {
			fprintf(stderr, "Error in run_command: %d\n", ret);
		}

more:
		if (__verbose)
			fprintf(stderr, "%s%s%s ", __color_start, __prompt, __color_end);
	}

	finalize(argc, argv);

	return EXIT_SUCCESS;
}
