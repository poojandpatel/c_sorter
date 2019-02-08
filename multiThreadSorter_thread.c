#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <pthread.h>
#include "multiThreadSorter_thread.h"

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t dir_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mother_mutex = PTHREAD_MUTEX_INITIALIZER;

char *startDirectory = ".";	// holds start directory
char *outputDirectory = "."; // holds output directory
char *col_to_sort = NULL;
int thread_count = 0;
rowNode *broccoli = NULL;
tNode *motherlist = NULL;

int main(int argc, char *argv[])
{
	if (checkFlags(argc, argv) == -1)
		return -1;
	
	motherlist = (tNode *)malloc(sizeof(tNode));
	pthread_t motherthread;

	pthread_create(&motherthread, NULL, dirHandler, startDirectory);
	motherlist->thread_id = motherthread;
	pthread_join(motherthread, NULL);
	thread_count++;

	fflush(stdout);
	printf("INITIAL PID: %d\n", getpid());
	printf("TIDS of all spawned thread: ");
	printf("%x, ", motherlist->thread_id);
	tNode *temp = motherlist;
	temp = temp->next;
	while (temp->next != NULL)
	{
		printf("%x, ", temp->thread_id);
		temp = temp->next;
	}
	printf("%x", temp->thread_id);
	printf("\nTotal number of threads: %d\n", thread_count);

	if (broccoli != NULL)
	{
		mergesort(&broccoli);
		writeoutput();
	}
	else
		fprintf(stderr, "No valid files founds\n");

	//free broccoli and motherlist

	while (motherlist)
	{
		temp = motherlist;
		motherlist = motherlist->next;
		free(temp);
	}

	return 0;
}

void writeoutput()
{
	char *outputfile = (char *)malloc(sizeof(char) * 46);
	char *outputdir = (char *)malloc(sizeof(char) * (strlen(outputDirectory) + 47));
	strcat(outputfile, "AllFiles-sorted-");
	strcat(outputfile, col_to_sort);
	strcat(outputfile, ".csv");
	outputfile[strlen(outputfile)] = '\0';

	// build output dir path
	strcat(outputdir, outputDirectory);
	strcat(outputdir, "/");
	strcat(outputdir, outputfile);
	//printf("outputdir: %s\n", outputdir);

	FILE *fd_w = fopen(outputdir, "w+");

	char *all_cols = "color,director_name,num_critic_for_reviews,duration,director_facebook_likes,actor_3_facebook_likes,actor_2_name,actor_1_facebook_likes,gross,genres,actor_1_name,movie_title,num_voted_users,cast_total_facebook_likes,actor_3_name,facenumber_in_poster,plot_keywords,movie_imdb_link,num_user_for_reviews,language,country,content_rating,budget,title_year,actor_2_facebook_likes,imdb_score,aspect_ratio,movie_facebook_likes";
	fprintf(fd_w, "%s\n", all_cols);
	rowNode *temp = broccoli;
	while (temp)
	{
		int i = 0;
		for (i = 0; i < 27; i++)
			fprintf(fd_w, "%s,", temp->col[i]);

		fprintf(fd_w, "%s\n", temp->col[27]);
		temp = temp->next;
	}
	//free the linked list
	rowNode *current = broccoli;
	while (current)
	{
		rowNode *next = current->next;
		free(current);
		current = next;
	}
	fclose(fd_w);
	return;
}

/*
* retunrs the correct spot to put the col_value
*/
int find_loc(char *col_name)
{
	if (strcmp(col_name, "color") == 0)
		return 0;
	else if (strcmp(col_name, "director_name") == 0)
		return 1;
	else if (strcmp(col_name, "num_critic_for_reviews") == 0)
		return 2;
	else if (strcmp(col_name, "duration") == 0)
		return 3;
	else if (strcmp(col_name, "director_facebook_likes") == 0)
		return 4;
	else if (strcmp(col_name, "actor_3_facebook_likes") == 0)
		return 5;
	else if (strcmp(col_name, "actor_2_name") == 0)
		return 6;
	else if (strcmp(col_name, "actor_1_facebook_likes") == 0)
		return 7;
	else if (strcmp(col_name, "gross") == 0)
		return 8;
	else if (strcmp(col_name, "genres") == 0)
		return 9;
	else if (strcmp(col_name, "actor_1_name") == 0)
		return 10;
	else if (strcmp(col_name, "movie_title") == 0)
		return 11;
	else if (strcmp(col_name, "num_voted_users") == 0)
		return 12;
	else if (strcmp(col_name, "cast_total_facebook_likes") == 0)
		return 13;
	else if (strcmp(col_name, "actor_3_name") == 0)
		return 14;
	else if (strcmp(col_name, "facenumber_in_poster") == 0)
		return 15;
	else if (strcmp(col_name, "plot_keywords") == 0)
		return 16;
	else if (strcmp(col_name, "movie_imdb_link") == 0)
		return 17;
	else if (strcmp(col_name, "num_user_for_reviews") == 0)
		return 18;
	else if (strcmp(col_name, "language") == 0)
		return 19;
	else if (strcmp(col_name, "country") == 0)
		return 20;
	else if (strcmp(col_name, "content_rating") == 0)
		return 21;
	else if (strcmp(col_name, "budget") == 0)
		return 22;
	else if (strcmp(col_name, "title_year") == 0)
		return 23;
	else if (strcmp(col_name, "actor_2_facebook_likes") == 0)
		return 24;
	else if (strcmp(col_name, "imdb_score") == 0)
		return 25;
	else if (strcmp(col_name, "aspect_ratio") == 0)
		return 26;
	else if (strcmp(col_name, "movie_facebook_likes") == 0)
		return 27;

	return -1;
}

