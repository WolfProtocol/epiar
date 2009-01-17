/*
 * Filename      : xml.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Monday, April 21, 2008
 * Last Modified : Monday, April 21, 2008
 * Purpose       : Interface with XML files
 * Notes         :
 */

#include "Utilities/file.h"
#include "Utilities/log.h"
#include "Utilities/xml.h"

XMLFile::XMLFile() {
	// hella cool constructor (hcc)
	xmlPtr = NULL;
}

XMLFile::XMLFile( string filename ) {
	xmlPtr = NULL;
	
	Open( filename );
}

bool XMLFile::Open( string filename ) {
	u_byte *buf = NULL;
	long bufSize = 0;
	File xmlfile;
	
	if( xmlfile.Open( filename ) == false ) {
		Log::Error( "Could not find file %s", filename.c_str() );
		return( false );
	}
	
	buf = (u_byte *)xmlfile.Read( &bufSize, 0 );
	if( buf == NULL ) {
		Log::Error( "Could not load XML from archive. Buffer failed to allocate." );
		return( NULL );
	}

	xmlPtr = xmlParseMemory( (const char *)buf, bufSize );
	free( buf );
	
	this->filename = filename;
	
	return( true );
}

XMLFile::~XMLFile() {
	Close();
}

bool XMLFile::Close() {
	if( xmlPtr ) xmlFreeDoc( xmlPtr );
	xmlPtr = NULL;
	
	return( true );
}

string XMLFile::Get( string path ) {
	xmlNodePtr cur;
	
	// take apart the path and put it in a queue
	queue<string> pathTree;
	string nodeBuffer;
	for( unsigned int i = 0; i < path.length(); i++ ) {
		if(path[i] != '/') {
			// normal character. we record it to the nodeBuffer so we know the full node name when a '/' is found
			nodeBuffer += path[i];
		} else {
			// we are leaving the parent node, so push it to the queue
			pathTree.push( nodeBuffer );
			nodeBuffer = "";
		}
	}
	if( nodeBuffer.length() ) pathTree.push( nodeBuffer ); // our last node won't end in a '/', so we can't forget it here
	
	// initialize the xml navigation cursor
	cur = xmlDocGetRootElement( xmlPtr );
	
	if( cur == NULL ) {
		Log::Warning( "XML file () appears to be empty." );
		return( string() );
	}
	
	// which node are we looking for?
	string nodeToLocate = pathTree.front();
	
	while( !pathTree.empty() ) {
		if( cur == NULL ) {
			break; // at the end of the tree and didn't find anything
		}
		
		if( !xmlStrcmp( cur->name, (const xmlChar *)nodeToLocate.c_str() ) ) {
			// found it
			
			// is there any else in the path? if not, we retrieve the value
			// else, we move on into the child tree
			if( nodeToLocate == pathTree.back() ) {
				// nothing left to look for, return this value
				xmlChar *subKey = xmlNodeListGetString( xmlPtr, cur->xmlChildrenNode, 1 );
				return( string( (char *)subKey ) );
			} else {
				// more path, explore the child tree
				cur = cur->xmlChildrenNode;
				pathTree.pop(); // remove the element we just found off the tree and get the next
				nodeToLocate = pathTree.front();
			}
		}
		
		cur = cur->next;
	}

	// didn't find it
	return( string() );
}
