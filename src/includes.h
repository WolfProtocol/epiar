/**\file		includes.h
 * \author		Christopher Thielen
 * \author		and others
 * \date		Saturday, November 21, 2009
 * \date		Thursday, December 24, 2015
 * \brief		Contains common system libraries.
 * \details
 */

#ifndef __H_INCLUDES__
#define __H_INCLUDES__

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501
// The Microsoft GL header files require windows.h to be included first.
#include <windows.h>
#define snprintf _snprintf
// Microsoft's math.h requires this define so that it defines M_PI etc.
#define _USE_MATH_DEFINES

#endif // _WIN32

// Does this conflict with Log?
#include <math.h>

// System includes
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <time.h>
#include <assert.h>
#include <queue>
#include <errno.h>
#include <stack>
#include <stdio.h>
#include <stdlib.h>

// Library includes
#include <SDL2/SDL.h>
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <zlib.h>

// Local includes
#include "defines.h"
#include "version.h"

#include <algorithm>

using namespace std;

#endif // __H_INCLUDES__
