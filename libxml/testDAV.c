/*
 * testDAV.c : a small tester program for WebDAV operations
 *
 * See Copyright for the status of this software.
 *
 * Daniel.Veillard@w3.org
 */

#ifdef WIN32
#include "win32config.h"
#else
#include "config.h"
#endif

#include "xmlversion.h"

#ifdef LIBXML_HTTP_ENABLED
#include <stdio.h>
#include <string.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <libxml/xmlmemory.h>
#include <libxml/nanohttp.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

static int debug = 0;

typedef struct nodeinfo {
    struct nodeinfo *next;
    char *name;
    char *size;
    char *date;
    char *content;
} nodeinfo, *nodeinfoPtr;

char current_protocol[20] = "http";
char current_hostname[100] = "localhost";
char current_path[1024] = "/";
char current_file[1024] = "";
int current_port = 80;

void scanURL(const char *URL) {
    const char *cur = URL;
    char buf[4096];
    int index = 0;
    int port = 0;

    buf[index] = 0;
    while (*cur != 0) {
        if ((cur[0] == ':') && (cur[1] == '/') && (cur[2] == '/')) {
	    buf[index] = 0;
	    strcpy(current_protocol, buf);
	    index = 0;
            cur += 3;
	    break;
	}
	buf[index++] = *cur++;
    }
    if (*cur == 0) return;

    buf[index] = 0;
    while (1) {
        if (cur[0] == ':') {
	    buf[index] = 0;
	    strcpy(current_hostname, buf);
	    index = 0;
	    cur += 1;
	    while ((*cur >= '0') && (*cur <= '9')) {
	        port *= 10;
		port += *cur - '0';
		cur++;
	    }
	    if (port != 0) current_port = port;
	    while ((cur[0] != '/') && (*cur != 0)) 
	        cur++;
	    break;
	}
        if ((*cur == '/') || (*cur == 0)) {
	    buf[index] = 0;
	    strcpy(current_hostname, buf);
	    index = 0;
	    break;
	}
	buf[index++] = *cur++;
    }
    if (*cur == 0) {
        strcpy(current_path, "/");
        strcpy(current_file, "");
    }else {
        buf[index] = 0;
	while (*cur != 0) {
	    if ((cur[0] == '#') || (cur[0] == '?'))
	        break;
	    buf[index++] = *cur++;
	}
	buf[index] = 0;
	while ((index > 0) && (buf[index] != '/')) index--;
	strcpy(current_file, &buf[index + 1]);
	buf[index + 1] = 0;
	strcpy(current_path, buf);
    }	
}

