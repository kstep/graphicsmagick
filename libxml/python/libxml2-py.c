/* Generated */

#include <Python.h>
#include "config.h"
#include <libxml/xmlversion.h>
#include <libxml/tree.h>
#include "libxml_wrap.h"
#include "libxml2-py.h"

PyObject *
libxml_xmlIsBaseChar(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    int c;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlIsBaseChar", &c))
        return(NULL);

    c_retval = xmlIsBaseChar(c);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCatalogDump(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    FILE * out;
    PyObject *pyobj_out;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlCatalogDump", &pyobj_out))
        return(NULL);
    out = (FILE *) PyFile_Get(pyobj_out);

    xmlCatalogDump(out);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParsePI(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParsePI", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParsePI(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParseDocTypeDecl(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseDocTypeDecl", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParseDocTypeDecl(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlFreeDoc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlDocPtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlFreeDoc", &pyobj_cur))
        return(NULL);
    cur = (xmlDocPtr) PyxmlNode_Get(pyobj_cur);

    xmlFreeDoc(cur);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathPositionFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathPositionFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathPositionFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParseNamespace(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseNamespace", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParseNamespace(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlCleanupParser(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlCleanupParser();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlRecoverDoc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlDocPtr c_retval;
    xmlChar * cur;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlRecoverDoc", &cur))
        return(NULL);

    c_retval = xmlRecoverDoc(cur);
    py_retval = libxml_xmlDocPtrWrap((xmlDocPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlDebugDumpDTD(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    FILE * output;
    PyObject *pyobj_output;
    xmlDtdPtr dtd;
    PyObject *pyobj_dtd;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlDebugDumpDTD", &pyobj_output, &pyobj_dtd))
        return(NULL);
    output = (FILE *) PyFile_Get(pyobj_output);
    dtd = (xmlDtdPtr) PyxmlNode_Get(pyobj_dtd);

    xmlDebugDumpDTD(output, dtd);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlUnsetProp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;
    xmlChar * name;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlUnsetProp", &pyobj_node, &name))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlUnsetProp(node, name);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseReference(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseReference", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParseReference(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParserGetIsValid(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParserGetIsValid", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = ctxt->valid;
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseNmtoken(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseNmtoken", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParseNmtoken(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_htmlParseCharRef(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    htmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:htmlParseCharRef", &pyobj_ctxt))
        return(NULL);
    ctxt = (htmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = htmlParseCharRef(ctxt);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlSaveFormatFileEnc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    char * filename;
    xmlDocPtr cur;
    PyObject *pyobj_cur;
    char * encoding;
    int format;

    if (!PyArg_ParseTuple(args, (char *)"zOzi:xmlSaveFormatFileEnc", &filename, &pyobj_cur, &encoding, &format))
        return(NULL);
    cur = (xmlDocPtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlSaveFormatFileEnc(filename, cur, encoding, format);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCatalogRemove(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlChar * value;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlCatalogRemove", &value))
        return(NULL);

    c_retval = xmlCatalogRemove(value);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathSubstringAfterFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathSubstringAfterFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathSubstringAfterFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParseComment(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseComment", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParseComment(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathCastNumberToString(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    double val;

    if (!PyArg_ParseTuple(args, (char *)"d:xmlXPathCastNumberToString", &val))
        return(NULL);

    c_retval = xmlXPathCastNumberToString(val);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNamespaceParseNSDef(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlNamespaceParseNSDef", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlNamespaceParseNSDef(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlACatalogRemove(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlCatalogPtr catal;
    PyObject *pyobj_catal;
    xmlChar * value;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlACatalogRemove", &pyobj_catal, &value))
        return(NULL);
    catal = (xmlCatalogPtr) Pycatalog_Get(pyobj_catal);

    c_retval = xmlACatalogRemove(catal, value);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNewDoc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlDocPtr c_retval;
    xmlChar * version;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlNewDoc", &version))
        return(NULL);

    c_retval = xmlNewDoc(version);
    py_retval = libxml_xmlDocPtrWrap((xmlDocPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNodeSetLang(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNodePtr cur;
    PyObject *pyobj_cur;
    xmlChar * lang;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlNodeSetLang", &pyobj_cur, &lang))
        return(NULL);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    xmlNodeSetLang(cur, lang);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathEvalExpr(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathEvalExpr", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathEvalExpr(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathGetContextDoc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlDocPtr c_retval;
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathGetContextDoc", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);

    c_retval = ctxt->doc;
    py_retval = libxml_xmlDocPtrWrap((xmlDocPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlPopInput(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlPopInput", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlPopInput(ctxt);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlDecodeEntities(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;
    int len;
    int what;
    xmlChar end;
    xmlChar end2;
    xmlChar end3;

    if (!PyArg_ParseTuple(args, (char *)"Oiiccc:xmlDecodeEntities", &pyobj_ctxt, &len, &what, &end, &end2, &end3))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlDecodeEntities(ctxt, len, what, end, end2, end3);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlKeepBlanksDefault(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    int val;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlKeepBlanksDefault", &val))
        return(NULL);

    c_retval = xmlKeepBlanksDefault(val);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlDebugDumpDocument(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    FILE * output;
    PyObject *pyobj_output;
    xmlDocPtr doc;
    PyObject *pyobj_doc;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlDebugDumpDocument", &pyobj_output, &pyobj_doc))
        return(NULL);
    output = (FILE *) PyFile_Get(pyobj_output);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    xmlDebugDumpDocument(output, doc);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlCopyPropList(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlAttrPtr c_retval;
    xmlNodePtr target;
    PyObject *pyobj_target;
    xmlAttrPtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlCopyPropList", &pyobj_target, &pyobj_cur))
        return(NULL);
    target = (xmlNodePtr) PyxmlNode_Get(pyobj_target);
    cur = (xmlAttrPtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlCopyPropList(target, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlSetProp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlAttrPtr c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;
    xmlChar * name;
    xmlChar * value;

    if (!PyArg_ParseTuple(args, (char *)"Ozz:xmlSetProp", &pyobj_node, &name, &value))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlSetProp(node, name, value);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_htmlParseElement(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    htmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:htmlParseElement", &pyobj_ctxt))
        return(NULL);
    ctxt = (htmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    htmlParseElement(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlCreateEntityParserCtxt(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlParserCtxtPtr c_retval;
    xmlChar * URL;
    xmlChar * ID;
    xmlChar * base;

    if (!PyArg_ParseTuple(args, (char *)"zzz:xmlCreateEntityParserCtxt", &URL, &ID, &base))
        return(NULL);

    c_retval = xmlCreateEntityParserCtxt(URL, ID, base);
    py_retval = libxml_xmlParserCtxtPtrWrap((xmlParserCtxtPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathBooleanFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathBooleanFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathBooleanFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlNanoFTPScanProxy(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    char * URL;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlNanoFTPScanProxy", &URL))
        return(NULL);

    xmlNanoFTPScanProxy(URL);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlDebugDumpDocumentHead(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    FILE * output;
    PyObject *pyobj_output;
    xmlDocPtr doc;
    PyObject *pyobj_doc;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlDebugDumpDocumentHead", &pyobj_output, &pyobj_doc))
        return(NULL);
    output = (FILE *) PyFile_Get(pyobj_output);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    xmlDebugDumpDocumentHead(output, doc);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlNewParserCtxt(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {
    PyObject *py_retval;
    xmlParserCtxtPtr c_retval;

    c_retval = xmlNewParserCtxt();
    py_retval = libxml_xmlParserCtxtPtrWrap((xmlParserCtxtPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlGetDocCompressMode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlGetDocCompressMode", &pyobj_doc))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlGetDocCompressMode(doc);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNodeGetLang(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlNodeGetLang", &pyobj_cur))
        return(NULL);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlNodeGetLang(cur);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNextAttribute(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlXPathNextAttribute", &pyobj_ctxt, &pyobj_cur))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlXPathNextAttribute(ctxt, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlGetLineNo(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    long c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlGetLineNo", &pyobj_node))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlGetLineNo(node);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlDocSetRootElement(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlNodePtr root;
    PyObject *pyobj_root;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlDocSetRootElement", &pyobj_doc, &pyobj_root))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);
    root = (xmlNodePtr) PyxmlNode_Get(pyobj_root);

    c_retval = xmlDocSetRootElement(doc, root);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathValueFlipSign(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathValueFlipSign", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathValueFlipSign(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_htmlAutoCloseTag(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    htmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * name;
    htmlNodePtr elem;
    PyObject *pyobj_elem;

    if (!PyArg_ParseTuple(args, (char *)"OzO:htmlAutoCloseTag", &pyobj_doc, &name, &pyobj_elem))
        return(NULL);
    doc = (htmlDocPtr) PyxmlNode_Get(pyobj_doc);
    elem = (htmlNodePtr) PyxmlNode_Get(pyobj_elem);

    c_retval = htmlAutoCloseTag(doc, name, elem);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathCastBooleanToNumber(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    double c_retval;
    int val;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlXPathCastBooleanToNumber", &val))
        return(NULL);

    c_retval = xmlXPathCastBooleanToNumber(val);
    py_retval = libxml_doubleWrap((double) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseEncName(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseEncName", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParseEncName(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCatalogConvert(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {
    PyObject *py_retval;
    int c_retval;

    c_retval = xmlCatalogConvert();
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlURIGetScheme(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const char * c_retval;
    xmlURIPtr URI;
    PyObject *pyobj_URI;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlURIGetScheme", &pyobj_URI))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    c_retval = URI->scheme;
    py_retval = libxml_charPtrConstWrap((const char *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathIsNaN(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    double val;

    if (!PyArg_ParseTuple(args, (char *)"d:xmlXPathIsNaN", &val))
        return(NULL);

    c_retval = xmlXPathIsNaN(val);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlIsBlank(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    int c;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlIsBlank", &c))
        return(NULL);

    c_retval = xmlIsBlank(c);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathPopNumber(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    double c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathPopNumber", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    c_retval = xmlXPathPopNumber(ctxt);
    py_retval = libxml_doubleWrap((double) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlURIEscape(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlChar * str;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlURIEscape", &str))
        return(NULL);

    c_retval = xmlURIEscape(str);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCatalogResolveURI(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlChar * URI;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlCatalogResolveURI", &URI))
        return(NULL);

    c_retval = xmlCatalogResolveURI(URI);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNextSelf(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlXPathNextSelf", &pyobj_ctxt, &pyobj_cur))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlXPathNextSelf(ctxt, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNodeGetSpacePreserve(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlNodeGetSpacePreserve", &pyobj_cur))
        return(NULL);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlNodeGetSpacePreserve(cur);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathTranslateFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathTranslateFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathTranslateFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlCheckUTF8(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    unsigned char * utf;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlCheckUTF8", &utf))
        return(NULL);

    c_retval = xmlCheckUTF8(utf);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlRemoveID(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlAttrPtr attr;
    PyObject *pyobj_attr;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlRemoveID", &pyobj_doc, &pyobj_attr))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);
    attr = (xmlAttrPtr) PyxmlNode_Get(pyobj_attr);

    c_retval = xmlRemoveID(doc, attr);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNodeGetContent(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlNodeGetContent", &pyobj_cur))
        return(NULL);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlNodeGetContent(cur);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlClearParserCtxt(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlClearParserCtxt", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlClearParserCtxt(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlURIGetFragment(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const char * c_retval;
    xmlURIPtr URI;
    PyObject *pyobj_URI;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlURIGetFragment", &pyobj_URI))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    c_retval = URI->fragment;
    py_retval = libxml_charPtrConstWrap((const char *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathGetContextSize(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathGetContextSize", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);

    c_retval = ctxt->contextSize;
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathCastNumberToBoolean(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    double val;

    if (!PyArg_ParseTuple(args, (char *)"d:xmlXPathCastNumberToBoolean", &val))
        return(NULL);

    c_retval = xmlXPathCastNumberToBoolean(val);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathEval(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlXPathObjectPtr c_retval;
    xmlChar * str;
    xmlXPathContextPtr ctx;
    PyObject *pyobj_ctx;

    if (!PyArg_ParseTuple(args, (char *)"zO:xmlXPathEval", &str, &pyobj_ctx))
        return(NULL);
    ctx = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctx);

    c_retval = xmlXPathEval(str, ctx);
    py_retval = libxml_xmlXPathObjectPtrWrap((xmlXPathObjectPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlStrchr(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const xmlChar * c_retval;
    xmlChar * str;
    xmlChar val;

    if (!PyArg_ParseTuple(args, (char *)"zc:xmlStrchr", &str, &val))
        return(NULL);

    c_retval = xmlStrchr(str, val);
    py_retval = libxml_xmlCharPtrConstWrap((const xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNextParent(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlXPathNextParent", &pyobj_ctxt, &pyobj_cur))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlXPathNextParent(ctxt, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathRoot(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathRoot", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathRoot(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlCopyNodeList(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlCopyNodeList", &pyobj_node))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlCopyNodeList(node);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlSetTreeDoc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNodePtr tree;
    PyObject *pyobj_tree;
    xmlDocPtr doc;
    PyObject *pyobj_doc;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlSetTreeDoc", &pyobj_tree, &pyobj_doc))
        return(NULL);
    tree = (xmlNodePtr) PyxmlNode_Get(pyobj_tree);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    xmlSetTreeDoc(tree, doc);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlFreeURI(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlURIPtr uri;
    PyObject *pyobj_uri;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlFreeURI", &pyobj_uri))
        return(NULL);
    uri = (xmlURIPtr) PyURI_Get(pyobj_uri);

    xmlFreeURI(uri);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlRegisterHTTPPostCallbacks(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlRegisterHTTPPostCallbacks();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlTextConcat(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNodePtr node;
    PyObject *pyobj_node;
    xmlChar * content;
    int len;

    if (!PyArg_ParseTuple(args, (char *)"Ozi:xmlTextConcat", &pyobj_node, &content, &len))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    xmlTextConcat(node, content, len);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathNotFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathNotFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathNotFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParseVersionInfo(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseVersionInfo", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParseVersionInfo(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathSumFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathSumFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathSumFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlNewDocComment(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * content;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlNewDocComment", &pyobj_doc, &content))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlNewDocComment(doc, content);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_htmlFreeParserCtxt(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    htmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:htmlFreeParserCtxt", &pyobj_ctxt))
        return(NULL);
    ctxt = (htmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    htmlFreeParserCtxt(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlAddChildList(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlNodePtr parent;
    PyObject *pyobj_parent;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlAddChildList", &pyobj_parent, &pyobj_cur))
        return(NULL);
    parent = (xmlNodePtr) PyxmlNode_Get(pyobj_parent);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlAddChildList(parent, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlAddChild(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlNodePtr parent;
    PyObject *pyobj_parent;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlAddChild", &pyobj_parent, &pyobj_cur))
        return(NULL);
    parent = (xmlNodePtr) PyxmlNode_Get(pyobj_parent);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlAddChild(parent, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCharStrndup(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    char * cur;
    int len;

    if (!PyArg_ParseTuple(args, (char *)"zi:xmlCharStrndup", &cur, &len))
        return(NULL);

    c_retval = xmlCharStrndup(cur, len);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlStringDecodeEntities(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlChar * str;
    int what;
    xmlChar end;
    xmlChar end2;
    xmlChar end3;

    if (!PyArg_ParseTuple(args, (char *)"Oziccc:xmlStringDecodeEntities", &pyobj_ctxt, &str, &what, &end, &end2, &end3))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlStringDecodeEntities(ctxt, str, what, end, end2, end3);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNewTextChild(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlNodePtr parent;
    PyObject *pyobj_parent;
    xmlNsPtr ns;
    PyObject *pyobj_ns;
    xmlChar * name;
    xmlChar * content;

    if (!PyArg_ParseTuple(args, (char *)"OOzz:xmlNewTextChild", &pyobj_parent, &pyobj_ns, &name, &content))
        return(NULL);
    parent = (xmlNodePtr) PyxmlNode_Get(pyobj_parent);
    ns = (xmlNsPtr) PyxmlNode_Get(pyobj_ns);

    c_retval = xmlNewTextChild(parent, ns, name, content);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathContainsFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathContainsFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathContainsFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlCleanupPredefinedEntities(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlCleanupPredefinedEntities();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_valuePop(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlXPathObjectPtr c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:valuePop", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    c_retval = valuePop(ctxt);
    py_retval = libxml_xmlXPathObjectPtrWrap((xmlXPathObjectPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNextFollowing(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlXPathNextFollowing", &pyobj_ctxt, &pyobj_cur))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlXPathNextFollowing(ctxt, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathCastBooleanToString(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    int val;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlXPathCastBooleanToString", &val))
        return(NULL);

    c_retval = xmlXPathCastBooleanToString(val);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlBuildURI(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlChar * URI;
    xmlChar * base;

    if (!PyArg_ParseTuple(args, (char *)"zz:xmlBuildURI", &URI, &base))
        return(NULL);

    c_retval = xmlBuildURI(URI, base);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNanoFTPProxy(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    char * host;
    int port;
    char * user;
    char * passwd;
    int type;

    if (!PyArg_ParseTuple(args, (char *)"zizzi:xmlNanoFTPProxy", &host, &port, &user, &passwd, &type))
        return(NULL);

    xmlNanoFTPProxy(host, port, user, passwd, type);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlIsLetter(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    int c;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlIsLetter", &c))
        return(NULL);

    c_retval = xmlIsLetter(c);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathSetContextDoc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlDocPtr doc;
    PyObject *pyobj_doc;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlXPathSetContextDoc", &pyobj_ctxt, &pyobj_doc))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    ctxt->doc = doc;
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlStrsub(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlChar * str;
    int start;
    int len;

    if (!PyArg_ParseTuple(args, (char *)"zii:xmlStrsub", &str, &start, &len))
        return(NULL);

    c_retval = xmlStrsub(str, start, len);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathCompareValues(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int inf;
    int strict;

    if (!PyArg_ParseTuple(args, (char *)"Oii:xmlXPathCompareValues", &pyobj_ctxt, &inf, &strict))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    c_retval = xmlXPathCompareValues(ctxt, inf, strict);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlAddDocEntity(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlEntityPtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * name;
    int type;
    xmlChar * ExternalID;
    xmlChar * SystemID;
    xmlChar * content;

    if (!PyArg_ParseTuple(args, (char *)"Ozizzz:xmlAddDocEntity", &pyobj_doc, &name, &type, &ExternalID, &SystemID, &content))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlAddDocEntity(doc, name, type, ExternalID, SystemID, content);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNextFollowingSibling(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlXPathNextFollowingSibling", &pyobj_ctxt, &pyobj_cur))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlXPathNextFollowingSibling(ctxt, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlRecoverFile(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlDocPtr c_retval;
    char * filename;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlRecoverFile", &filename))
        return(NULL);

    c_retval = xmlRecoverFile(filename);
    py_retval = libxml_xmlDocPtrWrap((xmlDocPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathEqualValues(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathEqualValues", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    c_retval = xmlXPathEqualValues(ctxt);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathPopBoolean(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathPopBoolean", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    c_retval = xmlXPathPopBoolean(ctxt);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlURIGetOpaque(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const char * c_retval;
    xmlURIPtr URI;
    PyObject *pyobj_URI;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlURIGetOpaque", &pyobj_URI))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    c_retval = URI->opaque;
    py_retval = libxml_charPtrConstWrap((const char *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseExternalSubset(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlChar * ExternalID;
    xmlChar * SystemID;

    if (!PyArg_ParseTuple(args, (char *)"Ozz:xmlParseExternalSubset", &pyobj_ctxt, &ExternalID, &SystemID))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParseExternalSubset(ctxt, ExternalID, SystemID);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlStrcasestr(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const xmlChar * c_retval;
    xmlChar * str;
    xmlChar * val;

    if (!PyArg_ParseTuple(args, (char *)"zz:xmlStrcasestr", &str, &val))
        return(NULL);

    c_retval = xmlStrcasestr(str, val);
    py_retval = libxml_xmlCharPtrConstWrap((const xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParserSetValidate(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;
    int validate;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlParserSetValidate", &pyobj_ctxt, &validate))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    ctxt->validate = validate;
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathNumberFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathNumberFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathNumberFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParseStartTag(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseStartTag", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParseStartTag(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlUTF8Strloc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlChar * utf;
    xmlChar * utfchar;

    if (!PyArg_ParseTuple(args, (char *)"zz:xmlUTF8Strloc", &utf, &utfchar))
        return(NULL);

    c_retval = xmlUTF8Strloc(utf, utfchar);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlURISetScheme(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlURIPtr URI;
    PyObject *pyobj_URI;
    char * scheme;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlURISetScheme", &pyobj_URI, &scheme))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    if (URI->scheme != NULL) xmlFree(URI->scheme);
    URI->scheme = xmlStrdup((const xmlChar *)scheme);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlNewDtd(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlDtdPtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * name;
    xmlChar * ExternalID;
    xmlChar * SystemID;

    if (!PyArg_ParseTuple(args, (char *)"Ozzz:xmlNewDtd", &pyobj_doc, &name, &ExternalID, &SystemID))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlNewDtd(doc, name, ExternalID, SystemID);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlIsMixedElement(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * name;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlIsMixedElement", &pyobj_doc, &name))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlIsMixedElement(doc, name);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathGetFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const xmlChar * c_retval;
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathGetFunction", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);

    c_retval = ctxt->function;
    py_retval = libxml_xmlCharPtrConstWrap((const xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlDebugDumpString(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    FILE * output;
    PyObject *pyobj_output;
    xmlChar * str;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlDebugDumpString", &pyobj_output, &str))
        return(NULL);
    output = (FILE *) PyFile_Get(pyobj_output);

    xmlDebugDumpString(output, str);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlSaveFileEnc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    char * filename;
    xmlDocPtr cur;
    PyObject *pyobj_cur;
    char * encoding;

    if (!PyArg_ParseTuple(args, (char *)"zOz:xmlSaveFileEnc", &filename, &pyobj_cur, &encoding))
        return(NULL);
    cur = (xmlDocPtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlSaveFileEnc(filename, cur, encoding);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathSetContextNode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlNodePtr node;
    PyObject *pyobj_node;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlXPathSetContextNode", &pyobj_ctxt, &pyobj_node))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    ctxt->node = node;
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlCleanupOutputCallbacks(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlCleanupOutputCallbacks();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlUTF8Strsize(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlChar * utf;
    int len;

    if (!PyArg_ParseTuple(args, (char *)"zi:xmlUTF8Strsize", &utf, &len))
        return(NULL);

    c_retval = xmlUTF8Strsize(utf, len);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlDebugDumpAttr(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    FILE * output;
    PyObject *pyobj_output;
    xmlAttrPtr attr;
    PyObject *pyobj_attr;
    int depth;

    if (!PyArg_ParseTuple(args, (char *)"OOi:xmlDebugDumpAttr", &pyobj_output, &pyobj_attr, &depth))
        return(NULL);
    output = (FILE *) PyFile_Get(pyobj_output);
    attr = (xmlAttrPtr) PyxmlNode_Get(pyobj_attr);

    xmlDebugDumpAttr(output, attr, depth);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathCmpNodes(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlNodePtr node1;
    PyObject *pyobj_node1;
    xmlNodePtr node2;
    PyObject *pyobj_node2;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlXPathCmpNodes", &pyobj_node1, &pyobj_node2))
        return(NULL);
    node1 = (xmlNodePtr) PyxmlNode_Get(pyobj_node1);
    node2 = (xmlNodePtr) PyxmlNode_Get(pyobj_node2);

    c_retval = xmlXPathCmpNodes(node1, node2);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathStringEvalNumber(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    double c_retval;
    xmlChar * str;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlXPathStringEvalNumber", &str))
        return(NULL);

    c_retval = xmlXPathStringEvalNumber(str);
    py_retval = libxml_doubleWrap((double) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParserSetReplaceEntities(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;
    int replaceEntities;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlParserSetReplaceEntities", &pyobj_ctxt, &replaceEntities))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    ctxt->replaceEntities = replaceEntities;
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlIsExtender(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    int c;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlIsExtender", &c))
        return(NULL);

    c_retval = xmlIsExtender(c);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathRegisterNs(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlChar * prefix;
    xmlChar * ns_uri;

    if (!PyArg_ParseTuple(args, (char *)"Ozz:xmlXPathRegisterNs", &pyobj_ctxt, &prefix, &ns_uri))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);

    c_retval = xmlXPathRegisterNs(ctxt, prefix, ns_uri);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_namePush(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlChar * value;

    if (!PyArg_ParseTuple(args, (char *)"Oz:namePush", &pyobj_ctxt, &value))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = namePush(ctxt, value);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlIsRef(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlNodePtr elem;
    PyObject *pyobj_elem;
    xmlAttrPtr attr;
    PyObject *pyobj_attr;

    if (!PyArg_ParseTuple(args, (char *)"OOO:xmlIsRef", &pyobj_doc, &pyobj_elem, &pyobj_attr))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);
    elem = (xmlNodePtr) PyxmlNode_Get(pyobj_elem);
    attr = (xmlAttrPtr) PyxmlNode_Get(pyobj_attr);

    c_retval = xmlIsRef(doc, elem, attr);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathCastStringToNumber(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    double c_retval;
    xmlChar * val;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlXPathCastStringToNumber", &val))
        return(NULL);

    c_retval = xmlXPathCastStringToNumber(val);
    py_retval = libxml_doubleWrap((double) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlDocCopyNode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    int recursive;

    if (!PyArg_ParseTuple(args, (char *)"OOi:xmlDocCopyNode", &pyobj_node, &pyobj_doc, &recursive))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlDocCopyNode(node, doc, recursive);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCharStrdup(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    char * cur;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlCharStrdup", &cur))
        return(NULL);

    c_retval = xmlCharStrdup(cur);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlStrcasecmp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlChar * str1;
    xmlChar * str2;

    if (!PyArg_ParseTuple(args, (char *)"zz:xmlStrcasecmp", &str1, &str2))
        return(NULL);

    c_retval = xmlStrcasecmp(str1, str2);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNewCDataBlock(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * content;
    int len;

    if (!PyArg_ParseTuple(args, (char *)"Ozi:xmlNewCDataBlock", &pyobj_doc, &content, &len))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlNewCDataBlock(doc, content, len);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCatalogResolvePublic(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlChar * pubID;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlCatalogResolvePublic", &pubID))
        return(NULL);

    c_retval = xmlCatalogResolvePublic(pubID);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlUTF8Strpos(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlChar * utf;
    int pos;

    if (!PyArg_ParseTuple(args, (char *)"zi:xmlUTF8Strpos", &utf, &pos))
        return(NULL);

    c_retval = xmlUTF8Strpos(utf, pos);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlURISetQuery(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlURIPtr URI;
    PyObject *pyobj_URI;
    char * query;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlURISetQuery", &pyobj_URI, &query))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    if (URI->query != NULL) xmlFree(URI->query);
    URI->query = xmlStrdup((const xmlChar *)query);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlNewComment(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlChar * content;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlNewComment", &content))
        return(NULL);

    c_retval = xmlNewComment(content);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlIsCombining(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    int c;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlIsCombining", &c))
        return(NULL);

    c_retval = xmlIsCombining(c);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNextDescendant(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlXPathNextDescendant", &pyobj_ctxt, &pyobj_cur))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlXPathNextDescendant(ctxt, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseXMLDecl(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseXMLDecl", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParseXMLDecl(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlACatalogResolve(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlCatalogPtr catal;
    PyObject *pyobj_catal;
    xmlChar * pubID;
    xmlChar * sysID;

    if (!PyArg_ParseTuple(args, (char *)"Ozz:xmlACatalogResolve", &pyobj_catal, &pubID, &sysID))
        return(NULL);
    catal = (xmlCatalogPtr) Pycatalog_Get(pyobj_catal);

    c_retval = xmlACatalogResolve(catal, pubID, sysID);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNodeSetContentLen(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNodePtr cur;
    PyObject *pyobj_cur;
    xmlChar * content;
    int len;

    if (!PyArg_ParseTuple(args, (char *)"Ozi:xmlNodeSetContentLen", &pyobj_cur, &content, &len))
        return(NULL);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    xmlNodeSetContentLen(cur, content, len);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlURISetUser(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlURIPtr URI;
    PyObject *pyobj_URI;
    char * user;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlURISetUser", &pyobj_URI, &user))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    if (URI->user != NULL) xmlFree(URI->user);
    URI->user = xmlStrdup((const xmlChar *)user);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathFreeContext(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathFreeContext", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);

    xmlXPathFreeContext(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlDelEncodingAlias(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    char * alias;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlDelEncodingAlias", &alias))
        return(NULL);

    c_retval = xmlDelEncodingAlias(alias);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNormalizeFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathNormalizeFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathNormalizeFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParsePEReference(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParsePEReference", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParsePEReference(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParserSetLineNumbers(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;
    int linenumbers;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlParserSetLineNumbers", &pyobj_ctxt, &linenumbers))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    ctxt->linenumbers = linenumbers;
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlStrEqual(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlChar * str1;
    xmlChar * str2;

    if (!PyArg_ParseTuple(args, (char *)"zz:xmlStrEqual", &str1, &str2))
        return(NULL);

    c_retval = xmlStrEqual(str1, str2);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathRegisteredNsCleanup(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathRegisteredNsCleanup", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);

    xmlXPathRegisteredNsCleanup(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlURISetOpaque(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlURIPtr URI;
    PyObject *pyobj_URI;
    char * opaque;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlURISetOpaque", &pyobj_URI, &opaque))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    if (URI->opaque != NULL) xmlFree(URI->opaque);
    URI->opaque = xmlStrdup((const xmlChar *)opaque);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParsePITarget(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParsePITarget", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParsePITarget(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNextChild(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlXPathNextChild", &pyobj_ctxt, &pyobj_cur))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlXPathNextChild(ctxt, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlSaveFormatFile(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    char * filename;
    xmlDocPtr cur;
    PyObject *pyobj_cur;
    int format;

    if (!PyArg_ParseTuple(args, (char *)"zOi:xmlSaveFormatFile", &filename, &pyobj_cur, &format))
        return(NULL);
    cur = (xmlDocPtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlSaveFormatFile(filename, cur, format);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNodeIsText(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlNodeIsText", &pyobj_node))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlNodeIsText(node);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseEndTag(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseEndTag", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParseEndTag(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathGetContextPosition(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathGetContextPosition", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);

    c_retval = ctxt->proximityPosition;
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCatalogGetPublic(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const xmlChar * c_retval;
    xmlChar * pubID;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlCatalogGetPublic", &pubID))
        return(NULL);

    c_retval = xmlCatalogGetPublic(pubID);
    py_retval = libxml_xmlCharPtrConstWrap((const xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlFreeNode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlFreeNode", &pyobj_cur))
        return(NULL);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    xmlFreeNode(cur);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlURISetAuthority(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlURIPtr URI;
    PyObject *pyobj_URI;
    char * authority;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlURISetAuthority", &pyobj_URI, &authority))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    if (URI->authority != NULL) xmlFree(URI->authority);
    URI->authority = xmlStrdup((const xmlChar *)authority);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlStrncmp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlChar * str1;
    xmlChar * str2;
    int len;

    if (!PyArg_ParseTuple(args, (char *)"zzi:xmlStrncmp", &str1, &str2, &len))
        return(NULL);

    c_retval = xmlStrncmp(str1, str2, len);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseDocument(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseDocument", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParseDocument(ctxt);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseFile(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlDocPtr c_retval;
    char * filename;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlParseFile", &filename))
        return(NULL);

    c_retval = xmlParseFile(filename);
    py_retval = libxml_xmlDocPtrWrap((xmlDocPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNewParserContext(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlXPathParserContextPtr c_retval;
    xmlChar * str;
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"zO:xmlXPathNewParserContext", &str, &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);

    c_retval = xmlXPathNewParserContext(str, ctxt);
    py_retval = libxml_xmlXPathParserContextPtrWrap((xmlXPathParserContextPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlACatalogDump(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlCatalogPtr catal;
    PyObject *pyobj_catal;
    FILE * out;
    PyObject *pyobj_out;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlACatalogDump", &pyobj_catal, &pyobj_out))
        return(NULL);
    catal = (xmlCatalogPtr) Pycatalog_Get(pyobj_catal);
    out = (FILE *) PyFile_Get(pyobj_out);

    xmlACatalogDump(catal, out);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlRemoveProp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlAttrPtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlRemoveProp", &pyobj_cur))
        return(NULL);
    cur = (xmlAttrPtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlRemoveProp(cur);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseElement(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseElement", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParseElement(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathMultValues(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathMultValues", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathMultValues(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlEncodeEntitiesReentrant(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * input;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlEncodeEntitiesReentrant", &pyobj_doc, &input))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlEncodeEntitiesReentrant(doc, input);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlSetDocCompressMode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    int mode;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlSetDocCompressMode", &pyobj_doc, &mode))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    xmlSetDocCompressMode(doc, mode);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlGetParameterEntity(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlEntityPtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * name;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlGetParameterEntity", &pyobj_doc, &name))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlGetParameterEntity(doc, name);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlLineNumbersDefault(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    int val;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlLineNumbersDefault", &val))
        return(NULL);

    c_retval = xmlLineNumbersDefault(val);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlReplaceNode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlNodePtr old;
    PyObject *pyobj_old;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlReplaceNode", &pyobj_old, &pyobj_cur))
        return(NULL);
    old = (xmlNodePtr) PyxmlNode_Get(pyobj_old);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlReplaceNode(old, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCopyProp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlAttrPtr c_retval;
    xmlNodePtr target;
    PyObject *pyobj_target;
    xmlAttrPtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlCopyProp", &pyobj_target, &pyobj_cur))
        return(NULL);
    target = (xmlNodePtr) PyxmlNode_Get(pyobj_target);
    cur = (xmlAttrPtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlCopyProp(target, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathParseNCName(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathParseNCName", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    c_retval = xmlXPathParseNCName(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseURI(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlURIPtr c_retval;
    char * str;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlParseURI", &str))
        return(NULL);

    c_retval = xmlParseURI(str);
    py_retval = libxml_xmlURIPtrWrap((xmlURIPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseDoc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlDocPtr c_retval;
    xmlChar * cur;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlParseDoc", &cur))
        return(NULL);

    c_retval = xmlParseDoc(cur);
    py_retval = libxml_xmlDocPtrWrap((xmlDocPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlPedanticParserDefault(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    int val;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlPedanticParserDefault", &val))
        return(NULL);

    c_retval = xmlPedanticParserDefault(val);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlURIUnescapeString(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    char * c_retval;
    char * str;
    int len;
    char * target;

    if (!PyArg_ParseTuple(args, (char *)"ziz:xmlURIUnescapeString", &str, &len, &target))
        return(NULL);

    c_retval = xmlURIUnescapeString(str, len, target);
    py_retval = libxml_charPtrWrap((char *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlIsIdeographic(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    int c;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlIsIdeographic", &c))
        return(NULL);

    c_retval = xmlIsIdeographic(c);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlDocDump(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    FILE * f;
    PyObject *pyobj_f;
    xmlDocPtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlDocDump", &pyobj_f, &pyobj_cur))
        return(NULL);
    f = (FILE *) PyFile_Get(pyobj_f);
    cur = (xmlDocPtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlDocDump(f, cur);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlIsChar(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    int c;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlIsChar", &c))
        return(NULL);

    c_retval = xmlIsChar(c);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlPrintURI(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    FILE * stream;
    PyObject *pyobj_stream;
    xmlURIPtr uri;
    PyObject *pyobj_uri;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlPrintURI", &pyobj_stream, &pyobj_uri))
        return(NULL);
    stream = (FILE *) PyFile_Get(pyobj_stream);
    uri = (xmlURIPtr) PyURI_Get(pyobj_uri);

    xmlPrintURI(stream, uri);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlURIEscapeStr(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlChar * str;
    xmlChar * list;

    if (!PyArg_ParseTuple(args, (char *)"zz:xmlURIEscapeStr", &str, &list))
        return(NULL);

    c_retval = xmlURIEscapeStr(str, list);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlTextMerge(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlNodePtr first;
    PyObject *pyobj_first;
    xmlNodePtr second;
    PyObject *pyobj_second;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlTextMerge", &pyobj_first, &pyobj_second))
        return(NULL);
    first = (xmlNodePtr) PyxmlNode_Get(pyobj_first);
    second = (xmlNodePtr) PyxmlNode_Get(pyobj_second);

    c_retval = xmlTextMerge(first, second);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlSaveUri(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlURIPtr uri;
    PyObject *pyobj_uri;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlSaveUri", &pyobj_uri))
        return(NULL);
    uri = (xmlURIPtr) PyURI_Get(pyobj_uri);

    c_retval = xmlSaveUri(uri);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlStrlen(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlChar * str;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlStrlen", &str))
        return(NULL);

    c_retval = xmlStrlen(str);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNewGlobalNs(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNsPtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * href;
    xmlChar * prefix;

    if (!PyArg_ParseTuple(args, (char *)"Ozz:xmlNewGlobalNs", &pyobj_doc, &href, &prefix))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlNewGlobalNs(doc, href, prefix);
    py_retval = libxml_xmlNsPtrWrap((xmlNsPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlSetNsProp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlAttrPtr c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;
    xmlNsPtr ns;
    PyObject *pyobj_ns;
    xmlChar * name;
    xmlChar * value;

    if (!PyArg_ParseTuple(args, (char *)"OOzz:xmlSetNsProp", &pyobj_node, &pyobj_ns, &name, &value))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);
    ns = (xmlNsPtr) PyxmlNode_Get(pyobj_ns);

    c_retval = xmlSetNsProp(node, ns, name, value);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseDTD(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlDtdPtr c_retval;
    xmlChar * ExternalID;
    xmlChar * SystemID;

    if (!PyArg_ParseTuple(args, (char *)"zz:xmlParseDTD", &ExternalID, &SystemID))
        return(NULL);

    c_retval = xmlParseDTD(ExternalID, SystemID);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_htmlParseChunk(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    htmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;
    char * chunk;
    int size;
    int terminate;

    if (!PyArg_ParseTuple(args, (char *)"Ozii:htmlParseChunk", &pyobj_ctxt, &chunk, &size, &terminate))
        return(NULL);
    ctxt = (htmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = htmlParseChunk(ctxt, chunk, size, terminate);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlFreeParserCtxt(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlFreeParserCtxt", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlFreeParserCtxt(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlGetNodePath(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlGetNodePath", &pyobj_node))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlGetNodePath(node);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlSkipBlankChars(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlSkipBlankChars", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlSkipBlankChars(ctxt);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlHandleEntity(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlEntityPtr entity;
    PyObject *pyobj_entity;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlHandleEntity", &pyobj_ctxt, &pyobj_entity))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);
    entity = (xmlEntityPtr) PyxmlNode_Get(pyobj_entity);

    xmlHandleEntity(ctxt, entity);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlURISetFragment(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlURIPtr URI;
    PyObject *pyobj_URI;
    char * fragment;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlURISetFragment", &pyobj_URI, &fragment))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    if (URI->fragment != NULL) xmlFree(URI->fragment);
    URI->fragment = xmlStrdup((const xmlChar *)fragment);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlNamespaceParseNCName(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlNamespaceParseNCName", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlNamespaceParseNCName(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathRegisteredFuncsCleanup(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathRegisteredFuncsCleanup", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);

    xmlXPathRegisteredFuncsCleanup(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathTrueFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathTrueFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathTrueFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlNanoHTTPCleanup(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlNanoHTTPCleanup();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathNextPreceding(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlXPathNextPreceding", &pyobj_ctxt, &pyobj_cur))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlXPathNextPreceding(ctxt, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseTextDecl(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseTextDecl", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParseTextDecl(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlSearchNsByHref(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNsPtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlNodePtr node;
    PyObject *pyobj_node;
    xmlChar * href;

    if (!PyArg_ParseTuple(args, (char *)"OOz:xmlSearchNsByHref", &pyobj_doc, &pyobj_node, &href))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlSearchNsByHref(doc, node, href);
    py_retval = libxml_xmlNsPtrWrap((xmlNsPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCopyNamespace(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNsPtr c_retval;
    xmlNsPtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlCopyNamespace", &pyobj_cur))
        return(NULL);
    cur = (xmlNsPtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlCopyNamespace(cur);
    py_retval = libxml_xmlNsPtrWrap((xmlNsPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCreateFileParserCtxt(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlParserCtxtPtr c_retval;
    char * filename;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlCreateFileParserCtxt", &filename))
        return(NULL);

    c_retval = xmlCreateFileParserCtxt(filename);
    py_retval = libxml_xmlParserCtxtPtrWrap((xmlParserCtxtPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCleanupEncodingAliases(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlCleanupEncodingAliases();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathStartsWithFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathStartsWithFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathStartsWithFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlInitParser(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlInitParser();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathCeilingFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathCeilingFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathCeilingFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParseURIReference(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlURIPtr uri;
    PyObject *pyobj_uri;
    char * str;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlParseURIReference", &pyobj_uri, &str))
        return(NULL);
    uri = (xmlURIPtr) PyURI_Get(pyobj_uri);

    c_retval = xmlParseURIReference(uri, str);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNodeSetBase(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNodePtr cur;
    PyObject *pyobj_cur;
    xmlChar * uri;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlNodeSetBase", &pyobj_cur, &uri))
        return(NULL);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    xmlNodeSetBase(cur, uri);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParseMemory(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlDocPtr c_retval;
    char * buffer;
    int size;

    if (!PyArg_ParseTuple(args, (char *)"zi:xmlParseMemory", &buffer, &size))
        return(NULL);

    c_retval = xmlParseMemory(buffer, size);
    py_retval = libxml_xmlDocPtrWrap((xmlDocPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPatherror(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    char * file;
    int line;
    int no;

    if (!PyArg_ParseTuple(args, (char *)"Ozii:xmlXPatherror", &pyobj_ctxt, &file, &line, &no))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPatherror(ctxt, file, line, no);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParseContent(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseContent", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParseContent(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlInitCharEncodingHandlers(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlInitCharEncodingHandlers();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_namePop(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:namePop", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = namePop(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_htmlSaveFileEnc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    char * filename;
    xmlDocPtr cur;
    PyObject *pyobj_cur;
    char * encoding;

    if (!PyArg_ParseTuple(args, (char *)"zOz:htmlSaveFileEnc", &filename, &pyobj_cur, &encoding))
        return(NULL);
    cur = (xmlDocPtr) PyxmlNode_Get(pyobj_cur);

    c_retval = htmlSaveFileEnc(filename, cur, encoding);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlStrncat(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlChar * cur;
    xmlChar * add;
    int len;

    if (!PyArg_ParseTuple(args, (char *)"zzi:xmlStrncat", &cur, &add, &len))
        return(NULL);

    c_retval = xmlStrncat(cur, add, len);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseSDDecl(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseSDDecl", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParseSDDecl(ctxt);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNamespaceURIFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathNamespaceURIFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathNamespaceURIFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlStrdup(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlChar * cur;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlStrdup", &cur))
        return(NULL);

    c_retval = xmlStrdup(cur);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNewDocFragment(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlNewDocFragment", &pyobj_doc))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlNewDocFragment(doc);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseMisc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseMisc", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParseMisc(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParserHandleReference(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParserHandleReference", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParserHandleReference(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParseNotationDecl(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseNotationDecl", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParseNotationDecl(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlAddDtdEntity(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlEntityPtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * name;
    int type;
    xmlChar * ExternalID;
    xmlChar * SystemID;
    xmlChar * content;

    if (!PyArg_ParseTuple(args, (char *)"Ozizzz:xmlAddDtdEntity", &pyobj_doc, &name, &type, &ExternalID, &SystemID, &content))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlAddDtdEntity(doc, name, type, ExternalID, SystemID, content);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlACatalogAdd(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlCatalogPtr catal;
    PyObject *pyobj_catal;
    xmlChar * type;
    xmlChar * orig;
    xmlChar * replace;

    if (!PyArg_ParseTuple(args, (char *)"Ozzz:xmlACatalogAdd", &pyobj_catal, &type, &orig, &replace))
        return(NULL);
    catal = (xmlCatalogPtr) Pycatalog_Get(pyobj_catal);

    c_retval = xmlACatalogAdd(catal, type, orig, replace);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlGetEncodingAlias(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const char * c_retval;
    char * alias;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlGetEncodingAlias", &alias))
        return(NULL);

    c_retval = xmlGetEncodingAlias(alias);
    py_retval = libxml_charPtrConstWrap((const char *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlGetLastChild(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlNodePtr parent;
    PyObject *pyobj_parent;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlGetLastChild", &pyobj_parent))
        return(NULL);
    parent = (xmlNodePtr) PyxmlNode_Get(pyobj_parent);

    c_retval = xmlGetLastChild(parent);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlURIGetUser(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const char * c_retval;
    xmlURIPtr URI;
    PyObject *pyobj_URI;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlURIGetUser", &pyobj_URI))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    c_retval = URI->user;
    py_retval = libxml_charPtrConstWrap((const char *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNanoHTTPScanProxy(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    char * URL;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlNanoHTTPScanProxy", &URL))
        return(NULL);

    xmlNanoHTTPScanProxy(URL);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathGetFunctionURI(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const xmlChar * c_retval;
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathGetFunctionURI", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);

    c_retval = ctxt->functionURI;
    py_retval = libxml_xmlCharPtrConstWrap((const xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlSetListDoc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNodePtr list;
    PyObject *pyobj_list;
    xmlDocPtr doc;
    PyObject *pyobj_doc;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlSetListDoc", &pyobj_list, &pyobj_doc))
        return(NULL);
    list = (xmlNodePtr) PyxmlNode_Get(pyobj_list);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    xmlSetListDoc(list, doc);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlGetID(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlAttrPtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * ID;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlGetID", &pyobj_doc, &ID))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlGetID(doc, ID);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseCatalogFile(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlDocPtr c_retval;
    char * filename;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlParseCatalogFile", &filename))
        return(NULL);

    c_retval = xmlParseCatalogFile(filename);
    py_retval = libxml_xmlDocPtrWrap((xmlDocPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlLsCountNode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlLsCountNode", &pyobj_node))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlLsCountNode(node);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathDivValues(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathDivValues", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathDivValues(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlFreeNodeList(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlFreeNodeList", &pyobj_cur))
        return(NULL);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    xmlFreeNodeList(cur);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlFreeDtd(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlDtdPtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlFreeDtd", &pyobj_cur))
        return(NULL);
    cur = (xmlDtdPtr) PyxmlNode_Get(pyobj_cur);

    xmlFreeDtd(cur);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlSubstituteEntitiesDefault(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    int val;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlSubstituteEntitiesDefault", &val))
        return(NULL);

    c_retval = xmlSubstituteEntitiesDefault(val);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNewFloat(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlXPathObjectPtr c_retval;
    double val;

    if (!PyArg_ParseTuple(args, (char *)"d:xmlXPathNewFloat", &val))
        return(NULL);

    c_retval = xmlXPathNewFloat(val);
    py_retval = libxml_xmlXPathObjectPtrWrap((xmlXPathObjectPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNewValueTree(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlXPathObjectPtr c_retval;
    xmlNodePtr val;
    PyObject *pyobj_val;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathNewValueTree", &pyobj_val))
        return(NULL);
    val = (xmlNodePtr) PyxmlNode_Get(pyobj_val);

    c_retval = xmlXPathNewValueTree(val);
    py_retval = libxml_xmlXPathObjectPtrWrap((xmlXPathObjectPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_htmlDocDump(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    FILE * f;
    PyObject *pyobj_f;
    xmlDocPtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:htmlDocDump", &pyobj_f, &pyobj_cur))
        return(NULL);
    f = (FILE *) PyFile_Get(pyobj_f);
    cur = (xmlDocPtr) PyxmlNode_Get(pyobj_cur);

    c_retval = htmlDocDump(f, cur);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCreateDocParserCtxt(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlParserCtxtPtr c_retval;
    xmlChar * cur;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlCreateDocParserCtxt", &cur))
        return(NULL);

    c_retval = xmlCreateDocParserCtxt(cur);
    py_retval = libxml_xmlParserCtxtPtrWrap((xmlParserCtxtPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlFreeCatalog(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlCatalogPtr catal;
    PyObject *pyobj_catal;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlFreeCatalog", &pyobj_catal))
        return(NULL);
    catal = (xmlCatalogPtr) Pycatalog_Get(pyobj_catal);

    xmlFreeCatalog(catal);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlURISetServer(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlURIPtr URI;
    PyObject *pyobj_URI;
    char * server;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlURISetServer", &pyobj_URI, &server))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    if (URI->server != NULL) xmlFree(URI->server);
    URI->server = xmlStrdup((const xmlChar *)server);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlACatalogResolveSystem(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlCatalogPtr catal;
    PyObject *pyobj_catal;
    xmlChar * sysID;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlACatalogResolveSystem", &pyobj_catal, &sysID))
        return(NULL);
    catal = (xmlCatalogPtr) Pycatalog_Get(pyobj_catal);

    c_retval = xmlACatalogResolveSystem(catal, sysID);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNewDocText(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * content;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlNewDocText", &pyobj_doc, &content))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlNewDocText(doc, content);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlStopParser(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlStopParser", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlStopParser(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathNextNamespace(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlXPathNextNamespace", &pyobj_ctxt, &pyobj_cur))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlXPathNextNamespace(ctxt, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlACatalogResolvePublic(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlCatalogPtr catal;
    PyObject *pyobj_catal;
    xmlChar * pubID;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlACatalogResolvePublic", &pyobj_catal, &pubID))
        return(NULL);
    catal = (xmlCatalogPtr) Pycatalog_Get(pyobj_catal);

    c_retval = xmlACatalogResolvePublic(catal, pubID);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlSaveFile(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    char * filename;
    xmlDocPtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"zO:xmlSaveFile", &filename, &pyobj_cur))
        return(NULL);
    cur = (xmlDocPtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlSaveFile(filename, cur);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNewText(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlChar * content;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlNewText", &content))
        return(NULL);

    c_retval = xmlNewText(content);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlStringGetNodeList(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * value;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlStringGetNodeList", &pyobj_doc, &value))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlStringGetNodeList(doc, value);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlGetPredefinedEntity(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlEntityPtr c_retval;
    xmlChar * name;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlGetPredefinedEntity", &name))
        return(NULL);

    c_retval = xmlGetPredefinedEntity(name);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathStringLengthFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathStringLengthFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathStringLengthFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathCastNodeToNumber(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    double c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathCastNodeToNumber", &pyobj_node))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlXPathCastNodeToNumber(node);
    py_retval = libxml_doubleWrap((double) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseQuotedString(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseQuotedString", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParseQuotedString(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlUTF8Strsub(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlChar * utf;
    int start;
    int len;

    if (!PyArg_ParseTuple(args, (char *)"zii:xmlUTF8Strsub", &utf, &start, &len))
        return(NULL);

    c_retval = xmlUTF8Strsub(utf, start, len);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathSubValues(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathSubValues", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathSubValues(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlNewCharRef(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * name;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlNewCharRef", &pyobj_doc, &name))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlNewCharRef(doc, name);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNextDescendantOrSelf(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlXPathNextDescendantOrSelf", &pyobj_ctxt, &pyobj_cur))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlXPathNextDescendantOrSelf(ctxt, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathVariableLookupNS(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlXPathObjectPtr c_retval;
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlChar * name;
    xmlChar * ns_uri;

    if (!PyArg_ParseTuple(args, (char *)"Ozz:xmlXPathVariableLookupNS", &pyobj_ctxt, &name, &ns_uri))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);

    c_retval = xmlXPathVariableLookupNS(ctxt, name, ns_uri);
    py_retval = libxml_xmlXPathObjectPtrWrap((xmlXPathObjectPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCreateIntSubset(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlDtdPtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * name;
    xmlChar * ExternalID;
    xmlChar * SystemID;

    if (!PyArg_ParseTuple(args, (char *)"Ozzz:xmlCreateIntSubset", &pyobj_doc, &name, &ExternalID, &SystemID))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlCreateIntSubset(doc, name, ExternalID, SystemID);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlURIGetAuthority(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const char * c_retval;
    xmlURIPtr URI;
    PyObject *pyobj_URI;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlURIGetAuthority", &pyobj_URI))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    c_retval = URI->authority;
    py_retval = libxml_charPtrConstWrap((const char *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNewContext(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlXPathContextPtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathNewContext", &pyobj_doc))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlXPathNewContext(doc);
    py_retval = libxml_xmlXPathContextPtrWrap((xmlXPathContextPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNodeSetSpacePreserve(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNodePtr cur;
    PyObject *pyobj_cur;
    int val;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlNodeSetSpacePreserve", &pyobj_cur, &val))
        return(NULL);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    xmlNodeSetSpacePreserve(cur, val);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlValidNormalizeAttributeValue(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlNodePtr elem;
    PyObject *pyobj_elem;
    xmlChar * name;
    xmlChar * value;

    if (!PyArg_ParseTuple(args, (char *)"OOzz:xmlValidNormalizeAttributeValue", &pyobj_doc, &pyobj_elem, &name, &value))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);
    elem = (xmlNodePtr) PyxmlNode_Get(pyobj_elem);

    c_retval = xmlValidNormalizeAttributeValue(doc, elem, name, value);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlConvertSGMLCatalog(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlCatalogPtr catal;
    PyObject *pyobj_catal;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlConvertSGMLCatalog", &pyobj_catal))
        return(NULL);
    catal = (xmlCatalogPtr) Pycatalog_Get(pyobj_catal);

    c_retval = xmlConvertSGMLCatalog(catal);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlShellPrintNode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNodePtr node;
    PyObject *pyobj_node;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlShellPrintNode", &pyobj_node))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    xmlShellPrintNode(node);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathLangFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathLangFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathLangFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlSearchNs(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNsPtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlNodePtr node;
    PyObject *pyobj_node;
    xmlChar * nameSpace;

    if (!PyArg_ParseTuple(args, (char *)"OOz:xmlSearchNs", &pyobj_doc, &pyobj_node, &nameSpace))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlSearchNs(doc, node, nameSpace);
    py_retval = libxml_xmlNsPtrWrap((xmlNsPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNanoFTPInit(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlNanoFTPInit();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_htmlIsAutoClosed(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    htmlDocPtr doc;
    PyObject *pyobj_doc;
    htmlNodePtr elem;
    PyObject *pyobj_elem;

    if (!PyArg_ParseTuple(args, (char *)"OO:htmlIsAutoClosed", &pyobj_doc, &pyobj_elem))
        return(NULL);
    doc = (htmlDocPtr) PyxmlNode_Get(pyobj_doc);
    elem = (htmlNodePtr) PyxmlNode_Get(pyobj_elem);

    c_retval = htmlIsAutoClosed(doc, elem);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlInitializePredefinedEntities(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlInitializePredefinedEntities();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathCastStringToBoolean(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlChar * val;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlXPathCastStringToBoolean", &val))
        return(NULL);

    c_retval = xmlXPathCastStringToBoolean(val);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCopyNode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;
    int recursive;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlCopyNode", &pyobj_node, &recursive))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlCopyNode(node, recursive);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_htmlNewDoc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    htmlDocPtr c_retval;
    xmlChar * URI;
    xmlChar * ExternalID;

    if (!PyArg_ParseTuple(args, (char *)"zz:htmlNewDoc", &URI, &ExternalID))
        return(NULL);

    c_retval = htmlNewDoc(URI, ExternalID);
    py_retval = libxml_xmlDocPtrWrap((xmlDocPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCatalogAdd(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlChar * type;
    xmlChar * orig;
    xmlChar * replace;

    if (!PyArg_ParseTuple(args, (char *)"zzz:xmlCatalogAdd", &type, &orig, &replace))
        return(NULL);

    c_retval = xmlCatalogAdd(type, orig, replace);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_nodePush(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlNodePtr value;
    PyObject *pyobj_value;

    if (!PyArg_ParseTuple(args, (char *)"OO:nodePush", &pyobj_ctxt, &pyobj_value))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);
    value = (xmlNodePtr) PyxmlNode_Get(pyobj_value);

    c_retval = nodePush(ctxt, value);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseName(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseName", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParseName(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_htmlHandleOmittedElem(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    int val;

    if (!PyArg_ParseTuple(args, (char *)"i:htmlHandleOmittedElem", &val))
        return(NULL);

    c_retval = htmlHandleOmittedElem(val);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCheckVersion(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    int version;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlCheckVersion", &version))
        return(NULL);

    xmlCheckVersion(version);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParseAttributeListDecl(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseAttributeListDecl", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParseAttributeListDecl(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlCreateMemoryParserCtxt(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlParserCtxtPtr c_retval;
    char * buffer;
    int size;

    if (!PyArg_ParseTuple(args, (char *)"zi:xmlCreateMemoryParserCtxt", &buffer, &size))
        return(NULL);

    c_retval = xmlCreateMemoryParserCtxt(buffer, size);
    py_retval = libxml_xmlParserCtxtPtrWrap((xmlParserCtxtPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlStrcmp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlChar * str1;
    xmlChar * str2;

    if (!PyArg_ParseTuple(args, (char *)"zz:xmlStrcmp", &str1, &str2))
        return(NULL);

    c_retval = xmlStrcmp(str1, str2);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseAttValue(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseAttValue", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParseAttValue(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseSystemLiteral(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseSystemLiteral", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParseSystemLiteral(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlFreeProp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlAttrPtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlFreeProp", &pyobj_cur))
        return(NULL);
    cur = (xmlAttrPtr) PyxmlNode_Get(pyobj_cur);

    xmlFreeProp(cur);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlNewNsProp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlAttrPtr c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;
    xmlNsPtr ns;
    PyObject *pyobj_ns;
    xmlChar * name;
    xmlChar * value;

    if (!PyArg_ParseTuple(args, (char *)"OOzz:xmlNewNsProp", &pyobj_node, &pyobj_ns, &name, &value))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);
    ns = (xmlNsPtr) PyxmlNode_Get(pyobj_ns);

    c_retval = xmlNewNsProp(node, ns, name, value);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNewBoolean(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlXPathObjectPtr c_retval;
    int val;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlXPathNewBoolean", &val))
        return(NULL);

    c_retval = xmlXPathNewBoolean(val);
    py_retval = libxml_xmlXPathObjectPtrWrap((xmlXPathObjectPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlURISetPort(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlURIPtr URI;
    PyObject *pyobj_URI;
    int port;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlURISetPort", &pyobj_URI, &port))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    URI->port = port;
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlCopyDoc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlDocPtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    int recursive;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlCopyDoc", &pyobj_doc, &recursive))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlCopyDoc(doc, recursive);
    py_retval = libxml_xmlDocPtrWrap((xmlDocPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNewNs(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNsPtr c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;
    xmlChar * href;
    xmlChar * prefix;

    if (!PyArg_ParseTuple(args, (char *)"Ozz:xmlNewNs", &pyobj_node, &href, &prefix))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlNewNs(node, href, prefix);
    py_retval = libxml_xmlNsPtrWrap((xmlNsPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlDebugDumpNode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    FILE * output;
    PyObject *pyobj_output;
    xmlNodePtr node;
    PyObject *pyobj_node;
    int depth;

    if (!PyArg_ParseTuple(args, (char *)"OOi:xmlDebugDumpNode", &pyobj_output, &pyobj_node, &depth))
        return(NULL);
    output = (FILE *) PyFile_Get(pyobj_output);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    xmlDebugDumpNode(output, node, depth);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlIsBlankNode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlIsBlankNode", &pyobj_node))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlIsBlankNode(node);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathSubstringFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathSubstringFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathSubstringFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlNodeAddContent(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNodePtr cur;
    PyObject *pyobj_cur;
    xmlChar * content;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlNodeAddContent", &pyobj_cur, &content))
        return(NULL);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    xmlNodeAddContent(cur, content);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlCatalogResolveSystem(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlChar * sysID;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlCatalogResolveSystem", &sysID))
        return(NULL);

    c_retval = xmlCatalogResolveSystem(sysID);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlSetCompressMode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    int mode;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlSetCompressMode", &mode))
        return(NULL);

    xmlSetCompressMode(mode);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathRegisteredVariablesCleanup(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathRegisteredVariablesCleanup", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);

    xmlXPathRegisteredVariablesCleanup(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParseCharData(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;
    int cdata;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlParseCharData", &pyobj_ctxt, &cdata))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParseCharData(ctxt, cdata);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParseVersionNum(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseVersionNum", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParseVersionNum(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCopyCharMultiByte(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlChar * out;
    int val;

    if (!PyArg_ParseTuple(args, (char *)"zi:xmlCopyCharMultiByte", &out, &val))
        return(NULL);

    c_retval = xmlCopyCharMultiByte(out, val);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlUnlinkNode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlUnlinkNode", &pyobj_cur))
        return(NULL);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    xmlUnlinkNode(cur);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParseCharRef(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseCharRef", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParseCharRef(ctxt);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNodeSetContent(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNodePtr cur;
    PyObject *pyobj_cur;
    xmlChar * content;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlNodeSetContent", &pyobj_cur, &content))
        return(NULL);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    xmlNodeSetContent(cur, content);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlNormalizeURIPath(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    char * path;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlNormalizeURIPath", &path))
        return(NULL);

    c_retval = xmlNormalizeURIPath(path);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathIdFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathIdFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathIdFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlCleanupInputCallbacks(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlCleanupInputCallbacks();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlCatalogResolve(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlChar * pubID;
    xmlChar * sysID;

    if (!PyArg_ParseTuple(args, (char *)"zz:xmlCatalogResolve", &pubID, &sysID))
        return(NULL);

    c_retval = xmlCatalogResolve(pubID, sysID);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_htmlIsScriptAttribute(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlChar * name;

    if (!PyArg_ParseTuple(args, (char *)"z:htmlIsScriptAttribute", &name))
        return(NULL);

    c_retval = htmlIsScriptAttribute(name);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathIsInf(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    double val;

    if (!PyArg_ParseTuple(args, (char *)"d:xmlXPathIsInf", &val))
        return(NULL);

    c_retval = xmlXPathIsInf(val);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlShellPrintXPathError(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    int errorType;
    char * arg;

    if (!PyArg_ParseTuple(args, (char *)"iz:xmlShellPrintXPathError", &errorType, &arg))
        return(NULL);

    xmlShellPrintXPathError(errorType, arg);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathConcatFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathConcatFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathConcatFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlElemDump(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    FILE * f;
    PyObject *pyobj_f;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OOO:xmlElemDump", &pyobj_f, &pyobj_doc, &pyobj_cur))
        return(NULL);
    f = (FILE *) PyFile_Get(pyobj_f);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    xmlElemDump(f, doc, cur);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathVariableLookup(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlXPathObjectPtr c_retval;
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlChar * name;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlXPathVariableLookup", &pyobj_ctxt, &name))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);

    c_retval = xmlXPathVariableLookup(ctxt, name);
    py_retval = libxml_xmlXPathObjectPtrWrap((xmlXPathObjectPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_htmlNodeDumpFileFormat(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    FILE * out;
    PyObject *pyobj_out;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlNodePtr cur;
    PyObject *pyobj_cur;
    char * encoding;
    int format;

    if (!PyArg_ParseTuple(args, (char *)"OOOzi:htmlNodeDumpFileFormat", &pyobj_out, &pyobj_doc, &pyobj_cur, &encoding, &format))
        return(NULL);
    out = (FILE *) PyFile_Get(pyobj_out);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = htmlNodeDumpFileFormat(out, doc, cur, encoding, format);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlStrndup(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlChar * cur;
    int len;

    if (!PyArg_ParseTuple(args, (char *)"zi:xmlStrndup", &cur, &len))
        return(NULL);

    c_retval = xmlStrndup(cur, len);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlGetDtdAttrDesc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlAttributePtr c_retval;
    xmlDtdPtr dtd;
    PyObject *pyobj_dtd;
    xmlChar * elem;
    xmlChar * name;

    if (!PyArg_ParseTuple(args, (char *)"Ozz:xmlGetDtdAttrDesc", &pyobj_dtd, &elem, &name))
        return(NULL);
    dtd = (xmlDtdPtr) PyxmlNode_Get(pyobj_dtd);

    c_retval = xmlGetDtdAttrDesc(dtd, elem, name);
    py_retval = libxml_xmlAttributePtrWrap((xmlAttributePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNewPI(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlChar * name;
    xmlChar * content;

    if (!PyArg_ParseTuple(args, (char *)"zz:xmlNewPI", &name, &content))
        return(NULL);

    c_retval = xmlNewPI(name, content);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlAddPrevSibling(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlNodePtr cur;
    PyObject *pyobj_cur;
    xmlNodePtr elem;
    PyObject *pyobj_elem;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlAddPrevSibling", &pyobj_cur, &pyobj_elem))
        return(NULL);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);
    elem = (xmlNodePtr) PyxmlNode_Get(pyobj_elem);

    c_retval = xmlAddPrevSibling(cur, elem);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlHasNsProp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlAttrPtr c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;
    xmlChar * name;
    xmlChar * nameSpace;

    if (!PyArg_ParseTuple(args, (char *)"Ozz:xmlHasNsProp", &pyobj_node, &name, &nameSpace))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlHasNsProp(node, name, nameSpace);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlAddEncodingAlias(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    char * name;
    char * alias;

    if (!PyArg_ParseTuple(args, (char *)"zz:xmlAddEncodingAlias", &name, &alias))
        return(NULL);

    c_retval = xmlAddEncodingAlias(name, alias);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlLsOneNode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    FILE * output;
    PyObject *pyobj_output;
    xmlNodePtr node;
    PyObject *pyobj_node;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlLsOneNode", &pyobj_output, &pyobj_node))
        return(NULL);
    output = (FILE *) PyFile_Get(pyobj_output);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    xmlLsOneNode(output, node);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_htmlCreateFileParserCtxt(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    htmlParserCtxtPtr c_retval;
    char * filename;
    char * encoding;

    if (!PyArg_ParseTuple(args, (char *)"zz:htmlCreateFileParserCtxt", &filename, &encoding))
        return(NULL);

    c_retval = htmlCreateFileParserCtxt(filename, encoding);
    py_retval = libxml_xmlParserCtxtPtrWrap((xmlParserCtxtPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathPopString(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathPopString", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    c_retval = xmlXPathPopString(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseMarkupDecl(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseMarkupDecl", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParseMarkupDecl(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlDocGetRootElement(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlDocGetRootElement", &pyobj_doc))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlDocGetRootElement(doc);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParserSetLoadSubset(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;
    int loadsubset;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlParserSetLoadSubset", &pyobj_ctxt, &loadsubset))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    ctxt->loadsubset = loadsubset;
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParseEntity(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlDocPtr c_retval;
    char * filename;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlParseEntity", &filename))
        return(NULL);

    c_retval = xmlParseEntity(filename);
    py_retval = libxml_xmlDocPtrWrap((xmlDocPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlLoadCatalog(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    char * filename;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlLoadCatalog", &filename))
        return(NULL);

    c_retval = xmlLoadCatalog(filename);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlInitializeCatalog(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlInitializeCatalog();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlACatalogResolveURI(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlCatalogPtr catal;
    PyObject *pyobj_catal;
    xmlChar * URI;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlACatalogResolveURI", &pyobj_catal, &URI))
        return(NULL);
    catal = (xmlCatalogPtr) Pycatalog_Get(pyobj_catal);

    c_retval = xmlACatalogResolveURI(catal, URI);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlStrcat(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlChar * cur;
    xmlChar * add;

    if (!PyArg_ParseTuple(args, (char *)"zz:xmlStrcat", &cur, &add))
        return(NULL);

    c_retval = xmlStrcat(cur, add);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathEvalExpression(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlXPathObjectPtr c_retval;
    xmlChar * str;
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"zO:xmlXPathEvalExpression", &str, &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);

    c_retval = xmlXPathEvalExpression(str, ctxt);
    py_retval = libxml_xmlXPathObjectPtrWrap((xmlXPathObjectPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlIsDigit(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    int c;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlIsDigit", &c))
        return(NULL);

    c_retval = xmlIsDigit(c);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlGetProp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;
    xmlChar * name;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlGetProp", &pyobj_node, &name))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlGetProp(node, name);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCreateURI(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {
    PyObject *py_retval;
    xmlURIPtr c_retval;

    c_retval = xmlCreateURI();
    py_retval = libxml_xmlURIPtrWrap((xmlURIPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_nodePop(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:nodePop", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = nodePop(ctxt);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNextAncestor(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlXPathNextAncestor", &pyobj_ctxt, &pyobj_cur))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlXPathNextAncestor(ctxt, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseEntityDecl(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseEntityDecl", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParseEntityDecl(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlFreeNsList(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNsPtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlFreeNsList", &pyobj_cur))
        return(NULL);
    cur = (xmlNsPtr) PyxmlNode_Get(pyobj_cur);

    xmlFreeNsList(cur);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathParseName(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathParseName", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    c_retval = xmlXPathParseName(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNewDocTextLen(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * content;
    int len;

    if (!PyArg_ParseTuple(args, (char *)"Ozi:xmlNewDocTextLen", &pyobj_doc, &content, &len))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlNewDocTextLen(doc, content, len);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathFreeParserContext(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathFreeParserContext", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathFreeParserContext(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParserGetDirectory(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    char * c_retval;
    char * filename;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlParserGetDirectory", &filename))
        return(NULL);

    c_retval = xmlParserGetDirectory(filename);
    py_retval = libxml_charPtrWrap((char *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCatalogSetDebug(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    int level;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlCatalogSetDebug", &level))
        return(NULL);

    c_retval = xmlCatalogSetDebug(level);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_htmlParseDoc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    htmlDocPtr c_retval;
    xmlChar * cur;
    char * encoding;

    if (!PyArg_ParseTuple(args, (char *)"zz:htmlParseDoc", &cur, &encoding))
        return(NULL);

    c_retval = htmlParseDoc(cur, encoding);
    py_retval = libxml_xmlDocPtrWrap((xmlDocPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNewDocNode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlNsPtr ns;
    PyObject *pyobj_ns;
    xmlChar * name;
    xmlChar * content;

    if (!PyArg_ParseTuple(args, (char *)"OOzz:xmlNewDocNode", &pyobj_doc, &pyobj_ns, &name, &content))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);
    ns = (xmlNsPtr) PyxmlNode_Get(pyobj_ns);

    c_retval = xmlNewDocNode(doc, ns, name, content);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlURIGetServer(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const char * c_retval;
    xmlURIPtr URI;
    PyObject *pyobj_URI;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlURIGetServer", &pyobj_URI))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    c_retval = URI->server;
    py_retval = libxml_charPtrConstWrap((const char *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlUTF8Strndup(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlChar * utf;
    int len;

    if (!PyArg_ParseTuple(args, (char *)"zi:xmlUTF8Strndup", &utf, &len))
        return(NULL);

    c_retval = xmlUTF8Strndup(utf, len);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNodeListGetRawString(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlNodePtr list;
    PyObject *pyobj_list;
    int inLine;

    if (!PyArg_ParseTuple(args, (char *)"OOi:xmlNodeListGetRawString", &pyobj_doc, &pyobj_list, &inLine))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);
    list = (xmlNodePtr) PyxmlNode_Get(pyobj_list);

    c_retval = xmlNodeListGetRawString(doc, list, inLine);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCopyDtd(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlDtdPtr c_retval;
    xmlDtdPtr dtd;
    PyObject *pyobj_dtd;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlCopyDtd", &pyobj_dtd))
        return(NULL);
    dtd = (xmlDtdPtr) PyxmlNode_Get(pyobj_dtd);

    c_retval = xmlCopyDtd(dtd);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseExtParsedEnt(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseExtParsedEnt", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParseExtParsedEnt(ctxt);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_htmlSaveFileFormat(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    char * filename;
    xmlDocPtr cur;
    PyObject *pyobj_cur;
    char * encoding;
    int format;

    if (!PyArg_ParseTuple(args, (char *)"zOzi:htmlSaveFileFormat", &filename, &pyobj_cur, &encoding, &format))
        return(NULL);
    cur = (xmlDocPtr) PyxmlNode_Get(pyobj_cur);

    c_retval = htmlSaveFileFormat(filename, cur, encoding, format);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlDebugDumpAttrList(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    FILE * output;
    PyObject *pyobj_output;
    xmlAttrPtr attr;
    PyObject *pyobj_attr;
    int depth;

    if (!PyArg_ParseTuple(args, (char *)"OOi:xmlDebugDumpAttrList", &pyobj_output, &pyobj_attr, &depth))
        return(NULL);
    output = (FILE *) PyFile_Get(pyobj_output);
    attr = (xmlAttrPtr) PyxmlNode_Get(pyobj_attr);

    xmlDebugDumpAttrList(output, attr, depth);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlURIGetPort(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlURIPtr URI;
    PyObject *pyobj_URI;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlURIGetPort", &pyobj_URI))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    c_retval = URI->port;
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlSetNs(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNodePtr node;
    PyObject *pyobj_node;
    xmlNsPtr ns;
    PyObject *pyobj_ns;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlSetNs", &pyobj_node, &pyobj_ns))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);
    ns = (xmlNsPtr) PyxmlNode_Get(pyobj_ns);

    xmlSetNs(node, ns);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParseCDSect(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseCDSect", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParseCDSect(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlUnsetNsProp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;
    xmlNsPtr ns;
    PyObject *pyobj_ns;
    xmlChar * name;

    if (!PyArg_ParseTuple(args, (char *)"OOz:xmlUnsetNsProp", &pyobj_node, &pyobj_ns, &name))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);
    ns = (xmlNsPtr) PyxmlNode_Get(pyobj_ns);

    c_retval = xmlUnsetNsProp(node, ns, name);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlGetDtdQAttrDesc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlAttributePtr c_retval;
    xmlDtdPtr dtd;
    PyObject *pyobj_dtd;
    xmlChar * elem;
    xmlChar * name;
    xmlChar * prefix;

    if (!PyArg_ParseTuple(args, (char *)"Ozzz:xmlGetDtdQAttrDesc", &pyobj_dtd, &elem, &name, &prefix))
        return(NULL);
    dtd = (xmlDtdPtr) PyxmlNode_Get(pyobj_dtd);

    c_retval = xmlGetDtdQAttrDesc(dtd, elem, name, prefix);
    py_retval = libxml_xmlAttributePtrWrap((xmlAttributePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlIsID(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlNodePtr elem;
    PyObject *pyobj_elem;
    xmlAttrPtr attr;
    PyObject *pyobj_attr;

    if (!PyArg_ParseTuple(args, (char *)"OOO:xmlIsID", &pyobj_doc, &pyobj_elem, &pyobj_attr))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);
    elem = (xmlNodePtr) PyxmlNode_Get(pyobj_elem);
    attr = (xmlAttrPtr) PyxmlNode_Get(pyobj_attr);

    c_retval = xmlIsID(doc, elem, attr);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_htmlSetMetaEncoding(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    htmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * encoding;

    if (!PyArg_ParseTuple(args, (char *)"Oz:htmlSetMetaEncoding", &pyobj_doc, &encoding))
        return(NULL);
    doc = (htmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = htmlSetMetaEncoding(doc, encoding);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCopyNamespaceList(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNsPtr c_retval;
    xmlNsPtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlCopyNamespaceList", &pyobj_cur))
        return(NULL);
    cur = (xmlNsPtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlCopyNamespaceList(cur);
    py_retval = libxml_xmlNsPtrWrap((xmlNsPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNanoHTTPInit(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlNanoHTTPInit();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathCastNodeToString(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathCastNodeToString", &pyobj_node))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlXPathCastNodeToString(node);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlURIGetQuery(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const char * c_retval;
    xmlURIPtr URI;
    PyObject *pyobj_URI;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlURIGetQuery", &pyobj_URI))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    c_retval = URI->query;
    py_retval = libxml_charPtrConstWrap((const char *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNextChar(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlNextChar", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlNextChar(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlGetDtdElementDesc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlElementPtr c_retval;
    xmlDtdPtr dtd;
    PyObject *pyobj_dtd;
    xmlChar * name;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlGetDtdElementDesc", &pyobj_dtd, &name))
        return(NULL);
    dtd = (xmlDtdPtr) PyxmlNode_Get(pyobj_dtd);

    c_retval = xmlGetDtdElementDesc(dtd, name);
    py_retval = libxml_xmlElementPtrWrap((xmlElementPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCatalogCleanup(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlCatalogCleanup();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathNextPrecedingSibling(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlXPathNextPrecedingSibling", &pyobj_ctxt, &pyobj_cur))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlXPathNextPrecedingSibling(ctxt, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlHasProp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlAttrPtr c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;
    xmlChar * name;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlHasProp", &pyobj_node, &name))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlHasProp(node, name);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlFreePropList(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlAttrPtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlFreePropList", &pyobj_cur))
        return(NULL);
    cur = (xmlAttrPtr) PyxmlNode_Get(pyobj_cur);

    xmlFreePropList(cur);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathFloorFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathFloorFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathFloorFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathLocalNameFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathLocalNameFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathLocalNameFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlStrncasecmp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlChar * str1;
    xmlChar * str2;
    int len;

    if (!PyArg_ParseTuple(args, (char *)"zzi:xmlStrncasecmp", &str1, &str2, &len))
        return(NULL);

    c_retval = xmlStrncasecmp(str1, str2, len);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParsePubidLiteral(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParsePubidLiteral", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParsePubidLiteral(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlGetDtdQElementDesc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlElementPtr c_retval;
    xmlDtdPtr dtd;
    PyObject *pyobj_dtd;
    xmlChar * name;
    xmlChar * prefix;

    if (!PyArg_ParseTuple(args, (char *)"Ozz:xmlGetDtdQElementDesc", &pyobj_dtd, &name, &prefix))
        return(NULL);
    dtd = (xmlDtdPtr) PyxmlNode_Get(pyobj_dtd);

    c_retval = xmlGetDtdQElementDesc(dtd, name, prefix);
    py_retval = libxml_xmlElementPtrWrap((xmlElementPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlEncodeEntities(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const xmlChar * c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * input;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlEncodeEntities", &pyobj_doc, &input))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlEncodeEntities(doc, input);
    py_retval = libxml_xmlCharPtrConstWrap((const xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCatalogGetSystem(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const xmlChar * c_retval;
    xmlChar * sysID;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlCatalogGetSystem", &sysID))
        return(NULL);

    c_retval = xmlCatalogGetSystem(sysID);
    py_retval = libxml_xmlCharPtrConstWrap((const xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCopyChar(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    int len;
    xmlChar * out;
    int val;

    if (!PyArg_ParseTuple(args, (char *)"izi:xmlCopyChar", &len, &out, &val))
        return(NULL);

    c_retval = xmlCopyChar(len, out, val);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlLoadACatalog(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlCatalogPtr c_retval;
    char * filename;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlLoadACatalog", &filename))
        return(NULL);

    c_retval = xmlLoadACatalog(filename);
    py_retval = libxml_xmlCatalogPtrWrap((xmlCatalogPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNewDocProp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlAttrPtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * name;
    xmlChar * value;

    if (!PyArg_ParseTuple(args, (char *)"Ozz:xmlNewDocProp", &pyobj_doc, &name, &value))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlNewDocProp(doc, name, value);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlGetNsProp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;
    xmlChar * name;
    xmlChar * nameSpace;

    if (!PyArg_ParseTuple(args, (char *)"Ozz:xmlGetNsProp", &pyobj_node, &name, &nameSpace))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlGetNsProp(node, name, nameSpace);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlLoadSGMLSuperCatalog(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlCatalogPtr c_retval;
    char * filename;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlLoadSGMLSuperCatalog", &filename))
        return(NULL);

    c_retval = xmlLoadSGMLSuperCatalog(filename);
    py_retval = libxml_xmlCatalogPtrWrap((xmlCatalogPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlURISetPath(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlURIPtr URI;
    PyObject *pyobj_URI;
    char * path;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlURISetPath", &pyobj_URI, &path))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    if (URI->path != NULL) xmlFree(URI->path);
    URI->path = xmlStrdup((const xmlChar *)path);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_htmlInitAutoClose(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    htmlInitAutoClose();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParseEncodingDecl(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseEncodingDecl", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParseEncodingDecl(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlIsPubidChar(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    int c;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlIsPubidChar", &c))
        return(NULL);

    c_retval = xmlIsPubidChar(c);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlCatalogIsEmpty(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlCatalogPtr catal;
    PyObject *pyobj_catal;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlCatalogIsEmpty", &pyobj_catal))
        return(NULL);
    catal = (xmlCatalogPtr) Pycatalog_Get(pyobj_catal);

    c_retval = xmlCatalogIsEmpty(catal);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseEntityRef(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlEntityPtr c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseEntityRef", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParseEntityRef(ctxt);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathModValues(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathModValues", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathModValues(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParserSetPedantic(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;
    int pedantic;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlParserSetPedantic", &pyobj_ctxt, &pedantic))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    ctxt->pedantic = pedantic;
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathGetContextNode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathGetContextNode", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);

    c_retval = ctxt->node;
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_htmlNodeDumpFile(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    FILE * out;
    PyObject *pyobj_out;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OOO:htmlNodeDumpFile", &pyobj_out, &pyobj_doc, &pyobj_cur))
        return(NULL);
    out = (FILE *) PyFile_Get(pyobj_out);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    htmlNodeDumpFile(out, doc, cur);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathParserGetContext(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlXPathContextPtr c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathParserGetContext", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    c_retval = ctxt->context;
    py_retval = libxml_xmlXPathContextPtrWrap((xmlXPathContextPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParserHandlePEReference(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParserHandlePEReference", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlParserHandlePEReference(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlCleanupCharEncodingHandlers(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlCleanupCharEncodingHandlers();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlRegisterDefaultOutputCallbacks(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlRegisterDefaultOutputCallbacks();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlNodeAddContentLen(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNodePtr cur;
    PyObject *pyobj_cur;
    xmlChar * content;
    int len;

    if (!PyArg_ParseTuple(args, (char *)"Ozi:xmlNodeAddContentLen", &pyobj_cur, &content, &len))
        return(NULL);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    xmlNodeAddContentLen(cur, content, len);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathRoundFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathRoundFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathRoundFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlParserGetDoc(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlDocPtr c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParserGetDoc", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = ctxt->myDoc;
    py_retval = libxml_xmlDocPtrWrap((xmlDocPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXIncludeProcess(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXIncludeProcess", &pyobj_doc))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlXIncludeProcess(doc);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNewProp(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlAttrPtr c_retval;
    xmlNodePtr node;
    PyObject *pyobj_node;
    xmlChar * name;
    xmlChar * value;

    if (!PyArg_ParseTuple(args, (char *)"Ozz:xmlNewProp", &pyobj_node, &name, &value))
        return(NULL);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    c_retval = xmlNewProp(node, name, value);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlURIGetPath(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const char * c_retval;
    xmlURIPtr URI;
    PyObject *pyobj_URI;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlURIGetPath", &pyobj_URI))
        return(NULL);
    URI = (xmlURIPtr) PyURI_Get(pyobj_URI);

    c_retval = URI->path;
    py_retval = libxml_charPtrConstWrap((const char *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathFalseFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathFalseFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathFalseFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlFreeNs(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNsPtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlFreeNs", &pyobj_cur))
        return(NULL);
    cur = (xmlNsPtr) PyxmlNode_Get(pyobj_cur);

    xmlFreeNs(cur);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathSubstringBeforeFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathSubstringBeforeFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathSubstringBeforeFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlCheckLanguageID(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlChar * lang;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlCheckLanguageID", &lang))
        return(NULL);

    c_retval = xmlCheckLanguageID(lang);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNewChild(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlNodePtr parent;
    PyObject *pyobj_parent;
    xmlNsPtr ns;
    PyObject *pyobj_ns;
    xmlChar * name;
    xmlChar * content;

    if (!PyArg_ParseTuple(args, (char *)"OOzz:xmlNewChild", &pyobj_parent, &pyobj_ns, &name, &content))
        return(NULL);
    parent = (xmlNodePtr) PyxmlNode_Get(pyobj_parent);
    ns = (xmlNsPtr) PyxmlNode_Get(pyobj_ns);

    c_retval = xmlNewChild(parent, ns, name, content);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlInitParserCtxt(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlInitParserCtxt", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    xmlInitParserCtxt(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathInit(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlXPathInit();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlStringLenGetNodeList(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * value;
    int len;

    if (!PyArg_ParseTuple(args, (char *)"Ozi:xmlStringLenGetNodeList", &pyobj_doc, &value, &len))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlStringLenGetNodeList(doc, value, len);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNewNodeSet(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlXPathObjectPtr c_retval;
    xmlNodePtr val;
    PyObject *pyobj_val;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathNewNodeSet", &pyobj_val))
        return(NULL);
    val = (xmlNodePtr) PyxmlNode_Get(pyobj_val);

    c_retval = xmlXPathNewNodeSet(val);
    py_retval = libxml_xmlXPathObjectPtrWrap((xmlXPathObjectPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlAddNextSibling(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlNodePtr cur;
    PyObject *pyobj_cur;
    xmlNodePtr elem;
    PyObject *pyobj_elem;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlAddNextSibling", &pyobj_cur, &pyobj_elem))
        return(NULL);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);
    elem = (xmlNodePtr) PyxmlNode_Get(pyobj_elem);

    c_retval = xmlAddNextSibling(cur, elem);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathStringFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathStringFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathStringFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_htmlParseFile(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    htmlDocPtr c_retval;
    char * filename;
    char * encoding;

    if (!PyArg_ParseTuple(args, (char *)"zz:htmlParseFile", &filename, &encoding))
        return(NULL);

    c_retval = htmlParseFile(filename, encoding);
    py_retval = libxml_xmlDocPtrWrap((xmlDocPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNewDocRawNode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlNsPtr ns;
    PyObject *pyobj_ns;
    xmlChar * name;
    xmlChar * content;

    if (!PyArg_ParseTuple(args, (char *)"OOzz:xmlNewDocRawNode", &pyobj_doc, &pyobj_ns, &name, &content))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);
    ns = (xmlNsPtr) PyxmlNode_Get(pyobj_ns);

    c_retval = xmlNewDocRawNode(doc, ns, name, content);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNodeSetName(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlNodePtr cur;
    PyObject *pyobj_cur;
    xmlChar * name;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlNodeSetName", &pyobj_cur, &name))
        return(NULL);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    xmlNodeSetName(cur, name);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlEncodeSpecialChars(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * input;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlEncodeSpecialChars", &pyobj_doc, &input))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlEncodeSpecialChars(doc, input);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlRemoveRef(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlAttrPtr attr;
    PyObject *pyobj_attr;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlRemoveRef", &pyobj_doc, &pyobj_attr))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);
    attr = (xmlAttrPtr) PyxmlNode_Get(pyobj_attr);

    c_retval = xmlRemoveRef(doc, attr);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParserGetWellFormed(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParserGetWellFormed", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = ctxt->wellFormed;
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathLastFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathLastFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathLastFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathCountFunction(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    int nargs;

    if (!PyArg_ParseTuple(args, (char *)"Oi:xmlXPathCountFunction", &pyobj_ctxt, &nargs))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathCountFunction(ctxt, nargs);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlNodeListGetString(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlNodePtr list;
    PyObject *pyobj_list;
    int inLine;

    if (!PyArg_ParseTuple(args, (char *)"OOi:xmlNodeListGetString", &pyobj_doc, &pyobj_list, &inLine))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);
    list = (xmlNodePtr) PyxmlNode_Get(pyobj_list);

    c_retval = xmlNodeListGetString(doc, list, inLine);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlDebugDumpOneNode(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    FILE * output;
    PyObject *pyobj_output;
    xmlNodePtr node;
    PyObject *pyobj_node;
    int depth;

    if (!PyArg_ParseTuple(args, (char *)"OOi:xmlDebugDumpOneNode", &pyobj_output, &pyobj_node, &depth))
        return(NULL);
    output = (FILE *) PyFile_Get(pyobj_output);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    xmlDebugDumpOneNode(output, node, depth);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlRecoverMemory(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlDocPtr c_retval;
    char * buffer;
    int size;

    if (!PyArg_ParseTuple(args, (char *)"zi:xmlRecoverMemory", &buffer, &size))
        return(NULL);

    c_retval = xmlRecoverMemory(buffer, size);
    py_retval = libxml_xmlDocPtrWrap((xmlDocPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlReconciliateNs(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlNodePtr tree;
    PyObject *pyobj_tree;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlReconciliateNs", &pyobj_doc, &pyobj_tree))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);
    tree = (xmlNodePtr) PyxmlNode_Get(pyobj_tree);

    c_retval = xmlReconciliateNs(doc, tree);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlGetDtdEntity(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlEntityPtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * name;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlGetDtdEntity", &pyobj_doc, &name))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlGetDtdEntity(doc, name);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNanoFTPCleanup(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlNanoFTPCleanup();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlNewTextLen(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlChar * content;
    int len;

    if (!PyArg_ParseTuple(args, (char *)"zi:xmlNewTextLen", &content, &len))
        return(NULL);

    c_retval = xmlNewTextLen(content, len);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathRegisterAllFunctions(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathRegisterAllFunctions", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);

    xmlXPathRegisterAllFunctions(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathAddValues(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlXPathAddValues", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);

    xmlXPathAddValues(ctxt);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlUTF8Strlen(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlChar * utf;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlUTF8Strlen", &utf))
        return(NULL);

    c_retval = xmlUTF8Strlen(utf);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlDebugDumpNodeList(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    FILE * output;
    PyObject *pyobj_output;
    xmlNodePtr node;
    PyObject *pyobj_node;
    int depth;

    if (!PyArg_ParseTuple(args, (char *)"OOi:xmlDebugDumpNodeList", &pyobj_output, &pyobj_node, &depth))
        return(NULL);
    output = (FILE *) PyFile_Get(pyobj_output);
    node = (xmlNodePtr) PyxmlNode_Get(pyobj_node);

    xmlDebugDumpNodeList(output, node, depth);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_htmlSaveFile(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    char * filename;
    xmlDocPtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"zO:htmlSaveFile", &filename, &pyobj_cur))
        return(NULL);
    cur = (xmlDocPtr) PyxmlNode_Get(pyobj_cur);

    c_retval = htmlSaveFile(filename, cur);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlDebugDumpEntities(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    FILE * output;
    PyObject *pyobj_output;
    xmlDocPtr doc;
    PyObject *pyobj_doc;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlDebugDumpEntities", &pyobj_output, &pyobj_doc))
        return(NULL);
    output = (FILE *) PyFile_Get(pyobj_output);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    xmlDebugDumpEntities(output, doc);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_htmlGetMetaEncoding(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const xmlChar * c_retval;
    htmlDocPtr doc;
    PyObject *pyobj_doc;

    if (!PyArg_ParseTuple(args, (char *)"O:htmlGetMetaEncoding", &pyobj_doc))
        return(NULL);
    doc = (htmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = htmlGetMetaEncoding(doc);
    py_retval = libxml_xmlCharPtrConstWrap((const xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNewCString(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlXPathObjectPtr c_retval;
    char * val;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlXPathNewCString", &val))
        return(NULL);

    c_retval = xmlXPathNewCString(val);
    py_retval = libxml_xmlXPathObjectPtrWrap((xmlXPathObjectPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlGetIntSubset(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlDtdPtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlGetIntSubset", &pyobj_doc))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlGetIntSubset(doc);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlRegisterDefaultInputCallbacks(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {

    xmlRegisterDefaultInputCallbacks();
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathIsNodeType(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlChar * name;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlXPathIsNodeType", &name))
        return(NULL);

    c_retval = xmlXPathIsNodeType(name);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseElementDecl(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlParseElementDecl", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParseElementDecl(ctxt);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlScanName(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;

    if (!PyArg_ParseTuple(args, (char *)"O:xmlScanName", &pyobj_ctxt))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlScanName(ctxt);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNewCatalog(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlCatalogPtr c_retval;
    int sgml;

    if (!PyArg_ParseTuple(args, (char *)"i:xmlNewCatalog", &sgml))
        return(NULL);

    c_retval = xmlNewCatalog(sgml);
    py_retval = libxml_xmlCatalogPtrWrap((xmlCatalogPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlAddSibling(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlNodePtr cur;
    PyObject *pyobj_cur;
    xmlNodePtr elem;
    PyObject *pyobj_elem;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlAddSibling", &pyobj_cur, &pyobj_elem))
        return(NULL);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);
    elem = (xmlNodePtr) PyxmlNode_Get(pyobj_elem);

    c_retval = xmlAddSibling(cur, elem);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNewString(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlXPathObjectPtr c_retval;
    xmlChar * val;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlXPathNewString", &val))
        return(NULL);

    c_retval = xmlXPathNewString(val);
    py_retval = libxml_xmlXPathObjectPtrWrap((xmlXPathObjectPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_htmlNewDocNoDtD(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    htmlDocPtr c_retval;
    xmlChar * URI;
    xmlChar * ExternalID;

    if (!PyArg_ParseTuple(args, (char *)"zz:htmlNewDocNoDtD", &URI, &ExternalID))
        return(NULL);

    c_retval = htmlNewDocNoDtD(URI, ExternalID);
    py_retval = libxml_xmlDocPtrWrap((xmlDocPtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlParseChunk(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    int c_retval;
    xmlParserCtxtPtr ctxt;
    PyObject *pyobj_ctxt;
    char * chunk;
    int size;
    int terminate;

    if (!PyArg_ParseTuple(args, (char *)"Ozii:xmlParseChunk", &pyobj_ctxt, &chunk, &size, &terminate))
        return(NULL);
    ctxt = (xmlParserCtxtPtr) PyparserCtxt_Get(pyobj_ctxt);

    c_retval = xmlParseChunk(ctxt, chunk, size, terminate);
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlGetCompressMode(ATTRIBUTE_UNUSED PyObject *self,ATTRIBUTE_UNUSED  PyObject *args) {
    PyObject *py_retval;
    int c_retval;

    c_retval = xmlGetCompressMode();
    py_retval = libxml_intWrap((int) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlStrstr(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const xmlChar * c_retval;
    xmlChar * str;
    xmlChar * val;

    if (!PyArg_ParseTuple(args, (char *)"zz:xmlStrstr", &str, &val))
        return(NULL);

    c_retval = xmlStrstr(str, val);
    py_retval = libxml_xmlCharPtrConstWrap((const xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlXPathNextAncestorOrSelf(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlXPathParserContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlXPathNextAncestorOrSelf", &pyobj_ctxt, &pyobj_cur))
        return(NULL);
    ctxt = (xmlXPathParserContextPtr) PyxmlXPathParserContext_Get(pyobj_ctxt);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlXPathNextAncestorOrSelf(ctxt, cur);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNodeGetBase(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlChar * c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlNodePtr cur;
    PyObject *pyobj_cur;

    if (!PyArg_ParseTuple(args, (char *)"OO:xmlNodeGetBase", &pyobj_doc, &pyobj_cur))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);
    cur = (xmlNodePtr) PyxmlNode_Get(pyobj_cur);

    c_retval = xmlNodeGetBase(doc, cur);
    py_retval = libxml_xmlCharPtrWrap((xmlChar *) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlNewReference(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlNodePtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * name;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlNewReference", &pyobj_doc, &name))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlNewReference(doc, name);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlGetDocEntity(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    xmlEntityPtr c_retval;
    xmlDocPtr doc;
    PyObject *pyobj_doc;
    xmlChar * name;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlGetDocEntity", &pyobj_doc, &name))
        return(NULL);
    doc = (xmlDocPtr) PyxmlNode_Get(pyobj_doc);

    c_retval = xmlGetDocEntity(doc, name);
    py_retval = libxml_xmlNodePtrWrap((xmlNodePtr) c_retval);
    return(py_retval);
}

PyObject *
libxml_xmlLoadCatalogs(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    char * paths;

    if (!PyArg_ParseTuple(args, (char *)"z:xmlLoadCatalogs", &paths))
        return(NULL);

    xmlLoadCatalogs(paths);
    Py_INCREF(Py_None);
    return(Py_None);
}

PyObject *
libxml_xmlXPathNsLookup(ATTRIBUTE_UNUSED PyObject *self, PyObject *args) {
    PyObject *py_retval;
    const xmlChar * c_retval;
    xmlXPathContextPtr ctxt;
    PyObject *pyobj_ctxt;
    xmlChar * prefix;

    if (!PyArg_ParseTuple(args, (char *)"Oz:xmlXPathNsLookup", &pyobj_ctxt, &prefix))
        return(NULL);
    ctxt = (xmlXPathContextPtr) PyxmlXPathContext_Get(pyobj_ctxt);

    c_retval = xmlXPathNsLookup(ctxt, prefix);
    py_retval = libxml_xmlCharPtrConstWrap((const xmlChar *) c_retval);
    return(py_retval);
}

