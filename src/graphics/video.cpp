/**\file			video.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Thursday, December 24, 2015
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "graphics/video.h"
#include "utilities/file.h"
#include "utilities/log.h"
#include "utilities/xml.h"
#include "utilities/trig.h"

/**\class Rect
 * \brief Wrapper for a rectangle.
 * \fn Rect::Rect()
 *  \brief Creates a rectangle with 0 for x,y, width, and height.
 * \fn Rect::Rect( float x, float y, float w, float h )
 *  \brief Creates a rectangle with specified dimensions.
 * \fn Rect::Rect( int x, int y, int w, int h )
 *  \brief Creates a rectangle with specified dimensions. (float version)
 * \var Rect::x
 *  \brief x coordinate
 */
/**\var Rect::y
 *  \brief y coordinate
 */
/**\var Rect::w
 *  \brief width
 */
/**\var Rect::h
 *  \brief height
 */

/**\class Video
 * \brief Video handling. */

int Video::w = 0;
int Video::h = 0;
int Video::w2 = 0;
int Video::h2 = 0;
stack<Rect> Video::cropRects;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

/**\brief Initializes the Video display.
 */
bool Video::Initialize( void ) {
	//char buf[32] = {0};
	//const SDL_VideoInfo *videoInfo;

	// initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) != 0 ) {
		LogMsg(ERR, "Could not initialize SDL: %s", SDL_GetError() );
		return( false );
	} else {
		LogMsg(DEBUG, "SDL video initialized using FIXME driver."); //, SDL_VideoDriverName( buf, 31 ) );
	}

	atexit( SDL_Quit );

	//SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	//videoInfo = SDL_GetVideoInfo();

	int w = OPTION( int, "options/video/w" );
	int h = OPTION( int, "options/video/h" );
	bool fullscreen = OPTION( bool, "options/video/fullscreen" );

	// // Sanitize Width and Height
	// // TODO: Surely 0 is invalid, but what's the lower limit?
	// if( (w <= 0) || (w > videoInfo->current_w) ) { w = videoInfo->current_w; }
	// if( (h <= 0) || (h > videoInfo->current_w) ) { h = videoInfo->current_h; }
	//
	// if( OPTION( int, "options/video/fullscreen" ) ) {
	// 	// fullscreen set, use native resolution
	// 	w = videoInfo->current_w;
	// 	h = videoInfo->current_h;
	// }

	Video::SetWindow( w, h, OPTION( int, "options/video/bpp"), fullscreen );

	return( true );
}

/**\brief Shuts down the Video display.
 */
bool Video::Shutdown( void ) {

	EnableMouse();

	return( true );
}

/**\brief Sets the window properties.
 */
