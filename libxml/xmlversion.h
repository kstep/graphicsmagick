/*
 * xmlversion.h : compile-time version informations for the XML parser.
 *
 * See Copyright for the status of this software.
 *
 * Daniel.Veillard@w3.org
 */

#ifndef __XML_VERSION_H__
#define __XML_VERSION_H__

#define LIBXML_VERSION 20100
#define LIBXML_VERSION_STRING "20100"

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
 * Whether XPath is configured in
 */
#if 1
#define LIBXML_XPATH_ENABLED
#else
#define LIBXML_XPATH_DISABLED
#endif

/*
 * Whether iconv support is available
 */
#if 0
#define LIBXML_ICONV_ENABLED
#else
#define LIBXML_ICONV_DISABLED
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
#if 1
#define DEBUG_MEMORY_LOCATION
#endif

#endif


