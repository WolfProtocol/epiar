/**\file			models.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "utilities/components.h"
#include "engine/models.h"

/** \class Model
 *  \brief Common ship attributes shared between a style of ship.
 *  \details A Model is like the blueprint specification for a kind of ship.
 *           The Model describes the key attributes of a ship: what it looks
 *           like, how much it can store, how fast it can manuever, etc.
 *
 *           Notice though that most of the important attributes are not
 *           defined in this class but are defined as a part of the Outfit
 *           class.  This is because a ship can improve it's base statistics by
 *           installing a better Engine, better armor etc.  The Model defines
 *           that baseline.
 *
 *  \see Outfit
 */

/** \class WeaponSlot
 *  \brief A Description of a port on a Model where a Weapon can be attached.
 *  \details The WeaponSlot is a named location on a ship like "front turret"
 *           or "rear swivel slot".  Each WeaponSlot is located on the model
 *           based on what the model looks like.  So for example, if the
 *           Model's image looks like it has a weapon in it's nose, then there
 *           should be a WeaponSlot in the nose.
 *
 *           Each WeaponSlot is assigned to a firing group (0 or 1).  When a
 *           Ship fires, all the Weapons on that WeaponSlot will fire at once.
 *
 *           Some WeaponSlots come preconfigured with a Weapon, but this weapon
 *           can be swapped out at any shipyard.
 */

/**\brief Creates an empty Model object.
 */
Model::Model()
	:image(NULL)
	,defaultEngine(NULL)
	,thrustOffset(0)
{
	SetName("");
	SetMass(1.0f);
	SetRotationsPerSecond(0.2f);
	SetHullStrength(100);
	SetShieldStrength(100);
	SetCargoSpace(10);
}

/**\brief Assignment operator (copy fields).
 */
Model& Model::operator=(const Model& other) {
	name = other.name;
	image = other.image;
	description = other.description;
	defaultEngine = other.defaultEngine;
	mass = other.mass;
	thrustOffset = other.thrustOffset;
	rotPerSecond = other.rotPerSecond;
	maxSpeed = other.maxSpeed;
	msrp = other.msrp;
	cargoSpace = other.cargoSpace;
	hullStrength = other.hullStrength;
	shieldStrength = other.shieldStrength;
	return *this;
}

/**\brief Creates a Model with the given parameters.
 * \param _name Name of the ship
 * \param _image Image of the ship
 * \param _mass Mass of the ship
 * \param _thrustOffset For animation
 * \param _rotPerSecond Rotation per second
 * \param _maxSpeed Maximum speed
 * \param _hullStrength Maximum damage it can take
 * \param _shieldStrength Maximum damage it can take
 * \param _msrp Price
 * \param _cargoSpace Tons of cargo space
 */


Model::Model(
		string _name,
		Image* _image,
		string _description,
		Engine* _defaultEngine,
		float _mass,
		short int _thrustOffset,
		float _rotPerSecond,
		float _maxSpeed,
		int _hullStrength,
		int _shieldStrength,
		int _msrp,
		int _cargoSpace,
		vector<WeaponSlot>& _weaponSlots) :
	image(_image),
	defaultEngine(_defaultEngine),
	thrustOffset(_thrustOffset)
{
	SetName(_name);
	SetDescription(_description);
	SetMass(_mass);
	SetRotationsPerSecond(_rotPerSecond);
	SetMaxSpeed(_maxSpeed);
	SetMSRP(_msrp);
	SetCargoSpace(_cargoSpace);
	SetHullStrength(_hullStrength);
	SetShieldStrength(_shieldStrength);
	ConfigureWeaponSlots(_weaponSlots);
	//((Component*)this)->SetName(_name);
}

/**\brief For parsing XML file into fields.
 */
