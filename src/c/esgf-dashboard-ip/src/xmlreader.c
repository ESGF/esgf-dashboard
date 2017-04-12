/*
 *  xmlreader.c
 *  
 *  Author: University of Salento and CMCC 
 *     
 */
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <libxml/xpathInternals.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/tree.h>
#include "ftpget.h"
#include <error.h>
#include <sys/time.h>
#include "libpq-fe.h"
#include "../include/config.h"
#include "../include/dbAccess.h"
#include "../include/debug.h"

/**
 * print_element_names:
 * @a_node: the initial xml node to consider.
 *
 * Prints the names of the all the xml elements
 * that are siblings or children of a given xml node.
 */

xmlXPathObjectPtr
getnodeset (xmlDocPtr doc, xmlChar *xpath, xmlChar *ns_prefix, xmlChar *ns_uri) 
{
	int status;
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;


	context = xmlXPathNewContext(doc);
	
	if (context == NULL) 
	{
		printf("Error in xmlXPathNewContext\n");
		return NULL;
	}
	
	xmlXPathRegisterNs(context, ns_prefix, ns_uri);
	
	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL) 
	{
		printf("Error in xmlXPathEvalExpression\n");
		return NULL;
	}
	
	if(xmlXPathNodeSetIsEmpty(result->nodesetval))
	{
		xmlXPathFreeObject(result);
		//printf("No result\n");
		return NULL;
	}
	
	return result;
}
int child_node_count(const xmlNodePtr nodePtr){
    
    int i = 0;
    xmlNodePtr node;
    for(node = nodePtr->children;node;node = node->next,i++);
    return i--;
}


int count_tag(xmlDocPtr * doc, char *tag)
{
  int num_children=0;
  xmlXPathObjectPtr result;
  int i=0;
  xmlNodePtr node;
  result = getnodeset(doc, BAD_CAST tag, NULL, NULL);
  if (result)
	{
		for(i = 0; i < result->nodesetval->nodeNr; i++)
		{  
			node = result->nodesetval->nodeTab[i];
			num_children = child_node_count(node);
			//printf("num_children %d\n",num_children);
	        }      
        
		xmlXPathFreeObject(result);
	}
  return num_children;
}
int count_metadata(xmlDocPtr * doc, char *tag, char *projectName)
{
  int num_children=0;
  xmlXPathObjectPtr result;
  int i=0;
  int num=0;
  xmlNodePtr node;
  xmlNode *cur_node;
  xmlChar *prop = NULL;
  result = getnodeset(doc, BAD_CAST tag, NULL, NULL);
  if (result)
        {
                for(i = 0; i < result->nodesetval->nodeNr; i++)
                {
                   node = result->nodesetval->nodeTab[i];
                   prop = xmlGetProp(node, (xmlChar *)"name");
                   //printf("prop vale %s\n", prop);
                   if(xmlStrcasecmp(prop, (xmlChar *)projectName)==0)
                   {
                       for (cur_node = node->children; cur_node; cur_node = cur_node->next) {
                          if (cur_node->type == XML_ELEMENT_NODE) {
                              num++;
                           }
                       }              
                   }
                   free(prop);
                   prop=NULL;
                }

                xmlXPathFreeObject(result);
        }
  return num;
}


int retrieve_tag(xmlDocPtr * doc, char *tag, struct dataset_project ***datasetproj, int cnt, int c, int j, int first_query, int num)
{
  int num_children=0;
  xmlChar *prop = NULL;
  xmlXPathObjectPtr result;
  int i,k=0;
  xmlNodePtr node,node1;
  char *str=NULL;
  int res_met=0;
  result = getnodeset(doc, BAD_CAST tag, NULL, NULL);
  if (result)
        {
                for(i = 0; i < result->nodesetval->nodeNr; i++)
                {
                        node = result->nodesetval->nodeTab[i];
                        node = node->children;
                        for (node1 = node; node1; node1 = node1->next)
                        {
                             xmlChar *content=xmlNodeGetContent(node1);
                             (*datasetproj)[cnt]->first[c]->first[j]->value[k]=xmlStrdup(content);
                             xmlFree(content);
                             k++;
                        }
                }
                (*datasetproj)[cnt]->first[c]->first[j]->size=k;
                xmlXPathFreeObject(result);
        }
        else
        {
           (*datasetproj)[cnt]->first[c]->first[j]->size=0;
        }
        
  return num_children;
}

static void
read_element_names(xmlNode * a_node,struct dataset_project ***datasetproj, int i, int j)
{
    xmlNode *cur_node = NULL;
    struct project* first1=NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            first1=NULL;
            first1=(struct project*)calloc (1, sizeof(struct project));
            first1->project=xmlNodeGetContent(cur_node);
            (*datasetproj)[i]->first[j]=first1;
            j++;
        }

        read_element_names(cur_node->children, datasetproj, i, j);
    }
}


int
get_datasetid_solr(xmlNode * a_node, struct dataset_project ***datasetproj,int cnt, int num, char** id_query)
{
    xmlNode *cur_node = NULL;
    xmlChar *prop = NULL;
    xmlChar *prop1 = NULL;
    xmlNode * b_node=NULL;
    int num_str=0;
    char str_dataset_id[2048] = { '\0' };
    
    int res, proj=0;
    int i=0;
    int j=0;
     


    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            //printf("node type: Element, name: %s\n", cur_node->name);
            prop = xmlGetProp(cur_node, (xmlChar *)"name");
            if(prop)
            {
              if(xmlStrcasecmp(prop, (xmlChar *)"id")==0)
              {
                prop1 = xmlNodeGetContent(cur_node);
                (*datasetproj)[cnt]->id=strdup(prop1); 
                xmlFree(prop1);
              }
              if(xmlStrcasecmp(prop, (xmlChar *)"version")==0)
              {
                prop1 = xmlNodeGetContent(cur_node);
                (*datasetproj)[cnt]->version=strdup(prop1); 
                xmlFree(prop1);
              }
              if(xmlStrcasecmp(prop, (xmlChar *)"dataset_id")==0)
              {
                prop1 = xmlNodeGetContent(cur_node);
                (*datasetproj)[cnt]->id_query=atoi(id_query[cnt]);
                (*datasetproj)[cnt]->dataset_id=strdup(prop1);
                //printf("++++++query vale %d\n", (*datasetproj)[cnt]->id_query);

#if 0
                char *tmp_str=strstr(prop1, "|");
                char *tmp_str1=strstr(prop1, "|");
                if(tmp_str)
                {
                   *tmp_str = 0; 
                   sprintf(str_dataset_id,"%s%s%s",prop1,"%7C",tmp_str1+1); 
                   //printf("str_dataset_id vale %s\n", str_dataset_id);
                }
          
                (*datasetproj)[cnt]->dataset_id=strdup(str_dataset_id); 
                /*char *tmp_str=strstr(prop1, "|");
                if(tmp_str)
                {
                   *tmp_str = 0; 
                }
                char *tmp_str1=strstr(prop1, ".v");
                if(tmp_str1)
                  (*datasetproj)[cnt]->version=strdup(tmp_str1+2);*/
#endif
                xmlFree(prop1);
              }
              if(xmlStrcasecmp(prop, (xmlChar *)"project")==0)
              {
                    read_element_names(cur_node->children, datasetproj, cnt, j);
              }
            }
            xmlFree(prop);
        }

        get_datasetid_solr(cur_node->children,datasetproj, cnt,num, id_query);

      }
    return 0;
}

