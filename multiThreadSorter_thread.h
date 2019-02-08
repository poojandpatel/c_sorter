#ifndef threadsort_h
#define threadsort_h

#define STDIN 0
#define STDOUT 1
#define STDERR 2

typedef struct rowNode{
    char *sort;
    char *col[28];
    struct rowNode *next;
}rowNode;

typedef struct tNode {
	unsigned int thread_id;
	struct tNode *next;
}tNode;

int isNum;
void writeoutput();
int find_loc(char* col_name);
int count(char* line);
char* tokenize(char* str);
char* trim(char *str);
char *get_filename_ext(char *filename);
int checkFlags(int argc, char *argv[]);
void* dirHandler(void* dir_path);
void* fileHandler(void *filePath);

rowNode* mergelist_str(rowNode *f_ref, rowNode *s_ref);
rowNode* mergelist_int(rowNode *f_ref, rowNode *s_ref);
void split(rowNode* head, rowNode **f_ref, rowNode **s_ref);
void mergesort(rowNode **list_to_sort);

#endif