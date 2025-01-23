#pragma once
#include <memory>
#include "Application/Character/Player/Player.h"
#include "Application/Character/Player/Child/Child.h"
class PlayerManager
{
public:
	void initialize();
	void finalize();

	void begin();
	void update();
	void begin_rendering();
	void late_update();

	void draw() const;

#ifdef _DEBUG
	void debug_update();
#endif // _DEBUG

private:

	void DetachChildFromPlayer(Player* player, Child* child);
	bool ApproximatelyEqual(const Vector3& a, const Vector3& b, float epsilon = 1e-5f);
private:
	std::unique_ptr<Player> player;
	std::unique_ptr<Child> child;

};

