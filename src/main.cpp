/**\file		main.cpp
 * \author		Christopher Thielen (chris@epiar.net)
 * \author		and others.
 * \date		Created:	Sunday, June 4, 2006
 * \brief		Main entry point of Epiar codebase
 * \details
 *	This file performs two functions:
 *		- Runs the Epiar scenario.
 *		- Parse command line arguments.
 */

#include "includes.h"
#include "common.h"
#include "audio/audio.h"
#include "tests/graphics.h"
#include "graphics/font.h"
#include "graphics/video.h"
#include "menu.h"
#include "ui/ui.h"
#include "utilities/argparser.h"
#include "utilities/filesystem.h"
#include "utilities/log.h"
#include "utilities/lua.h"
#include "utilities/xml.h"
#include "utilities/timer.h"

#ifdef EPIAR_COMPILE_TESTS
#include "Tests/tests.h"
#endif // EPIAR_COMPILE_TESTS

// main configuration file, used through the tree (extern in common.h)
XMLFile *skinfile = NULL;
// main font used throughout the game
Font *SansSerif = NULL, *BitType = NULL, *Serif = NULL, *Mono = NULL;
ArgParser *argparser = NULL;
bool interpolateOn = true;

void Main_OS                ( int argc, char **argv ); ///< Run OS Specific setup code
void Main_Load_Settings     (); ///< Load the settings files
void Main_Init_Singletons   (); ///< Initialize global Singletons
void Main_Parse_Args        ( int argc, char **argv ); ///< Parse Command Line Arguments
void Main_Log_Environment   ( void ); ///< Record Environment variables
void Main_Close_Singletons  ( void ); ///< Close global Singletons

/**Main
 * \return 0 always
 * \details
 * This function does the following:
 *  - Load options
 *  - Load fonts
 *  - Runs the Scenario routine
 *  - Calls any cleanup code
 */
int main( int argc, char **argv ) {
	// Basic Setup
	Main_OS( argc, argv );
	Main_Load_Settings();

	// Respond to Command Line Arguments
	Main_Parse_Args( argc, argv );
	Main_Log_Environment();

	// THE GAME
	Main_Init_Singletons();
	Menu::Run();

	LogMsg(INFO, "Epiar shutting down." );

	// Close everything and Quit
	Main_Close_Singletons();

	return( 0 );
}

/** \details
 *  The OS Specific code here sets up OS specific environment variables and
 *  paths that are vital for normal operation.
 *
 *  Since nothing has is loaded or initialized before this code, do not use any
 *  code that is epiar specific (OPTIONS, Log, Lua, etc).
 *
 *  \param[in] argc standard c argc
 *  \param[in] argv standard c argv
 */
void Main_OS( int argc, char **argv ) {

#ifdef __APPLE__
	string path = argv[0];
	if( path.find("MacOS/Epiar") ){ // If this is being run from inside a Bundle
		// Chdir to the Bundle Contents
		string ContentsPath = path.substr(0, path.find("MacOS/Epiar") );
		chdir(ContentsPath.c_str());
	}
#endif

#ifdef _WIN32
	// This attaches a console to the parent process if it has a console
	if(AttachConsole(ATTACH_PARENT_PROCESS)){
		freopen("CONOUT$","wb",stdout);  // reopen stout handle as console window output
		freopen("CONOUT$","wb",stderr); // reopen stderr handle as console window output
	}
	#if defined(_MSC_VER) && defined(DEBUG)
		int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
		// Turn on leak-checking bit
		tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
		// Set flag to the new value
		_CrtSetDbgFlag( tmpFlag );
	#endif//_MSC_VER
#endif //_WIN32

	Filesystem::Init( argv[0] );
}

/** \brief Load the options files
 *  \details This will load the options.xml and skin.xml files.
 *           The options.xml file defines miscellaneous flags and numerical settings.
 *           The skin.xml file defines the non-png aspects of the User Interface.
 *  \todo If these files do not exist, reasonable defaults should be loaded instead.
 */
