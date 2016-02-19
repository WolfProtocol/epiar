/**\file			engines.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "engine/engines.h"
#include "utilities/components.h"
#include "utilities/log.h"

/**\class Engine
 * \brief A ship propeller system.
 */

/**\brief Initializes a new Engine to default values.
 *
 */
Engine::Engine() :
	thrustsound(NULL),
	foldDrive(false),
	flareAnimation("")
{
	SetName("");
	SetMSRP(500);
	SetForceOutput(5.0f);
}

/**\brief Assignment constructor - copies all fields.
 */
Engine& Engine::operator= (const Engine& other) {
	Outfit(*this) = Outfit(other);

	name = other.name;
	thrustsound = other.thrustsound;
	forceOutput = other.forceOutput;
	foldDrive = other.foldDrive;
	flareAnimation = other.flareAnimation;
	return *this;
}

/**\brief Initializes a new Engine with the given parameters.
 * \param _name Name of the Engine
 * \param _thrustsound Pointer to a Sound object that will be played for thrust
 * \param _forceOutput The amount of force the Engine generates
 * \param _msrp Price of the Engine
 * \param foldDrive Fold capable
 * \param _flareAnimation Thrust animation
 */
Engine::Engine( string _name, Image* _pic, string _description, Sound* _thrustsound, float _forceOutput,
		short int _msrp, bool _foldDrive, string _flareAnimation) :
	thrustsound(_thrustsound),
	foldDrive(_foldDrive),
	flareAnimation(_flareAnimation)
{
	SetName(_name);
	SetDescription(_description);
	SetMSRP(_msrp);
	SetPicture(_pic);
	SetForceOutput(_forceOutput);
}

/**\brief Parser to parse the XML file
 */
bool Engine::FromXMLNode( xmlDocPtr doc, xmlNodePtr node ) {
	xmlNodePtr  attr;
	string value;

	if( (attr = FirstChildNamed(node,"description")) ){
		value = NodeToString(doc,attr);
		SetDescription( value );
	} else {
		LogMsg( WARN, "%s does not have a description.", GetName().c_str() );
	}

	if( (attr = FirstChildNamed(node,"forceOutput")) ){
		value = NodeToString(doc,attr);
		SetForceOutput( static_cast<float> (atof( value.c_str() )));
	} else return false;

	if( (attr = FirstChildNamed(node,"msrp")) ){
		value = NodeToString(doc,attr);
		SetMSRP( (short int)atoi( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node,"foldDrive")) ){
		value = NodeToString(doc,attr);
		foldDrive = (atoi( value.c_str() ) != 0);
	} else return false;

	if( (attr = FirstChildNamed(node,"flareAnimation")) ){
		flareAnimation = NodeToString(doc,attr);
	} else return false;

	if( (attr = FirstChildNamed(node,"thrustSound")) ){
		thrustsound = Sound::Get( NodeToString(doc,attr) );
	} else return false;

	if( (attr = FirstChildNamed(node,"picName")) ){
		Image* pic = Image::Get( NodeToString(doc,attr) );
		// This image can be accessed by either the path or the Engine Name
		Image::Store(name, pic);
		SetPicture(pic);
	} else return false;

	return true;
}

/**\brief Converts the Engine object to an XML node.
 */
xmlNodePtr Engine::ToXMLNode(string componentName) {
	char buff[256];
	xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str());

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "description", BAD_CAST this->GetDescription().c_str() );

	snprintf(buff, sizeof(buff), "%1.1f", this->GetForceOutput() );
	xmlNewChild(section, NULL, BAD_CAST "forceOutput", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetMSRP() );
	xmlNewChild(section, NULL, BAD_CAST "msrp", BAD_CAST buff );
	xmlNewChild(section, NULL, BAD_CAST "foldDrive", BAD_CAST (this->GetFoldDrive()?"1":"0") );
	xmlNewChild(section, NULL, BAD_CAST "flareAnimation", BAD_CAST this->GetFlareAnimation().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "thrustSound", BAD_CAST this->thrustsound->GetPath().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "picName", BAD_CAST this->GetPicture()->GetPath().c_str() );

	return section;
}

/**\fn Engine::GetFlareAnimation()
 * \brief Gets the animation.
 */

/**\fn Engine::GetFoldDrive()
 * \brief Retrieves fold capability
 */

/**\class Engines
 * \brief Handles ship engines. */

Engines::Engines() {
	rootName = "engines";
	componentName = "engine";
}

/**\fn Engines::GetEngine(string name)
 * \brief Returns the named Engine
 */

/**\fn Engines::newComponent()
 * \brief Creates a new Engine object.
 */

