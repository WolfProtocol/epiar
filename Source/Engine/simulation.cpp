/**\file			simulation.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: July 2006
 * \date			Modified: Tuesday, June 23, 2009
 * \brief			Contains the main game loop
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Audio/music.h"
#include "Engine/hud.h"
#include "Engine/simulation.h"
#include "Engine/commodities.h"
#include "Engine/alliances.h"
#include "Engine/technologies.h"
#include "Engine/starfield.h"
#include "Graphics/video.h"
#include "Input/input.h"
#include "Sprites/player.h"
#include "Sprites/gate.h"
#include "Sprites/spritemanager.h"
#include "UI/ui.h"
#include "Utilities/camera.h"
#include "Utilities/file.h"
#include "Utilities/log.h"
#include "Utilities/timer.h"
#include "Utilities/lua.h"
#include "AI/ai.h"

/**\class Simulation
 * \brief Handles main game loop. */

float Simulation::currentFPS = 0.;
bool Simulation::paused = false;

/**\brief Loads an empty Simulation.
 */
Simulation::Simulation( void ) {
	commodities = Commodities::Instance();
	engines = Engines::Instance();
	planets = Planets::Instance();
	models = Models::Instance();
	weapons = Weapons::Instance();
	alliances = Alliances::Instance();
	technologies = Technologies::Instance();
	outfits = Outfits::Instance();
	players = Players::Instance();
	currentFPS = 0.;
}

/**\brief Loads the XML file.
 * \param filename Name of the file
 * \return true if success
 */
bool Simulation::Load( string filename ) {
	if( !Open(filename) ) {
		return false;
	}
	return Parse();
}

/**\brief Pauses the simulation
 */
void Simulation::pause(){
	paused = true;
}

/**\brief Unpauses the simulation
 */
void Simulation::unpause(){
	paused = false;
}

/**\brief Main game loop
 * \return true
 */
bool Simulation::Run() {
	bool quit = false;
	Input inputs;
	int fpsCount = 0; // for FPS calculations
	int fpsTotal= 0; // for FPS calculations
	Uint32 fpsTS = 0; // timestamp of last FPS printing

	Timer::Update(); // Start the Timer

	// Grab the camera and give it coordinates
	Camera *camera = Camera::Instance();
	camera->Focus(0, 0);
	
	Timer::Initialize();

	// Generate a starfield
	Starfield starfield( OPTION(int, "options/simulation/starfield-density") );

	// Create a spritelist
	SpriteManager *sprites = SpriteManager::Instance();

	Planets *planets = Planets::Instance();
	list<string>* planetNames = planets->GetNames();
	for( list<string>::iterator pname = planetNames->begin(); pname != planetNames->end(); ++pname){
		sprites->Add(  planets->GetPlanet(*pname) );
	}

	// Start the Lua Universe
	if( !( Lua::Load("Resources/Scripts/universe.lua") ))
	{
		LogMsg(ERR,"Fatal error starting Lua.");
		quit = true;
	}
    if( 0 == OPTION(int,"options/development/editor-mode") ){
        if( !( Lua::Load("Resources/Scripts/player.lua") ))
        {
            LogMsg(ERR,"Fatal error starting Lua.");
            quit = true;
        }
    } else {
        if( !( Lua::Load("Resources/Scripts/editor.lua") ))
        {
            LogMsg(ERR,"Fatal error starting Lua.");
            quit = true;
        }
    }
    Lua::Call("Start");

	// Message appear in reverse order, so this is upside down
	Hud::Alert("-----------------------------------");
	Hud::Alert("Please Report all bugs to epiar.net");
	Hud::Alert("Epiar is currently under development.");

	fpsTS = Timer::GetTicks();

	// Load sample game music
	if(bgmusic && OPTION(int, "options/sound/background"))
		bgmusic->Play();

	// main game loop
	while( !quit ) {
		quit = inputs.Update();
		
		int logicLoops = Timer::Update();
		if( !paused ) {
			while(logicLoops--) {
				Lua::Call("Update");
				// Update cycle
				starfield.Update();
				sprites->Update();
				camera->Update();
				Hud::Update();
			}
		}

		// Erase cycle
		Video::Erase();
		
		// Draw cycle
		starfield.Draw();
		sprites->Draw();
		Hud::Draw();
		UI::Draw();
		Video::Update();
		
		// Don't kill the CPU (play nice)
		Timer::Delay();
		
		// Counting Frames
		fpsCount++;
		fpsTotal++;

		// Update the fps once per second
		if( (Timer::GetTicks() - fpsTS) >1000 ) {
			Simulation::currentFPS = static_cast<float>(1000.0 *
					((float)fpsCount / (Timer::GetTicks() - fpsTS)));
			fpsTS = Timer::GetTicks();
			fpsCount = 0;
			if( currentFPS < 0.1f )
			{
				// The game has effectively stopped..
				LogMsg(ERR,"Sorry, the framerate has dropped to zero. Please report this as a bug to 'epiar-devel@epiar.net'");
				UI::Save();
				sprites->Save();
				quit = true;
			}

			if( OPTION(int, "options/log/ui") )
			{
				UI::Save();
			}

			if( OPTION(int, "options/log/sprites") )
			{
				sprites->Save();
			}
		}
	}

	Players::Instance()->Save(Get("players"));
	optionsfile->Save();

	LogMsg(INFO,"Average Framerate: %f Frames/Second", 1000.0 *((float)fpsTotal / Timer::GetTicks() ) );
	return true;
}