void Main_Load_Settings() {
	Options::Initialize( "options.xml" );

	// Logging
	Options::AddDefault( "options/log/xml", 0 );
	Options::AddDefault( "options/log/out", 0 );
	Options::AddDefault( "options/log/alert", 0 );
	Options::AddDefault( "options/log/ui", 0 );
	Options::AddDefault( "options/log/sprites", 0 );

	// Video
	Options::AddDefault( "options/video/w", 1024 );
	Options::AddDefault( "options/video/h", 768 );
	Options::AddDefault( "options/video/bpp", 32 );
	Options::AddDefault( "options/video/fullscreen", 0 );
	Options::AddDefault( "options/video/fps", 60 );

	// Sound
	Options::AddDefault( "options/sound/musicvolume", 0.5f );
	Options::AddDefault( "options/sound/soundvolume", 0.5f );
	Options::AddDefault( "options/sound/background", 1 );
	Options::AddDefault( "options/sound/weapons", 1 );
	Options::AddDefault( "options/sound/engines", 1 );
	Options::AddDefault( "options/sound/explosions", 1 );
	Options::AddDefault( "options/sound/buttons", 1 );

	// Simultaion
	Options::AddDefault( "options/scenario/automatic-load", 0 );

	// Timing
	Options::AddDefault( "options/timing/screen-swap", 0 ); // FIXME, 0=disabled until the transition is better
	Options::AddDefault( "options/timing/mouse-fade", 500 );
	Options::AddDefault( "options/timing/target-zoom", 500 );
	Options::AddDefault( "options/timing/alert-drop", 3500 );
	Options::AddDefault( "options/timing/alert-fade", 2500 );

	// Development
	Options::AddDefault( "options/development/debug-ai", 0 );
	Options::AddDefault( "options/development/debug-ui", 0 );

	// Allow the Options to be used
	Options::Unlock();

	skinfile = new XMLFile();
	if( !skinfile->Open("data/skin/skin.xml") )
	{
		// Create the default Skin file
		skinfile->New("data/skin/skin.xml", "Skin");

		// UI - Default
		skinfile->Set( "Skin/UI/Default/Font", "data/fonts/FreeSans.ttf");
		skinfile->Set( "Skin/UI/Default/Color", "0xFFFFFF");
		skinfile->Set( "Skin/UI/Default/Size", 12);

		// UI - Textbox
		skinfile->Set( "Skin/UI/Textbox/Font", "data/fonts/ConsolaMono.ttf");
		skinfile->Set( "Skin/UI/Textbox/Color/Foreground", "0xCCCCCC");
		skinfile->Set( "Skin/UI/Textbox/Color/Background", "0x666666");
		skinfile->Set( "Skin/UI/Textbox/Color/Edge", "0x262626");

		// UI - Tab
		skinfile->Set( "Skin/UI/Tab/Color/Active", "0x393939");
		skinfile->Set( "Skin/UI/Tab/Color/Inactive", "0x262626");

		// HUD - Alert
		skinfile->Set( "Skin/HUD/Alert/Font", "data/fonts/FreeSans.ttf");
		skinfile->Set( "Skin/HUD/Alert/Color", "0xFFFFFF");
		skinfile->Set( "Skin/HUD/Alert/Size", 12);

		skinfile->Save();
	}
}

/** \details
 *  This will initialize the singletons for this Epiar instance:
 *   - Audio
 *   - Fonts
 *   - Timer
 *   - Video
 *   - ArgParser
 *
 *  Singletons should be kept to a minimum whenever possible.
 *
 *  \param[in] argc standard c argc
 *  \param[in] argv standard c argv
 *
 *  \TODO Remove Fonts with a style.
 *  \TODO Add Logger
 *
 *  \warn This may exit early on Errors
 */
void Main_Init_Singletons() {
	Audio::Instance()->Initialize();
	Audio::Instance()->SetMusicVol ( OPTION(float,"options/sound/musicvolume") );
	Audio::Instance()->SetSoundVol ( OPTION(float,"options/sound/soundvolume") );

	Timer::Initialize();
	Video::Initialize();

	SansSerif       = new Font( "data/fonts/FreeSans.ttf", 12 );
	BitType         = new Font( "data/fonts/visitor2.ttf", 12 );
	Serif           = new Font( "data/fonts/FreeSerif.ttf", 12 );
	Mono            = new Font( "data/fonts/ConsolaMono.ttf", 12 );

	UI::Initialize("Main Screen");

	srand ( time(NULL) );
}

/** \details
 *  This cleanup is done for completeness, but the normal runtime should do all
 *  of this automatically.
 *  \warn Do not run any non-trivial code after calling this.
 */
void Main_Close_Singletons( void ) {
	Options::Save();

	// free the main font files
	delete SansSerif;
	delete BitType;
	delete Serif;
	delete Mono;

	Video::Shutdown();
	Audio::Instance()->Shutdown();

	// free the configuration file data
	delete skinfile;

	Filesystem::Close();
	Log::Instance().Close();
}

/** \details
 *  This processes all of the command line arguments using the ArgParser. As a
 *  general rule there are two kinds of Arguments:
 *   - Help Arguments that print the version, usage, etc.
 *   - OPTION Arguments that override a normal OPTION value.
 *   - Test Arguments that run Epiar Unit tests and then exit.
 *
 *  \warn This may exit early.
 */
