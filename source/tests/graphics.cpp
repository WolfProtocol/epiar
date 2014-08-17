/**\file			graphics.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Saturday, January 31, 2009
 * \date			Modified: Sunday, November 22, 2009
 * \brief			Graphics demo & debugging
 * \details
 */

#include "includes.h"
#include "common.h"
#include "graphics/image.h"
#include "graphics/video.h"

int test_graphics(int argc, char **argv) {

	int cnt=5;
	do{
		Video::Erase();

		// draw a grid
		// -- draw the horizontal lines
		for(int j = 50; j < Video::GetHeight(); j += 50) {
			Video::DrawRect(0, j, Video::GetWidth(), 1, .4f, .4f, .4f);
		}
		// -- draw the vertical lines
		for(int i = 50; i < Video::GetWidth(); i += 50) {
			Video::DrawRect(i, 0, 1, Video::GetHeight(), .4f, .4f, .4f);
		}

		// draw four test circles
		Video::DrawCircle(50, 50, 50, 1., 1., 1., 1.);
		Video::DrawCircle(50, 550, 50, 1., 1., 1., 1.);
		Video::DrawCircle(750, 50, 50, 1., 1., 1., 1.);
		Video::DrawCircle(750, 550, 50, 1., 1., 1., 1.);

		// load an image and draw it in the center of the screen
		Image planet2("resources/graphics/planet2.png");
		planet2.DrawCentered(400+cnt*50, 300, 0.);

		Image frigate("resources/graphics/terran-frigate.png");
		frigate.DrawCentered(400+cnt*50, 300, 45.);

		Video::Update();

		SDL_Delay(1000);
	}while(cnt--);
	return 0;
}

