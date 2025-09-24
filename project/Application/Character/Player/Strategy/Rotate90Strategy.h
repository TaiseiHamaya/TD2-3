#pragma once
#include "RotateStrategyBase.h"

class Rotate90Strategy : public RotateStrategyBase
{
public:
	~Rotate90Strategy() = default;
	// 回転可能かどうかを判定し、必要ならプレイヤーの回転状態もセットする
	bool CanRotate(Player& player, const Vector3& childDirection) const override;
};