bool Video::SetWindow( int w, int h, int bpp, bool fullscreen ) {
	//const SDL_VideoInfo *videoInfo; // handle to SDL video information
	//Uint32 videoFlags = 0; // bitmask to pass to SDL_SetVideoMode()
	//int ret = 0;

	// get information about the video card (namely, does it support
	// hardware surfaces?)
	// videoInfo = SDL_GetVideoInfo();
	// if(! videoInfo )
	// 	LogMsg(WARN, "SDL_GetVideoInfo() returned NULL" );

	// // enable OpenGL and various other options
	// videoFlags = SDL_OPENGL;
	// videoFlags |= SDL_GL_DOUBLEBUFFER;
	// videoFlags |= SDL_HWPALETTE;

	// enable fullscreen if set (see main.cpp, parseOptions)
	//if( fullscreen ) videoFlags |= SDL_FULLSCREEN;

	// // using SDL given information, use hardware surfaces if supported by
	// // the video card
	// if( videoInfo->hw_available ) {
	// 	videoFlags |= SDL_HWSURFACE;
	// 	LogMsg(DEBUG, "Using hardware surfaces." );
	// } else {
	// 	videoFlags |= SDL_SWSURFACE;
	// 	LogMsg(DEBUG, "Not using hardware surfaces." );
	// }
	//
	// // using SDL given information, set hardware acceleration if supported
	// if( videoInfo->blit_hw ) {
	// 	videoFlags |= SDL_HWACCEL;
	// 	LogMsg(DEBUG, "Using hardware accelerated blitting." );
	// } else {
	// 	LogMsg(DEBUG, "Not using hardware accelerated blitting." );
	// }

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	//SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1); // vsync

	// ret = SDL_VideoModeOK( w, h, bpp, videoFlags );
	// if( !ret ) {
	// 	LogMsg(WARN, "Video mode %dx%dx%d not available.", w, h, bpp );
	// } else {
	// 	LogMsg(DEBUG, "Video mode %dx%dx%d supported.", w, h, bpp );
	// }

	SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,   16);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,  16);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 16);

	// Set the application icon (must be done before SDL_SetVideoMode)
	SDL_Surface *icon = NULL;
	File icon_file( "data/graphics/icon.bmp" ); // File is used to calculate path
	icon = SDL_LoadBMP( icon_file.GetAbsolutePath().c_str() );
	if(icon != NULL) {
		SDL_SetWindowIcon(window, icon);
		SDL_FreeSurface(icon); icon = NULL;
	} else {
		LogMsg(WARN, "Unable to load window icon data/graphics/icon.bmp." );
	}

	// set window title
	//SDL_WM_SetCaption("Epiar", "Epiar");

	window = SDL_CreateWindow("Epiar", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if(window == NULL) {
		LogMsg(ERR, "Could not create window: %s", SDL_GetError() );
		return( false );
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(renderer == NULL) {
		SDL_DestroyWindow(window); window = NULL;
		LogMsg(ERR, "Could not create renderer: %s", SDL_GetError() );
		return( false );
	}

	// // finally, set the video mode (creating a window)
	// if( ( screen = SDL_SetVideoMode( w, h, bpp, videoFlags ) ) == NULL ) {
	// 	LogMsg(ERR, "Could not set video mode: %s", SDL_GetError() );
	// 	return( false );
	// }

	// set up some needed opengl facilities
	glEnable( GL_TEXTURE_2D );
	glShadeModel( GL_SMOOTH );
	glClearColor( 0.0f, 0.0f, 0.0f, 0.5f );
	glClearDepth( 1.0f );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// for motion blur
	glClearAccum(0.0, 0.0, 0.0, 1.0);
	glClear(GL_ACCUM_BUFFER_BIT);

	// set up a pseudo-2D viewpoint
	glViewport( 0, 0, w, h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, w, h, 0, -1, 1);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Video::w = w;
	// Video::h = h;

	SDL_GetWindowSize(window, &Video::w, &Video::h);

	// compute the half dimensions
	w2 = w / 2;
	h2 = h / 2;

	LogMsg(DEBUG, "Video mode initialized at %dx%dxbpp fixme\n", Video::w, Video::h );

	return( true );
}

/**\brief Register Lua functions for Video related operations.
 */
void Video::RegisterVideo(lua_State *L) {
	static const luaL_Reg videoFunctions[] = {
		{"getWidth", &Video::lua_getWidth},
		{"getHeight", &Video::lua_getHeight},
		{NULL, NULL}
	};

	luaL_openlib(L, EPIAR_VIDEO, videoFunctions, 0);

	lua_pop(L,1);
}

/**\brief Same as Video::GetWidth (Lua callable)
 */
int Video::lua_getWidth(lua_State *L) {
	lua_pushinteger(L, GetWidth() );
	return 1;
}

/**\brief Same as Video::GetHeight (Lua callable)
 */
int Video::lua_getHeight(lua_State *L) {
	lua_pushinteger(L, GetHeight() );
	return 1;
}

/**\brief Video updates.
 */
void Video::Update( void ) {
	glFlush();
	//SDL_GL_SwapBuffers();
	SDL_RenderPresent(renderer);
	//glAccum(GL_ACCUM, 0.8f);
	glFinish();
}

/**\brief Pre-draw commands.
 */
void Video::PreDraw( void ) {
	// Clear the draw and depth buffers
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Take the contents of the current accumulation buffer and copy it to the colour buffer with each pixel multiplied by a factor
	// i.e. we clear the screen, draw the last frame again (which we saved in the accumulation buffer), then draw our stuff at its new location on top of that
	//glAccum(GL_RETURN, 0.75f);

	// Clear the accumulation buffer (don't worry, we re-grab the screen into the accumulation buffer after drawing our current frame!)
	//glClear(GL_ACCUM_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Video::Blur( void ) {
	float q = .6f;

	glAccum(GL_MULT, q);
	glAccum(GL_ACCUM, 1.0f - q);
	glAccum(GL_RETURN, 1.0f);
}

/**\brief Clears screen.
 */
void Video::Erase( void ) {
	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//glLoadIdentity();
	SDL_RenderClear(renderer);
}

/**\brief Returns the width of the screen.
 */
int Video::GetWidth( void ) {
	return( w );
}

/**\brief Returns the height of the screen.
 */
int Video::GetHeight( void ) {
	return( h );
}

/**\brief draws a point, a single pixel, on the screen
 */
void Video::DrawPoint( int x, int y, float r, float g, float b ) {
	glDisable(GL_TEXTURE_2D);
	glColor3f( r, g, b );
	glRecti( x, y, x + 1, y + 1 );
}

/**\brief Draw a point using Coordinate and Color.
 */
void Video::DrawPoint( Coordinate c, Color col ) {
	DrawPoint( (int)c.GetX(), (int)c.GetY(), col.r, col.g, col.b );
}

/**\brief Draw a Line.
 */
void Video::DrawLine( Coordinate p1, Coordinate p2, Color c, float a ) {
	DrawLine( p1.GetX(), p1.GetY(), p2.GetX(), p2.GetY(), c.r, c.g, c.b, a );
}

/**\brief Draw a Line.
 */
void Video::DrawLine( int x1, int y1, int x2, int y2, Color c, float a ) {
	DrawLine( x1, y1, x2, y2, c.r, c.g, c.b, a );
}

/**\brief Draw a Line.
 */
void Video::DrawLine( int x1, int y1, int x2, int y2, float r, float g, float b, float a ) {
	glColor4f( r, g, b, a );
	glBegin(GL_LINES);
	glVertex2d(x1,y1);
	glVertex2d(x2,y2);
	glEnd();
}


/**\brief Draws a filled rectangle
 */
void Video::DrawRect( int x, int y, int w, int h, float r, float g, float b, float a ) {
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glColor4f( r, g, b, a );
	glRecti( x, y, x + w, y + h );
}

void Video::DrawRect( int x, int y, int w, int h, Color c, float a ) {
	DrawRect( x, y, w, h, c.r, c.g, c.b, a );
}

void Video::DrawRect( Coordinate p, int w, int h, Color c, float a ) {
	DrawRect( p.GetX(), p.GetY(), w, h, c.r, c.g, c.b, a );
}

void Video::DrawBox( int x, int y, int w, int h, Color c, float a ) {
	DrawBox( x, y, w, h, c.r, c.g, c.b, a );
}

/**\brief Draws an unfilled rectangle
 */
void Video::DrawBox( int x, int y, int w, int h, float r, float g, float b, float a ) {
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glColor4f( r, g, b, a );
	glBegin(GL_LINE_STRIP);
	glVertex2d(x,y);
	glVertex2d(x+w,y);
	glVertex2d(x+w,y+h);
	glVertex2d(x,y+h);
	glVertex2d(x,y);
	glEnd();

}

/**\brief Draws a circle.
 */
void Video::DrawCircle( Coordinate c, int radius, float line_width, float r, float g, float b, float a) {
	DrawCircle( (int)c.GetX(), (int)c.GetY(), radius, line_width, r, g, b, a );
}

/**\brief Draws a circle with Color
 */
void Video::DrawCircle( Coordinate c, int radius, float line_width, Color col, float a) {
	DrawCircle( (int)c.GetX(), (int)c.GetY(), radius, line_width, col.r, col.g, col.b, a);
}

/**\brief Draws a circle
 */
void Video::DrawCircle( int x, int y, int radius, float line_width, float r, float g, float b, float a) {
	glDisable(GL_TEXTURE_2D);
	glColor4f( r, g, b, a );
	glLineWidth(line_width);
	glBegin(GL_LINE_STRIP);
	Trig* t = Trig::Instance();
	for(int angle = 0; angle < 360; angle += 5)
	{
		glVertex2d(radius * t->GetCos(angle) + x, radius * t->GetSin(angle) + y);
	}
	// One more point to finish the circle. (ang=0)
	glVertex2d(radius + x, y);
	glEnd();
	// Reset Line Width
	glLineWidth(1);
}

void Video::DrawFilledCircle( Coordinate p, int radius, Color c, float a) {
	DrawFilledCircle( p.GetX(), p.GetY(), radius, c.r, c.g, c.b, a );
}

void Video::DrawFilledCircle( int x, int y, int radius, Color c, float a) {
	DrawFilledCircle( x, y, radius, c.r, c.g, c.b, a );
}

/**\brief Draw a filled circle.
 */
void Video::DrawFilledCircle( int x, int y, int radius, float r, float g, float b, float a) {
	glColor4f(r, g, b, a);
	glEnable(GL_BLEND);
	glBegin(GL_TRIANGLE_STRIP);

	Trig* t = Trig::Instance();

	for(int angle = 0; angle < 360; angle += 5) {
		glVertex2d(x, y);
		glVertex2d(radius * t->GetCos(angle) + x, radius * t->GetSin(angle) + y);
	}

	// One more triangle to finish the circle. (ang=0)
	glVertex2d(x, y);
	glVertex2d(radius + x, y);
	glEnd();
}

/**\brief Draws a targeting overlay.
 */
void Video::DrawTarget( int x, int y, int w, int h, int d, float r, float g, float b, float a ) {
	float w2 = w / 2.;
	float h2 = h / 2.;

	// d is for 'depth' and is the number of crosshair pixels
	glColor4f(r, g, b, a);

	glBegin(GL_LINES);

		// Upper Left Corner
		glVertex2d(x - w2, y - h2); glVertex2d(x - w2, y - h2 + d);
		glVertex2d(x - w2, y - h2); glVertex2d(x - w2 + d, y - h2);

		// Upper Right Corner
		glVertex2d(x + w2, y - h2); glVertex2d(x + w2, y - h2 + d);
		glVertex2d(x + w2, y - h2); glVertex2d(x + w2 - d, y - h2);

		// Lower Left Corner
		glVertex2d(x - w2, y + h2); glVertex2d(x - w2, y + h2 - d);
		glVertex2d(x - w2, y + h2); glVertex2d(x - w2 + d, y + h2);

		// Lower Right Corner
		glVertex2d(x + w2, y + h2); glVertex2d(x + w2, y + h2 - d);
		glVertex2d(x + w2, y + h2); glVertex2d(x + w2 - d, y + h2);

	glEnd();
}

/**\brief Enables the mouse
 */
void Video::EnableMouse( void ) {
	SDL_ShowCursor( 1 );
}

/**\brief Disables the mouse
 */
void Video::DisableMouse( void ) {
	SDL_ShowCursor( 0 );
}

/**\brief Returns the half width.
 */
int Video::GetHalfWidth( void ) {
	return( w2 );
}

/**\brief Returns the half height.
 */
int Video::GetHalfHeight( void ) {
	return( h2 );
}

/**\brief Set crop rectangle.
 */
void Video::SetCropRect( int x, int y, int w, int h ){
	int xn, yn, wn, hn;

	if (cropRects.empty()) {
		glEnable(GL_SCISSOR_TEST);

		xn = x;
		yn = y;
		wn = w;
		hn = h;
	} else {
		// Need to detect which part of crop rectangle is within previous rectangle
		// So we don't miss things that needs to be cropped.
		Rect prevrect = cropRects.top();

		int rightprev = TO_INT(prevrect.x + prevrect.w);
		int right = x + w;
		int botprev = TO_INT(prevrect.y + prevrect.h);
		int bot = y + h;

		xn = prevrect.x > x ? TO_INT(prevrect.x) : x;	       // Left
		yn = prevrect.y > y ? TO_INT(prevrect.y) : y;	       // Top
		wn = rightprev > right ? right - xn : rightprev - xn;  // Right
		hn = botprev > bot ? bot - yn : botprev - yn;		   // Bottom

		if(wn < 0) wn = 0; // crops don't overlap. we still record this as they're going to call unsetcrop, unaware of the issue
		if(hn < 0) hn = 0; // same reason as line above
	}

	cropRects.push(Rect( xn, yn, wn, hn ));

	// Need to convert top down y-axis
	glScissor( xn, Video::h - (yn + hn), wn, hn );
}

/**\brief Unset the previous crop rectangle after use.
 */
void Video::UnsetCropRect( void ) {
	if (!cropRects.empty()) // Shouldn't be empty
		cropRects.pop();
	else
		LogMsg(WARN,"You unset the crop rect too many times.");

	if ( cropRects.empty() ) {
		glDisable(GL_SCISSOR_TEST);
	} else {
		// Set's the previous crop rectangle.
		Rect prevrect = cropRects.top();

		glScissor( TO_INT(prevrect.x), Video::h - (TO_INT(prevrect.y) + TO_INT(prevrect.h)), TO_INT(prevrect.w), TO_INT(prevrect.h) );
	}
}

/**\brief Takes a screenshot of the game and saves it to an Image.
 */
Image *Video::CaptureScreen( void ) {
	GLuint screenCapture;

	glGenTextures( 1, &screenCapture );

	glBindTexture(GL_TEXTURE_2D, screenCapture);

	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 0, 0, w, h, 0);

	Image *screenshot = new Image( screenCapture, w, h );

	return( screenshot );
}

