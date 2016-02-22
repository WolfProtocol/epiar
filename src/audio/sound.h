/**\file			sound.h
 * \author			Maoserr
 * \date			Created: Monday, February 08, 2010
 * \date			Modified: Monday, February 08, 2010
 * \brief			Implements sound playing abilities.
 * \details
 */

#ifndef __H_SOUND__
#define __H_SOUND__

#include "utilities/coordinate.h"
#include "utilities/file.h"
#include "utilities/resource.h"

class Sound : public Resource {
	public:
		static Sound *Get( const string& filename );
		Sound( const string& filename );
		~Sound( void );
		bool Play( void );
		bool Play( Coordinate offset );
		bool PlayNoRestart( Coordinate offset );
		bool SetVolume( float volume );
		void SetFactors( double fade, float pan );
		string GetPath( void ) { return pathName.GetRelativePath(); }

	private:
		Mix_Chunk *sound;
		File pathName;
		int channel;		// Last channel the sound is playing on.
		double fadefactor;	// Scale factor to fade by as distance drops off
		float panfactor;	// Scale factor to pan by, higher = more sensitive
		int volume;		// Volume for this sound
};


#endif // __H_SOUND__