int read_conf_file(xmlNode * a_node, char *projectName, struct dataset_project ***datasetproj, int num_metadata, int cnt, int i)
{
    xmlNode *cur_node = NULL;
    xmlChar *prop = NULL;
    xmlChar *prop1 = NULL;
    
    int res, prj=0;
    int k=0;
    struct metadata_entry *first1=NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE){
	if(xmlStrcasecmp(cur_node->name, (xmlChar *)"project")==0) {
	    prop = xmlGetProp(cur_node, (xmlChar *)"name");
	    if(prop)
	    {
		if(xmlStrcasecmp(prop, (xmlChar *)projectName)==0)
		{
                   a_node=cur_node->children;
                   for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
                      if(xmlStrcasecmp(cur_node->name, (xmlChar *)"metadata")==0)
                      {
                         prop1 = xmlGetProp(cur_node, (xmlChar *)"occ");
                         if(prop1)
                         {
                            (*datasetproj)[cnt]->first[i]->first[k]=(struct metadata_entry*)calloc (1, sizeof(struct metadata_entry));
                            (*datasetproj)[cnt]->first[i]->first[k]->occ=strdup((char *)(prop1));
                            xmlChar* content=xmlNodeGetContent(cur_node);
                            (*datasetproj)[cnt]->first[i]->first[k]->name=strdup((char *) content);
                            (*datasetproj)[cnt]->first[i]->first[k]->value = (char**)calloc (1, sizeof(char *));
                            (*datasetproj)[cnt]->first[i]->first[k]->value[0]=NULL;
                            xmlFree(content);
                            content=NULL;
                            k++;
                         }
                         xmlFree(prop1);
                      }
                   }
                   prj=1;    

                }
            }
            xmlFree(prop);
	  }
        }
        if (prj==0)
        {
          read_conf_file(cur_node->children, projectName, datasetproj,num_metadata,cnt,i);
        }
        else
        {
           (*datasetproj)[cnt]->first[i]->first[k]=(struct metadata_entry*)calloc (1, sizeof(struct metadata_entry));
           (*datasetproj)[cnt]->first[i]->first[k]->occ=NULL;
           (*datasetproj)[cnt]->first[i]->first[k]->name=NULL;
           (*datasetproj)[cnt]->first[i]->first[k]->value=(char **)calloc (1, sizeof(char *));
           (*datasetproj)[cnt]->first[i]->first[k]->value[0]=NULL; 
           return 0;
        }
    }
    return 0;
}

int get_metadata_solr(xmlDocPtr * doc, xmlNode * a_node, struct dataset_project ***datasetproj, int cnt, int j, int num, int query)
{
    xmlNode *cur_node = NULL;
    xmlChar *prop = NULL;
    char str[100];
    str[0]='\0';
    
    int res;
    int k=0;
   
    if(query==1)
    {
       sprintf(str, "//%s[@name='%s']",  (*datasetproj)[cnt]->first[j]->first[num]->occ, (*datasetproj)[cnt]->first[j]->first[num]->name);
       res=retrieve_tag(doc, str, datasetproj, cnt, j, num, query, num);
    }
    else
    {
       for(k=0; k<num; k++)
       {
          if(strcmp((*datasetproj)[cnt]->first[j]->first[k]->occ, "str")==0)
            sprintf(str, "//arr[@name='%s']", (*datasetproj)[cnt]->first[j]->first[k]->name);
          else
            sprintf(str, "//%s[@name='%s']", (*datasetproj)[cnt]->first[j]->first[k]->occ,(*datasetproj)[cnt]->first[j]->first[k]->name);
          res=retrieve_tag(doc, str, datasetproj, cnt, j, k, query, num);
       }
    }
  return res;
}

