#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/tree.h>


#define OASIS_TEST "conf/xmlconf.xml"


/************************************************************************
 * 									*
 * 			HTML ouput					*
 * 									*
 ************************************************************************/
char buffer[50000];

void
xmlHTMLEncodeSend(void) {
    char *result;

    result = (char *) xmlEncodeEntitiesReentrant(NULL, BAD_CAST buffer);
    if (result) {
	fprintf(stderr, "%s", result);
	xmlFree(result);
    }
    buffer[0] = 0;
}

/**
 * xmlHTMLPrintFileInfo:
 * @input:  an xmlParserInputPtr input
 * 
 * Displays the associated file and line informations for the current input
 */

void
xmlHTMLPrintFileInfo(xmlParserInputPtr input) {
    fprintf(stderr, "<p>");
    if (input != NULL) {
	if (input->filename) {
	    sprintf(&buffer[strlen(buffer)], "%s:%d: ", input->filename,
		    input->line);
	} else {
	    sprintf(&buffer[strlen(buffer)], "Entity: line %d: ", input->line);
	}
    }
    xmlHTMLEncodeSend();
}

/**
 * xmlHTMLPrintFileContext:
 * @input:  an xmlParserInputPtr input
 * 
 * Displays current context within the input content for error tracking
 */

void
xmlHTMLPrintFileContext(xmlParserInputPtr input) {
    const xmlChar *cur, *base;
    int n;

    if (input == NULL) return;
    fprintf(stderr, "<pre>\n");
    cur = input->cur;
    base = input->base;
    while ((cur > base) && ((*cur == '\n') || (*cur == '\r'))) {
	cur--;
    }
    n = 0;
    while ((n++ < 80) && (cur > base) && (*cur != '\n') && (*cur != '\r'))
        cur--;
    if ((*cur == '\n') || (*cur == '\r')) cur++;
    base = cur;
    n = 0;
    while ((*cur != 0) && (*cur != '\n') && (*cur != '\r') && (n < 79)) {
        sprintf(&buffer[strlen(buffer)], "%c", (unsigned char) *cur++);
	n++;
    }
    sprintf(&buffer[strlen(buffer)], "\n");
    cur = input->cur;
    while ((*cur == '\n') || (*cur == '\r'))
	cur--;
    n = 0;
    while ((cur != base) && (n++ < 80)) {
        sprintf(&buffer[strlen(buffer)], " ");
        base++;
    }
    sprintf(&buffer[strlen(buffer)],"^\n");
    xmlHTMLEncodeSend();
    fprintf(stderr, "</pre>");
}

/**
 * xmlHTMLError:
 * @ctx:  an XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 * 
 * Display and format an error messages, gives file, line, position and
 * extra parameters.
 */
void
xmlHTMLError(void *ctx, const char *msg, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr input;
    xmlParserInputPtr cur = NULL;
    va_list args;

    buffer[0] = 0;
    input = ctxt->input;
    if ((input != NULL) && (input->filename == NULL) && (ctxt->inputNr > 1)) {
	cur = input;
        input = ctxt->inputTab[ctxt->inputNr - 2];
    }
        
    xmlHTMLPrintFileInfo(input);

    fprintf(stderr, "<b>error</b>: ");
    va_start(args, msg);
    vsprintf(&buffer[strlen(buffer)], msg, args);
    va_end(args);
    xmlHTMLEncodeSend();
    fprintf(stderr, "</p>\n");

    xmlHTMLPrintFileContext(input);
    xmlHTMLEncodeSend();
}

/**
 * xmlHTMLWarning:
 * @ctx:  an XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 * 
 * Display and format a warning messages, gives file, line, position and
 * extra parameters.
 */
void
xmlHTMLWarning(void *ctx, const char *msg, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr input;
    xmlParserInputPtr cur = NULL;
    va_list args;

    buffer[0] = 0;
    input = ctxt->input;
    if ((input != NULL) && (input->filename == NULL) && (ctxt->inputNr > 1)) {
	cur = input;
        input = ctxt->inputTab[ctxt->inputNr - 2];
    }
        

    xmlHTMLPrintFileInfo(input);
        
    fprintf(stderr, "<b>warning</b>: ");
    va_start(args, msg);
    vsprintf(&buffer[strlen(buffer)], msg, args);
    va_end(args);
    xmlHTMLEncodeSend();
    fprintf(stderr, "</p>\n");

    xmlHTMLPrintFileContext(input);
    xmlHTMLEncodeSend();
}

/**
 * xmlHTMLValidityError:
 * @ctx:  an XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 * 
 * Display and format an validity error messages, gives file,
 * line, position and extra parameters.
 */
void
xmlHTMLValidityError(void *ctx, const char *msg, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr input;
    va_list args;

    buffer[0] = 0;
    input = ctxt->input;
    if ((input->filename == NULL) && (ctxt->inputNr > 1))
        input = ctxt->inputTab[ctxt->inputNr - 2];
        
    xmlHTMLPrintFileInfo(input);

    fprintf(stderr, "<b>validity error</b>: ");
    va_start(args, msg);
    vsprintf(&buffer[strlen(buffer)], msg, args);
    va_end(args);
    xmlHTMLEncodeSend();
    fprintf(stderr, "</p>\n");

    xmlHTMLPrintFileContext(input);
    xmlHTMLEncodeSend();
}

