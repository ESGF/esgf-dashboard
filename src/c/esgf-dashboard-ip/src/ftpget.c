/*
 *  ftpget.c
 *    
 *  Author: University of Salento and CMCC 
 *         
 */

#include <stdio.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include "ftpget.h"
#include "error.h"

int my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
  struct FtpFile *out=(struct FtpFile *)stream;
  if(out && !out->stream) {
    /* open file for writing */
    char tmp_file[2048] = {'\0'};
    sprintf (tmp_file, ".work/%s", out->filename);
    out->stream=fopen(tmp_file, "wb");
    if(!out->stream)
    {
      return -1; /* failure, can't open file to write */
    }
  }
  return fwrite(buffer, size, nmemb, out->stream);
}

int ftp_download_file(struct FtpFile **file, int size)
{
	CURL *curl;
	CURLcode res;
	int cnt;
	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if(curl) 
	{
		for(cnt=0; cnt< size; cnt++)
		{
                   int counter=3;
                     while(counter--)
                     {
                        
			curl_easy_setopt(curl, CURLOPT_URL, file[cnt]->URL);
			/* Define our callback to get called when there's data to be written */
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
			/* Set a pointer to our struct to pass to the callback */
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, file[cnt]);

			/* Switch on full protocol/debug output */
			//curl_easy_setopt(curl, CURLOPT_STDERR, respfile[cnt]);
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, "FALSE");
			res = curl_easy_perform(curl);
                        if(res==0)
                        {
			  if(file[cnt]->stream)
			  fclose(file[cnt]->stream); /* close the local file */
                          break;
                       }
                       if(counter==0)
                         break;
                       counter--;
                       sleep(5);
                     }
		}
		/* always cleanup */
  		curl_easy_cleanup(curl);
		
	//	free(respfile);
	}
	curl_global_cleanup();
	return res;
	//return SUCCESS;
}