int alloca_struct_FtpFile(struct FtpFile **ftpfile, char** URL, char** id_query, char** flagid, int size)
{
    int res, cnt, j=0;

    for (cnt=0; cnt < size; cnt++)
    {
       ftpfile[cnt] = calloc (1, sizeof(struct FtpFile));

       if(!ftpfile[cnt])
       {
         //pmesg(LOG_ERROR, __FILE__, __LINE__,"Not enough memory. Error in calloc");
         for(j=cnt-1; j>=0; j--)
         {
	   if(ftpfile[j])
	     free(ftpfile[j]);
         }
         free(ftpfile);
         return ERROR_CALLOC;
       }
       ftpfile[cnt]->id_query=atoi(id_query[cnt]);
       ftpfile[cnt]->URL = strdup(URL[cnt]);
       ftpfile[cnt]->flag = strdup(flagid[cnt]);
       //printf("URL vale %s\n", URL[cnt]);
       if(!ftpfile[cnt]->URL)
       {
         //pmesg(LOG_ERROR, __FILE__, __LINE__,"Not enough memory. Error in strdup");
	 //fprintf(stderr, "\n[%s:%d] Not enough memory. Error in strdup()\n", __FILE__, __LINE__);
	 free_struct_FtpFile(ftpfile);
	 return STRDUP_ERROR;
       }
#if 0
       char *filename=NULL;
       char *str_1=NULL;

       if(URL[cnt])
       {
         str_1=strdup(URL[cnt]);
         //printf("str_1 vale %s\n", str_1);
         filename=strrchr(str_1, '/');
         //printf("filename vale %s\n", filename+1);
         free(str_1);
         
       }
        
       char *str=NULL;
       str=strrchr(filename+1, '*');
       if(str!=NULL)
	 *str='\0';
       str=strstr(filename+1, "id:");
       if(str!=NULL)
       {
         str_1=strrchr(str, '|');
         if(str_1!=NULL)
           *str_1='\0';
       } 
       struct timeval tv;
       gettimeofday(&tv,NULL);
       char str_2[100];
       str_2[0]='\0';
       if(str!=NULL)
       {
         sprintf(str_2, "%s_%ld", str+3, tv.tv_usec);
       }
       else
       {
         sprintf(str_2, "%s_%ld", filename+1, tv.tv_usec);           
       }
       //free(filename);
       filename=NULL;
       filename=strdup(str_2);
       printf("filename**** vale %s\n", filename);
       ftpfile[cnt]->filename = strdup(filename);
       free(filename);
       filename=NULL;
#endif
       char str_2[100]={'\0'};;
       struct timeval tv;
       gettimeofday(&tv,NULL);
       //sprintf(str_2, "file_%ld", tv.tv_usec);           
       sprintf(str_2, "%s.xml", id_query[cnt]);           
       ftpfile[cnt]->filename = strdup(str_2);

       if(!ftpfile[cnt]->filename)
       {
         //pmesg(LOG_ERROR, __FILE__, __LINE__,"Not enough memory. Error in strdup");
	 //fprintf(stderr, "\n[%s:%d] Not enough memory. Error in strdup()\n", __FILE__, __LINE__);
	 free_struct_FtpFile(ftpfile);
	 return STRDUP_ERROR;
       }
    }
    
    ftpfile[size] = calloc (1, sizeof(struct FtpFile));
    if(!ftpfile[size])
    {
       //pmesg(LOG_ERROR, __FILE__, __LINE__,"Not enough memory. Error in calloc");
       return ERROR_CALLOC;
    }
    ftpfile[size]->URL=NULL;
    ftpfile[size]->filename=NULL;
    return SUCCESS;
}
void free_struct_FtpFile(struct FtpFile **ftp)
{
	if(ftp!=NULL)
	{
		int size;
		for(size=0; ftp[size]!=NULL; size++)
		{
			if(ftp[size]->URL!=NULL)
                        {
				free(ftp[size]->URL);
                                ftp[size]->URL=NULL;
                        }
			if(ftp[size]->filename!=NULL)
                        {
				free(ftp[size]->filename);
                                ftp[size]->filename=NULL;
                        }
			free(ftp[size]);
                        ftp[size]=NULL;
		}
                free(ftp[size]);
                ftp[size]=NULL;
		free(ftp);
                ftp=NULL;
	}
} 
void free_datasetid(char **datasetid)
{
  if(datasetid!=NULL)
  {
    int size;
    for (size=0; datasetid[size]!=NULL; size++)
    {
      free(datasetid[size]);
      datasetid[size]=NULL;
    }
    free(datasetid);
    datasetid=NULL;
  }
}
void free_struct_metadata(struct metadata_entry **ftp)
{
	if(ftp!=NULL)
	{
		int size, size2=0;
	        for(size=0; ftp[size]!=NULL; size++)	
		{
			if(ftp[size]->occ!=NULL)
                        {
				free(ftp[size]->occ);
                                ftp[size]->occ=NULL;
                        }
			if(ftp[size]->name!=NULL)
                        {
				free(ftp[size]->name);
                                ftp[size]->name=NULL;
                        }
                        for(size2=0; ftp[size]->value[size2]!=NULL; size2++)
                        {
                                 if(ftp[size]->value[size2])
                                    free(ftp[size]->value[size2]);
                                 ftp[size]->value[size2]=NULL;
                        }
			if(ftp[size]->value!=NULL)
                        {
                                if(ftp[size]->value)
				  free(ftp[size]->value);
                                ftp[size]->value=NULL;
                        }
			free(ftp[size]);
                        ftp[size]=NULL;
		}
                free(ftp);
                ftp=NULL;
	}
}

