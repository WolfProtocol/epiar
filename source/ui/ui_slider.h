/**\file			ui_slider.h
 * \author			Maoserr
 * \date			Created: Saturday, March 13, 2010
 * \date			Modified: Saturday, March 13, 2010
 * \brief			Creates a slider widget
 */

#ifndef __H_SLIDER__
#define __H_SLIDER__

#include "ui/ui.h"

class Slider : public Widget {
	public:
		Slider( int x, int y, int w, int h, const string& label, float value = 0.5f);

		float GetVal( void ){ return this->val;};
		float GetMin( void ){ return this->minval;};
		float GetMax( void ){ return this->maxval;};

		void Draw( int relx=0, int rely = 0 );

		string GetType( void ) {return string("Slider"); }
		virtual int GetMask( void ) { return WIDGET_SLIDER; }

		void SetVal( float value );

	protected:
		bool MouseDrag( int xi, int yi );
		bool MouseLDown( int xi, int yi );
		bool MouseLUp( int xi, int yi );

	private:
		// Utility functions to convert between pixel and values
		int ValToPixel( float value );
		float PixelToVal( int pixels);

		float minval,maxval,val;

		Image *left;
		Image *right;
		Image *background;
		Image *bar;
		Image *handle;
};

#endif // __H_SLIDER__