nodeinfoPtr nodeinfoNew() {
    nodeinfoPtr ret = xmlMalloc(sizeof(nodeinfo));
    if (ret == NULL) return(NULL);
    memset(ret, 0, sizeof(nodeinfo));
    return(ret);
}
void nodeinfoFree(nodeinfoPtr info) {
    if (info == NULL) return;
    if (info->name) xmlFree(info->name);
    if (info->date) xmlFree(info->date);
    if (info->size) xmlFree(info->date);
    if (info->content) xmlFree(info->content);
}
void nodeinfoListFree(nodeinfoPtr info) {
    nodeinfoPtr cur = info;

    while (cur != NULL) {
        info = info->next;
	nodeinfoFree(cur);
	cur = info;
    }
}
void nodeinfoPrint(nodeinfoPtr info) {
    if (info == NULL) return;
    if (info->date) printf("%s\t", info->date);
    if (info->name) printf("%s\t", info->name);
    else printf("???\t");
    if (info->size) printf("%s\t", info->size);
    if (info->content) printf("%s\t", info->content);
    printf("\n");
}
void nodeinfoListPrint(nodeinfoPtr info) {
    nodeinfoPtr cur = info;

    while (cur != NULL) {
        info = info->next;
	nodeinfoPrint(cur);
	cur = info;
    }
}
int propfindQuery(const char *URL, char **result) {
    void *ctxt;
    char *contentType = "text/xml; charset=\"utf-8\"";
    char *output;
    int len;
    int size = 40000;
    int returnCode;
    char *verboseQuery = 
"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\
<D:propfind xmlns:D=\"DAV:\">\
    <D:allprop/>\
</D:propfind>";


    if ((URL == NULL) || (result == NULL)) return(-1);
    
    if (URL[strlen(URL) - 1] == '/')
	ctxt = xmlNanoHTTPMethod(URL, "PROPFIND", NULL,
				 &contentType, "Depth: 1\r\n");
    else
	ctxt = xmlNanoHTTPMethod(URL, "PROPFIND", verboseQuery,
				 &contentType, "Depth: 0\r\n");
    if (ctxt == NULL) {
	fprintf(stderr, "PROPFIND %s failed\n", URL);
	return(-1);
    }

    returnCode = xmlNanoHTTPReturnCode(ctxt);
    if ((returnCode >= 300) || 
        ((contentType != NULL) && (strncmp(contentType, "text/xml", 8)))) {
	xmlNanoHTTPClose(ctxt);
	if (contentType != NULL) xmlFree(contentType);
	return(-1);
    }
    if (contentType != NULL) xmlFree(contentType);

    output = (char *) xmlMalloc(size + 1);
    if (output == NULL) {
        fprintf(stderr, "out of memory\n");
	xmlNanoHTTPClose(ctxt);
	return(-1);
    }
    len = xmlNanoHTTPRead(ctxt, output, size);
    if (len < 0) {
	fprintf(stderr, "cannot read PROPFIND %s result\n", URL);
	xmlNanoHTTPClose(ctxt);
	xmlFree(output);
	return(-1);
    }
    /* !!!!!!!!! handle returns size > size !!!!!!!!!! */
    output[len] = 0;
    xmlNanoHTTPClose(ctxt);

    *result = output;
    return(returnCode);
}

void parsePropfindProp(xmlNodePtr cur, nodeinfoPtr node) {
    cur = cur->children;

    /*
     * prop ANY
     */
    while (cur != NULL) {
	if (!strcmp((char *) cur->name, "getlastmodified")) {
	    node->date = (char *) xmlNodeGetContent(cur);
	} else if (!strcmp((char *) cur->name, "getcontenttype")) {
	    node->content = (char *) xmlNodeGetContent(cur);
	} else if (!strcmp((char *) cur->name, "getcontentlength")) {
	    node->size = (char *) xmlNodeGetContent(cur);
	}
	cur = cur->next;
    }
}

void parsePropfindPropstat(xmlNodePtr cur, nodeinfoPtr node) {
    cur = cur->children;

    /*
     * propstat (prop, status, responsedescription?)
     */
    while (cur != NULL) {
	if (!strcmp((char *) cur->name, "prop")) {
	    parsePropfindProp(cur, node);
	} else if (!strcmp((char *) cur->name, "status")) {
	} else if (!strcmp((char *) cur->name, "responsedescription")) {
	}
	cur = cur->next;
    }
}

nodeinfoPtr parsePropfindHref(xmlNodePtr cur) {
    nodeinfoPtr ret;

    ret = nodeinfoNew();
    ret->name = (char *) xmlNodeGetContent(cur);
    return(ret);
}

nodeinfoPtr parsePropfindResponse(xmlNodePtr cur) {
    nodeinfoPtr ret, last, res = NULL;

    ret = last = NULL;
    cur = cur->children;

    /*
     * response (href, ((href*, status)|(propstat+)), responsedescription?
     */
    while (cur != NULL) {
	if (!strcmp((char *) cur->name, "href")) {
	    res = parsePropfindHref(cur);
	    if (res != NULL) {
		if (ret == NULL) ret = last = res;
		else last->next = res;
		while (last->next != NULL) last = last->next;
	    }
	} else if (!strcmp((char *) cur->name, "propstat")) {
	    parsePropfindPropstat(cur, res);
	} else if (!strcmp((char *) cur->name, "status")) {
	} else if (!strcmp((char *) cur->name, "responsedescription")) {
	}
	cur = cur->next;
    }
    return(ret);
}