int count_rows(char *line)
{
	int comma_count = 0;											 // keeps track of the column
	char *last = (char *)malloc(sizeof(char)); // last character read in
	int i = 0;
	int length = strlen(line);
	//find the starting point
	while (i < length)
	{
		*last = line[i];
		i++;
		if (*last == '"')
		{
			do
			{
				*last = line[i];
				i++;
			} while (*last != '"');
		}
		else if (*last == ',')
		{
			comma_count++;
		}
	}
	free(last);
	return comma_count;
}

char *tokenize(char *str)
{
	//printf("string to tokenize: %s\n", str);
	char *tok = NULL;
	int end = -1;
	char last;
	while (last != ',')
	{
		end++;
		last = str[end];
		if (last == '"')
		{
			do
			{
				end++;
				last = str[end];
			} while (last != '"');
		}
	}

	if (end != -1)
	{
		tok = (char *)malloc(sizeof(char) * end + 1);
		int i = 0;
		for (i = 0; i < end; i++)
			tok[i] = str[i];
		tok[end] = '\0';
		//printf("tok: %s\n", tok);
		return tok;
	}
	tok = str;
	return tok;
}

/*
 * trims leading and trailing spaces from the string
 * @param char *str is the string to modify
 * @return returns a char* with leading and trailing spaces removed  
 */
char *trim(char *str)
{
	int length = strlen(str);
	char *lead = str;
	char *trail = str + (length - 1);
	int i = 0;
	if (lead == trail)
		return str;

	//remove leading white space by inc leadPtr unitl it reaches a character
	while (isspace(*lead))
		lead++;
	str = lead;

	//remove trailing space
	if (isspace(*trail))
	{
		while (isspace(*trail))
		{
			--trail;
		}
		while (lead != trail)
		{
			lead++;
			i++;
		}
		i++;
		*(str + i) = '\0';
	}

	return str;
}

// gets the extension of a file
char *get_filename_ext(char *filename)
{
	char *dot = strrchr(filename, '.');
	if (!dot || dot == filename)
		return "";
	return dot + 1;
}

// checks for validity of the input
int checkFlags(int argc, char *argv[])
{
	if(argc < 3 || argc > 7)
	{
		fprintf(stderr,"Invalid number of arguments passed in\n");
		return -1;
	}
	int opt;
	while ((opt = getopt(argc, argv, "c:d:o:")) != -1)
	{
		switch (opt)
		{
		case 'c':
			//printf("Column To Sort: %s\n", optarg);
			if (strcmp(optarg, "-c") == 0 || strcmp(optarg, "-d") == 0 || strcmp(optarg, "-o") == 0)
				return -1;
			col_to_sort = optarg;
			break;
		case 'd':
			//printf("Start Directory: %s\n", optarg);
			if (strcmp(optarg, "-c") == 0 || strcmp(optarg, "-d") == 0 || strcmp(optarg, "-o") == 0)
				return -1;
			startDirectory = optarg;
			break;
		case 'o':
			//printf("Output Directory: %s\n", optarg);
			if (strcmp(optarg, "-c") == 0 || strcmp(optarg, "-d") == 0 || strcmp(optarg, "-o") == 0)
				return -1;
			outputDirectory = optarg;
			break;
		case '?':
			fprintf(stderr, "Invalid Argument Passed In\n");
			return -1;
		}
	}

	char *numeric = "num_critic_for_reviews,duration,director_facebook_likes,actor_3_facebook_likes,actor_1_facebook_likes,gross,num_voted_users,cast_total_facebook_likes,facenumber_in_poster,num_user_for_reviews,budget,title_year,actor_2_facebook_likes,imdb_score,aspect_ratio,movie_facebook_likes";
	
	if (strstr(numeric, col_to_sort) != NULL)
		isNum = 1;
	else
		isNum = 0;
	
	if(find_loc(col_to_sort) == -1)
	{
		fprintf(stderr, "Invalid column name\n");
		return -1;
	}
		
		
	if (strcmp(outputDirectory, ".") != 0)
	{
		DIR *dir = opendir(outputDirectory);
		if (!dir)
		{
			fprintf(stderr, "Invalid output directory\n");
			return -1;
		}
	}

	if (strcmp(startDirectory, ".") != 0)
	{
		DIR *dir = opendir(startDirectory);
		if (!dir)
		{
			fprintf(stderr, "Invalid start directory\n");
			return -1;
		}
	}

	return 0;
}