/**\brief Takes a screenshot of the game and saves it to a file.
 */
void Video::SaveScreenshot( string filename ) {
	// unsigned int size = w * h * 4;
	// int *pixelData, *pixelDataOrig;
	//
	// pixelData = (int *)malloc( size );
	// pixelDataOrig = (int *)malloc( size );
	// memset( pixelData, 0, size );
	// memset( pixelDataOrig, 0, size );
	//
	// glPixelStorei( GL_PACK_ROW_LENGTH, 0 ) ;
	// glPixelStorei( GL_PACK_ALIGNMENT, 1 ) ;
	// glReadPixels( 0, 0, w, h, GL_BGRA, GL_UNSIGNED_BYTE, pixelDataOrig ) ;
	//
	// // image if flipped vertically - fix this
	// for(int x = 1; x < w; x++) {
	// 	for(int y = 1; y < h; y++) {
	// 		pixelData[x + (y * w)] = pixelDataOrig[x + ((h-y) * w)];
	// 	}
	// }
	//
	// SDL_Surface *s = SDL_CreateRGBSurfaceFrom( pixelData, w, h, screen->format->BitsPerPixel, screen->pitch, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
	//
	// if( filename == "" ) filename = string("Screenshot_") + Log::GetTimestamp() + string(".bmp");
	//
	// SDL_SaveBMP( s, filename.c_str() );
	//
	// SDL_FreeSurface( s );
	// free( pixelData );
	// free( pixelDataOrig );

	LogMsg(WARN, "Video::SaveScreenshot() is unimplemented." );
}
