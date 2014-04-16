/**\file			ui_picture.cpp
 * \author			Matt Zweig
 * \date			Created: Tuesday, November 2, 2009
 * \date			Modified: Friday, November 14, 2009
 * \brief			Widget for displaying Images
 * \details
 */

#include "includes.h"
#include "Graphics/video.h"
#include "Graphics/image.h"
#include "UI/ui.h"
#include "UI/ui_picture.h"
#include "Utilities/log.h"

/** \addtogroup UI
 * @{
 */

/**\class Picture
 * \brief UI picture. */

/**\brief The default settings for a Picture.
 */
void Picture::Default( int x, int y, int w, int h ) {
	this->x = x;
	this->y = y;

	this->w = w;
	this->h = h;
	this->name = "";
	rotation = 0.;
	bitmap = NULL;

	color = BLACK;
	alpha = 0.0f;
	stretch = false;
}

/**\brief Initialize from an Image pointer.
 */
Picture::Picture( int x, int y, int w, int h, Image* pic, bool allow_stretching, bool center ) {
	Default(x, y, w, h);

	bitmap = pic;
	if( bitmap ) {
		name = bitmap->GetPath();
	}

	// If the bitmap exists, this must have a name.  Otherwise the name should be blank.
	if(bitmap) assert(name != "");
	else assert(name == "");

	if(( allow_stretching == false) && bitmap) {
		if( bitmap->GetWidth() < w ) this->w = bitmap->GetWidth();
		if( bitmap->GetHeight() < h ) this->h = bitmap->GetHeight();
	}

	stretch = allow_stretching;

	if(center) {
		Center(x, y);
	}
}

/**\brief Initialize from an Image pointer.
 */
Picture::Picture( int x, int y, Image* pic ) {
	Default(x, y, w, h);

	bitmap = pic;
	if( bitmap ) {
		name = bitmap->GetPath();
		this->w = bitmap->GetWidth();
		this->h = bitmap->GetHeight();
	}

	// If the bitmap exists, this must have a name.  Otherwise the name should be blank.
	if(bitmap) assert(name != "");
	else assert(name == "");
}

/**\brief Initialize from an Image name
 */
Picture::Picture( int x, int y, int w, int h, string filename ) {
	Default(x, y, w, h);

	bitmap = Image::Get(filename);
	if( bitmap )
	{
		// Only allow down-sampling
		if(w > bitmap->GetWidth()) this->w = bitmap->GetWidth();
		if(h > bitmap->GetHeight()) this->h = bitmap->GetHeight();

		name = bitmap->GetPath();
	}

	// If the bitmap exists, this must have a name.  Otherwise the name should be blank.
	if(bitmap) assert(name != "");
	else assert(name == "");
}

/**\brief Initialize from an Image name with centering
 */
Picture::Picture( int x, int y, int w, int h, string filename, bool center ) {
	Picture(x, y, w, h, filename);
	Center(x, y);
}

/**\brief Initialize from an Image name using the Image size
 *
 */
Picture::Picture( int x, int y, string filename ) {
	Default(x, y, 0, 0);

	bitmap = Image::Get(filename);
	if( bitmap ) {
		w = bitmap->GetWidth();
		h = bitmap->GetHeight();
	}

	name = filename;
	assert( !((bitmap != NULL) ^ (name != "")) ); // (NOT XOR) If the bitmap exists, it must have a name.  Otherwise the name should be blank.
}

/**\brief Rotate the Image in this picture to a specific angle.
 */
void Picture::Rotate(double angle) {
	rotation = angle;
}

/**\brief Center the Image on (x, y).
 */
void Picture::Center(int x, int y) {
	this->x = x - (w / 2);
	this->y = y - (h / 2);
}

/**\brief Draw this Picture
 */
void Picture::Draw( int relx, int rely ) {
	int x, y;
	x = this->x + relx;
	y = this->y + rely;

	if( !hidden) {
		// The Picture size
		Video::DrawRect( x, y,
		               w, h,
		               color.r,color.g,color.b,alpha );
		if(bitmap != NULL) {
			if(stretch) {
				bitmap->DrawStretch( x, y, w, h, static_cast<float>(rotation));
			} else {
				bitmap->DrawFit( x, y, w, h, static_cast<float>(rotation));
			}
		}
	}

	Widget::Draw(relx,rely);
}

/**\brief Change the Image in this Picture.
 */
void Picture::Set( Image *img ){
	// Potential Memory Leak
	// If the previous bitmap was created from new,
	// then that image is now lost.
	// We can't delete it though, since it could be shared (eg, Ship Model).
	bitmap = img;
	name = img->GetPath();
	assert( !((bitmap!=NULL) ^ (name!="")) ); // (NOT XOR) If the bitmap exists, it must have a name.  Otherwise the name should be blank.

	w = bitmap->GetWidth();
	h = bitmap->GetHeight();
}

/**\brief Change the Image in this Picture.
 */
void Picture::Set( string filename ){
	// Potential Memory Leak
	// If the previous bitmap was created from new,
	// then that image is now lost
	bitmap = Image::Get(filename);
	name = bitmap->GetPath();
	assert( !((bitmap!=NULL) ^ (name!="")) ); // (NOT XOR) If the bitmap exists, it must have a name.  Otherwise the name should be blank.

	w = bitmap->GetWidth();
	h = bitmap->GetHeight();
}

/**\brief Set the Background color and alpha
 * \details Since the default alpha is 0.0, Pictures default to no background.
 */
void Picture::SetColor( float r, float g, float b, float a) {
	color = Color(r,g,b);
	alpha = a;
}

/** @} */