void *dirHandler(void *dir_path)
{
	char *basePath = (char *)dir_path;
	fflush(stdout);
	DIR *dir = opendir(basePath);
	tNode *dirHead = NULL;
	char *name;
	int local = 1;
	if (!dir)
	{
		fprintf(stderr, "invalid directory path: %s\n", basePath);
		pthread_exit(NULL);
	}

	struct dirent *dp = NULL;
	while ((dp = readdir(dir)) != NULL)
	{
		if (dp->d_type == DT_DIR)
		{
			name = dp->d_name;
			char *path = (char *)malloc(sizeof(char) * 100000);
			if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
				continue;
			strncpy(path, basePath, strlen(basePath));
			strcat(path, "/");
			strcat(path, dp->d_name);
			pthread_t dirThread;
			tNode *dir = (tNode *)malloc(sizeof(tNode));
			tNode *dir_local = (tNode *)malloc(sizeof(tNode));
			dir->next = NULL;
			dir_local->next = NULL;

			pthread_create(&dirThread, NULL, dirHandler, path);

			pthread_mutex_lock(&dir_mutex);
			thread_count++;
			dir->thread_id = dirThread + thread_count;
			dir_local->thread_id = dir->thread_id;
			if (dirHead == NULL)
				dirHead = dir_local;
			else
			{
				dir_local->next = dirHead;
				dirHead = dir_local;
			}
			dir->next = motherlist;
			motherlist = dir;
			pthread_mutex_unlock(&dir_mutex);
			if(pthread_join(dirThread, NULL) != 0)
			{
				fprintf(stderr, "dirthread_join fail\n");
				pthread_exit(NULL);
			}
		}
		else
		{
			char *path = (char *)malloc(sizeof(char) * 100000);
			//makes the filePath
			void *tempHead;
			strncpy(path, basePath, strlen(basePath));
			strcat(path, "/");
			strcat(path, dp->d_name);
			pthread_t fileThread;
			tNode *file = (tNode *)malloc(sizeof(tNode));
			file->next = NULL;

			tNode *file_local = (tNode *)malloc(sizeof(tNode));
			file_local->next = NULL;

			pthread_create(&fileThread, NULL, fileHandler, path);
			pthread_mutex_lock(&file_mutex);
			thread_count++;
			pthread_mutex_unlock(&file_mutex);
			file->thread_id = fileThread + thread_count;
			file_local->thread_id = file->thread_id;
			printf("INITIAL PID: %d\n", getpid());
			printf("TIDS of all spawned thread: \n");
			printf("Total number of Threads: %d\n\n", 0);
			
			if (pthread_join(fileThread, &tempHead) != 0)
			{
				fprintf(stderr, "thread_join fail\n");
				pthread_mutex_lock(&file_mutex);
				
				if (dirHead == NULL)
					dirHead = file_local;
				else
				{
					file_local->next = dirHead;
					dirHead = file_local;
				}
				file->next = motherlist;
				motherlist = file;
				pthread_mutex_unlock(&file_mutex);
				pthread_exit(NULL);
			}
			else
			{
				pthread_mutex_lock(&file_mutex);
				if (dirHead == NULL)
					dirHead = file_local;
				else
				{
					file_local->next = dirHead;
					dirHead = file_local;
				}
				file->next = motherlist;
				motherlist = file;
				//add filehead to main head
				rowNode *file_to_add = (rowNode *)tempHead;
				//sort file before adding
				mergesort(&file_to_add);
				//add to main list
				if (broccoli == NULL)
					broccoli = file_to_add;
				else
				{
					rowNode *temp = broccoli;
					while (temp->next != NULL)
					{
						temp = temp->next;
					}
					temp->next = file_to_add;
				}
				pthread_mutex_unlock(&file_mutex);
			}
		}
	}
	if (dirHead == NULL)
	{
		printf("INITIAL PID: %d\n", getpid());
		printf("TIDS of all spawned threads: \n");
		printf("Total number of Threads: %d\n\n", 0);
	}
	else
	{
		printf("INITIAL PID: %d\n", getpid());
		printf("TIDS of all spawned threads: ");
		tNode *temp = dirHead;
		while (temp->next != NULL)
		{
			printf("%x, ", temp->thread_id);
			temp = temp->next;
			local++;
		}
		printf("%x", temp->thread_id);
		printf("\nTotal number of Threads: %d\n\n", local);
	}
	// free the dirHead
	tNode *temp;
	while (dirHead)
	{
		temp = dirHead;
		dirHead = dirHead->next;
		free(temp);
	}

	closedir(dir);
	pthread_exit(NULL);
}

