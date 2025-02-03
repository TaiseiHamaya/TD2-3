#pragma once

#include <vector>

#include "Vector2.h"

/// <summary>
///	線形補間
/// </summary>
/// <typeparam name="type">型名</typeparam>
/// <param name="from">t=0のときの値</param>
/// <param name="to">t=1のときの値</param>
/// <param name="t">媒介変数</param>
/// <returns></returns>
template<typename type>
type Lerp(const type from, const type to, const float t);

Vector2 Bezier(std::vector<Vector2> node, const float t);

namespace Easing {

namespace In {

static float Sine(float t);
static float Quad(float t);
static float Cubic(float t);
static float Quart(float t);
static float Quint(float t);
static float Expo(float t);
static float Circ(float t);
static float Back(float t);
static float Elastic(float t);
static float Bounce(float t);

};

namespace Out {

static float Sine(float t);
static float Quad(float t);
static float Cubic(float t);
static float Quart(float t);
static float Quint(float t);
static float Expo(float t);
static float Circ(float t);
static float Back(float t);
static float Elastic(float t);
static float Bounce(float t);

};

namespace InOut {

static float Sine(float t);
static float Quad(float t);
static float Cubic(float t);
static float Quart(float t);
static float Quint(float t);
static float Expo(float t);
static float Circ(float t);
static float Back(float t);
static float Elastic(float t);
static float Bounce(float t);

};

};

template<typename type>
inline type Lerp(const type from, const type to, const float t) {
	return type(from * (1 - t)) + type(to * t);
}
