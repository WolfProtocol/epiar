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
#include "ui/ui_textbox.h"
#include "utilities/log.h"
#include "utilities/lua.h"

/** \addtogroup UI
 * @{
 */

Font *Textbox::font = NULL;
Color Textbox::foreground = WHITE;
Color Textbox::background = GREY;
Color Textbox::edge = BLACK;

/**\class Textbox
 * \brief UI textbox. */

/**\brief This is used to construct the Textbox.*/
Textbox::Textbox( int x, int y, int w, int rows, string text, string label ) {
	int rowHeight;

	if ( font == NULL ) {
		font = new Font( SKIN( "Skin/UI/Textbox/Font" ) );
		foreground = Color( SKIN( "Skin/UI/Textbox/Color/Foreground" ) );
		background = Color( SKIN( "Skin/UI/Textbox/Color/Background" ) );
		edge       = Color( SKIN( "Skin/UI/Textbox/Color/Edge" ) );
	}

	rowHeight = ((font->LineHeight()+9)/10)*10; // Round the rowHeight up to the nearest 10 pixels
	rowPad = (rowHeight - font->LineHeight()) / 2; // Pad the text to center it in the row

	this->x = x;
	this->y = y;
	this->w = w;
	this->h = rows * rowHeight;
	this->name = label;

	this->text = text;
}

/**\brief Draws the textbox.*/
void Textbox::Draw( int relx, int rely ) {
	int x, y;

	x = GetX() + relx;
	y = GetY() + rely;

	// draw the button (loaded image is simply scaled)
	Video::DrawRect( x, y, w, h, background );
	Video::DrawRect( x + 1, y + 1, w - 2, h - 2, edge );

	// draw the text
	Video::SetCropRect(x, y, this->w, this->h);
	int tw = font->Render( x + rowPad + 3, y + rowPad - 3, text );

	// draw the cursor (if it has focus and we're on an even second (easy blink every second))
	if( IsActive() && ((SDL_GetTicks() % 500) < 300) && !this->disabled ) {
		Video::DrawRect( x + 4 + tw, y + 3, 1, h - 6, foreground );
	}
	Video::UnsetCropRect();

	Widget::Draw(relx,rely);
}

bool Textbox::KeyPress( SDL_Keycode key ) {
	string keyname = SDL_GetKeyName( key );
	stringstream key_ss;
	string key_s;

	switch(key){
	// Ignore Modifiers
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
	case SDLK_RMETA:
	case SDLK_LMETA:
	case SDLK_RALT:
	case SDLK_LALT:
	case SDLK_RCTRL:
	case SDLK_LCTRL:
	case SDLK_RSUPER:
	case SDLK_LSUPER:
	// Special Non-Printable Keys
	case SDLK_ESCAPE:
	// TODO: add cursor movement support
	case SDLK_LEFT:
	case SDLK_RIGHT:
	case SDLK_UP:
	case SDLK_DOWN:
		return false;
	default:
		break;
	}

	key_ss << (char)key;
	key_ss >> key_s;

	if(keyname == "backspace") {
		int len = text.length() - 1;
		if(len < 0) len = 0;
		text.erase( len );
	} else if(keyname == "space") {
		text.append( " " );
	} else {
		text.append( key_s );
	}

	return true;
}

/** @} */
