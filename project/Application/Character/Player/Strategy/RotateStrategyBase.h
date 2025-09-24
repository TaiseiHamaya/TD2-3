#pragma once
#include <Library/Math/Vector3.h>

class Player;
class Child;

class RotateStrategyBase
{
public:
	~RotateStrategyBase() = default;
	// 回転可能かどうかを判定し、必要ならプレイヤーの回転状態もセットする
	virtual bool CanRotate(Player& player, const Vector3& childDirection) const = 0;
};

