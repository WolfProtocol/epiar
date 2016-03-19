/**\file		xmlfile.h
 * \author		Christopher Thielen (chris@epiar.net)
 * \date		Created: Monday, April 21, 2008
 * \date		Modified: Saturday, March 19, 2016
 * \brief		Interface with XML files
 * \details
 *
 */


#ifndef __H_XML__
#define __H_XML__

#include "includes.h"
#include <zlib.h>

class XMLFile {
	public:
		XMLFile();
		~XMLFile();

		bool New( const string& rootName );
		bool Open( const string& filename );
		bool Save( void );
		bool Save( const string& filename );
		bool Close();

		void SetFileName( const string& _filename ) { filename = _filename; }
		string GetFileName( ) { return filename; }

		string Get( const string& path ); // cast/convert to return value is needed
		void Set( const string& path, const string& value );
		void Set( const string& path, const float value );
		void Set( const string& path, const int value );

		bool Has( const string& path );

		bool Copy( XMLFile *other );

	protected:
		string filename;

	private:
		xmlDocPtr xmlPtr;
		map<string,xmlNodePtr> values;
		
		void Forget();
		xmlNodePtr FindNode( const string& path, bool createIfMissing = false );
};

vector<string> TokenizedString(const string& path, const string& tokens);

#define NodeNameIs( node, text ) ( !xmlStrcmp( ((node)->name), (const xmlChar *)(text) ) )

xmlNodePtr FirstChildNamed( xmlNodePtr node, const char* text );
xmlNodePtr NextSiblingNamed( xmlNodePtr child, const char* text );
string NodeToString( xmlDocPtr doc, xmlNodePtr node );
int NodeToInt( xmlDocPtr doc, xmlNodePtr node );
float NodeToFloat( xmlDocPtr doc, xmlNodePtr node );

#endif // __H_XML__
