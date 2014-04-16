/**\file			audio.cpp
 * \author			Maoserr
 * \date			Created: Saturday, February 06, 2010
 * \date			Modified: Saturday, February 06, 2010
 * \brief			Abstraction to SDL_mixer interface.
 * \details
 * This files is responsible for overall Audio system configuration.  To play
 * music or sound see the corresponding Music and Sound classes.
 */

#include "includes.h"
#include "audio/audio.h"
#include "Utilities/log.h"

/**\class Audio
 * \brief This class is responsible for overall Audio system configuration.
 * \details
 * The Audio instance is implemented as a singleton.
 * \sa Sound
 * \sa Music
 */

/**\brief Creates or retrieves the current instance of the Audio device.
 */
Audio& Audio::Instance(){
	static Audio _instance;
	return _instance;
}

/**\brief Audio system initialization.
 */
bool Audio::Initialize( void ){
	if ( this -> initstatus )
		return false;				// Already initialized

	if(SDL_Init(SDL_INIT_AUDIO) < 0) {
		LogMsg(CRITICAL,"SDL could not initialize audio: %s", SDL_GetError());
	}

	if(Mix_OpenAudio(this->audio_rate,
				this->audio_format,
				this->audio_channels,
				this->audio_buffers)){
		LogMsg(CRITICAL,"Audio initialization failed!: %s", Mix_GetError());
		return false;
	}

	Mix_QuerySpec(&this->audio_rate, &this->audio_format, &this->audio_channels);
	LogMsg(INFO,"SDL_mixer gave us rate %d, format %d, channels %d", this->audio_rate, this->audio_format, this->audio_channels);

#if defined(SDL_MIXER_MAJOR_VERSION) && (SDL_MIXER_MAJOR_VERSION>1) \
	&& (SDL_MIXER_MINOR_VERSON>2) && (SDL_MIXER_PATCHLEVEL>=10)
	// Load MOD and OGG libraries (If SDL_mixer version supports it)
	const SDL_version *mix_version=Mix_Linked_Version();
	if( (mix_version->major >= 1) && (mix_version->minor >= 2) && (mix_version->patch >= 10) ){
		LogMsg(INFO,"This SDL_mixer version supports dynamic library loading.");
		Mix_Init( MIX_INIT_MOD | MIX_INIT_OGG );
	}
#endif // SDL_MIXER version requirements

	// Allocate channels
	Mix_AllocateChannels( this->max_chan);
	assert( this->max_chan == static_cast<unsigned int>(this->GetTotalChannels()) );

	return true;
}

/**\brief Audio system shutdown.
 */
bool Audio::Shutdown( void ){
	/* This is the cleaning up part */
	this->HaltAll();


#if defined(SDL_MIXER_MAJOR_VERSION) && (SDL_MIXER_MAJOR_VERSION>1) \
	&& (SDL_MIXER_MINOR_VERSON>2) && (SDL_MIXER_PATCHLEVEL>=10)
	// Free every library loaded
	const SDL_version *mix_version=Mix_Linked_Version();
	if( (mix_version->major>=1) && (mix_version->minor>=2) && (mix_version->patch>=10) ){
		while(Mix_Init(0))
			Mix_Quit();
	}
#endif // SDL_Mixer version requirements

	// Query number of times audio device was opened (should be 1)
	int freq, chan, ntimes;
	Uint16 format;
	ntimes = Mix_QuerySpec( &freq, &format, &chan );

	LogMsg(INFO,"Audio Query: %d Frequencies, Format: %d, Channels: %s.", freq, format, (chan==2?"Stereo":"Mono"));

	if(ntimes != 1 ) {
		LogMsg(WARN,"Audio was initialized %d times.", ntimes);
	}

	// Close only if open
	if( ntimes ) {
		Mix_CloseAudio();
	}

	return true;
}

/**\brief Halts all currently playing sounds.
 */
void Audio::HaltAll( void ){
	Mix_HaltChannel( -1 );			// Halts all channels
}

/**\brief Sets the music volume (Range from 0 - 1 ).
 */
