#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <curl/curl.h>

int main(int argc, char** argv) {
  CURL *curl;
  CURLcode curl_res;
  CURLINFO info;
  long http_code;
  double c_length;  
  FILE *tmp;
  FILE *file;
  char buffer[10024];

  tmp=fopen("curl.tmp", "w");
  if(tmp==NULL) {
    printf("ERROR to open file curl.tmp\n");
    exit(2);
  }

  printf("init curl session\n");
  curl = curl_easy_init();
  printf("set url to download\n");
  curl_easy_setopt(curl, CURLOPT_URL, argv[1]);

  printf("set file handler to write\n");
  curl_easy_setopt(curl, CURLOPT_WRITEDATA,  tmp);

  printf("download the file\n");
  curl_res = curl_easy_perform(curl);
  if(curl_res) 
   {
    printf("ERROR in dowloading file\n");
    fclose(tmp);
    curl_easy_cleanup(curl);
    return -1;
   }
	
  fclose(tmp);
  curl_easy_cleanup(curl);

  file=fopen("curl.tmp", "r");

  if (file == NULL)    
    return -1;

  int i=0;
  if ( file != NULL )
	{
	char line [ 1024 ]; /* or other suitable maximum line size */
	while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line */
		{
		i++;
		fputs ( line, stdout ); /* write the line */
		}
	fclose ( file );
	} 
 printf("Crawling completed!!! [%d]\n",i);
 return 0;
}