void *fileHandler(void *path)
{
	char *filePath = (char *)path;
	char *ext = get_filename_ext(filePath);
	// if invalid file, print to stderr and leave
	if (strcmp(ext, "csv") != 0)
	{
		fprintf(stderr, "Invalid File Type %s\n", filePath);
		pthread_exit(NULL);
	}
	FILE *fd = fopen(filePath, "r");
	//printf("path: %s\n\n", filePath);
	if (fd == NULL)
	{
		fprintf(stderr, "invalid file %s\n", filePath);
		pthread_exit(NULL);
	}
	if (strstr(filePath, "AllFiles-sorted") != NULL)
	{
		fprintf(stderr, "File Sorted %s\n", filePath);
		fclose(fd);
		pthread_exit(NULL);
	}

	// check if file is empty
	int i = 0;
	fseek(fd, 0, SEEK_END);
	i = ftell(fd);
	if (i == 0)
	{
		fprintf(stderr, "Empty File %s\n", filePath);
		fclose(fd);
		pthread_exit(NULL);
	}

	// reopen file
	fd = fopen(filePath, "r");
	rowNode *fileHead = NULL; // initialize the file linked list called fileHead

	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	int n_cols_head = 0;
	int n_cols_row = 0;
	// convert header to an array
	read = getline(&line, &len, fd);
	char *row = strdup(line);
	n_cols_head = count_rows(line) + 1;
	char *header_arr[n_cols_head];

	char *tok = row;
	int count = 0;
	int loc_check = 0;
	while (strcmp(tok, "\0") != 0 && count < n_cols_head)
	{
		tok = strsep(&row, ",");
		if (strstr(tok, "\n") != NULL)
			tok[strlen(tok) - 1] = '\0';
		loc_check = find_loc(tok);
		if (loc_check == -1)
		{
			fprintf(stderr, "invalid col name \"%s\" found in %s \n", tok, filePath);
			pthread_exit(NULL);
		}
		header_arr[count] = tok;
		count++;
	}
	// reset vars for reuse
	count = 0;
	tok = NULL;
	char *col_belongs_to;
	int loc = -1;
	int tok_len;
	while ((read = getline(&line, &len, fd) != -1))
	{
		row = strdup(line); // so we can have a different address
		n_cols_row = count_rows(row) + 1;
		if (n_cols_head != n_cols_row)
		{
			fprintf(stderr, "invalid amount of columns in row %s\n", filePath);
			pthread_exit(NULL);
		}

		// Build the file linkedlist
		rowNode *row_to_add = (rowNode *)malloc(sizeof(rowNode));
		for (i = 0; i < 28; i++)
			row_to_add->col[i] = "";
		row_to_add->next = NULL;

		while (count < (n_cols_row - 1))
		{
			tok = tokenize(row);
			tok_len = strlen(tok) + 1;
			row = row + tok_len;
			//put in correct spot
			col_belongs_to = header_arr[count];
			loc = find_loc(col_belongs_to);
			//add it to the row
			row_to_add->col[loc] = tok;
			count++; //don't forget to reset
		}

		//for the last token
		tok = row;
		tok[strlen(tok) - 1] = '\0';
		col_belongs_to = header_arr[count];
		loc = find_loc(col_belongs_to);
		row_to_add->col[loc] = tok;

		loc = find_loc(col_to_sort);
		if (strcmp(row_to_add->col[loc], "") != 0)
			row_to_add->sort = trim(row_to_add->col[loc]);
		else
			row_to_add->sort = "";

		count = 0;
		if (fileHead == NULL)
			fileHead = row_to_add;
		else
		{
			row_to_add->next = fileHead;
			fileHead = row_to_add;
		}
	}
	pthread_exit(fileHead);
}