/*
 * registration.cpp
 * File defining a single function in which all classes and default resources should be registered
 */

#include "game/ObjectFactory.h"
#include "game/Player.h"
#include "game/world/SimplePhysicsObject.h"
#include "game/world/Wall.h"

void registerClasses() {
    ObjectFactory *fac = ObjectFactory::get();
    fac->registerClass(Player::getClassName(), Player::read)
        .registerAttribute("ID", "id", ATYPE_OBJECT_ID)
        .registerAttribute("Position", "pos", ATYPE_POINT)
    ;

    fac->registerClass(SimplePhysicsObject::getClassName(), SimplePhysicsObject::read)
        .registerAttribute("ID", "id", ATYPE_OBJECT_ID)
        .registerAttribute("Texture", "tex", ATYPE_RESOURCE_ID)
        .registerAttribute("Volume", "vol", ATYPE_BOX)
        .registerAttribute("Color", "cr", ATYPE_BOX)
    ;

    fac->registerClass(Wall::getClassName(), Wall::read)
        .registerAttribute("ID", "id", ATYPE_OBJECT_ID)
        .registerAttribute("North Texture", "tex.north", ATYPE_RESOURCE_ID)
        .registerAttribute("South Texture", "tex.south", ATYPE_RESOURCE_ID)
        .registerAttribute("East Texture",  "tex.east",  ATYPE_RESOURCE_ID)
        .registerAttribute("West Texture",  "tex.west",  ATYPE_RESOURCE_ID)
        .registerAttribute("Top Texture",   "tex.up",    ATYPE_RESOURCE_ID)
        .registerAttribute("Bottom Texture", "tex.down", ATYPE_RESOURCE_ID)
        .registerAttribute("Volume", "vol", ATYPE_BOX)
        .registerAttribute("Color", "cr", ATYPE_BOX)
    ;
}