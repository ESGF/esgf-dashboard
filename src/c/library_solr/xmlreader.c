/**
 * section: Tree
 * synopsis: Navigates a tree to print element names
 * purpose: Parse a file to a tree, use xmlDocGetRootElement() to
 *          get the root element, then walk the document and print
 *          all the element name in document order.
 * usage: tree1 filename_or_URL
 * test: tree1 test2.xml > tree1.tmp ; diff tree1.tmp tree1.res ; rm tree1.tmp
 * author: Dodji Seketeli
 * copy: see Copyright for the status of this software.
 */
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
#include <libpq-fe.h>
#define BUFSIZE 1024

/*
 *To compile this file using gcc you can type
 *gcc `xml2-config --cflags --libs` -o xmlexample libxml2-example.c
 */

// legge il file indicato, nella directory indicata,
// e copia il valore di execution_cmd nella stringa cmd_name
// ritona un valore negativo e cmd_name = "" se l'operazione fallisce

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
			printf("num_children %d\n",num_children);
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
              if(xmlStrcasecmp(prop, (xmlChar *)"dataset_id")==0)
              {
                prop1 = xmlNodeGetContent(cur_node);
                (*datasetproj)[cnt]->id_query=atoi(id_query[cnt]);
                (*datasetproj)[cnt]->dataset_id=strdup(prop1); 
                char *tmp_str=strstr(prop1, "|");
                if(tmp_str)
                {
                   *tmp_str = 0; 
                }
                char *tmp_str1=strstr(prop1, ".v");
                if(tmp_str1)
                  (*datasetproj)[cnt]->version=strdup(tmp_str1+2);
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

static void
exit_nicely(PGconn *conn)
{
    PQfinish(conn);
    exit(1);
}

static void usage (char *name)
{
  printf ("%s: Usage:\n", name);
  printf ("\t nome del file xml, nome del database, operazione (0 per download ftp full release, 1 per update)\n");
}

/**
 * Simple example to parse a file called "file.xml", 
 * walk down the DOM, and print the name of the 
 * xml elements nodes.
 */
int
main(int argc, char **argv)
{

    const char *conninfo;
    PGconn     *conn;
    PGresult   *res1;
    int         nFields;
    char **datasetid=NULL;
    char **queryid=NULL;
    char **URL=NULL;
    char **id_query=NULL;
    int res, cnt, j=0;
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL, *root_children = NULL;
    char str_url[256];
    str_url[0]='\0';
  
    int size=0;
    int i=0;
    //char *projectName="obs4MIPs"; 
    char *projectName=NULL; 
    //char *esgf_node="esgf-test.cmcc.it";
    char *esgf_node="esgf-node.jpl.nasa.gov";

    char *filename_conf="conf/conf.xml";
    LIBXML_TEST_VERSION

    conninfo = "dbname = esgcet2 user=dbsuper password=abcdef";

while(1)
{

/* Make a connection to the database */
    conn = PQconnectdb(conninfo);

    /* Check to see that the backend connection was successfully made */
    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s",
                PQerrorMessage(conn));
        exit_nicely(conn);
    }

    /*
     * Should PQclear PGresult whenever it is no longer needed to avoid memory
     * leaks
     */

    /*
     * Fetch rows from pg_database, the system catalog of databases
     */
    //res1 = PQexec(conn, "select \"url_path\",\"id\" from esgf_dashboard.dashboard_queue;");
    res1 = PQexec(conn, "select url_path,id from esgf_dashboard.dashboard_queue where processed=0 limit 10;");
    if (PQresultStatus(res1) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "DECLARE CURSOR failed: %s", PQerrorMessage(conn));
        PQclear(res1);
        exit_nicely(conn);
    }