bool Model::FromXMLNode( xmlDocPtr doc, xmlNodePtr node ) {
	xmlNodePtr  attr;
	string value;

	if( (attr = FirstChildNamed(node,"image")) ){
		image = Image::Get( NodeToString(doc,attr) );
		Image::Store(name, image);
		SetPicture(image);
	} else return false;

	if( (attr = FirstChildNamed(node,"description")) ){
		value = NodeToString(doc,attr);
		SetDescription( value );
	} else {
		LogMsg( WARN, "%s does not have a description.", GetName().c_str() );
	}

	if( (attr = FirstChildNamed(node,"engine")) ){
		defaultEngine = Engines::Instance()->GetEngine( NodeToString(doc,attr) );
	} else return false;

	if( (attr = FirstChildNamed(node,"mass")) ){
		value = NodeToString(doc,attr);
		SetMass( static_cast<float> (atof( value.c_str() )));
	} else return false;

	if( (attr = FirstChildNamed(node,"rotationsPerSecond")) ){
		value = NodeToString(doc,attr);
		SetRotationsPerSecond( static_cast<float>(atof( value.c_str() )));
	} else return false;

	if( (attr = FirstChildNamed(node,"thrustOffset")) ){
		value = NodeToString(doc,attr);
		thrustOffset = static_cast<short>(atoi( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node,"maxSpeed")) ){
		value = NodeToString(doc,attr);
		SetMaxSpeed( static_cast<float>(atof( value.c_str() )));
	} else return false;

	if( (attr = FirstChildNamed(node,"msrp")) ){
		value = NodeToString(doc,attr);
		SetMSRP( (short int)atoi( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node,"cargoSpace")) ){
		value = NodeToString(doc,attr);
		SetCargoSpace( atoi( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node,"hullStrength")) ){
		value = NodeToString(doc,attr);
		SetHullStrength( (short)atoi( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node,"shieldStrength")) ){
		value = NodeToString(doc,attr);
		SetShieldStrength( (short)atoi( value.c_str() ));
	} else return false;

	if( (attr = FirstChildNamed(node,"weaponSlots")) ){
		// pass the weaponSlots XML node into a handler function
		ConfigureWeaponSlots( doc, attr );
	} else {
		//LogMsg( WARN, "Did not find weapon slot configuration - ship cannot have weapons.");
	}

	return true;
}

/**\brief Converts the Model to an XML node.
 */
xmlNodePtr Model::ToXMLNode(string componentName) {
	char buff[256];
    xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str());

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "description", BAD_CAST this->GetDescription().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "image", BAD_CAST this->GetImage()->GetPath().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "engine", BAD_CAST this->GetDefaultEngine()->GetName().c_str() );
	snprintf(buff, sizeof(buff), "%1.2f", this->GetMass() );
	xmlNewChild(section, NULL, BAD_CAST "mass", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%1.2f", this->GetRotationsPerSecond() );
	xmlNewChild(section, NULL, BAD_CAST "rotationsPerSecond", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetThrustOffset() );
	xmlNewChild(section, NULL, BAD_CAST "thrustOffset", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%1.1f", this->GetMaxSpeed() );
	xmlNewChild(section, NULL, BAD_CAST "maxSpeed", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetHullStrength() );
	xmlNewChild(section, NULL, BAD_CAST "hullStrength", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetShieldStrength() );
	xmlNewChild(section, NULL, BAD_CAST "shieldStrength", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetMSRP() );
	xmlNewChild(section, NULL, BAD_CAST "msrp", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetCargoSpace() );
	xmlNewChild(section, NULL, BAD_CAST "cargoSpace", BAD_CAST buff );

	char *ntos = (char*)malloc(256);
	xmlNodePtr wsPtr = xmlNewNode(NULL, BAD_CAST "weaponSlots");
	for(unsigned int w=0;w<weaponSlots.size();w++){
		WeaponSlot *slot = &weaponSlots[w];

		xmlNodePtr slotPtr = xmlNewNode(NULL, BAD_CAST "slot");
		xmlNewChild(slotPtr, NULL, BAD_CAST "name", BAD_CAST slot->name.c_str() );
		xmlNodePtr coordPtr = xmlNewNode(NULL, BAD_CAST "coord");
		snprintf(ntos, 256, "%d", slot->x);
		xmlNewChild(coordPtr, NULL, BAD_CAST "x", BAD_CAST ntos);
		snprintf(ntos, 256, "%d", slot->y);
		xmlNewChild(coordPtr, NULL, BAD_CAST "y", BAD_CAST ntos);
		xmlAddChild(slotPtr, coordPtr);
		snprintf(ntos, 256, "%.1f", slot->angle);
		xmlNewChild(slotPtr, NULL, BAD_CAST "angle", BAD_CAST ntos);
		snprintf(ntos, 256, "%.1f", slot->motionAngle);
		xmlNewChild(slotPtr, NULL, BAD_CAST "motionAngle", BAD_CAST ntos);
		xmlNewChild(slotPtr, NULL, BAD_CAST "content", BAD_CAST ((slot->content == NULL)
		                                             ? ""
		                                             : slot->content->GetName().c_str()) );
		snprintf(ntos, 256, "%d", slot->firingGroup);
		xmlNewChild(slotPtr, NULL, BAD_CAST "firingGroup", BAD_CAST ntos);
		xmlAddChild(wsPtr, slotPtr);
	}
	xmlAddChild(section, wsPtr);
	free(ntos);

	return section;
}

/**\brief Configure the ship's weapon slots based on the XML node weaponSlots.
 */
bool Model::ConfigureWeaponSlots( xmlDocPtr doc, xmlNodePtr node ) {

	xmlNodePtr slotPtr;
	string value;

	//if( (slotPtr = FirstChildNamed(node,"slot")) ){
        for( slotPtr = FirstChildNamed(node,"slot"); slotPtr != NULL; slotPtr = NextSiblingNamed(slotPtr,"slot") ){
		WeaponSlot newSlot;

		xmlNodePtr attr;

		if( (attr = FirstChildNamed(slotPtr,"name")) ){
			value = NodeToString(doc,attr);
			newSlot.name = value;
		} else return false;

		if( (attr = FirstChildNamed(slotPtr,"coord")) ){
			value = NodeToString(doc,attr);
			// go deeper...

			xmlNodePtr coordAttr;
			if( (coordAttr = FirstChildNamed(attr,"x")) ){
				newSlot.x = NodeToInt(doc,coordAttr);
			} else return false;
			if( (coordAttr = FirstChildNamed(attr,"y")) ){
				newSlot.y = NodeToInt(doc,coordAttr);
			} else return false;
		} else return false;

		if( (attr = FirstChildNamed(slotPtr,"angle")) ){
			newSlot.angle = NodeToFloat(doc,attr);
		} else return false;

		if( (attr = FirstChildNamed(slotPtr,"motionAngle")) ){
			value = NodeToString(doc,attr);
			newSlot.motionAngle = atof(value.c_str());
		} else return false;

		if( (attr = FirstChildNamed(slotPtr,"content")) ){
			// this check is necessary because NodeToString() won't translate <item></item> into ""
			if(attr->xmlChildrenNode)
				value = NodeToString(doc,attr);
			else
				value = ""; // slot is empty

			newSlot.content = Weapons::Instance()->GetWeapon( value );
		} else return false;

		if( (attr = FirstChildNamed(slotPtr,"firingGroup")) ){
			newSlot.firingGroup = NodeToInt(doc,attr);
		} else return false;

		weaponSlots.push_back(newSlot);
	}

        return true;
}

/**\brief Configure the ship's weapon slots based on a list passed in (probably from the constructor)
 */
bool Model::ConfigureWeaponSlots( vector<WeaponSlot>& slots ) {
        this->weaponSlots = slots;
        return true;
}

/**\brief Return the total number of weapon slots of any kind that this Model (probably a Model) has.
 */
int Model::GetWeaponSlotCount(){
	return this->weaponSlots.size();
}

void Model::WSDebug(WeaponSlot slot){
	LogMsg(DEBUG,
		"WeaponSlots: name=%s x=%d y=%d angle=%f motionAngle=%f content=%s firingGroup=%d",
		slot.name.c_str(),
		slot.x,
		slot.y,
		slot.angle,
		slot.motionAngle,
		(slot.content == NULL)
			? "empty"
			: slot.content->GetName().c_str(),
		slot.firingGroup);
}

void Model::WSDebug(vector<WeaponSlot>& slots){
	LogMsg(DEBUG, "WeaponSlots for Model: %s", GetName().c_str() );
	for(unsigned int i = 0; i < slots.size(); i++){
		WSDebug(slots[i]);
	}
}

/**\fn Model::GetImage()
 *  \brief Retrieves a pointer to the Image
 * \fn Model::GetThrustOffset()
 *  \brief Retrieves the offset of the thrust from center
 */


/**\class Models
 * \brief Collection of Model objects
 */
Models *Models::pInstance = 0; // initialize pointer

/**\brief Returns or creates the Model instance.
 * \return Pointer to the Model instance
 */
Models *Models::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Models; // create the solid instance
		pInstance->rootName = "models";
		pInstance->componentName = "model";
	}
	return( pInstance );
}

/**\fn Models::GetModel(string name)
 *  \brief Retrieves the Model by name
 * \fn Models::newComponent()
 *  \brief Creates a new Model
 * \fn Models::Models(const Models&)
 *  \brief An empty constructor.
 * \fn Models::operator= (const Models&)
 *  \brief An empty assignment operator.
 */
