#pragma once
#include "util/Memory.h"
#include "util/ScriptUtil.h"
#define MAX_DISTANCE 5000.0f
#define MAX_DISTANCE_LIMIT 10000.0f
#define ATTACK_RADIUS 500
#define MAX_TARGETS 3

#define COOLDOWN_GAME 2 * 12
#define COOLDOWN_REAL 2
class AimHelper {
public:
	static bCVector GetAimPosition();
	static bCVector GetAimDirection();
	static GEBool ValidAimPosition(bCVector position);
	static void DrawSphereAtAimPosition();
	static void DrawSphereAroundFocus(Entity focus, GEI32 radius);
	static void DrawBoxAtAimPosition();
	static void TeleportToAimPosition();
	static GEBool IsUsingFreeAimSpell();
	static GEBool IsUsingCollisionSpell();
	static bTPtrArray<eCEntity*> GetEntitiesInRadius(bCVector position, GEI32 radius);
private:
	AimHelper() {}
};