/* first, print out the attribute names */
    nFields = PQnfields(res1);
    size=PQntuples(res1);
    printf("size vale %d\n", size);
    printf("nFields %d\n", nFields);
    URL=(char**) calloc (size+1, sizeof(char *));
    id_query=(char**) calloc (size+1, sizeof(char *));
    
    char url_comp[2056]={'\0'};

    char *str_url1=NULL;
    char *str_u=NULL;
    /* next, print out the rows */
    for (i = 0; i < PQntuples(res1); i++)
    {
        str_url1=strdup(PQgetvalue(res1, i, 0));
       
        str_u=strstr(str_url1,"esg_dataroot/");
        if(str_u)
        {
          //printf("url vale %s\n", str_u+13);
          sprintf(url_comp, "http://%s/solr/files/select/?q=url:*esg_dataroot/%s*", esgf_node, str_u+13);
          printf("url vale %s\n", url_comp);
        }
        else
          sprintf(url_comp, "http://%s/solr/files/select/?q=url:*%s*", esgf_node, str_url1);
        free(str_url1);

        //URL[i]=strdup(PQgetvalue(res1, i, 0));
        URL[i]=strdup(url_comp);
        id_query[i]=strdup(PQgetvalue(res1, i, 1));
       
        //printf("%s\n", PQgetvalue(res1, i, 0));
        //printf("%s\n", PQgetvalue(res1, i, 1));
    }
    size=PQntuples(res1);
    printf("size vale %d\n", size);
    PQclear(res1);
    /* close the connection to the database and cleanup */
    PQfinish(conn);
    URL[i]=NULL;
    id_query[i]=NULL;


