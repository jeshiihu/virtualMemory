#include "simulator.h" 

void init(int psize, int winsize) {
	page_size = psize;
	window_size = winsize;
	table_size = 9;
	list_of_page_size = 0;
	table = malloc(table_size*sizeof(llist*));
	numberOfElements = 0;
	list_of_pages = malloc(1*sizeof(int));
}

void put(unsigned int address, int value) {
	llist* newItem = ll_new(address, value);
	ht_insert(table, table_size, newItem);

	addToHistory(address);
}

int get(unsigned int address) {
	llist* foundLlist = ht_search(table, table_size, address);

	addToHistory(address);
	return foundLlist->data;
}

void done() {
	fprintf(stdout, "=== The history of the working set ===\n");

	//TEST CODE
	list_of_page_size = 7;
	list_of_pages = malloc(list_of_page_size*sizeof(int));
	list_of_pages[0] = 2;
	list_of_pages[1] = 1;
	list_of_pages[2] = 2;
	list_of_pages[3] = 6;
	list_of_pages[4] = 2;
	list_of_pages[5] = 4;
	list_of_pages[6] = 1;

	window_size = 3; // expecting 2 3
	int win_count = 0; // window_size how many calls, needed to partition
	int* pages_per_curr_win = malloc(window_size*sizeof(int));

	int numberOfPartitions = (list_of_page_size + window_size - 1)/window_size;
	int numberOfPartitions_counter = 0;
	int* diffPagePartitions = malloc(numberOfPartitions*sizeof(int));

	int i;
	for(i = 0; i < list_of_page_size; i++) { // iterate through all 
		
		pages_per_curr_win[win_count] = list_of_pages[i];
		win_count++;

		if(win_count == window_size || i == list_of_page_size-1) { //filled the set completely! 
			int numOfDifferentPages = getNumberOfDifferentPages(pages_per_curr_win, win_count);
			
			diffPagePartitions[numberOfPartitions_counter] = numOfDifferentPages;
			numberOfPartitions_counter++;

			win_count = 0; // reset it
		}
	}

	for(i = 0; i < numberOfPartitions; i++) {
		printf("%d\n", diffPagePartitions[i]);
	}

	printf("\n");
	printAverageWorkingSet(diffPagePartitions, numberOfPartitions);

	printf("\n");
	printSortedValues();
}

void printAverageWorkingSet(int* arr, int size) {
	int i;
	int sum = 0;

	for(i = 0; i < size; i++)
		sum += arr[i];

	float avg = (float)sum/(float)size;
	printf("=== Average working set size ===\n");
	printf("%f\n", avg);
}

int getNumberOfDifferentPages(int* arr, int size) {
	if(size <= 0)
		return 0;

	int numOfDiff = 1;
	int* diff_arr = malloc(numOfDiff*sizeof(int));
	// we made arr with size window_size, so should be safe to insert 0
	diff_arr[0] = arr[0];

	int i, j;
	for(i = 1; i < size; i++) {

		int duplicate = 0; // zero is bool FALSE
		for(j = 0; j < numOfDiff; j++) {
			if(arr[i] == diff_arr[j]) // duplicate page, don't add to diff_arr
				duplicate = 1; // 1 is bool TRUE
		}

		if(duplicate == 0) {// this is unique add it to the diff_arr 
			numOfDiff++;
			diff_arr = realloc(diff_arr, numOfDiff*sizeof(int));
			diff_arr[numOfDiff-1] = arr[i];
		}
	}

	return numOfDiff;
}

void printSortedValues() {
	printf("== Printing sorted values ==\n");
	int i;
	for(i = 0; i < numberOfElements; i++) {
		int val = get(i);
		printf("%d ", val);
	}

	printf("\n\n");
}

// adding page to our history
void addToHistory(unsigned int address) {
	list_of_page_size++;
	// if(list_of_page_size > 1)
	list_of_pages = realloc(list_of_pages, list_of_page_size*sizeof(int));
	int page = address/page_size;
	list_of_pages[list_of_page_size-1] = page;
}


// ======================= linked list functions to be used by hash table =======================
llist* ll_new(int key, double data) {
	llist* new = malloc(sizeof(llist));
	new->key = key;
	new->data = data;
	new->next = NULL;
	new->previous = NULL;
	
	return new;
}

// Example: head = ll_insert(head, new);
llist* ll_insert(llist* head, llist* new) {
	new->next = head;
	if(head != NULL)
		head->previous = new;
	head = new;
	
	return head;
}

// Example: head = ll_delete(head, item); (remember to free memory for item)
llist* ll_delete(llist* head, llist* item) {
	if(item->previous != NULL)
		item->previous->next = item->next;
	if(item->next != NULL)
		item->next->previous = item->previous;
	if(item == head)
		head = item->next;

	return head;
}

// Example: llist* item = ll_search(head, key)
llist* ll_search(llist* head, int key) {
	for(; head!=NULL; head=head->next) {
		if(head->key == key)
			return head;
	}

	return NULL;
}

// ======================= hash table functions =======================
// hash table functions
void ht_insert(llist** table, int size, llist* item) {
	int key = item->key;
	table[key%size] = ll_insert(table[key%size], item);
}

void ht_delete(llist** table, int size, llist* item) {
	int key = item->key;
	table[key%size] = ll_delete(table[key%size], item);
}

llist* ht_search(llist** table, int size, int key) {
	return ll_search(table[key%size], key);
}
