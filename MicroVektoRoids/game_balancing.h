#pragma once

// Game balancing and configuration values. For doc, see comments and code where it's used...

#define PLAYER_SHIP_DUST_ATTRACTION_RANGE 27
#define PLAYER_SHIP_DUST_ATTRACTION_POWER Fix4(1,4)
#define PLAYER_SHIP_DUST_COLLECTION_RANGE 3

#define ASTEROIDS_DUST_EJECTION_POWER Fix4(2,5)

// start time during which collectables are not collectable
#define COLLECTABLE_SPAWN_TIME 8
// drag factor applied each frame on collectable velocity
#define COLLECTABLE_DRAG Fix4(0,14)
