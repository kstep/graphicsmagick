/*
 * xmlversion.h : compile-time version informations for the XML parser.
 *
 * See Copyright for the status of this software.
 *
 * Daniel.Veillard@w3.org
 */

#ifndef __XML_VERSION_H__
#define __XML_VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * use those to be sure nothing nasty will happen if
 * your library and includes mismatch
 */
extern void xmlCheckVersion(int version);
#define LIBXML_DOTTED_VERSION "2.2.9"
#define LIBXML_VERSION 20209
#define LIBXML_VERSION_STRING "20209"
#define LIBXML_TEST_VERSION xmlCheckVersion(20209);

/*
 * Whether the FTP support is configured in
 */
#if 1
#define LIBXML_FTP_ENABLED
#else
#define LIBXML_FTP_DISABLED
#endif

/*
 * Whether the HTTP support is configured in
 */
#if 1
#define LIBXML_HTTP_ENABLED
#else
#define LIBXML_HTTP_DISABLED
#endif

/*
 * Whether the HTML support is configured in
 */
#if 1
#define LIBXML_HTML_ENABLED
#else
#define LIBXML_HTML_DISABLED
#endif

/*
 * Whether the Docbook support is configured in
#if @WITH_SGML@
#define LIBXML_SGML_ENABLED
#else
#define LIBXML_SGML_DISABLED
#endif
 */

/*
 * Whether XPath is configured in
 */
#if 1
#define LIBXML_XPATH_ENABLED
#else
#define LIBXML_XPATH_DISABLED
#endif

/*
 * Whether XPointer is configured in
 */
#if 1
#define LIBXML_XPTR_ENABLED
#else
#define LIBXML_XPTR_DISABLED
#endif

/*
 * Whether XInclude is configured in
 */
#if 1
#define LIBXML_XINCLUDE_ENABLED
#else
#define LIBXML_XINCLUDE_DISABLED
#endif

/*
 * Whether iconv support is available
 */
#ifndef WIN32
#if 1
#define LIBXML_ICONV_ENABLED
#else
#define LIBXML_ICONV_DISABLED
#endif
#endif

/*
 * Whether Debugging module is configured in
 */
#if 1
#define LIBXML_DEBUG_ENABLED
#else
#define LIBXML_DEBUG_DISABLED
#endif

/*
 * Whether the memory debugging is configured in
 */
#if 0
#define DEBUG_MEMORY_LOCATION
#endif

#ifndef LIBXML_DLL_IMPORT
#if defined(WIN32) && !defined(STATIC)
#define LIBXML_DLL_IMPORT __declspec(dllimport)
#else
#define LIBXML_DLL_IMPORT
#endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif


