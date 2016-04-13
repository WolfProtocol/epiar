/**\file			camera.h
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Tuesday, April 12, 2016
 * \brief
 * \details
 */

#ifndef __h_camera__
#define __h_camera__

#include "includes.h"
#include "sprites/sprite.h"
#include "utilities/coordinate.h"
#include "sprites/spritemanager.h"

class Camera {
	public:
		Camera();

		void Focus( double x, double y );
		void Focus( Sprite *sprite );

		void Flash( short duration );

		// takes world coordinates and translates them into drawing coords
		void TranslateWorldToScreen( Coordinate &world, Coordinate &screen );
		void TranslateScreenToWorld( Coordinate &world, Coordinate &screen );
		// gives the most recent change in camera coordinates
		void GetDelta( double *dx, double *dy );
		Coordinate GetFocusCoordinate();

		void Update( SpriteManager *sprites );

		// Draw any effects the camera may be responsible for
		void Draw( void );

		// moves the camera given a delta. this is not the common method to
		// move the camera, you should use Focus()
		void Move( int dx, int dy );
		void setZoom(float z){
			if(zoom+z > 0.68 && zoom+z < 1.22) {
				zoom = zoom+z;
			}
		}
		float getZoom() { return zoom; }
		/*shakes the camera (duration= how long to shake in updates),
		(intesity multiplyer is how hard to shake eg. 3)
		(source is the coordinate of the object that hit the ship)  */
		void Shake( Uint32 duration, int intensity, Coordinate* source );

	private:
		int cameraShakeDur;
		int cameraShakeXOffset;
		int cameraShakeYOffset;
		int cameraShakeXDec;
		int cameraShakeYDec;

		int focusID; // focused on sprite - always favored, use NULL to set camera to static locations
		double x, y; // point where camera is looking
		double dx, dy; // the difference in the current and last camera position
		               // (this is used by Starfield)
		float zoom; // current zoom, zoom = 1. means no zooming
		bool hasZoomed;
		void UpdateShake();

		// If timestamp < flashUntil, we draw a flash over the display (for the jump animation)
		Uint32 flashUntil;
};

#endif // __h_camera__