void free_struct_datasetproj(struct dataset_project **datasetproj)
{
int cnt;
    for(cnt=0; datasetproj[cnt]!=NULL; cnt++)
    {
        int size, size2, size3, size4;
        for(size2=0; datasetproj[cnt]->first[size2]!=NULL; size2++)
        {
          //printf("project**** vale %s\n", datasetproj[cnt]->first[size2]->project);
          //printf("size 2 vale %d\n", size2);
          for(size3=0; datasetproj[cnt]->first[size2]->first[size3]!=NULL; size3++)
          {
              if(datasetproj[cnt]->first[size2]->first[size3]!=NULL)
              {
                //printf("size 3 vale %d\n", size3);
                if(datasetproj[cnt]->first[size2]->first[size3]->occ)
                  //printf("1 vale %s\n", datasetproj[cnt]->first[size2]->first[size3]->occ);
                if(datasetproj[cnt]->first[size2]->first[size3]->name)
                  //printf("2 vale %s\n", datasetproj[cnt]->first[size2]->first[size3]->name);
                for(size4=0; datasetproj[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                {
                   //printf("3 vale %s\n", datasetproj[cnt]->first[size2]->first[size3]->value[size4]);
                   free(datasetproj[cnt]->first[size2]->first[size3]->value[size4]);
                   datasetproj[cnt]->first[size2]->first[size3]->value[size4]=NULL;
                }
                free(datasetproj[cnt]->first[size2]->first[size3]->value[size4]);
                datasetproj[cnt]->first[size2]->first[size3]->value[size4]=NULL;
                free(datasetproj[cnt]->first[size2]->first[size3]->name);
                datasetproj[cnt]->first[size2]->first[size3]->name=NULL;
                free(datasetproj[cnt]->first[size2]->first[size3]->occ);
                datasetproj[cnt]->first[size2]->first[size3]->occ=NULL;
                free(datasetproj[cnt]->first[size2]->first[size3]->value);
                datasetproj[cnt]->first[size2]->first[size3]->value=NULL;
              }
              free(datasetproj[cnt]->first[size2]->first[size3]->value);
              datasetproj[cnt]->first[size2]->first[size3]->value=NULL;
              free(datasetproj[cnt]->first[size2]->first[size3]);
              datasetproj[cnt]->first[size2]->first[size3]=NULL;
              //printf("SIZE 3 con incremento vale %d\n", size3);
         }
         //printf("SIZE 3 vale %d\n", size3);
         free(datasetproj[cnt]->first[size2]->first[size3]);
         datasetproj[cnt]->first[size2]->first[size3]=NULL;
         free(datasetproj[cnt]->first[size2]->project);
         free(datasetproj[cnt]->first[size2]->first);
       }
       for(size2=0; datasetproj[cnt]->first[size2]!=NULL; size2++)
       {
          free(datasetproj[cnt]->first[size2]);
          datasetproj[cnt]->first[size2]=NULL;
       }
       free(datasetproj[cnt]->first[size2]);
       datasetproj[cnt]->first[size2]=NULL;

       free(datasetproj[cnt]->id);
       datasetproj[cnt]->id=NULL;
       free(datasetproj[cnt]->dataset_id);
       datasetproj[cnt]->dataset_id=NULL;
       free(datasetproj[cnt]->version);
       datasetproj[cnt]->version=NULL;
      
       free(datasetproj[cnt]->first);
       datasetproj[cnt]->first=NULL;
       free(datasetproj[cnt]);
       datasetproj[cnt]=NULL;
    }
       free(datasetproj[cnt]);
       datasetproj[cnt]=NULL;

    free(datasetproj);
    datasetproj=NULL;
#if 0
	if(datasetproj!=NULL)
	{
		int size, size2, size3, size4;
	        for(size=0; datasetproj[size]!=NULL; size++)	
		{
			if(datasetproj[size]->id!=NULL)
                        {
				free(datasetproj[size]->id);
                                datasetproj[size]->id=NULL;
                        }
			if(datasetproj[size]->dataset_id!=NULL)
                        {
				free(datasetproj[size]->dataset_id);
                                datasetproj[size]->dataset_id=NULL;
                        }
			if(datasetproj[size]->version!=NULL)
                        {
				free(datasetproj[size]->version);
                                datasetproj[size]->version=NULL;
                        }
                        for(size2=0; datasetproj[size]->first[size2]!=NULL; size2++)
                        {
                          for(size3=0; datasetproj[size]->first[size2]->first[size3]!=NULL; size3++)
                          {
                              if(datasetproj[size]->first[size2]->first[size3]!=NULL)
                              {
                                 if(datasetproj[size]->first[size2]->first[size3]->occ)
                                 {
                                   free(datasetproj[size]->first[size2]->first[size3]->occ);
                                   datasetproj[size]->first[size2]->first[size3]->occ=NULL;
                                 }
                                 if(datasetproj[size]->first[size2]->first[size3]->name)
                                 {
                                   printf("nome del metadato da free %s\n", datasetproj[size]->first[size2]->first[size3]->name);
                                   free(datasetproj[size]->first[size2]->first[size3]->name);
                                   datasetproj[size]->first[size2]->first[size3]->name=NULL;
                                 }
                                 for(size4=0; datasetproj[size]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                                 {
                                   if(datasetproj[size]->first[size2]->first[size3]->value[size4])
                                   {
                                     free(datasetproj[size]->first[size2]->first[size3]->value[size4]);
                                     datasetproj[size]->first[size2]->first[size3]->value[size4]=NULL;
                                   }
                                 }
                                 free(datasetproj[size]->first[size2]->first[size3]);
                                 datasetproj[size]->first[size2]->first[size3]=NULL;
                              }
                              free(datasetproj[size]->first[size2]->first);
                              datasetproj[size]->first[size2]->first=NULL;
                          } 
                          if(datasetproj[size]->first[size2])
                          {
                            free(datasetproj[size]->first[size2]);
                            datasetproj[size]->first[size2]=NULL;
                          }
                                //datasetproj[size]->project[size2]=NULL;
                        }
                        free(datasetproj[size]->first);
			free(datasetproj[size]);
                        datasetproj[size]=NULL;
		}
                free(datasetproj);
                datasetproj=NULL;
	}
#endif
}
void myfree_array(char **arr, int size1)
{
   if(arr!=NULL)
   {
     int size;
     //for(size=0; size<size1; size++)
     for(size=0; arr[size]!=NULL; size++)
     {
           free(arr[size]);
           arr[size]=NULL;
     }
     free(arr);
     arr=NULL;
   }
}

int read_conf_project(char *filename_conf, struct dataset_project ***datasetproj, int cnt)
{
//read conf file
    int i=0;
    int res=0;
    xmlDocPtr doc;
    xmlNode *root_element = NULL;
    /*parse the file and get the DOM */
    doc = xmlReadFile(filename_conf, NULL, 0);
    if (doc == NULL)
    {
      fprintf(stderr, "\n[%s:%d] Error: could not parse file %s\n", __FILE__, __LINE__, filename_conf);
      return NULL;
    }

    
    //printf("num_metadata vale %d\n", num_metadata);
    
    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    //struct metadata_entry **metadata=calloc(num_metadata+1,sizeof(struct metadata_entry *));
    for(i=0; (*datasetproj)[cnt]->first[i]!=NULL; i++)
    {
      //printf("leggo file di configurazione con progetto %s\n", (*datasetproj)[cnt]->first[i]->project);
      int num_metadata=count_metadata(doc,"/projects/project", (*datasetproj)[cnt]->first[i]->project);
      //printf("num_metadata vale %d\n", num_metadata);
      (*datasetproj)[cnt]->first[i]->size=num_metadata;
      if(num_metadata>0)
      {
        (*datasetproj)[cnt]->first[i]->first=(struct metadata_entry**)calloc (num_metadata+2, sizeof(struct metadata_entry *));
        res = read_conf_file(root_element, (*datasetproj)[cnt]->first[i]->project, datasetproj, num_metadata, cnt, i);

        if(res!=SUCCESS)
        {
          //free_struct_metadata(metadata);
          return res;
        }
      }
      else
      {
        (*datasetproj)[cnt]->first[i]->first=(struct metadata_entry**)calloc (1, sizeof(struct metadata_entry *));
        (*datasetproj)[cnt]->first[i]->first[0]=NULL;
      }
   }
   //(*datasetproj)[cnt]->first[i]->first[0]=NULL;
#if 0
      int size1,size2;
      for(size1=0; datasetproj[size1]->first!=NULL; size1++)
      {
        for(size2=0; datasetproj[size1]->first[size2]!=NULL; size2++)
        {
          if(datasetproj[size1]->first[size2]->occ)
          {
            printf("size 2 vale %d\n", size2);
            printf("1 vale %s\n", datasetproj[size1]->first[size2]->occ);
            printf("2 vale %s\n", datasetproj[size1]->first[size2]->name);
          }
       }
     }
#endif
/* devo chiamare dopo*/
#if 0
    int size1;
    for(size1=0; metadata[size1]!=NULL; size1++)
    {
        
        printf("1 vale %s\n", metadata[size1]->occ);
        printf("2 vale %s\n", metadata[size1]->name);
        printf("3 vale %s, size vale %d\n", metadata[size1]->value, size1);
    }

    
    free_struct_metadata(metadata);
#endif

    xmlFreeDoc(doc);
    xmlCleanupParser();
    xmlMemoryDump();
//fine_lettura_file_conf
}
int read_shards(char *shardsName)
{
    int i=0;
    int *res=0;
    xmlDocPtr doc;
    xmlNode *root_element = NULL;
    /*parse the file and get the DOM */
    int size=getFilesize(shardsName); 
    if(size==0)
    {
      return -1;
    }

    doc = xmlReadFile(shardsName, NULL, 0);
    if (doc == NULL)
    {
      fprintf(stderr, "\n[%s:%d] Error: could not parse file %s\n", __FILE__, __LINE__, shardsName);
      return -1;
    }

    
    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    xmlNode *cur_node = NULL;
    xmlNode *a_node = NULL;
    xmlChar *prop = NULL;
    print_element(root_element,&res);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    xmlMemoryDump();
    return res;
}
void print_element(xmlNode * a_node, int *res)
{
    xmlNode *cur_node = NULL;
    xmlChar *prop = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
	  if(xmlStrcasecmp(cur_node->name, (xmlChar *)"str")==0) {
	      prop = xmlGetProp(cur_node, (xmlChar *)"name");
	      if(prop)
	      {
		if(xmlStrcasecmp(prop, (xmlChar *)"shards")==0)
		{
                     xmlChar* content=xmlNodeGetContent(cur_node);
                     char *tmp_str=strstr(content, "8982");
                     if(tmp_str)
                     {
                        //printf("content %s\n", content);
                        *res=1;
                     }
                     //else
                        //printf("content %s\n", "no found");
                }
              }
              xmlFree(prop);
           }
        }
        print_element(cur_node->children, res);
   }
}
int get_replica(xmlDocPtr doc,xmlNode *root_element)
{
    int *res=0;

    xmlNode *cur_node = NULL;
    xmlNode *a_node = NULL;
    xmlChar *prop = NULL;
    print_element_replica(root_element,&res);
    return res;
}
void print_element_replica(xmlNode * a_node, int *res)
{
    xmlNode *cur_node = NULL;
    xmlChar *prop = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
	  if(xmlStrcasecmp(cur_node->name, (xmlChar *)"bool")==0) {
	      prop = xmlGetProp(cur_node, (xmlChar *)"name");
	      if(prop)
	      {
		if(xmlStrcasecmp(prop, (xmlChar *)"replica")==0)
		{
                     xmlChar* content=xmlNodeGetContent(cur_node);
                     char *tmp_str=strstr(content, "false");
                     if(tmp_str)
                     {
                        //printf("content %s\n", content);
                        *res=1;
                     }
                     //else
                        //printf("content %s\n", "no found");
                }
              }
              xmlFree(prop);
           }
        }
        print_element_replica(cur_node->children, res);
   }
}
int read_config_feder(char *configName)
{
    int i=0;
    int res=0;
    xmlDocPtr doc;
    xmlNode *root_element = NULL;
    /*parse the file and get the DOM */
    doc = xmlReadFile(configName, NULL, 0);
    if (doc == NULL)
    {
      fprintf(stderr, "\n[%s:%d] Error: could not parse file %s\n", __FILE__, __LINE__, configName);
      return NULL;
    }

    
    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    xmlNode *cur_node = NULL;
    xmlNode *a_node = NULL;
    xmlChar *prop = NULL;
    char ip[128] = { '\0' };
    int port=0;
    char datanode[128] = { '\0' };
    delete_element_config_feder(root_element,res, ip, port, datanode);
    print_element_config_feder(root_element,res, ip, port);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    xmlMemoryDump();
    return res;
}
void print_element_config_feder(xmlNode * a_node, int res, char *ip, int port)
{
    xmlNode *cur_node = NULL;
    xmlChar *prop = NULL;
    

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
          if(xmlStrcasecmp(cur_node->name, (xmlChar *)"node")==0) {
              prop = xmlGetProp(cur_node, (xmlChar *)"ipNodeAddress");
              if(prop)
              {
                  //if(ip)
                    //free(ip);
                 //ip=NULL;
                 //ip=strdup(prop);
                 sprintf(ip, "%s", prop);               
                 //printf("content %s\n", prop);
              }
              xmlFree(prop);
              prop = xmlGetProp(cur_node, (xmlChar *)"port");
              if(prop)
              {
                 port=atoi(prop);
              }
              xmlFree(prop);
           }
          if(xmlStrcasecmp(cur_node->name, (xmlChar *)"datamart")==0) {
              prop = xmlGetProp(cur_node, (xmlChar *)"path");
              if(prop)
              {
                char buff[1024] = { 0 };
                sprintf(buff, "%s/%s_%d", FED_DIR, ip, res);
                get_download_federated(FED_DIR, buff, ip, prop, port); 
              }
              xmlFree(prop);
           }
           
        }
        res++;
        print_element_config_feder(cur_node->children, res, ip, port);
   }
}
int read_dmart_feder(char *dmartName)
{
    int i=0;
    int res=0;
    xmlDocPtr doc;
    xmlNode *root_element = NULL;
    /*parse the file and get the DOM */
    doc = xmlReadFile(dmartName, NULL, 0);
    if (doc == NULL)
    {
      fprintf(stderr, "\n[%s:%d] Error: could not parse file %s\n", __FILE__, __LINE__, dmartName);
      return -1;
    }

    
    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    xmlNode *cur_node = NULL;
    xmlNode *a_node = NULL;
    xmlChar *prop = NULL;
    char *tableName=NULL;

    PGconn * conn;
    PGresult *res2;
    char conninfo[1024] = {'\0'};
    char query[2048] = {'\0'};

    /* Connect to database */
    snprintf (conninfo, sizeof (conninfo), "host=%s port=%d dbname=%s user=%s password=%s", POSTGRES_HOST, POSTGRES_PORT_NUMBER,POSTGRES_DB_NAME, POSTGRES_USER,POSTGRES_PASSWD);
    conn = PQconnectdb ((const char *) conninfo);

    if (PQstatus(conn) != CONNECTION_OK)
    {
       pmesg(LOG_ERROR,__FILE__,__LINE__,"Connection to database failed: %s", PQerrorMessage(conn));
       PQfinish(conn);
       return -1;
     }
     res2 = PQexec(conn, QUERY8);
     if ((!res2) || (PQresultStatus (res2) != PGRES_COMMAND_OK))
     {
        pmesg(LOG_ERROR,__FILE__,__LINE__,"Open transaction failed\n");
        PQclear(res2);
        PQfinish(conn);
        return -2;
      }

#if 0
      sprintf(query, "%s", "delete from esgf_dashboard.cross_dmart_project_host_time; delete from esgf_dashboard.cross_dmart_project_host_geolocation; delete from esgf_dashboard.obs4mips_dmart_clients_host_time_geolocation; delete from esgf_dashboard.obs4mips_dmart_variable_host_time; delete from esgf_dashboard.obs4mips_dmart_source_host_time; delete from esgf_dashboard.obs4mips_dmart_realm_host_time; delete from esgf_dashboard.obs4mips_dmart_dataset_host_time; delete from esgf_dashboard.cmip5_dmart_experiment_host_time; delete from esgf_dashboard.cmip5_dmart_model_host_time; delete from esgf_dashboard.cmip5_dmart_variable_host_time; delete from esgf_dashboard.cmip5_dmart_dataset_host_time; delete from esgf_dashboard.cmip5_dmart_clients_host_time_geolocation;");

      res2 = PQexec(conn, query);
      if ((!res2) || (PQresultStatus (res2) != PGRES_COMMAND_OK))
      {
        pmesg(LOG_ERROR,__FILE__,__LINE__,"Open transaction failed\n");
        PQclear(res2);
        PQfinish(conn);
        return -2;
       }
       PQclear(res2);
#endif

       print_element_dmart_feder_names(root_element, tableName, conn);
       res2 = PQexec(conn, QUERY4);
       pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying to close the transaction\n");
       pmesg(LOG_DEBUG,__FILE__,__LINE__,"Query: [%s]\n",QUERY4);
       if ((!res2) || (PQresultStatus (res2) != PGRES_COMMAND_OK))
       {
         pmesg(LOG_ERROR,__FILE__,__LINE__,"Close transaction failed\n");
         PQclear(res2);
         PQfinish(conn);
         return -2;
        }
        PQclear(res2);

    PQfinish(conn);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    xmlMemoryDump();
    return res;
}
void print_element_dmart_feder_names(xmlNode * a_node, char *tableName, PGconn * conn2)
{
    xmlNode *cur_node = NULL;
    struct table_dmart datasetproj;
    int num_metadata=0;
    int cnt=0;
    int i=0;
    int kind_dmart=0; 
    char **fields=NULL;
    char **values=NULL;
    fields=(char **)calloc (2, sizeof(char*));
    values=(char **)calloc (2, sizeof(char*));
    xmlChar *prop=NULL; 
    char query[2048]={'\0'};
//0 - cmip5_dmart_experiment_host_time; 1 - cmip5_dmart_model_host_time; 2 - cmip5_dmart_variable_host_time; 3 - esgf_dashboard.cmip5_dmart_dataset_host_time; 4 - cross_dmart_project_host_time; 5 - cross_dmart_project_host_geolocation; 6 - cmip5_dmart_clients_host_time_geolocation; 7 - obs4mips_dmart_clients_host_time_geolocation; 8 - obs4mips_dmart_variable_host_time; 9 - obs4mips_dmart_source_host_time; 10 - obs4mips_dmart_realm_host_time; 11 - obs4mips_dmart_dataset_host_time; 12 - cross_dmart_isenes_kpis;   
                            PGresult *res2;
                            PGresult *res3;
    
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if(xmlStrcasecmp(cur_node->name, (xmlChar *)"table")==0)
	    {
               prop = xmlGetProp(cur_node, (xmlChar *)"name");
               if(prop)
	       {
                 tableName=strdup(prop);
               }
               xmlFree(prop);
            }
            if(xmlStrcasecmp(cur_node->name, (xmlChar *)"row")==0)
            {
                    //printf("******* %s**********\n","row");
                    read_elem_fed(cur_node->children, tableName, fields, values);
                    char *str_c=strrchr(fields[0],',');
                    if (str_c)
                      *str_c=0;
                    str_c=strrchr(values[0],',');
                    if (str_c)
                      *str_c=0;
                    
                    //printf("****fields %s\n****",fields[0]);
                    //printf("****values %s\n****",values[0]);
                    //if(strcmp(tableName, "obs4mips_dmart_clients_host_time_geolocation")==0)
                    //{

                            /* Connect to database */

                            res2 = PQexec(conn2, QUERY8);
                            if ((!res2) || (PQresultStatus (res2) != PGRES_COMMAND_OK))
                            {
                              pmesg(LOG_ERROR,__FILE__,__LINE__,"Open transaction failed\n");
                              PQclear(res2);
                              free(fields[0]);
                              free(values[0]);
                              fields[0]=NULL;
                              values[0]=NULL;
                              free(fields); 
                              free(values);
                              return -2;
                            }
                            PQclear(res2);

#if 0
                      if(strcmp(tableName, "cmip5_dmart_experiment_host_time")==0)
                      {
                         char str_tmp[2048]={'\0'};
                         sprintf(str_tmp, "%s", "total_size, number_of_downloads,number_of_successful_downloads,average_duration, number_of_users, number_of_replica_downloads, month, year, experiment_name, host_name");
                         free(fields[0]);
                         fields[0]=NULL;
                         fields[0]=strdup(str_tmp);
                      }
#endif
                 char str_tmp[2048]={'\0'};
                 if(strcmp(tableName, "all_data_usage")==0)
                 {
                   free(fields[0]);
                   fields[0]=NULL;
                   sprintf(str_tmp, "%s", "gb, downloads,files,users, month, year, host");
                   fields[0]=strdup(str_tmp);
                 }
                 if(strcmp(tableName, "all_data_usage_continent")==0)
                 {
                   free(fields[0]);
                   fields[0]=NULL;
                   sprintf(str_tmp, "%s", "gb, downloads,files,users, month, year, continent, host");
                   fields[0]=strdup(str_tmp);
                 }
                 if(strcmp(tableName, "obs4mips_data_usage")==0)
                 {
                   free(fields[0]);
                   fields[0]=NULL;
                   sprintf(str_tmp, "%s", "gb, downloads,files,users, month, year,host");
                   fields[0]=strdup(str_tmp);
                 }
                 if(strcmp(tableName, "obs4mips_data_usage_continent")==0)
                 {
                   free(fields[0]);
                   fields[0]=NULL;
                   sprintf(str_tmp, "%s", "gb, downloads,files,users, month, year,continent,host");
                   fields[0]=strdup(str_tmp);
                 }
                 if(strcmp(tableName, "cmip5_data_usage")==0)
                 {
                   free(fields[0]);
                   fields[0]=NULL;
                   sprintf(str_tmp, "%s", "gb, downloads,files,users, month, year,host");
                   fields[0]=strdup(str_tmp);
                 }
                 if(strcmp(tableName, "cmip5_data_usage_continent")==0)
                 {
                   free(fields[0]);
                   fields[0]=NULL;
                   sprintf(str_tmp, "%s", "gb, downloads,files,users, month, year,continent,host");
                   fields[0]=strdup(str_tmp);
                 }
                 if(strcmp(tableName, "cordex_data_usage")==0)
                 {
                   free(fields[0]);
                   fields[0]=NULL;
                   sprintf(str_tmp, "%s", "gb, downloads,files,users, month, year,host");
                   fields[0]=strdup(str_tmp);
                 }
                 if(strcmp(tableName, "cordex_data_usage_continent")==0)
                 {
                   free(fields[0]);
                   fields[0]=NULL;
                   sprintf(str_tmp, "%s", "gb, downloads,files,users, month, year,continent,host");
                   fields[0]=strdup(str_tmp);
                 }
                 sprintf(query, "INSERT INTO esgf_dashboard.%s(%s)values(%s);", tableName, fields[0], values[0]);
                      //printf("query vale %s\n", query);
                              res3 = PQexec(conn2, query);
                            if ((!res3) || (PQresultStatus (res3) != PGRES_COMMAND_OK))
                            {
                              pmesg(LOG_ERROR,__FILE__,__LINE__,"Failed query\n");
                              //printf("******++++%s++++********\n", "Duplicated row");
                              PQclear(res3);
                              free(fields[0]);
                              free(values[0]);
                              fields[0]=NULL;
                              values[0]=NULL;
                              free(fields); 
                              free(values);
                              return -2;
                            }
                            PQclear(res3);
                            res2 = PQexec(conn2, QUERY4);
                            pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying to close the transaction\n");
                            pmesg(LOG_DEBUG,__FILE__,__LINE__,"Query: [%s]\n",QUERY4);
                            if ((!res2) || (PQresultStatus (res2) != PGRES_COMMAND_OK))
                            {
                              pmesg(LOG_ERROR,__FILE__,__LINE__,"Close transaction failed\n");
                              //printf("******++++%s++++********\n", "Close transaction failed");
                              PQclear(res2);
                              free(fields[0]);
                              free(values[0]);
                              fields[0]=NULL;
                              values[0]=NULL;
                              free(fields); 
                              free(values);
                              return -2;
                             }
                             PQclear(res2);
                    //}      
                    free(fields[0]);
                    free(values[0]);
                    fields[0]=NULL;
                    values[0]=NULL;
                    free(fields); 
                    free(values);
                        
            }      
		  //if(xmlStrcasecmp(prop, (xmlChar *)"obs4mips_dmart_clients_host_time_geolocation")==0)
	          //{
                    //sprintf(query, "INSERT INTO esgf_dashboard.%s(total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, month, year, latitude, longitude, host_name) values (%d, %d, %d, %d, %d, '%s', '%s', %14.11f, %14.11f, '%s');",  
                  //}
               
                 //printf("****%s****\n", cur_node->name);
            //if(xmlStrcasecmp(cur_node->name, (xmlChar *)"row")==0)
            //{
            //   printf("******* %s**********\n","row");
            //   read_elem_fed(cur_node->children, tableName, datasetproj, num_metadata, cnt, i);
            //}
        }
        print_element_dmart_feder_names(cur_node->children, tableName, conn2);
    }
}

