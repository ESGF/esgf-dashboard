#include "../include/config.h"

/* track the current level in the xml tree */
static char    *last_content;

char *xmlName;
int jump = 0; //variable that prevents an error in the parser library when occur a parsing of an xml file that is not indented

/* first when start element is encountered */
void start_element(void *data, const char *element, const char **attribute)
{
	//printf("\nSONO IN START_ELEMENT");
	int i;


	for (i = 0; attribute[i]; i += 2) 
	{
		//printf("\n Attribute: %s = '%s'", attribute[i], attribute[i + 1]); //print the attributes in the tag
		
		if (!strcmp(xmlName, "config.xml")) //if the XML file is the configuration file
		{
			parsing_Configxml(attribute[i], attribute[i+1]); //set the value of ipNodeAddress, datamart e url
		}	
	}
}


/* decrement the current level of the tree */
void end_element(void *data, const char *el)
{
	//printf("\nSONO IN END_ELEMENT");
	int i;
	
	if (strcmp(xmlName, "config.xml")) //if the XML file is NOT the configuration file
	{	
		parsing_Dataxml(last_content, &jump);
	}	

	//printf("\nLAST CONTENT: %s\n", last_content); //print the conte content between two tags
	jump = 0;
}

void handle_data(void *data, const char *content, int length)
{
	//printf("\nSONO IN handle_data");
	char *tmp = malloc(length);
	strncpy(tmp, content, length);
	tmp[length] = '\0';
	data = (void *) tmp;
	last_content = tmp;         /* TODO: concatenate the text nodes? */
	jump = 1;
}

void parse_xml(struct fileXML *file) 
{		
	xmlName = malloc(strlen(file->name) + 1);
	strcpy(xmlName, file->name);
	
	XML_Parser parser = XML_ParserCreate(NULL);
	XML_SetElementHandler(parser, start_element, end_element);
	XML_SetCharacterDataHandler(parser, handle_data);

	if (XML_Parse(parser, file->content, file->size, XML_TRUE) == XML_STATUS_ERROR) /* parse the xml */
	{
	    printf("Error: %s\n", XML_ErrorString(XML_GetErrorCode(parser)));
	}
	XML_ParserFree(parser);
	return 0;
}
