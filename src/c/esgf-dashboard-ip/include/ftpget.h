#define FED_DIR "./.fed" 
struct FtpFile {
  char *URL;
  char *filename;
  int id_query;
  int flag;
  FILE *stream;
  int res_down;
};
struct metadata_value{
  char **value;
  struct metadata_value *next;
};
struct metadata_entry{
  char *name;
  char *occ;
  char **value;
  int size;
};
//typedef struct metadata_entry *metadata_entry;
struct project{
  char *project;
  struct metadata_entry **first;
  int size;
};
struct dataset_project{
  int id_query; //key of the dashboard_queue table
  char *id;     //id read from the solr for querying again the solr
  char *dataset_id; //dataset_id read from the solr for dashboard datawarehouse --> parsing before | 
  char *version; //prendere l'ultimo valore dopo il punto v1 --> considerare 1
  int size; //numero di projects
  int flag;
  //char **project;   //project read from the solr for dashboard datawarehouse
  struct project **first;
};
struct table_dmart{
  char *table;
  struct field_entry **first;
  int size;
}; 
struct field_entry {
   char *entry_name;
   char *entry_value;
   int size;
};

//typedef struct dataset_project *dataset_project;
