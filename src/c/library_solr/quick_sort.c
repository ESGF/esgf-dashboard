// quickSort.c
#include <stdio.h>

void quickSort( int[], int, int);
int partition( int[], int, int);


void main() 
{
	//int a[] = { 10, 25, 26};

        int* a=calloc(3, sizeof(int));
        a[0]=10;
        a[1]=25;
        a[2]=26;
    
	int i;
	printf("\n\nUnsorted array is:  ");
	for(i = 0; i < 3; ++i)
		printf(" %d ", a[i]);

	quickSort( a, 0, 2);

	printf("\n\nSorted array is:  ");
	for(i = 0; i < 3; ++i)
		printf(" %d ", a[i]);

}



void quickSort( int* a, int l, int r)
{
   int j;

   if( l < r ) 
   {
   	// divide and conquer
        j = partition( a, l, r);
       quickSort( a, l, j-1);
       quickSort( a, j+1, r);
   }
	
}



int partition( int *a, int l, int r) {
   int pivot, i, j, t;
   pivot = a[l];
   i = l; j = r+1;
		
   while( 1)
   {
   	do ++i; while( a[i] <= pivot && i <= r );
   	do --j; while( a[j] > pivot );
   	if( i >= j ) break;
   	t = a[i]; a[i] = a[j]; a[j] = t;
   }
   t = a[l]; a[l] = a[j]; a[j] = t;
   return j;
}

