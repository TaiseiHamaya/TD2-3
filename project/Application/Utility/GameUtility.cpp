#include "GameUtility.h"
#include <cmath>

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
