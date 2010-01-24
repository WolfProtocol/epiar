/**\file			spritemanager.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \brief
 * \details
 */


#ifndef __H_SPRITEMANAGER__
#define __H_SPRITEMANAGER__

#include "Sprites/sprite.h"
#include "Utilities/quadtree.h"


class SpriteManager {
	public:
		static SpriteManager *Instance();

		void Add( Sprite *sprite );
		bool Delete( Sprite *sprite );
		
		void Update();
		void Draw();

		bool LoadNPCs( string filename );
		
		Sprite *GetSpriteByID(int id);
		list<Sprite*> *GetSprites();
		list<Sprite*> *GetSpritesNear(Coordinate c, float r);

		Coordinate GetQuadrantCenter( Coordinate point );
		int GetNumQuadrants() { return trees.size(); }
		int GetNumSprites();

	protected:
		SpriteManager();
	private:
		// Use the tree when referring to the sprites at a location.
        map<Coordinate,QuadTree*> trees;
		// Use the list when referring to all sprites.
		list<Sprite*> *spritelist;
		// Use the map when referring to sprites by their unique ID.
		map<int,Sprite*> *spritelookup;
		
		list<Sprite *> spritesToDelete;
		static SpriteManager *pInstance;
		bool DeleteSprite( Sprite *sprite );
		QuadTree* GetQuadrant( Coordinate point );
};

#endif // __H_SPRITEMANAGER__

