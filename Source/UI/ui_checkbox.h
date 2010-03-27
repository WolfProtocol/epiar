/**\file			ui_checkbox.h
 * \author			Christopher Thielen (chris@luethy.net)
 * \date			Created: Monday, December 28, 2009
 * \brief
 * \details
 */

#ifndef __H_CHECKBOX__
#define __H_CHECKBOX__

#include "Graphics/image.h"
#include "UI/ui.h"

#define CHECKBOX_W 10
#define CHECKBOX_H 10

class Checkbox : public Widget {
	public:
		Checkbox( int x, int y, bool checked, string label);
		
		void Draw( int relx = 0, int rely = 0 );

		int GetW( void ) { return CHECKBOX_W; };
		int GetH( void ) { return CHECKBOX_H; };

		bool IsChecked() {return checked;}
		void Set(bool val) {checked = val;}
	
		bool MouseLUp( int wx, int wy );
		string GetType( void ) { return string("Checkbox"); }
	private:
		string label;
		bool checked;
};

#endif // __H_CHECKBOX__
