/*
 * Filename      : effect.h
 * Author(s)     : Matt Zweig
 * Date Created  : Tuesday, December 15, 2009
 * Last Modified : Tuesday, December 15, 2009
 * Purpose       : Sprite SubClass for Animated backgrounds
 * Notes         :
 */

#ifndef __H_EFFECT__
#define __H_EFFECT__

#include "graphics/animation.h"
#include "sprites/sprite.h"
#include "graphics/image.h"
#include "includes.h"

class Effect : public Sprite {
	public:
		Effect(Coordinate pos, string filename, float loopPercent);
		~Effect();
		void Update( lua_State *L );
		void Draw(void);
		virtual int GetDrawOrder( void ) {
			return( DRAW_ORDER_EFFECT);
		}
	private:
		Animation *visual;
};

#endif // __H_EFFECT__
