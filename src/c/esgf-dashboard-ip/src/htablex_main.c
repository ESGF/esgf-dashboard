/* Copyright (c) 2011 the authors listed at the following URL, and/or
the authors of referenced articles or incorporated external code:
http://en.literateprograms.org/Hash_table_(C)?action=history&offset=20100620072342

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Retrieved from: http://en.literateprograms.org/Hash_table_(C)?oldid=16749
*/

#include"hashtbl.h"
#include<stdio.h>

int main()
{
	HASHTBL *hashtbl;
	char *spain, *italy;

	if(!(hashtbl=hashtbl_create(16, NULL))) {
		fprintf(stderr, "ERROR: hashtbl_create() failed\n");
		exit(EXIT_FAILURE);
	}

	hashtbl_insert(hashtbl, "France", "Paris");
	hashtbl_insert(hashtbl, "England", "London");
	hashtbl_insert(hashtbl, "Sweden", "Stockholm");
	hashtbl_insert(hashtbl, "Germany", "Berlin");
	hashtbl_insert(hashtbl, "Norway", "Oslo");
	hashtbl_insert(hashtbl, "Italy", "Rome");
	hashtbl_insert(hashtbl, "Spain", "Madrid");
	hashtbl_insert(hashtbl, "Estonia", "Tallinn");
	hashtbl_insert(hashtbl, "Netherlands", "Amsterdam");
	hashtbl_insert(hashtbl, "Ireland", "Dublin");

	printf("After insert:\n");
	italy=hashtbl_get(hashtbl, "Italy");
	printf("Italy: %s\n", italy?italy:"-");
	spain=hashtbl_get(hashtbl, "Spain");
	printf("Spain: %s\n", spain?spain:"-");


	hashtbl_remove(hashtbl, "Spain");

	printf("After remove:\n");
	spain=hashtbl_get(hashtbl, "Spain");
	printf("Spain: %s\n", spain?spain:"-");


	hashtbl_resize(hashtbl, 8);

	printf("After resize:\n");
	italy=hashtbl_get(hashtbl, "Italy");
	printf("Italy: %s\n", italy?italy:"-");

	hashtbl_destroy(hashtbl);

	return 0;
}

