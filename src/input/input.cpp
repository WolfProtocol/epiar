/**\file			input.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Sunday, June 4, 2006
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "input/input.h"
#include "engine/scenario.h"
#include "engine/scenario_lua.h"
#include "utilities/log.h"
#include "graphics/video.h"
#include "utilities/timer.h"

/**\class Input
 * \brief Processor for the Users mouse and keyboard actions
 * \details The Input polls SDL for any new events on every Update.
 *          It then converts the Mouse Up/Down events into Mouse Held events.
 *
 *          Input also handles Registered mappings between Keys and Lua Functions.
 *
 * \see InputEvent 
 */

/**\class InputEvent
 * \brief Handles Input events. For specific key handling, see the Lua scripts.
 *
 * \fn InputEvent::InputEvent( eventType type, keyState kstate, SDLKey key )
 *  \brief Handles input events based on SDLKey.
 * \fn InputEvent::InputEvent( eventType type, keyState kstate, int key )
 *  \brief Handles input events based on key code.
 * \fn InputEvent::InputEvent( eventType type, mouseState mstate, int mx, int my )
 *  \brief Handles mouse events.
 * \fn InputEvent::operator==(const InputEvent& e1, const InputEvent& e2 )
 *  \brief Compares to see if two input events are the same.
 * \fn InputEvent::operator <(const InputEvent& e1, const InputEvent& e2 )
 *  \brief Compares if keys or mouse coords are less than each other.
 * \var InputEvent::type
 *  \brief Type of event
 *
 * \var InputEvent::key
 *  \brief The key pressed
 * 
 * \var InputEvent::kstate
 *  \brief Key state (up, down, or hold)
 * 
 * \var InputEvent::mstate
 *  \brief Mouse state
 * 
 * \var InputEvent::mx
 *  \brief Mouse x coordinate.
 * 
 * \var InputEvent::my
 *  \brief Mouse y coordinate.
 */

/**\brief Stream to handle InputEvent.
 */
ostream& operator<<(ostream &out, const InputEvent&e) {
	static const char _mouseMeanings[3] = {'M','U','D'};
	static const char _keyMeanings[4] = {'^','V','P','T'};
	if ( e.type == KEY ) {
		out << "KEY([" << SDL_GetKeyName(e.key) << "] " << e.key << ' ' << _keyMeanings[int( e.kstate )] << ")";
	} else { // Mouse
		out << "MOUSE(" << e.mx<< ',' << e.my << ' ' << _mouseMeanings[int( e.mstate )] << ")";
	}
	return out;
}

/**\brief Input constructor.
 */
Input::Input() {
	memset( heldKeys, 0, sizeof( bool ) * SDLK_LAST );
	lastMouseMove = Timer::GetTicks();
}

/**\brief Polls the event queue and sends the list of events to subsystems.
 */
list<InputEvent> Input::Update( ) {
	SDL_Event event;
	
	events.clear();

	while( SDL_PollEvent( &event ) ) {
		switch( event.type ) {
			case SDL_QUIT:
				exit(1);
			case SDL_KEYDOWN:
			{
				_UpdateHandleKeyDown( &event );
				break;
			}
			case SDL_KEYUP:
			{
				_UpdateHandleKeyUp( &event );
				break;
			}
			case SDL_MOUSEMOTION:
			{
				_UpdateHandleMouseMotion( &event );
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				_UpdateHandleMouseUp( &event );
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				_UpdateHandleMouseDown( &event );
				break;
			}
			default:
				break;
		}
	}

	// Constantly emit InputEvent for held down Keys
	for(int k=0;k<SDLK_LAST;k++) {
		if(heldKeys[k])
			events.push_back( InputEvent( KEY, KEYPRESSED, k ) );
	}

	if((Timer::GetTicks() - lastMouseMove > OPTION(Uint32,"options/timing/mouse-fade")) ){
		Video::DisableMouse();
	}
	
	// this could be false - returning quitSignal doesn't imply quitting
	return events;
}

/**\brief Converts SDL_MouseButtonEvent to Epiar's Input model.
 */
mouseState Input::_CheckMouseState( Uint8 button, bool up ){
	switch (button){
		case SDL_BUTTON_LEFT:
			return (up? MOUSELUP : MOUSELDOWN);
		case SDL_BUTTON_MIDDLE:
			return (up? MOUSEMUP : MOUSEMDOWN);
		case SDL_BUTTON_RIGHT:
			return (up? MOUSERUP : MOUSERDOWN);
		// We'll only handle one event, return unhandled on down events.
		case SDL_BUTTON_WHEELUP:
			return (up? MOUSEWUP : UNHANDLED);
		case SDL_BUTTON_WHEELDOWN:
			return (up? MOUSEWDOWN : UNHANDLED);
	}
	return UNHANDLED;
}

/**\brief Translates mouse movement events to Epiar.
 */
void Input::_UpdateHandleMouseMotion( SDL_Event *event ) {
	assert( event );

	Uint16 x, y;

	// translate so (0,0) is lower-left of screen
	x = event->motion.x;
	y = event->motion.y;
	
	events.push_back( InputEvent( MOUSE, MOUSEMOTION, x, y ) );
	Video::EnableMouse();
	lastMouseMove = Timer::GetTicks();
}

/**\brief Translates mouse down events to Epiar events.
 */
