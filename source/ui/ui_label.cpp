/**\file			ui_label.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Friday, April 25, 2008
 * \date			Modified: Friday, April 25, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "graphics/video.h"
#include "graphics/font.h"
#include "ui/ui.h"
#include "ui/ui_label.h"
#include "utilities/log.h"

/** \addtogroup UI
 * @{
 */

/**\class Label
 * \brief UI label. */

/**\brief Constructor
 */
Label::Label( int _x, int _y, string input, bool _centered) {
	x = _x;
	y = _y;

	// w/h is dependent upon the text given
	
	centered = _centered;
	SetText( input );
}

/**\brief Draw the Label
 */
void Label::Draw(  int relx, int rely ) {
	int sx, sy;
	
	sx = GetX() + relx;
	sy = GetY() + rely;
	
	// draw the label
	Font::XPos xpositioning = (centered) ? (Font::CENTER) : (Font::LEFT);
	Font::YPos ypositioning = (centered) ? (Font::MIDDLE) : (Font::TOP);

	UI::font->Render( sx, sy, text, xpositioning, ypositioning );

	Widget::Draw( relx, rely + UI::font->TightHeight() / 2 );
}

/**\brief Set the text string of this Widget
 */
void Label::SetText(string newText) {
	text = newText;
	name = text;
	if( text.find("\n") != string::npos )
	{
		LogMsg(WARN, "Multiline Label: %s at %ld", text.c_str(), text.find("\n") );
	}
	w = UI::font->TextWidth( text );
	h = UI::font->TightHeight( );
}

/**\brief Append some text to the current text
 */
void Label::AppendText(string moreText) {
	SetText( text + moreText );
}

/** @} */
