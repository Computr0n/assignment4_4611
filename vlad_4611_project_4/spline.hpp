#ifndef SPLINE_HPP
#define SPLINE_HPP

#include <glm/glm.hpp>
#include <vector>
using glm::vec3;

class SplinePoint3 {
public:
    float t;    // time
    vec3 p, dp; // value and derivative
    SplinePoint3(float t, vec3 p, vec3 dp): t(t), p(p), dp(dp) {}
};

class Spline3 {
public:
    // list of spline control points. assumed to be in increasing order of t
    std::vector<SplinePoint3> points;

    // value of t for first and last control points
    float minTime() {return points.front().t;}
    float maxTime() {return points.back().t;}

    // returns i such that t lies between points[i].t and points[i+1].t.
    // also modifies t to lie in [minTime(), maxTime()] if necessary.
    int findSegment(float &t);

    // returns value of spline function at time t
    vec3 getValue(float t);

    // returns d/dt of spline function at time t
    vec3 getDerivative(float t);
};

inline int Spline3::findSegment(float &t) {

    // TODO: If t is outside the range [minTime(), maxTime()], replace
    // it with the closest time in that range. Then, find the segment
    // that contains t, so that you can perform cubic Hermite
    // interpolation within it.


	float minTime	= this->minTime();
	float maxTime	= this->maxTime();
	
	if (t > maxTime) //fmod rather than setting to lowest so that future t values also return correct results
		t = fmod(t, maxTime); 

	int segment	= 0;
	for (int i = 0; i < this->points.size(); i++)
	{
		if (this->points[i].t <= t)
		{
			segment++;
		}
		else { break; }
	}
	return segment;
	
}

inline vec3 Spline3::getValue(float t) {
	float x = 0, y = 0, z = 0;
	int seg = findSegment(t);

	float minTime = this->points[seg-1].t;
	float maxTime = this->points[seg].t;
	float new_t = t - minTime;
	float newMax = maxTime - minTime;
	float rt = new_t / newMax;  //This is the rescaled time (hence "rt") -- this is always be between 0 and 1. 

	float derivativeScaler = maxTime - minTime;

	vec3 p0		= this->points[seg - 1].p;
	vec3 dp0	= this->points[seg - 1].dp *derivativeScaler;
	vec3 p1		= this->points[seg].p;
	vec3 dp1	= this->points[seg].dp * derivativeScaler;
	
	float paren1 = (2 * pow(rt, 3) - 3 * pow(rt, 2) + 1);  //multiply by p0
	float paren2 = (pow(rt, 3) - 2 * pow(rt, 2) + rt); //multiply by p'0
	float paren3 = (-2 * pow(rt, 3) + 3 * pow(rt, 2)); //multiply by p1
	float paren4 = (pow(rt, 3) - pow(rt, 2)); //multiply by p'1

	vec3 value = paren1*(p0) + paren2*(dp0) + paren3*(p1) + paren4*(dp1);
	return vec3(value.x, value.y, value.z);;
}

inline vec3 Spline3::getDerivative(float t) {

    // TODO: Find the segment that contains t. Differentiate the cubic
    // Hermite interpolation formula to find the derivative of the
    // spline function. Be careful about how rescaling affects
    // derivatives.
	float x = 0, y = 0, z = 0;
	int seg = findSegment(t);

	float minTime = this->points[seg-1].t;
	float maxTime = this->points[seg].t;
	float new_t = t - minTime;
	float newMax = maxTime - minTime;
	float rt = new_t / newMax;  //This is the rescaled time (hence "rt") -- this should allways be between 0 and 1. //assert (0 <= rt <= 1);

	float derivativeScaler = maxTime - minTime;

	vec3 p0		= this->points[seg - 1].p;
	vec3 dp0	= derivativeScaler * this->points[seg - 1].dp;
	vec3 p1		= this->points[seg].p;
	vec3 dp1	= derivativeScaler * this->points[seg].dp;
	
				// (2t^3			- 3t^2			 + 1)
				// (6t^2			- 6t   )
	float paren1 = (6 * pow(rt, 2)	- 6 * rt);  //multiply by p0
				// (t^3				- 2t^2				+ t)
				// (3t^2			- 4t		+ 1)
	float paren2 = (3 * pow(rt, 2)	- 4 * rt	+ 1); //multiply by p'0
				// (-2t^3			+ 3t^2)
				// (-6t^2			+ 6t)
	float paren3 = (-6 * pow(rt, 2) + 6 * rt); //multiply by p1
				// (t^3				- t^2)
				// (3t^2			- 2t)
	float paren4 = (3 * pow(rt, 2)	- 2 * rt); //multiply by p'1


	vec3 value = paren1*(p0) + paren2*(dp0) + paren3*(p1) + paren4*(dp1);
	return value;

}

#endif
