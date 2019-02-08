#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "multiThreadSorter_thread.h"

/*
* mergelist_str will be called if the val_to_sort is of type char* and it sorts f_half and s_half recursively
* @param f_ref is the pointer to the first half of the string
* @param s_ref is the pointer to the second half of the string
* @return returns the sorted_list in the order desired
*/
rowNode* mergelist_str(rowNode *f_ref, rowNode *s_ref)
{
	rowNode *sorted_list;
	if(f_ref == NULL)
		return s_ref;
	else if(s_ref == NULL)
		return f_ref;
	//printf("%s compared with %s\n", f_ref->sort, s_ref->sort);
	if(strcmp(f_ref->sort, s_ref->sort) <= 0)
	{
		sorted_list = f_ref;
		sorted_list->next = mergelist_str(f_ref->next, s_ref);
	}
	else
	{
		sorted_list = s_ref;
		sorted_list->next = mergelist_str(f_ref, s_ref->next);
	}
		 
	return sorted_list;
}

/*
* mergelist_int will be called if the val_to_sort is of type int and it sorts f_half and s_half recursively
* @param f_ref is the pointer to the first half of the string
* @param s_ref is the pointer to the second half of the string
* @return returns the sorted_list in the order desired
*/
rowNode* mergelist_int(rowNode *f_ref, rowNode *s_ref)
{
	rowNode *sorted_list = NULL;
	if(f_ref == NULL)
		return s_ref;
	else if(s_ref == NULL)
		return f_ref;
	
	if(atof(f_ref->sort) <= atof(s_ref->sort))
	{
		sorted_list = f_ref;
		sorted_list->next = mergelist_int(f_ref->next, s_ref);
	}
	else
	{
		sorted_list = s_ref;
		sorted_list->next = mergelist_int(f_ref, s_ref->next);
	}
		 
	return sorted_list;
}

/*
* split utilizes a algorithm called fast-slow nodes to find the middle of the linked list and seperate into two equal parts if possible
* @param head_ref is the address to the head of the list 
* @param **f_ref and **s_ref are double pointers so the memory allocation of f_ref and s_ref is preserved outside the function call
*/
void split(rowNode* head_ref, rowNode **f_ref, rowNode **s_ref)
{
	rowNode *slow = head_ref;
	rowNode *fast = head_ref->next;

	while(fast != NULL)
	{
		fast = fast->next;
		if(fast != NULL)
		{
			fast = fast->next;
			slow = slow->next;
		}
	}
	
	*f_ref = head_ref;
	*s_ref = slow->next;
	slow->next = NULL;
}

/* uses the mergesort alogrithm with recursively calls it self to sort a list and utilzes the split() and mergelist() as helper function
* @param list_to_sort is a double pointer to the list we want to sort
*/
void mergesort(rowNode **list_to_sort)
{
	rowNode *head = *list_to_sort;
	rowNode *f_half;
	rowNode *s_half;
	if(head == NULL || head->next == NULL)
		return;
	
	split(head, &f_half, &s_half);

	mergesort(&f_half);
	mergesort(&s_half);
	
	if(isNum == 1)
		*list_to_sort = mergelist_int(f_half, s_half);
	else
		*list_to_sort = mergelist_str(f_half, s_half);
	//printf("in merge\n");
}

  