bool Audio::SetMusicVol( float volume ){
	bool exceed_bounds = false;
	if ( volume < 0 ){
		LogMsg(WARN,"Volume (%f) must be >= 0.", volume);
		volume = 0;
		exceed_bounds = true;
	} else if ( volume > 1 ){
		LogMsg(WARN,"Volume (%f) must be <= 1.", volume);
		volume = 1;
		exceed_bounds = true;
	}

	int volumeset;
	int volumeint = static_cast<int>(volume*AUDIO_MAX_VOL);
	Mix_VolumeMusic( volumeint );
	volumeset = Mix_VolumeMusic( -1 );
	if ( volumeset != volumeint ){
		LogMsg(ERR,"There was an error setting the volume.");
		return false;
	}
	if ( exceed_bounds )
		return false;

	this->music_vol = volume;
	return true;
}

/**\brief Sets sound volume (Range from 0 - 1).
 */
bool Audio::SetSoundVol( float volume ){
	if ( volume < 0 ){
		LogMsg(WARN,"Volume (%f) must be >= 0.", volume);
		this->sound_vol = 0;
		return false;
	} else if ( volume > 1 ){
		LogMsg(WARN,"Volume (%f) must be <= 1.", volume);
		this->sound_vol = 1;
		return false;
	}

	this->sound_vol = volume;
	return true;
}

/**\brief Retrieves the first available channel.
 */
int Audio::GetFreeChannel( void ){
	/**\todo Optimization: We could consider dynamically allocating.*/
	// Find first available channel
	int foundchan = Mix_GroupAvailable( -1 );
	if ( foundchan != -1 )
		return foundchan;

	// No channels available, halt oldest used one
	assert( this->lastplayed.size() != 0 );
	Mix_HaltChannel( this->lastplayed.front() );

	return this->lastplayed.front();
}

/**\brief Retrieves total number of mixing channels.
 */
int Audio::GetTotalChannels( void ){
	return Mix_AllocateChannels( -1 );
}

/**\brief Wrapper for Mix_PlayChannel.
 * \param chan Use -1 for any available channel
 * \param chunk The Mix_Chunk to play
 * \param loop Specify if looping is desired ( chunk will play 1+loop times)
 * \details
 * Plays the chunk on specified channel.
 */
int Audio::PlayChannel( int chan, Mix_Chunk *chunk, int loop ){
	int chan_used;			// Channel that was used to play a sound
	if ( chan == -1 ){
		int freechan = this->GetFreeChannel();
		int chan_vol = Mix_Volume(freechan, -1);
		// Scale channel volume by global volume
		int scaled_vol = static_cast<int>(static_cast<float>(chan_vol)*this->sound_vol);
		Mix_Volume( freechan, scaled_vol );
		assert( Mix_Volume(freechan, -1) == scaled_vol);
		chan_used = Mix_PlayChannel( freechan, chunk, loop );
	}else{
		int chan_vol = Mix_Volume(chan, -1);
		// Scale channel volume by global volume
		int scaled_vol = static_cast<int>(static_cast<float>(chan_vol)*this->sound_vol);
		Mix_Volume( chan, scaled_vol );
		assert( Mix_Volume(chan,-1)  == scaled_vol);
		chan_used = Mix_PlayChannel( chan, chunk, loop );
	}

	/**\todo This could be optimized.*/
	this->lastplayed.push_back( chan_used );
	// Check if queue is full
	if ( this->lastplayed.size() > this->max_chan ){
		this->lastplayed.pop_front( );
		assert( lastplayed.size() <= this->max_chan );
	}
	return chan_used;
}

/**\brief Empty constructor (use initialization lists to initialize privates.
 */
Audio::Audio():
	initstatus( false ),
	audio_rate( 22050 ),
	audio_format( MIX_DEFAULT_FORMAT ),
	audio_channels( 2 ),
	audio_buffers( 1024 ),
	sound_vol( 1 ),
	max_chan( 16 )
{
}

/**\brief Empty destructor
 */
Audio::~Audio(){
}