void Main_Parse_Args( int argc, char **argv ) {
	// Parse command line options first.
	argparser = new ArgParser(argc, argv);

	argparser->SetOpt(SHORTOPT, "h",             "Display help screen");
	argparser->SetOpt(LONGOPT, "help",           "Display help screen");
	argparser->SetOpt(SHORTOPT, "v",             "Display program version");
	argparser->SetOpt(LONGOPT, "version",        "Display program version");
	argparser->SetOpt(LONGOPT, "disable-audio",  "Disables audio");
	argparser->SetOpt(LONGOPT, "fullscreen",     "Play in fullscreen mode");
	argparser->SetOpt(LONGOPT, "windowed",       "Play in windowed mode");
	argparser->SetOpt(LONGOPT, "nolog-xml",      "(Default) Disable logging messages to xml files.");
	argparser->SetOpt(LONGOPT, "log-xml",        "Log messages to xml files.");
	argparser->SetOpt(LONGOPT, "log-out",        "(Default) Log messages to console.");
	argparser->SetOpt(LONGOPT, "nolog-out",      "Disable logging messages to console.");
	argparser->SetOpt(VALUEOPT, "log-lvl",       "Logging level.(None,Fatal,Error,"
	                                             "\n\t\t\t\tWarn,Info,Debug)");
	argparser->SetOpt(VALUEOPT, "log-func",       "Filter log messages by function name.");
	argparser->SetOpt(VALUEOPT, "log-msg",       "Filter log messages by string content.");

	argparser->SetOpt(LONGOPT, "restore-defaults", "Restore options to default values.");

#ifdef EPIAR_COMPILE_TESTS
	argparser->SetOpt(VALUEOPT, "run-test",      "Run specified test");
#endif // EPIAR_COMPILE_TESTS

	// These are immediate options (I.E. they stop the argument processing immediately)
	if ( argparser->HaveShort("h") || argparser->HaveLong("help") ){
		argparser->PrintUsage();
		exit( 0 );
	}

	if ( argparser->HaveShort("v") || argparser->HaveLong("version") ){
		printf("%s\n", EPIAR_VERSION_FULL );
		exit( 0 );
	}

	if ( argparser->HaveLong("restore-defaults") ){
		printf("\nReseting all Epiar options to their default values.\n" );
		Options::RestoreDefaults();
		Options::Save();
		exit( 0 );
	}

	if ( argparser->HaveLong("fullscreen") ){
		SETOPTION("options/video/fullscreen",1);
	}

	if ( argparser->HaveLong("windowed") ){
		SETOPTION("options/video/fullscreen",0);
	}

#ifdef EPIAR_COMPILE_TESTS
	string testname = argparser->HaveValue("run-test");
	if ( !(testname.empty()) ) {
		Test testInst(testname);
		exit( testInst.RunTest( argc, argv ) );
	}
#endif // EPIAR_COMPILE_TESTS

	// Override OPTION values.

	// Following are cumulative options (I.E. you can have multiple of them)
	if ( argparser->HaveOpt("disable-audio") ) {
			SETOPTION("options/sound/background", 0);
			SETOPTION("options/sound/weapons", 0);
			SETOPTION("options/sound/engines", 0);
			SETOPTION("options/sound/explosions", 0);
			SETOPTION("options/sound/buttons", 0);
	}

	if      ( argparser->HaveOpt("log-xml") ) 	{ SETOPTION("options/log/xml", 1);}
	else if ( argparser->HaveOpt("nolog-xml") ) 	{ SETOPTION("options/log/xml", 0);}
	if      ( argparser->HaveOpt("log-out") ) 	{ SETOPTION("options/log/out", 1);}
	else if ( argparser->HaveOpt("nolog-out") ) 	{ SETOPTION("options/log/out", 0);}

	string funcfilt = argparser->HaveValue("log-func");
	string msgfilt = argparser->HaveValue("log-msg");
	string loglvl = argparser->HaveValue("log-lvl");

	if("" != funcfilt) Log::Instance().SetFuncFilter(funcfilt);
	if("" != msgfilt) Log::Instance().SetMsgFilter(msgfilt);
	if("" != loglvl) {
		if(Log::Instance().SetLevel( loglvl ) == false) {
			cout << "Log level: '" << loglvl << "' not understood." << endl;
		}
	}

	// Print unused options.
	list<string> unused = argparser->GetUnused();
	list<string>::iterator it;
	for ( it = unused.begin() ; it != unused.end(); it++ )
		cout << "\tUnknown options:\t" << (*it) << endl;
	if ( !unused.empty() ) {
		argparser->PrintUsage();

		exit( 1 );
	}
}

/** \details
 *  This records basic Epiar information about the current Environment.
 */
void Main_Log_Environment( void ) {
	LogMsg(INFO, "Epiar Version %s", EPIAR_VERSION_FULL );

#ifdef COMP_MSVC
	LogMsg(DEBUG, "Compiled with MSVC vers: _MSC_VER" );
#endif // COMP_MSVC

#ifdef COMP_GCC
	LogMsg(DEBUG, "Compiled with GCC vers: %d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__ );
#endif // COMP_GCC

	LogMsg(DEBUG, "Executable Path: %s", argparser->GetPath().c_str() );
}
