/**\file			ui_textbox.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Monday, November 9, 2009
 * \date			Modified: Monday, November 9, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "graphics/font.h"
#include "graphics/video.h"
#include "ui/ui.h"
#include "ui/ui_textarea.h"
#include "utilities/log.h"
#include "utilities/lua.h"

/** \addtogroup UI
 * @{
 */

Color Textarea::foreground = WHITE;
Color Textarea::background = GREY;
Color Textarea::edge = BLACK;

/**\class Textarea
 * \brief UI Textarea for editing multiple lines of text.
 */

/**\brief Constructor for the Textarea.
 */
Textarea::Textarea( int _x, int _y, int _w, int _h, string text, string label )
	:lines( Font::Get( SKIN( "Skin/UI/Textbox/Font" ) ), text, _w )
{
	x = _x;
	y = _y;
	w = _w;
	h = _h;
	foreground = Color( SKIN( "Skin/UI/Textbox/Color/Foreground" ) );
	background = Color( SKIN( "Skin/UI/Textbox/Color/Background" ) );
	edge       = Color( SKIN( "Skin/UI/Textbox/Color/Edge" ) );
	name = label;
}

/**\brief Draws the Textarea.
 * \todo Add blinking Cursor
 */
void Textarea::Draw( int relx, int rely ) {
	int x, y;

	x = GetX() + relx;
	y = GetY() + rely;

	// draw the button (loaded image is simply scaled)
	Video::DrawRect( x, y, w, h, background );
	Video::DrawRect( x + 1, y + 1, w - 2, h - 2, edge );

	// draw the text
	Video::SetCropRect(x, y, w, h);
	lines.Render( x, y, Font::LEFT, Font::TOP );
	Video::UnsetCropRect();

	Widget::Draw(relx,rely);
}

/**\brief Accept Key Presses
 */
bool Textarea::KeyPress( SDL_Keycode key ) {
	string keyname = SDL_GetKeyName( key );
	stringstream key_ss;
	string key_s;

	switch(key) {
		// Ignore Modifiers
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
		//case SDLK_RMETA:
		//case SDLK_LMETA:
		case SDLK_RALT:
		case SDLK_LALT:
		case SDLK_RCTRL:
		case SDLK_LCTRL:
		//case SDLK_RSUPER:
		//case SDLK_LSUPER:
		// Special Non-Printable Keys
		case SDLK_ESCAPE:
		// TODO: add cursor movement support
		case SDLK_LEFT:
		case SDLK_RIGHT:
		case SDLK_UP:
		case SDLK_DOWN:
			return false;

		case SDLK_BACKSPACE:
			lines.Erase( 1 );
			break;

		case SDLK_SPACE:
			lines.AppendText( " " );
			break;

		case SDLK_KP_ENTER:
		case SDLK_RETURN:
		//case '\n':
			lines.AppendText( "\n" );
			break;

		default:
			key_ss << (char)key;
			key_ss >> key_s;
			lines.AppendText( key_s );
			break;
	}

	return true;
}

/** @} */
