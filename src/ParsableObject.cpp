#include "ParsableObject.hpp"


void swSim::ParsableObject::ParseSwitch(xmlDocPtr doc, xmlNodePtr cur, int caseNumber) {
        return;  
}

void swSim::ParsableObject::Parse(xmlDocPtr doc, xmlNodePtr cur){ 
    xmlNodePtr cur2;
    cur2 = cur->xmlChildrenNode;
    while (cur2 != NULL){
        for(int i=0; i<this->parseNames.size(); i++) {
            if ((!xmlStrcmp(cur2->name, (const xmlChar *)this->parseNames.at(i).c_str()))){
                //std::printf("Parse tag: %s.\n", cur2->name);
                this->ParseSwitch(doc, cur2, i); 
            }
        }
        cur2 = cur2->next; 
    }
    return;  
}


xmlNodePtr swSim::ParsableObject::ParseSearch(xmlDocPtr doc, xmlNodePtr cur0, const char *value) 
{
    xmlNodePtr cur;
    cur = cur0->xmlChildrenNode;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)value))) {
            return cur; 
        }
        cur = cur->next;
    }
    return NULL; 
}

void swSim::ParsableObject::Do_parse(xmlDocPtr doc, xmlNodePtr cur, double *value) 
{
    xmlChar *key; // The pointer to convert xml objects
    char keystr[50]; // The xml tag character container

    key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
    sprintf(keystr, "%s", key);
    *value = strtod(keystr, NULL);
    xmlFree(key);
    return;
}

void swSim::ParsableObject::Do_parse(xmlDocPtr doc, xmlNodePtr cur, float *value) 
{
    xmlChar *key; // The pointer to convert xml objects
    char keystr[50]; // The xml tag character container

    key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
    sprintf(keystr, "%s", key);
    *value = strtof(keystr, NULL);
    xmlFree(key);
    return;
}

void swSim::ParsableObject::Do_parse(xmlDocPtr doc, xmlNodePtr cur, uint8_t *value) 
{
    xmlChar *key; // The pointer to convert xml objects
    char keystr[50]; // The xml tag character container

    key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
    sprintf(keystr, "%s", key);
    *value = strtol(keystr, NULL, 0);
    xmlFree(key);
    return;
}

void swSim::ParsableObject::Do_parse(xmlDocPtr doc, xmlNodePtr cur, uint32_t *value) 
{
    xmlChar *key; // The pointer to convert xml objects
    char keystr[50]; // The xml tag character container

    key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
    sprintf(keystr, "%s", key);
    *value = strtoul(keystr, NULL, 0);
    xmlFree(key);
    return;
}

void swSim::ParsableObject::Do_parse(xmlDocPtr doc, xmlNodePtr cur, uint64_t *value) 
{
    xmlChar *key; // The pointer to convert xml objects
    char keystr[50]; // The xml tag character container

    key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
    sprintf(keystr, "%s", key);
    *value = strtoull(keystr, NULL, 0);
    xmlFree(key);
}

void swSim::ParsableObject::Do_parse(xmlDocPtr doc, xmlNodePtr cur, bool *value) 
{
    xmlChar *key; // The pointer to convert xml objects
    char keystr[50]; // The xml tag character container
    int input; 

    key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
    sprintf(keystr, "%s", key);
    input = strtol(keystr, NULL,0);
    if (input == 1){
        *value = true; 
    }
    else{
        *value = false;
    }
    xmlFree(key);
    return;
}

void swSim::ParsableObject::Do_parse(xmlDocPtr doc, xmlNodePtr cur, char *value) 
{
    xmlChar *key; // The pointer to convert xml objects
    key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
    sprintf(value, "%s", key);
    xmlFree(key);
    return;
}