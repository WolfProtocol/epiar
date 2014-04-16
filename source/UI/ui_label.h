/**\file			ui_label.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Friday, April 25, 2008
 * \date			Modified: Friday, April 25, 2008
 * \brief
 * \details
 */

#ifndef __H_LABEL__
#define __H_LABEL__

#include "UI/ui.h"

class Label : public Widget {
	public:
		Label( int x, int y, string label, bool centered=false );
		~Label() { };
		
		void Draw(  int relx = 0, int rely = 0 );

		void SetText(string text);
		void AppendText(string text);
		string GetText(){return name;}
	
		string GetType( void ) {return string("Label");}
		virtual int GetMask( void ) { return WIDGET_LABEL; }
	
	private:
		bool centered;
		string text;
};

#endif // __H_LABEL__
