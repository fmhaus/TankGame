#pragma once

#include "engine/Types.h"

enum CollisionCategory : u64
{
	CATEGORY_MAP = 1 << 0,
	CATEGORY_TANK = 1 << 1,
	CATEGORY_PROJECTILE = 1 << 2,
	CATEGORY_ALL = (u64)-1
};