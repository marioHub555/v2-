#pragma once
#include "gui.h" // هذا الملف موجود ومجرب في مشروعك
#include <vector>

namespace GlobalFriends {
	extern std::vector<int> list;
	extern bool ignoreSameColor;
}

namespace AIMBOT {
	float getFov(Vector3 aimAngle);
	Vector3 calcAngle(actor_info* target);
	actor_info* getEnemy();
	void aim();
}