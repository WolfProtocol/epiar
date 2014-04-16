/*
 * Filename      : gate.h
 * Author(s)     : Matt Zweig
 * Date Created  : Tuesday, March 16, 2010
 * Last Modified : Tuesday, March 16, 2010
 * Purpose       : Sprite SubClass for Warp Gates
 * Notes         : A gate is a two-part Sprite that ships can move through
 */

#ifndef __H_GATE__
#define __H_GATE__

#include "includes.h"
#include "Sprites/sprite.h"
#include "Sprites/ship.h"
#include "Graphics/image.h"
#include "Graphics/animation.h"
#include "Utilities/components.h"

#define GATE_RADIUS 20000

class Gate : public Sprite, public Component {
	public:
		Gate(Coordinate pos = Coordinate(0,0), string name="" );

		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);

		virtual int GetDrawOrder( void ) {
			return( top? DRAW_ORDER_GATE_TOP : DRAW_ORDER_GATE_BOTTOM );
		}

		// These override the normal Sprite Setters
		// They modify both this and the partner Gate at the same time
		void SetAngle(float angle);
		void SetWorldPosition(Coordinate pos);
		void SetExit(int SpriteID);

		static void SetPair(Gate* one, Gate* two);

		Gate* GetTop();
		Sprite* GetExit();

		void Update( lua_State *L );
	private:
		bool top; ///< True if this Sprite is on Top.
		int partnerID; ///< The partner is the top/bottom of this gate
		int exitID; ///< Ships entering this gate will be transported to the Exit Gate

		void SendToRandomLocation(Sprite* ship);
		void SendToExit(Ship* ship);
		void SendRandomDistance(Sprite* ship);

		Gate(int topID);
		// These setters modify this and only this Gate (not the partner)
		void _SetAngle(float angle) { Sprite::SetAngle(angle); }
		void _SetWorldPosition(Coordinate c) { Sprite::SetWorldPosition(c); }
		Gate* GetPartner();
};

class Gates : public Components {
	public:
		static Gates *Instance();
		Gate *GetGate( string& GateName ) { return (Gate*) this->Get(GateName); }
		Component* newComponent() { return new Gate(); }
		
	protected:
		Gates() {};
		Gates( const Gates & );
		Gates& operator= (const Gates&);

	private:
		static Gates *pInstance;
};

#endif // __H_GATE__