/**\brief Returns the current frames per second
 */
float Simulation::GetFPS() {
	return Simulation::currentFPS;
}

/**\brief Parses an XML simulation file
 * \return true if successful
 */
bool Simulation::Parse( void ) {
	LogMsg(INFO, "Simulation version %d.%d.%d.", Get("version-major").c_str(), Get("version-minor").c_str(),  Get("version-macro").c_str());

	// Now load the various subsystems
	if( commodities->Load( Get("commodities") ) != true ) {
		LogMsg(ERR, "There was an error loading the commodities from '%s'.", Get("commodities").c_str() );
		return false;
	}
	if( engines->Load( Get("engines") ) != true ) {
		LogMsg(ERR, "There was an error loading the engines from '%s'.", Get("engines").c_str() );
		return false;
	}
	if( models->Load( Get("models") ) != true ) {
		LogMsg(ERR, "There was an error loading the models from '%s'.", Get("models").c_str() );
		return false;
	}
	if( weapons->Load( Get("weapons") ) != true ) {
		LogMsg(ERR, "There was an error loading the technologies from '%s'.", Get("weapons").c_str() );
		return false;
	}
	if( outfits->Load( Get("outfits") ) != true ) {
		LogMsg(ERR, "There was an error loading the outfits from '%s'.", Get("outfits").c_str() );
		return false;
	}
	if( technologies->Load( Get("technologies") ) != true ) {
		LogMsg(ERR, "There was an error loading the technologies from '%s'.", Get("technologies").c_str() );
		return false;
	}
	if( alliances->Load( Get("alliances") ) != true ) {
		LogMsg(ERR, "There was an error loading the alliances from '%s'.", Get("alliances").c_str() );
		return false;
	}
	if( planets->Load( Get("planets") ) != true ) {
		LogMsg(WARN, "There was an error loading the planets from '%s'.", Get("planets").c_str() );
		return false;
	}
	if( players->Load( Get("players"), true ) != true ) {
		LogMsg(WARN, "There was an error loading the players from '%s'.", Get("players").c_str() );
		return false;
	}
	
	bgmusic = Song::Get( Get("music") );
	if( bgmusic == NULL ) {
		LogMsg(WARN, "There was an error loading music from '%s'.", Get("music").c_str() );
	}

	return true;
}

/**\fn Simulation::isPaused()
 * \brief Checks to see if Simulation is paused
 */