/**
 * xmlHTMLValidityWarning:
 * @ctx:  an XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 * 
 * Display and format a validity warning messages, gives file, line,
 * position and extra parameters.
 */
void
xmlHTMLValidityWarning(void *ctx, const char *msg, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr input;
    va_list args;

    buffer[0] = 0;
    input = ctxt->input;
    if ((input->filename == NULL) && (ctxt->inputNr > 1))
        input = ctxt->inputTab[ctxt->inputNr - 2];

    xmlHTMLPrintFileInfo(input);
        
    fprintf(stderr, "<b>validity warning</b>: ");
    va_start(args, msg);
    vsprintf(&buffer[strlen(buffer)], msg, args);
    va_end(args);
    xmlHTMLEncodeSend();
    fprintf(stderr, "</p>\n");

    xmlHTMLPrintFileContext(input);
    xmlHTMLEncodeSend();
}

xmlDocPtr readOasisSuite(const char *filename) {
    int ret;
    xmlParserCtxtPtr ctxt;
    xmlSAXHandler silent, *old;

    xmlDocPtr doc;

    /*
     * strip blanks from the test result tree and read in the external
     * entities.
     */
    xmlKeepBlanksDefault(0);
    xmlSubstituteEntitiesDefault(1);

    /*
     * Do a silent parsing
     */
    ctxt = xmlCreateFileParserCtxt(filename);
    memcpy(&silent, ctxt->sax, sizeof(silent));
    old = ctxt->sax;
    silent.error = NULL;
    silent.warning = NULL;
    silent.fatalError = NULL;

    xmlParseDocument(ctxt);

    ret = ctxt->wellFormed;
    doc = ctxt->myDoc;
    ctxt->sax = old;
    xmlFreeParserCtxt(ctxt);
    if (!ret) {
        xmlFreeDoc(doc);
	doc = NULL;
    }

    /*
     * switch back to decent defaults.
     */
    xmlKeepBlanksDefault(1);
    xmlSubstituteEntitiesDefault(0);
    return(doc);
}

int analyzeTest(xmlNodePtr test) {
    xmlChar *type;
    xmlChar *sections;
    xmlChar *id;
    xmlChar *URI;

    id = xmlGetProp(test, (xmlChar *) "ID");
    if (id == NULL) {
        fprintf(stderr, "Test without ID\n");
	return(0);
    }
    type = xmlGetProp(test, (xmlChar *) "TYPE");
    if (type == NULL) {
        fprintf(stderr, "Test %s without TYPE\n", id);
	xmlFree(id);
	return(0);
    }
    URI = xmlGetProp(test, (xmlChar *) "URI");
    if (URI == NULL) {
        fprintf(stderr, "Test %s without URI\n", id);
	xmlFree(id);
	xmlFree(type);
	return(0);
    }
    sections = xmlGetProp(test, (xmlChar *) "SECTIONS");
    printf("Test: %s, type %s, URI %s", id, type, URI);
    if (sections != NULL) {
        printf(", sections %s\n", sections);
	xmlFree(sections);
    } else 
        printf("\n");
    xmlFree(id);
    xmlFree(type);
    xmlFree(URI);
    return(1);
}

int analyzeCases(xmlNodePtr cases) {
    xmlChar *profile;
    int nbtests = 0;
    xmlNodePtr child;

    profile = xmlGetProp(cases, (xmlChar *) "PROFILE");

    if (profile) {
	printf("\n  Suite: %s\n\n", (char *) profile);
    } else {
	printf("\n  Suite: unknown tests\n\n");
    }

    child = cases->children;
    while (child != NULL) {
        if ((child->type == XML_ELEMENT_NODE) &&
            (!strcmp(child->name, "TEST"))) {
	    nbtests += analyzeTest(child);
	}
	child = child->next;
    }

    if (profile) {
	printf("\n  Suite: %s : %d tests\n\n", (char *) profile, nbtests);
	xmlFree(profile);
    } else {
	printf("\n  Suite: %d tests\n\n", nbtests);
    }

    return(nbtests);
}

int analyzeSuite(xmlNodePtr suite) {
    xmlChar *profile;
    int nbtests = 0;
    xmlNodePtr child;

    if ((suite == NULL) || strcmp(suite->name, "TESTSUITE")) {
        fprintf(stderr, "No TESTSUITE root\n");
	return(-1);
    }
    profile = xmlGetProp(suite, (xmlChar *) "PROFILE");

    if (profile) {
	printf("\n     Testing: %s\n\n", (char *) profile);
    } else {
	printf("\n     Testing: unknown test suite\n\n");
    }

    child = suite->children;
    while (child != NULL) {
        if ((child->type == XML_ELEMENT_NODE) &&
            (!strcmp(child->name, "TESTCASES"))) {
	    nbtests += analyzeCases(child);
	}
	child = child->next;
    }

    if (profile) {
	printf("\n     Testing: %s : %d tests\n\n", (char *) profile, nbtests);
	xmlFree(profile);
    } else {
	printf("\n     Testing: test suite : %d tests\n\n", nbtests);
    }

    return(nbtests);
}

int main(int argc, char ** argv) {
    xmlDocPtr tree;

    tree = readOasisSuite(OASIS_TEST);
    
    if (tree == NULL) {
        fprintf(stderr, "failed to load %s\n", OASIS_TEST);
        return(-1);
    }
    analyzeSuite(xmlDocGetRootElement(tree));
    return(0);
}