nodeinfoPtr parsePropfindAnswer(xmlNodePtr cur) {
    nodeinfoPtr ret = NULL, last, res;

    if (!strcmp((char *) cur->name, "multistatus")) {
        ret = last = NULL;
        cur = cur->children;
	while (cur != NULL) {
	    if (!strcmp((char *) cur->name, "response")) {
		res = parsePropfindResponse(cur);
		if (res != NULL) {
		    if (ret == NULL) ret = last = res;
		    else last->next = res;
		    while (last->next != NULL) last = last->next;
		}
	    }
	    cur = cur->next;
	}
    }
    return(ret);
}

nodeinfoPtr propfind(char *file) {
    nodeinfoPtr ret;
    xmlDocPtr doc;
    char URL[1024];
    char *xmlResult = NULL;
    int result;

    if ((file != NULL) && (!strncmp(file, "http://", 7))) {
        scanURL(file);
    } else if (file != NULL) {
        strcpy(current_path, file);
    }
    if (current_file[0] == 0)
#ifdef HAVE_SNPRINTF
	snprintf(URL, sizeof(URL), "%s://%s:%d%s", current_protocol,
		 current_hostname, current_port, current_path);
#else
	sprintf(URL, "%s://%s:%d%s", current_protocol,
		 current_hostname, current_port, current_path);
#endif
    else
#ifdef HAVE_SNPRINTF
	snprintf(URL, sizeof(URL), "%s://%s:%d%s%s", current_protocol,
		 current_hostname, current_port, current_path, current_file);
#else
	sprintf(URL, "%s://%s:%d%s%s", current_protocol,
		 current_hostname, current_port, current_path, current_file);
#endif

    result = propfindQuery(URL, &xmlResult);
    if (result == -1) return(NULL);
    if (xmlResult == NULL) return(NULL);

    doc = xmlParseMemory(xmlResult, strlen(xmlResult));
    if (doc == NULL) {
        fprintf(stdout, "%s\n", xmlResult);
        xmlFree(xmlResult);
	return(NULL);
    }

    if (debug)
        fprintf(stdout, "%s\n", xmlResult);

    ret = parsePropfindAnswer(doc->children);
    xmlFreeDoc(doc);
    xmlFree(xmlResult);
    return(ret);
}

int cd(char *newpath) {
    if (newpath == NULL) {
        strcpy(current_path, "/");
        strcpy(current_file, "");
    } /* else if */
}

int ls(char *newpath) {
}

int main(int argc, char** argv) {
    nodeinfoPtr ret;
    int i;

    for (i = 1; i < argc ; i++) {
	if ((!strcmp(argv[i], "-debug")) || (!strcmp(argv[i], "--debug")))
	    debug++;
    }
    for (i = 1; i < argc ; i++) {
	if ((!strcmp(argv[i], "-debug")) || (!strcmp(argv[i], "--debug")))
	    continue;
        ret = propfind(argv[i]);
	nodeinfoListPrint(ret);
    }
    /**************
    int ret;
    while (1) {
        ret = fscanf("%s", line);
	if (!strncmp(line, "cd", 2)) {
	    cur = &line[2];
	    while ((*cur == ' ') || (*cur == "\t")) cur++;
	    if ((*cur != 0) && (*cur != '\n') && (*cur != '\r')) {
	        cd(cur);
	    } else {
	        cd(NULL);
	    }
	} else if (!strncmp(line, "ls", 2)) {
	    cur = &line[2];
	    while ((*cur == ' ') || (*cur == "\t")) cur++;
	    if ((*cur != 0) && (*cur != '\n') && (*cur != '\r')) {
	        ls(cur);
	    } else {
	        ls(NULL);
	    }
	}
    }
    **********/
    xmlCleanupParser();
    xmlMemoryDump();
    return(0);
}
#else /* !LIBXML_HTTP_ENABLED */
#include <stdio.h>
int main(int argc, char **argv) {
    printf("%s : HTTP support not compiled in\n", argv[0]);
    return(0);
}
#endif /* LIBXML_HTTP_ENABLED */
