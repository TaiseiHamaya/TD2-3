#include "BackGround.h"

#include <Library/Math/Transform2D.h>

#include <Engine/Module/World/Sprite/SpriteInstance.h>
#include <Engine/Runtime/WorldClock/WorldClock.h>

BackGround::BackGround() {
	backGroundSprite = std::make_unique<SpriteInstance>("backGround2.png");
	backGroundSprite2 = std::make_unique<SpriteInstance>("backGround.png");
}

BackGround::~BackGround() {}

void BackGround::update() {
	timer += WorldClock::DeltaSeconds();
	curScroll = scrollSpeed * timer;
	curScroll2 = scrollSpeed2 * timer;
	backGroundSprite->get_uv_transform().set_translate_x(curScroll);
	backGroundSprite2->get_uv_transform().set_translate_x(curScroll2);
}

void BackGround::debugUpdate() {}

void BackGround::begin_rendering() {

	backGroundSprite->begin_rendering();
	backGroundSprite2->begin_rendering();
}

void BackGround::draw() {
	backGroundSprite->draw();
	backGroundSprite2->draw();
}
