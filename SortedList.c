#include "SortedList.h"
#include <sched.h> //sched_yield()
#include <string.h> //strcmp()

/**
 * SortedList_insert ... insert an element into a sorted list
 *
 *	The specified element will be inserted in to
 *	the specified list, which will be kept sorted
 *	in ascending order based on associated keys
 *
 * @param SortedList_t *list ... header for the list
 * @param SortedListElement_t *element ... element to be added to the list
 */

 /*
 struct SortedListElement {
 	struct SortedListElement *prev;
 	struct SortedListElement *next;
 	const char *key;
 };
 */
void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{

  SortedListElement_t* curr = list;
  while(curr != list)
  {
    /*
    -if Return value < 0: str1 is less than str2.
    -if Return value > 0: str2 is less than str1.
    -if Return value = 0: str1 is equal to str2
    */
    if(strcmp(curr->key, element->key) >= 0)
      break;

    curr=curr->next;
  }

  if(opt_yield & INSERT_YIELD)
    sched_yield();
  //insert before curr
  element->next = curr;
  element->prev = curr->prev;
  curr->prev->next = element;
  curr->prev = element;
}

/**
 * SortedList_delete ... remove an element from a sorted list
 *
 *	The specified element will be removed from whatever
 *	list it is currently in.
 *
 *	Before doing the deletion, we check to make sure that
 *	next->prev and prev->next both point to this node
 *
 * @param SortedListElement_t *element ... element to be removed
 *
 * @return 0: element deleted successfully, 1: corrtuped prev/next pointers
 *
 */
int SortedList_delete( SortedListElement_t *element)
{
  if (element->next->prev == element && element->prev->next == element)
  {
    if(opt_yield & DELETE_YIELD)
			sched_yield();
    element->next->prev = element->prev;
    element->prev->next = element->next;
    return 0;
  }
  else
    return 1;

}

/**
 * SortedList_lookup ... search sorted list for a key
 *
 *	The specified list will be searched for an
 *	element with the specified key.
 *
 * @param SortedList_t *list ... header for the list
 * @param const char * key ... the desired key
 *
 * @return pointer to matching element, or NULL if none is found
 */
SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
  SortedListElement_t* curr = list->next;
  while(curr != list)
  {
    //found
    if(strcmp(curr->key, key) == 0)
      return curr;

    if(opt_yield & LOOKUP_YIELD)
			sched_yield();
    curr=curr->next;
  }
  return NULL;
}

/**
 * SortedList_length ... count elements in a sorted list
 *	While enumeratign list, it checks all prev/next pointers
 *
 * @param SortedList_t *list ... header for the list
 *
 * @return int number of elements in list (excluding head)
 *	   -1 if the list is corrupted
 */
int SortedList_length(SortedList_t *list)
{
  if(list == NULL) return -1;

  int length = 0;
  SortedListElement_t* curr = list->next;
  while(curr != list)
  {
    length++;
    if(opt_yield & LOOKUP_YIELD)
      sched_yield();
    curr=curr->next;
  }
  return length;
}
