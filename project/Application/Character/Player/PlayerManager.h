#pragma once
#include <memory>
#include "Application/Character/Player/Player.h"
#include "Application/Character/Player/Child/Child.h"
#include <Application/MapchipField.h>
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
	// マップチップの当たり判定
	void update_mapchip();
	// オブジェクトの結合
	void attach_child_to_player();
	// オブジェクトを切り離す処理
	void detach_child_from_player();
	// 値を比べるときの誤差を消す関数
	bool ApproximatelyEqual(const Vector3& a, const Vector3& b, float epsilon = 1e-5f);

public:
	void set_mapchip_field(MapchipField* mapchipField) { mapchipField_ = mapchipField; }


private:
	std::unique_ptr<Player> player;
	std::unique_ptr<Child> child;
	MapchipField* mapchipField_;
};

