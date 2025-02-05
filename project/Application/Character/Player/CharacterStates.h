#pragma once

// 回転の方向
enum class RotationDirection {
    Default,  // デフォルト
    Reverce,  // 反転
    Left,  // 左回転
    Right  // 右回転
};

// プレイヤーの状態
enum class PlayerState {
    Idle,           // 待機状態
    Moving,         // 移動中
    Rotating,       // 回転中
    Falling,        // 落下中
    MoveFailed,     // 移動失敗
    RotationFailed  // 回転失敗 (失敗理由は別に保持)
};

// 回転の失敗理由
enum class RotateType {
    None,                  // 回転なし
    Normal,                // 普通に回転成功
    Rotate90_Normal,       // 90度の回転が成功

    // 以降は失敗
    HitDiagonalFrontWall,   // 斜め前の壁にぶつかる
    HitSideWall,            // 隣の壁にぶつかる
    NextTileIsHole,         // 隣のマスに穴がある
    HitDiagonalBackWall,    // 斜め後ろの壁にぶつかる
    HitBackWall,            // 後ろの壁にぶつかる
    BackTileIsHole,          // 後ろに穴が開いてる

    // 以降は90度回転時の失敗
    Rotate90_NextPositionIsHole,        // 90度回転後の位置に穴がある
    Rotate90_HitObstacleDiagonalFront,  // 90度回転時に斜め前に障害物がある
    Rotate90_HitObstacleNextPosition    // 90度回転後の位置に障害物がある
};

// 移動の種類
enum class MoveType {
    Normal,             // 通常の移動
    ParentCarriesChild, // 親が子を持ち上げる
    ChildCarriesParent, // 子が親を持ち上げる
    SlidingOnIce,   // 氷の上を滑っている

    // 以降は失敗
    HitRock,            // 岩にぶつかる
    FallIntoHole,        // 穴に落ちる
    MoveOnChild,        // 子供の上に移動
};

enum class ChildAnimation {
    // 個別でフラグがほしい奴ら
    Rerease, // 放す
    Flustered, // 焦る
    // それ以外
    Normal,
};

enum class PlayerAnimation {
    // 個別でフラグがほしい奴ら
    Flustered, // 焦る
    // それ以外
    Normal,
};