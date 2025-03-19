#include "GameUtility.h"
#include <cmath>
#include <Library/Math/Basis.h>


bool GameUtility::approximately_equal(const Vector3& a, const Vector3& b, float epsilon)
{
    return std::fabs(a.x - b.x) < epsilon &&
        std::fabs(a.y - b.y) < epsilon &&
        std::fabs(a.z - b.z) < epsilon;
}

Vector3 GameUtility::rotate_direction_90_left(const Vector3& direction)
{
    return { -direction.z, direction.y, direction.x }; // 左回転
}

Vector3 GameUtility::rotate_direction_90_right(const Vector3& direction)
{
    return { direction.z, direction.y, -direction.x }; // 右回転
}

bool GameUtility::IsRotation90Degrees(const Quaternion& q1, const Quaternion& q2) {
    // 角度の許容誤差（5度）
    float DEGREE_TOLERANCE = 5.0f;
    float RADIAN_TOLERANCE = DEGREE_TOLERANCE * (3.1415f / 180.0f);

    // 相対回転を計算
    Quaternion relativeRotation = q2 * q1.inverse();

    // 回転角度を計算
    float theta = 2.0f * std::acos(std::fabs(relativeRotation.real())); // |w| を使う

    // 90度（±5度）の範囲内か判定
    float thetaDeg = theta * (180.0f / 3.1415f);
    return std::fabs(thetaDeg - 90.0f) <= DEGREE_TOLERANCE;
}