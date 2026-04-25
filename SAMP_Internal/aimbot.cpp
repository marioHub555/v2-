#include "aimbot.h"
#include <algorithm>
#include <cmath>

const double PI = 3.14159265358979323846;

namespace GlobalFriends {
	std::vector<int> list;
	bool ignoreSameColor = true;
}

// ???? ???? ???? ??????? ??? ?? ??? ????? ?????? ????????
bool isKeyDown(int vkey) {
	return (GetAsyncKeyState(vkey) & 0x8000) != 0;
}

void sinCos(float radians, float* sine, float* cosine)
{
	*sine = (float)sin(radians);
	*cosine = (float)cos(radians);
}

void angleVectors(Vector3 angles, Vector3* forward) {
	float sp, sy, cp, cy;
	sinCos(angles.y, &sy, &cy);
	sinCos(angles.x, &sp, &cp);
	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}

float AIMBOT::getFov(Vector3 aimAngle) {
	Vector3 ang, aim;
	angleVectors({ G::cam->alpha, G::cam->beta, 0 }, &aim);
	angleVectors(aimAngle, &ang);

	float dotProduct = ang.x * aim.x + ang.y * aim.y + ang.z * aim.z;
	float lengthSquared = aim.x * aim.x + aim.y * aim.y + aim.z * aim.z;
	float res = dotProduct / lengthSquared;
	if (res > 1.0f) res = 1.0f; if (res < -1.0f) res = -1.0f; // ????? ?? ??? Crash

	// ????? ???????? ??? ????? (???? ??? FOV ?????? ?????? ????)
	return (float)acos(res) * (float)(180.0 / PI);
}

Vector3 AIMBOT::calcAngle(actor_info *target) {
	Vector3 localPos = G::localplayerPtr->actor->gtaPed->max->pos;
	Vector3 posTarget = target->max->pos;

	if (G::config.aimbot.prediction) {
		posTarget += target->speed * (float)G::config.aimbot.predictionLvl;
	}

	posTarget.z += G::config.aimbot.heightChange;

	Vector3 delta = { (float)fabs(G::cam->cameraPos.x - posTarget.x), (float)fabs(G::cam->cameraPos.y - posTarget.y), G::cam->cameraPos.z - posTarget.z };
	float delta2DLength = (float)sqrt(delta.x * delta.x + delta.y * delta.y);
	float alpha = (float)asin(delta.x / delta2DLength);
	float beta = (float)acos(delta.x / delta2DLength);

	if ((localPos.x > posTarget.x) && (localPos.y < posTarget.y)) { beta = -beta; }
	if ((localPos.x > posTarget.x) && (localPos.y > posTarget.y)) { beta = beta; }
	if ((localPos.x < posTarget.x) && (localPos.y > posTarget.y)) { beta = (alpha + (1.5707f)); }
	if ((localPos.x < posTarget.x) && (localPos.y < posTarget.y)) { beta = (-alpha - (1.5707f)); }

	float pitch = atan2f(delta2DLength, delta.z) - 3.1415926f / 2.0f;

	if (G::cam->mode == 53 || G::cam->mode == 55) {
		pitch -= G::cam->fov * 0.1f / 70.0f;
		beta += G::cam->fov * 0.044f / 70.0f;
	}

	return { pitch, beta, 0 };
}

actor_info* AIMBOT::getEnemy() {
	float currFov;
	float lowestWeight = 999999.0f;
	actor_info* result = nullptr;

	auto pPlayerPool = G::samp->poolsPtr->playerpoolPtr;

	for (unsigned int i = 0; i <= (unsigned int)pPlayerPool->maxID; ++i) {
		if (i == (unsigned int)pPlayerPool->localID || !pPlayerPool->isListed[i])
			continue;

		remoteplayer* currentPlayer = pPlayerPool->remoteplayersPtrs[i];
		if (!currentPlayer || !currentPlayer->data || !currentPlayer->data->actor)
			continue;

		if (!(currentPlayer->data->actor->gtaPed->hp > 0))
			continue;

		if (std::find(GlobalFriends::list.begin(), GlobalFriends::list.end(), (int)i) != GlobalFriends::list.end())
			continue;

		// --- [ التعديل الجديد: فحص المسافة ] ---
		Vector3 localPos = G::localplayerPtr->actor->gtaPed->max->pos;
		Vector3 enemyPos = currentPlayer->data->actor->gtaPed->max->pos;

		// حساب المسافة الحقيقية
		float distance = sqrt(pow(enemyPos.x - localPos.x, 2) + pow(enemyPos.y - localPos.y, 2) + pow(enemyPos.z - localPos.z, 2));

		// إذا كان الشخص أبعد من المدى المحدد في القائمة، يتجاهله فوراً
		// تأكد من إضافة maxRange في ملف الـ Config
		if (distance > G::config.aimbot.maxRange)
			continue;

		// ---------------------------------------

		currFov = getFov(calcAngle(currentPlayer->data->actor->gtaPed));

		if (currFov < G::config.aimbot.fov) {
			float weight = currFov + (distance / 15.0f);

			if (weight < lowestWeight) {
				lowestWeight = weight;
				result = currentPlayer->data->actor->gtaPed;
			}
		}
	}
	return result;
}

void AIMBOT::aim() {
	if (!G::config.aimbot.enabled) return;

	// ??? ???? ??????? ?? ?????? (???? ??? ???? ?? ??????)
	if (!isKeyDown(G::config.aimbot.aimKey)) return;

	actor_info* target = getEnemy();
	if (target) {
		Vector3 targetAngle = calcAngle(target);
		float deltaAlpha = targetAngle.x - G::cam->alpha;
		float deltaBeta = targetAngle.y - G::cam->beta;

		if (deltaAlpha > PI) deltaAlpha -= 2 * PI;
		if (deltaAlpha < -PI) deltaAlpha += 2 * PI;
		// ????? ?????? Beta ????? ????? ??? ???? ???? ?????? 360 ????
		if (deltaBeta > PI) deltaBeta -= 2 * PI;
		if (deltaBeta < -PI) deltaBeta += 2 * PI;

		if (G::config.aimbot.smoothAim) {
			float smoothValue = 101.0f - G::config.aimbot.smoothSpeed;
			if (smoothValue < 1.0f) smoothValue = 1.0f;

			// ????? ???? (Deadzone) ????? ???? ???????
			if (fabs(deltaAlpha) < 0.0025f && fabs(deltaBeta) < 0.0025f) return;

			G::cam->alpha += deltaAlpha / smoothValue;
			G::cam->beta += deltaBeta / smoothValue;
		}
		else {
			G::cam->alpha = targetAngle.x;
			G::cam->beta = targetAngle.y;
		}
	}
}