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

    // TODO: Find the segment that contains t, and use the cubic
    // Hermite interpolation formula to find the interpolated value
    // within it. Note that the formula discussed in class is only
    // valid for t0 = 0, t1 = 1, so you will have to use a modified
    // formula.
	float x = 0, y = 0, z = 0;
	int seg = findSegment(t);

	float minTime = this->points[seg-1].t;
	float maxTime = this->points[seg].t;
	float new_t = t - minTime;
	float newMax = maxTime - minTime;
	float rt = new_t / newMax;  //This is the rescaled time (hence "rt") -- this should allways be between 0 and 1. //assert (0 <= rt <= 1);



	vec3 p0		= this->points[seg - 1].p;
	vec3 dp0	= this->points[seg - 1].dp;
	vec3 p1		= this->points[seg].p;
	vec3 dp1	= this->points[seg].dp;
	
					//(2t^3 - 3t^2 + 1)
	float paren1 = (pow(2 * rt, 3) - pow(3 * rt, 2) + 1);
					//(t^3 - 2t^2 + t)
	float paren2 = (pow(rt, 3) - pow(2 * rt, 2) + rt);
					//(-2t^3 + 3t^2)
	float paren3 = (pow(-2 * rt, 3) + pow(3 * rt, 2));
					//(t^3 - t^2)
	float paren4 = (pow(rt, 3) - pow(rt, 2));


	// Cubic Hermite Interpolation Equation :
	//  (2t^3 - 3t^2 + 1)*p0 + (t^3 - 2t^2 + t)*p'0 + (-2t^3 + 3t^2)*p1 + (t^3 - t^2)*p'1
	//
	//	(		2t^3	-	3t^2		 + 1)*p0	 + (	t^3	   -	   2t^2		+ t	)*p'0	  +	(		-2t^3	 +		3t^2	 )*p1	  +	(	t^3		-	t^2		)*p'1
//	x = (pow(2 * rt, 3) - pow(3 * rt, 2) + 1)*(p0.x) + (pow(rt, 3) - pow(2 * rt, 2) + rt)*(dp0.x) + (pow(-2 * rt, 3) + pow(3 * rt, 2))*(p1.x) + (pow(rt, 3) - pow(rt, 2))*(dp1.x);;
//	y = (pow(2 * rt, 3) - pow(3 * rt, 2) + 1)*(p0.y) + (pow(rt, 3) - pow(2 * rt, 2) + rt)*(dp0.y) + (pow(-2 * rt, 3) + pow(3 * rt, 2))*(p1.y) + (pow(rt, 3) - pow(rt, 2))*(dp1.y);
//	z = (pow(2 * rt, 3) - pow(3 * rt, 2) + 1)*(p0.z) + (pow(rt, 3) - pow(2 * rt, 2) + rt)*(dp0.z) + (pow(-2 * rt, 3) + pow(3 * rt, 2))*(p1.z) + (pow(rt, 3) - pow(rt, 2))*(dp1.z);


	x = paren1*(p0.x) + paren2*(dp0.x) + paren3*(p1.x) + paren4*(dp1.x);
	y = paren1*(p0.y) + paren2*(dp0.y) + paren3*(p1.y) + paren4*(dp1.y);
	z = paren1*(p0.z) + paren2*(dp0.z) + paren3*(p1.z) + paren4*(dp1.z);
	
	return vec3(x,-y,z);
	

}

inline vec3 Spline3::getDerivative(float t) {

    // TODO: Find the segment that contains t. Differentiate the cubic
    // Hermite interpolation formula to find the derivative of the
    // spline function. Be careful about how rescaling affects
    // derivatives.

    return vec3(0,0,0);

}

#endif