#if 0
int print_element_dmart_feder(xmlNode * a_node, int res)
{
    xmlNode *cur_node = NULL;
    xmlChar *prop = NULL;
    xmlChar *prop1 = NULL;
    
    int prj=0;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE){
	if(xmlStrcasecmp(cur_node->name, (xmlChar *)"table")==0) {
	    prop = xmlGetProp(cur_node, (xmlChar *)"name");
	    if(prop)
	    {
		if(xmlStrcasecmp(prop, (xmlChar *)"obs4mips_dmart_clients_host_time_geolocation")==0)
		{
                   a_node=cur_node->children;
                   for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
                      if(xmlStrcasecmp(cur_node->name, (xmlChar *)"field")==0)
                      {
                         prop1 = xmlGetProp(cur_node, (xmlChar *)"name");
                         if(prop1)
                         {
                            xmlChar* content=xmlNodeGetContent(cur_node);
                            printf("content %s\n", content);
                            xmlFree(content);
                            content=NULL;
                         }
                         xmlFree(prop1);
                      }
                   }
                 }

            }
            xmlFree(prop);
	  }
        }
    }
    return 0;
}
#endif
int getFilesize(const char* filename) {
    struct stat st;
    if(stat(filename, &st) != 0) {
        return 0;
    }
    return st.st_size;   
}
int read_elem_fed(xmlNode * a_node, char *tableName, char **fields, char **values)
{
    xmlNode *cur_node = NULL;
    xmlChar *prop = NULL;
    xmlChar *prop1 = NULL;
    
    int res, prj=0;
    int k=0;
    char str_field[2056] = { '\0' };
    char str_value[2056] = { '\0' };
    strcpy(str_field, "");
    strcpy(str_value, "");

                        for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
                          if(xmlStrcasecmp(cur_node->name, (xmlChar *)"field")==0)
                          {
                            prop1 = xmlGetProp(cur_node, (xmlChar *)"name");
                            if(prop1)
                            {
                               xmlChar* content=xmlNodeGetContent(cur_node);
                               //printf("name vale %s\n", prop1);
                               strcat(str_field, prop1);
                               strcat(str_field, ",");
                               //printf("value vale %s\n", content);
                               if(strcmp(tableName, "obs4mips_dmart_clients_host_time_geolocation")==0)
                               {
                                  if(k==5)
                                  {
                                    strcat(str_value, "'");
                                    strcat(str_value, content);
                                    strcat(str_value, "'");
                                    strcat(str_value, ",");
                                  }
                                  else
                                  {
                                    strcat(str_value, content);
                                    strcat(str_value, ",");
                                  }
                               }
                               if(strcmp(tableName, "cross_dmart_project_host_time")==0)
                               {
                                  if((k==6)||(k==7))
                                  {
                                    strcat(str_value, "'");
                                    strcat(str_value, content);
                                    strcat(str_value, "'");
                                    strcat(str_value, ",");
                                  }
                                  else
                                  {
                                    strcat(str_value, content);
                                    strcat(str_value, ",");
                                  }
                               }
                               if(strcmp(tableName, "cross_dmart_project_host_geolocation")==0)
                               {
                                  if((k==6)||(k==7))
                                  {
                                    strcat(str_value, "'");
                                    strcat(str_value, content);
                                    strcat(str_value, "'");
                                    strcat(str_value, ",");
                                  }
                                  else
                                  {
                                    strcat(str_value, content);
                                    strcat(str_value, ",");
                                  }
                               }
                               if((strcmp(tableName, "all_data_usage_continent")==0)||(strcmp(tableName, "cmip5_data_usage_continent")==0)||(strcmp(tableName, "obs4mips_data_usage_continent")==0)||(strcmp(tableName, "cordex_data_usage_continent")==0))
                               {
                                  if((k==6)||(k==7))
                                  {
                                    strcat(str_value, "'");
                                    strcat(str_value, content);
                                    strcat(str_value, "'");
                                    strcat(str_value, ",");
                                  }
                                  else
                                  {
                                    strcat(str_value, content);
                                    strcat(str_value, ",");
                                  }
                               }
                               if((strcmp(tableName, "all_data_usage")==0)||(strcmp(tableName, "cmip5_data_usage")==0)||(strcmp(tableName, "obs4mips_data_usage")==0)||(strcmp(tableName, "cordex_data_usage")==0))
                               {
                                  if(k==6)
                                  {
                                    strcat(str_value, "'");
                                    strcat(str_value, content);
                                    strcat(str_value, "'");
                                    strcat(str_value, ",");
                                  }
                                  else
                                  {
                                    strcat(str_value, content);
                                    strcat(str_value, ",");
                                  }
                               }
                               if(strcmp(tableName, "obs4mips_dmart_variable_host_time")==0)
                               {
                                  if((k==5)||(k==8)||(k==9)||(k==10))
                                  {
                                    strcat(str_value, "'");
                                    strcat(str_value, content);
                                    strcat(str_value, "'");
                                    strcat(str_value, ",");
                                  }
                                  else
                                  {
                                    strcat(str_value, content);
                                    strcat(str_value, ",");
                                  }
                               }
                               if(strcmp(tableName, "obs4mips_dmart_source_host_time")==0)
                               {
                                  if((k==5)||(k==8))
                                  {
                                    strcat(str_value, "'");
                                    strcat(str_value, content);
                                    strcat(str_value, "'");
                                    strcat(str_value, ",");
                                  }
                                  else
                                  {
                                    strcat(str_value, content);
                                    strcat(str_value, ",");
                                  }
                               }
                               if(strcmp(tableName, "obs4mips_dmart_realm_host_time")==0)
                               {
                                  if((k==5)||(k==8))
                                  {
                                    strcat(str_value, "'");
                                    strcat(str_value, content);
                                    strcat(str_value, "'");
                                    strcat(str_value, ",");
                                  }
                                  else
                                  {
                                    strcat(str_value, content);
                                    strcat(str_value, ",");
                                  }
                               }
                               if(strcmp(tableName, "obs4mips_dmart_dataset_host_time")==0)
                               {
                                  if((k==5)||(k==8)||(k==10)||(k==11))
                                  {
                                    strcat(str_value, "'");
                                    strcat(str_value, content);
                                    strcat(str_value, "'");
                                    strcat(str_value, ",");
                                  }
                                  else
                                  {
                                    strcat(str_value, content);
                                    strcat(str_value, ",");
                                  }
                               }
                               if(strcmp(tableName, "cmip5_dmart_experiment_host_time")==0)
                               {
                                  if((k==8)||(k==9))
                                  {
                                    strcat(str_value, "'");
                                    strcat(str_value, content);
                                    strcat(str_value, "'");
                                    strcat(str_value, ",");
                                  }
                                  else
                                  {
                                    strcat(str_value, content);
                                    strcat(str_value, ",");
                                  }
                               }
                               if(strcmp(tableName, "cmip5_dmart_model_host_time")==0)
                               {
                                  if((k==8)||(k==9))
                                  {
                                    strcat(str_value, "'");
                                    strcat(str_value, content);
                                    strcat(str_value, "'");
                                    strcat(str_value, ",");
                                  }
                                  else
                                  {
                                    strcat(str_value, content);
                                    strcat(str_value, ",");
                                  }
                               }
                               if(strcmp(tableName, "cmip5_dmart_variable_host_time")==0)
                               {
                                  if((k==8)||(k==9)||(k==10)||(k==11))
                                  {
                                    strcat(str_value, "'");
                                    strcat(str_value, content);
                                    strcat(str_value, "'");
                                    strcat(str_value, ",");
                                  }
                                  else
                                  {
                                    strcat(str_value, content);
                                    strcat(str_value, ",");
                                  }
                               }
                               if(strcmp(tableName, "cmip5_dmart_dataset_host_time")==0)
                               {
                                  if((k==8)||(k==9)||(k==11)||(k==12))
                                  {
                                    strcat(str_value, "'");
                                    strcat(str_value, content);
                                    strcat(str_value, "'");
                                    strcat(str_value, ",");
                                  }
                                  else
                                  {
                                    strcat(str_value, content);
                                    strcat(str_value, ",");
                                  }
                               }
                               if(strcmp(tableName, "cmip5_dmart_clients_host_time_geolocation")==0)
                               {
                                  if(k==8)
                                  {
                                    strcat(str_value, "'");
                                    strcat(str_value, content);
                                    strcat(str_value, "'");
                                    strcat(str_value, ",");
                                  }
                                  else
                                  {
                                    strcat(str_value, content);
                                    strcat(str_value, ",");
                                  }
                               }
                             
                               xmlFree(content);
                               content=NULL;
                               k++;
                            }
                            xmlFree(prop1);
                         }
                      }
                      fields[0]=strdup(str_field);
                      fields[1]=NULL;
                      values[0]=strdup(str_value);
                      values[1]=NULL;

    return 0;
 //}
 }
