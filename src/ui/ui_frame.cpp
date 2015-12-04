/**\file			ui_frame.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: August 24, 2010
 * \date			Modified: August 24, 2010
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "graphics/video.h"
#include "ui/ui.h"
#include "ui/ui_frame.h"
#include "utilities/log.h"

/** \addtogroup UI
 * @{
 */

/**\class Frame
 * \brief Frame handling. */

/**\brief Creates a new frame with specified parameters.
 */
Frame::Frame( int x, int y, int w, int h )
{

	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;

	// Load the bitmaps needed for drawing
	bitmaps[0] = Image::Get( "data/skin/ui_frame_up_left.png" );
	bitmaps[1] = Image::Get( "data/skin/ui_frame_up.png" );
	bitmaps[2] = Image::Get( "data/skin/ui_frame_up_right.png" );
	bitmaps[3] = Image::Get( "data/skin/ui_frame_left.png" );
	bitmaps[4] = Image::Get( "data/skin/ui_frame_right.png" );
	bitmaps[5] = Image::Get( "data/skin/ui_frame_low_left.png" );
	bitmaps[6] = Image::Get( "data/skin/ui_frame_low.png" );
	bitmaps[7] = Image::Get( "data/skin/ui_frame_low_right.png" );
	bitmaps[8] = Image::Get( "data/skin/ui_frame_back.png" );

	// All of these must exist
	assert( bitmaps[0] != NULL );
	assert( bitmaps[1] != NULL );
	assert( bitmaps[2] != NULL );
	assert( bitmaps[3] != NULL );
	assert( bitmaps[4] != NULL );
	assert( bitmaps[5] != NULL );
	assert( bitmaps[6] != NULL );
	assert( bitmaps[7] != NULL );
	assert( bitmaps[8] != NULL );

	SetInnerRect( 8, 8, 8, 8 );
}

/**\brief Adds a widget to the current Frame.
 */
Frame *Frame::AddChild( Widget *widget ){
	assert( widget != NULL );
	Container::AddChild( widget );
	return this;
}

/**\brief Draws the current frame.
 */
void Frame::Draw( int relx, int rely ) {
	int x, y;
	
	x = GetX() + relx;
	y = GetY() + rely;
	
	// Draw the background
	bitmaps[8]->DrawTiled( x + bitmaps[3]->GetWidth(), y + bitmaps[1]->GetHeight(), w - bitmaps[3]->GetWidth() - bitmaps[4]->GetWidth(), h - bitmaps[1]->GetHeight() - bitmaps[6]->GetHeight() );
	
	// Draw the top section
	bitmaps[0]->Draw( x, y );
	bitmaps[1]->DrawTiled( x + bitmaps[0]->GetWidth(), y, w - bitmaps[0]->GetWidth() - bitmaps[2]->GetWidth(), bitmaps[1]->GetHeight() );
	bitmaps[2]->Draw( x + w - bitmaps[2]->GetWidth(), y );
	
	// Draw the left and right sections
	bitmaps[3]->DrawTiled( x, y + bitmaps[0]->GetHeight(), bitmaps[3]->GetWidth(), h - bitmaps[0]->GetHeight() - bitmaps[5]->GetHeight() );
	bitmaps[4]->DrawTiled( x + w - bitmaps[4]->GetWidth(), y + bitmaps[0]->GetHeight(), bitmaps[4]->GetWidth(), h - bitmaps[0]->GetHeight() - bitmaps[5]->GetHeight() );
	
	// Draw the bottom section
	bitmaps[5]->Draw( x, y + h - bitmaps[5]->GetHeight() );
	bitmaps[6]->DrawTiled( x + bitmaps[5]->GetWidth(), y + h - bitmaps[6]->GetHeight(), w - bitmaps[5]->GetWidth() - bitmaps[7]->GetWidth(), bitmaps[6]->GetHeight() );
	bitmaps[7]->Draw( x + w - bitmaps[7]->GetWidth(), y + h - bitmaps[7]->GetHeight() );

	Container::Draw(relx,rely);
}

/** @} */
