/**\file			ui_checkbox.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Monday, December 28, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "graphics/font.h"
#include "graphics/video.h"
#include "ui/ui.h"
#include "ui/ui_checkbox.h"
#include "utilities/log.h"
#include "utilities/lua.h"

/** \addtogroup UI
 * @{
 */

/**\class Checkbox
 * \brief UI checkbox. */

/**\brief Constructs a new Checkbox.
 */
Checkbox::Checkbox( int x, int y, bool checked, string label ) {
	blank_box = Image::Get( "data/skin/ui_checkbox_blank.png" );
	check_box = Image::Get( "data/skin/ui_checkbox_check.png" );

	this->x = x;
	this->y = y;
	this->w = UI::font->TextWidth(label) + check_box->GetWidth() + 5;
	this->h = ( UI::font->LineHeight() > check_box->GetHeight() )
	          ? UI::font->LineHeight()
	          : check_box->GetHeight();
	
	this->name = label;
	this->checked = checked;
}

void Checkbox::Draw( int relx, int rely ) {
	int x, y;
	
	x = this->x + relx;
	y = this->y + rely;
	
	if( checked ) {
		check_box->Draw( x,y );
	} else {
		blank_box->Draw( x,y );
	}

	// draw the label
	UI::font->RenderTight( x + check_box->GetWidth() + 5, y, name );

	Widget::Draw(relx,rely);
}

bool Checkbox::MouseLUp( int xi, int yi ) {
	checked = !checked;
	Widget::MouseLUp( xi, yi );
	if(OPTION(int, "options/sound/buttons")) UI::beep->Play();
	return true;
}

/** @} */
