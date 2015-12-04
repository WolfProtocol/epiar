/**\file			ui.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */

#ifndef __H_UI__
#define __H_UI__

#include "includes.h"
#include "input/input.h"
#include "ui_widget.h"
#include "ui_container.h"
#include "ui_window.h"
#include "audio/sound.h"
#include "graphics/font.h"

// Macro to check if xi, yi point is inside a rectangle x,y,w,h
#define WITHIN_BOUNDS(xi,yi,x,y,w,h) ( (xi >= x) && (yi >= y) && (xi < (x+w) ) && (yi < (y+h)) )

class UI {
	public:
		UI() {}
		~UI();

		static bool Initialize( string screenName );
		
		static Widget *Add( Widget *widget );
		static void Draw( void );

		static void CloseAll( void );
		static void Close( Widget *widget );
		static void Close( void *widget );

		static void HandleInput( list<InputEvent> & events );
		static bool Active(void);
		
		static void RegisterKeyboardFocus( Widget *widget );
		static Widget *DetermineMouseFocus( int x, int y );

		static Widget *Search( string query );

		static bool IsAttached( Widget* possible );

		static void Save( void );

		static void SwapScreens(string newname, Image* oldBackground, Image* newBackground );

		static Font* font;
		static Font* boldFont;
		static Sound* beep;
		static Sound* hover;

		static void Defer( Widget*, int x, int y );
		static void DrawDeferred( void );
		static int GetZLayer() { return zlayer; }

		static void ModalDialog( Window *widget );
		static void ReleaseModality();
		
	private:
		static Container* NewScreen( string name );
		static bool HandleKeyboard( InputEvent i );
		static bool HandleMouse( InputEvent i );
		static bool DispatchMouse( Widget* widget, InputEvent i );

		// Use a currentScreen widget to handle events,
		// so we don't need to duplicate code.
		static Container *currentScreen;
		static map<string,Container*> screens;

		static int zlayer;
		typedef struct {
			Widget* widget;
			int x;
			int y;
		} draw_location;

		static list<draw_location> deferred;
		static list<draw_location> hovering;

		static Container *backgroundScreen;
		static bool modalEnabled;
};

#endif // __H_UI__