void Input::_UpdateHandleMouseDown( SDL_Event *event ) {
	assert( event );

	Uint16 x, y;
	mouseState state=_CheckMouseState(event->button.button,false);
	// translate so (0,0) is lower-left of screen
	x = event->button.x;
	y = event->button.y;


	if (state)
		events.push_back( InputEvent( MOUSE, state, x, y ) );
}

/**\brief Translates mouse up events to Epiar events.
 */
void Input::_UpdateHandleMouseUp( SDL_Event *event ) {
	assert( event );

	Uint16 x, y;
	mouseState state=_CheckMouseState(event->button.button,true);
	// translate so (0,0) is lower-left of screen					
	x = event->button.x;
	y = event->button.y;

	if (state)
		events.push_back( InputEvent( MOUSE, state, x, y ) );
}

/**\brief Translates key down events to Epiar events.
 */
void Input::_UpdateHandleKeyDown( SDL_Event *event ) {
	assert( event );

	events.push_back( InputEvent( KEY, KEYDOWN, event->key.keysym.sym ) );
	// typed events go here because SDL will repeat KEYDOWN events for us at the set SDL repeat rate
	PushTypeEvent( events, event->key.keysym.sym );
	heldKeys[ event->key.keysym.sym ] = 1;

}

/**\brief Translates key up events to Epiar events.
 */
void Input::_UpdateHandleKeyUp( SDL_Event *event ) {
	assert( event );

	events.push_back( InputEvent( KEY, KEYUP, event->key.keysym.sym ) );
	heldKeys[ event->key.keysym.sym ] = 0;
}

void Input::PushTypeEvent( list<InputEvent> & events, SDLKey key ) {
	int letter=key;

	// Convert lower to upper case characters
	// TODO: This assumes that every Epiar user has an American keyboard.
	//       I don't know how to do the keyboard independent character translations.
	if(heldKeys[SDLK_LSHIFT] || heldKeys[SDLK_RSHIFT]) {
		if(key >= SDLK_a && key <= SDLK_z) {
			letter -= 32;
		} else if(key >= SDLK_0 && key <= SDLK_9) {
			letter = ")!@#$%^&*("[key-SDLK_0];
		} else {
			switch(key){
			case SDLK_QUOTE:
				letter = '"'; break;
			case SDLK_SEMICOLON:
				letter = ':'; break;
			case SDLK_BACKQUOTE:
				letter = '~'; break;
			case SDLK_MINUS:
				letter = '_'; break;
			case SDLK_SLASH:
				letter = '?'; break;
			case SDLK_COMMA:
				letter = '<'; break;
			case SDLK_PERIOD:
				letter = '>'; break;
			case SDLK_BACKSLASH:
				letter = '|'; break;
			case SDLK_LEFTBRACKET:
				letter = '{'; break;
			case SDLK_RIGHTBRACKET:
				letter = '}'; break;
			case SDLK_EQUALS:
				letter = '+'; break;
			default:
				break;
			}
		}
	}

	// Keypresses that we want to accept, but turn into something different
	if((key == SDLK_RETURN) || (key == SDLK_KP_ENTER)) {
		letter='\n';
	}

	// DEBUG: Name = int = char -> emitted char
	//cout<<SDL_GetKeyName(key)<<" = "<<key<<" = '"<<char(key)<<"' -> '"<<char(letter)<<"'"<<endl;
	
	events.push_back( InputEvent( KEY, KEYTYPED, letter ) );
}

/**\brief Handle Lua key bindings.
 */
void Input::HandleLuaCallBacks( list<InputEvent> & events ) {
	list<InputEvent>::iterator i = events.begin();
	while( i != events.end() ) {
		map<InputEvent,string>::iterator val = eventMappings.find( *i );
		if( val != eventMappings.end() ){
			Lua::Run( val->second );
			i = events.erase( i );
		}
		else{
			++i;
		}
	}
}

/**\brief Register Lua events.
 */
void Input::RegisterCallBack( InputEvent event, string command ) {
	eventMappings.insert(make_pair(event, command));
}

/**\brief Unregister Lua events.
 */
void Input::UnRegisterCallBack( InputEvent event ) {
	eventMappings.erase(event);
}


/**\brief Search an InputEvent list for a specific Event
 * \param[in] events The list of Events to search
 * \param[in] trigger The trigger event to look for.
 * \returns true if the event was found and removed.
 */
bool Input::SearchSpecificEvent( list<InputEvent> & events, InputEvent trigger ) {
	list<InputEvent>::iterator i = events.begin();
	while( i != events.end() ) {
		if( (*i) == trigger ) {
			return true;
		}
		++i;
	}
	return false;
}

/**\brief Search an InputEvent list for a specific Event, then Remove it
 * \param[in] events The list of Events to search
 * \param[in] trigger The trigger event to look for and remove.
 * \returns true if the event was found and removed.
 */
bool Input::HandleSpecificEvent( list<InputEvent> & events, InputEvent trigger ) {
	bool found = SearchSpecificEvent(events,trigger);
	if( found ) {
		events.remove( trigger );
	}
	return found;
}

/**\brief Print out a list of events.
 * \warn This is ugly.  Not usable for production code.
 */
void Input::PrintEvents( string title, list<InputEvent> & events ) {
	list<InputEvent>::iterator i = events.begin();
	if( events.size() == 0 ) return;
	while( i != events.end() ) {
		cout<<*i << " ";
		++i;
	}
	cout << endl;
}