void delete_element_config_feder(xmlNode * a_node, int res, char *ip, int port, char *datanode)
{
    xmlNode *cur_node = NULL;
    xmlChar *prop = NULL;
    //int res1=0;   

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
          if(xmlStrcasecmp(cur_node->name, (xmlChar *)"node")==0) {
              prop = xmlGetProp(cur_node, (xmlChar *)"ipNodeAddress");
              if(prop)
              {
                  //if(ip)
                    //free(ip);
                 //ip=NULL;
                 //ip=strdup(prop);    
                 sprintf(ip, "%s", prop);           
                 //printf("content %s\n", prop);
              }
              xmlFree(prop);
              prop = xmlGetProp(cur_node, (xmlChar *)"port");
              if(prop)
              {
                 port=atoi(prop);
              }
              xmlFree(prop);
              prop = xmlGetProp(cur_node, (xmlChar *)"datanode");
              if(prop)
              {
                 //if(datanode)
                 //  free(datanode);
                 //datanode=NULL;
                 //datanode=strdup(prop);
                 sprintf(datanode, "%s", prop);           
              }
              xmlFree(prop);
           }
          if(xmlStrcasecmp(cur_node->name, (xmlChar *)"datamart")==0) {
              prop = xmlGetProp(cur_node, (xmlChar *)"name");
              if(prop)
              {
                //printf( "hostname=%s, datamart=%s datanode=%s\n", ip, prop,datanode);
                if(strcmp(ip, datanode)==0)
                   res=delete_federated(ip, prop);
                else
                {
                  res=delete_federated(ip, prop);
                  //printf("query delete with hostname=%s, datamart=%s\n", ip, prop); 
                  res=delete_federated(datanode, prop);
                  //printf("query delete with datanode=%s, datamart=%s\n", datanode, prop); 
                }
              }
              xmlFree(prop);
           }
           
        }
        res++;
        delete_element_config_feder(cur_node->children, res, ip, port, datanode);
   }
}
int delete_federated(char *hostname, char *datamart)
{
    PGconn * conn;
    PGresult *res2;
    char conninfo[1024] = {'\0'};
    char query[2048] = {'\0'};

    /* Connect to database */
    snprintf (conninfo, sizeof (conninfo), "host=%s port=%d dbname=%s user=%s password=%s", POSTGRES_HOST, POSTGRES_PORT_NUMBER,POSTGRES_DB_NAME, POSTGRES_USER,POSTGRES_PASSWD);
    conn = PQconnectdb ((const char *) conninfo);

    if (PQstatus(conn) != CONNECTION_OK)
    {
       pmesg(LOG_ERROR,__FILE__,__LINE__,"Connection to database failed: %s", PQerrorMessage(conn));
       PQfinish(conn);
       return -1;
     }
     res2 = PQexec(conn, QUERY8);
     if ((!res2) || (PQresultStatus (res2) != PGRES_COMMAND_OK))
     {
        pmesg(LOG_ERROR,__FILE__,__LINE__,"Open transaction failed\n");
        PQclear(res2);
        PQfinish(conn);
        return -2;
     }
     sprintf(query, "delete from esgf_dashboard.%s where host_name='%s';", datamart, hostname);
     
      //printf("query vale %s\n", query);
      res2 = PQexec(conn, query);
      if ((!res2) || (PQresultStatus (res2) != PGRES_COMMAND_OK))
      {
        pmesg(LOG_ERROR,__FILE__,__LINE__,"Open transaction failed\n");
        PQclear(res2);
        PQfinish(conn);
        return -2;
       }
       PQclear(res2);

       res2 = PQexec(conn, QUERY4);
       pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying to close the transaction\n");
       pmesg(LOG_DEBUG,__FILE__,__LINE__,"Query: [%s]\n",QUERY4);
       if ((!res2) || (PQresultStatus (res2) != PGRES_COMMAND_OK))
       {
         pmesg(LOG_ERROR,__FILE__,__LINE__,"Close transaction failed\n");
         PQclear(res2);
         PQfinish(conn);
         return -2;
        }
        PQclear(res2);
    PQfinish(conn);
    return 0;
}