/*    URL=(char **) calloc (size, sizeof(char *));

    URL[0]=strdup("http://sara.unisalento.it/~mirto/query1_0.xml");
    URL[1]=strdup("http://sara.unisalento.it/~mirto/query1_1.xml");
    URL[2]=strdup("http://sara.unisalento.it/~mirto/query1_2.xml");
*/
    struct FtpFile **ftpfile=NULL;
    ftpfile=calloc (size+2, sizeof(struct FtpFile *));
    if(!ftpfile)
    {
        myfree_array(URL,size+1);
        myfree_array(id_query,size+1);
        //pmesg(LOG_ERROR, __FILE__, __LINE__,"Not enough memory. Error in calloc");
	return ERROR_CALLOC;
    }

    res=alloca_struct_FtpFile(ftpfile, URL, id_query, size);
    if(res!=SUCCESS)
    {
       myfree_array(URL,size+1);
       myfree_array(id_query,size+1);
       free_struct_FtpFile(ftpfile);
       return res;
    }
    
    
    res=ftp_download_file(ftpfile,size);
   
    if(res!=SUCCESS)
    {
        //pmesg(LOG_ERROR, __FILE__, __LINE__,"Error in download files");
    }
 
   int size_eff=0;
   for (cnt=0; cnt < size; cnt++)
    {
      /*parse the file and get the DOM */
      doc = xmlReadFile(ftpfile[cnt]->filename, NULL, 0);
      if (doc == NULL)
      {
        //fprintf(stderr, "\n[%s:%d] Error: could not parse file %s\n", __FILE__, __LINE__, ftpfile[cnt]->filename);
        size_eff++;
        //return NULL;
      }
      xmlFreeDoc(doc);
      xmlCleanupCharEncodingHandlers();
      xmlCleanupParser();
      xmlMemoryDump();
    }
    size_eff=size-size_eff;
    printf("size_eff vale %d\n", size_eff);
    if(size_eff==0)  //significa che non ho scaricato nulla
    {
       myfree_array(URL,size+1);
       myfree_array(id_query,size+1);
       free_struct_FtpFile(ftpfile);
       sleep(5);
       continue;
       //return -1; //devo andare in sleep
    }

    int num_metadata=0;
    i=0;
    datasetid=calloc(size_eff+1, sizeof(char*));
    queryid=calloc(size_eff+1, sizeof(char*));
    for (cnt=0; cnt < size; cnt++)
    {
      doc = xmlReadFile(ftpfile[cnt]->filename, NULL, 0);
      if (doc != NULL)
      {
        //fprintf(stderr, "\n[%s:%d] Error: could not parse file %s\n", __FILE__, __LINE__, ftpfile[cnt]->filename);
        queryid[i]=strdup(id_query[cnt]);
        i++;
        //return NULL;
      }
      xmlFreeDoc(doc);
      xmlCleanupCharEncodingHandlers();
      xmlCleanupParser();
      xmlMemoryDump();      
    }

    struct dataset_project **datasetproj;
    datasetproj=calloc(size_eff+1, sizeof(struct dataset_project*));

    int query_kind=0;
    int cnt2=0;
    int flag=0;
    for (cnt=0; cnt < size; cnt++)
    {
      /*parse the file and get the DOM */
      doc = xmlReadFile(ftpfile[cnt]->filename, NULL, 0);
      if (doc == NULL)
      {
	//fprintf(stderr, "\n[%s:%d] Error: could not parse file %s\n", __FILE__, __LINE__, ftpfile[cnt]->filename);
        flag=1;
	//return NULL;
      }
    if(flag==0)
    { 
      /*Get the root element node */
      root_element = xmlDocGetRootElement(doc);
      res=count_tag(doc,"//arr[@name='project']");
      if(res!=0)
      { 
        int res1=0;
        //printf("file letto %s\n", ftpfile[cnt]->filename); 
        datasetproj[cnt2]=(struct dataset_project *) calloc(1, sizeof(struct dataset_project));
        datasetproj[cnt2]->first=(struct project **)calloc (res+1, sizeof(struct project *));
        res1 = get_datasetid_solr(root_element, &datasetproj, cnt2,res, queryid);
        datasetproj[cnt2]->first[res]=NULL;
        res1=read_conf_project(filename_conf,&datasetproj, cnt2);

        int size1,size2, size3,size4;
        for(size2=0; datasetproj[cnt2]->first[size2]!=NULL; size2++)
        {
          int num_metadata=datasetproj[cnt2]->first[size2]->size;
          if(num_metadata>0) 
            res = get_metadata_solr(doc, root_element, &datasetproj, cnt2, size2, num_metadata, query_kind);
        }
        sprintf(str_url, "http://%s/solr/datasets/select/?q=id:%s", esgf_node, datasetproj[cnt2]->dataset_id);
        printf("str_url seconda interrogazione al solr %s\n", str_url);
        //sprintf(str_url,"http://sara.unisalento.it/~mirto/query2_%d.xml", cnt2);
        if(datasetid[cnt2]!=NULL)
        {
          free(datasetid[cnt2]);
          datasetid[cnt2]=NULL;
        }
        datasetid[cnt2] = strdup(str_url);
        printf("datasetid[%d] vale %s\n", cnt2, datasetid[cnt2]);
        cnt2++;
      }
     }
      flag=0;
      xmlFreeDoc(doc);
      xmlCleanupCharEncodingHandlers();
      xmlCleanupParser();
      xmlMemoryDump();
      res=remove(ftpfile[cnt]->filename);
#if 0
      if(res!=0)
        pmesg(LOG_ERROR, __FILE__, __LINE__, "Error in remove the file %s\n", ftpfile[cnt]->filename);
      else
        pmesg(LOG_DEBUG, __FILE__, __LINE__, "File %s removed\n", ftpfile[cnt]->filename);
#endif
    }
    datasetid[cnt2]=NULL;  
    datasetproj[cnt2]=NULL;

    myfree_array(URL,size+1);
    //myfree_array(id_query,nFields+1);

    free_struct_FtpFile(ftpfile);

    ftpfile=calloc (cnt2+2, sizeof(struct FtpFile *));
   
    res=alloca_struct_FtpFile(ftpfile, datasetid, queryid, cnt2);
    if(res!=SUCCESS)
    {
       free_struct_FtpFile(ftpfile);
       return res;
    }
    myfree_array(id_query,size+1);
   
    res=ftp_download_file(ftpfile,cnt2);
   
    if(res!=SUCCESS)
    {
        //pmesg(LOG_ERROR, __FILE__, __LINE__,"Error in download files");
    }
    free_datasetid(datasetid);
    free_datasetid(queryid);

    int k=0;

    for (cnt=0; ftpfile[cnt]->filename!=NULL; cnt++)
    {
      /*parse the file and get the DOM */
      printf("FILENAME vale %s\n", ftpfile[cnt]->filename);
      doc = xmlReadFile(ftpfile[cnt]->filename, NULL, 0);
      if (doc == NULL)
      {
	fprintf(stderr, "\n[%s:%d] Error: could not parse file %s\n", __FILE__, __LINE__, ftpfile[cnt]->filename);
        free_struct_FtpFile(ftpfile);
        continue;
	//return NULL;
      }
      /*Get the root element node */
      root_element = xmlDocGetRootElement(doc);
      int size1,size2, size3,size4;
      for(size2=0; datasetproj[cnt]->first[size2]!=NULL; size2++)
      {
          int num_metadata=datasetproj[cnt]->first[size2]->size;
          int query_kind=1;
          if(num_metadata>0)
          {
             for(size3=0; datasetproj[cnt]->first[size2]->first[size3]!=NULL; size3++)
             {
               if(datasetproj[cnt]->first[size2]->first[size3]!=NULL)
               {
                 if((datasetproj[cnt]->first[size2]->first[size3]->name)&&(datasetproj[cnt]->first[size2]->first[size3]->size==0))
                 {
                    //printf("Sto vedendo se il metadato %s ha valori nella seconda query, with size2%d, size3%d\n", datasetproj[cnt]->first[size2]->first[size3]->name,size2,size3);
                    res = get_metadata_solr(doc, root_element, &datasetproj, cnt, size2, size3, query_kind);
                 }
               }
             }
          }
      }
      datasetproj[cnt]->size=size2; //number of projects 

        for(size2=0; datasetproj[cnt]->first[size2]!=NULL; size2++)
        {
          printf("id_query** vale %d\n", datasetproj[cnt]->id_query);
          printf("id** vale %s\n", datasetproj[cnt]->id);
          printf("dataset_id** vale %s\n", datasetproj[cnt]->dataset_id);
          printf("version** vale %s\n", datasetproj[cnt]->version);
          printf("project** vale %s\n", datasetproj[cnt]->first[size2]->project);
          for(size3=0; datasetproj[cnt]->first[size2]->first[size3]!=NULL; size3++)
          {
              if(datasetproj[cnt]->first[size2]->first[size3]!=NULL)
              {
                if(datasetproj[cnt]->first[size2]->first[size3]->name)
                {
                  printf("***metadato vale %s\n", datasetproj[cnt]->first[size2]->first[size3]->name);
                  //printf("***numero di metadati vale %d\n", datasetproj[cnt]->first[size2]->first[size3]->size);
                }
                  for(size4=0; datasetproj[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                    printf("*******valore vale %s\n", datasetproj[cnt]->first[size2]->first[size3]->value[size4]);
              }
         }
       }
      res=remove(ftpfile[cnt]->filename);
#if 0
      if(res!=0)
        pmesg(LOG_ERROR, __FILE__, __LINE__, "Error in remove the file %s\n", ftpfile[cnt]->filename);
      else
        pmesg(LOG_DEBUG, __FILE__, __LINE__, "File %s removed\n", ftpfile[cnt]->filename);
#endif
      xmlFreeDoc(doc);
      xmlCleanupCharEncodingHandlers();
      xmlCleanupParser();
      xmlMemoryDump();
    }

    check_cross_project(&datasetproj,esgf_node);
    //insert_datamart_cross_project();
    insert_dmart_cross_project();
    free_struct_datasetproj(datasetproj);
    
    free_struct_FtpFile(ftpfile);
    printf("%s\n", "attendo 5 sec");
    sleep(5);
    printf("%s\n", "prossimi 5 sec");
}
    return SUCCESS;
}
int alloca_struct_FtpFile(struct FtpFile **ftpfile, char** URL, char** id_query, int size)
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
       printf("URL vale %s\n", URL[cnt]);
       if(!ftpfile[cnt]->URL)
       {
         //pmesg(LOG_ERROR, __FILE__, __LINE__,"Not enough memory. Error in strdup");
	 //fprintf(stderr, "\n[%s:%d] Not enough memory. Error in strdup()\n", __FILE__, __LINE__);
	 free_struct_FtpFile(ftpfile);
	 return STRDUP_ERROR;
       }
       char *filename=NULL;
       char *str_1=NULL;
       str_1=strdup(URL[cnt]);
       filename=strdup(basename(str_1));
       free(str_1);
        
       char *str=NULL;
       str=strrchr(filename, '*');
       if(str!=NULL)
	 *str='\0';
       str=strstr(filename, "id:");
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
         sprintf(str_2, "%s_%ld", filename, tv.tv_usec);           
       }
       free(filename);
       filename=NULL;
       filename=strdup(str_2);
       printf("filename vale %s\n", filename);
       ftpfile[cnt]->filename = strdup(filename);
       free(filename);
       filename=NULL;
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


