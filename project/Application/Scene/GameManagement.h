#pragma once
#include <memory>


class SpriteInstance;

class GameManagement{
public:
    GameManagement();
    ~GameManagement();

    void init();
    void update();
    void begin_rendering();
    void darw();

    //アクセッサ
    void SetClearFlag(bool value){ clearFlag = value; }
    void SetFailedFlag(bool value){ failedFlag = value; }

private:
    std::unique_ptr<SpriteInstance> clearSprite;
    std::unique_ptr<SpriteInstance> failedSprite;

    bool clearFlag;
    bool failedFlag;

};