/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// EGeometry2DUtilities.h: Common 2D Geometry Utilities used in the engine.
/*=============================================================================*/
#ifndef ELITE_GEOMETRY_2D_UTILITIES
#define ELITE_GEOMETRY_2D_UTILITIES

// Undefine min-max from Windows
#undef min
#undef max

namespace Elite 
{
	/* --- TYPES --- */
	enum class Winding //OUTER shapes should always be given CCW, INNER shapes as CW
	{
		CCW = 0, //Outer
		CW = 1 //Inner
	};

	/* --- FUNCTIONS --- */
	/*! Checking if a container with points is ordered Clockwise or Counter Clockwise. */
	template<typename container>
	Winding GetPolygonWinding(const container& shape)
	{
		//Formula: (xn+1 - xn)(yn+1 + yn)
		//Not allowed to have "changing" winding per triangle, it's the outer space that counts.
		//If ignoring this, correct winding feedback can not be guaranteed!
		// THIS IS WRONG | THIS IS CORRECT	| THIS IS CORRECT
		//	0-------3		 0-------3			 0-------1
		//	|\		|		 |\		 |			 |\		 |
		//	| \		|		 | \	 |			 | \	 |
		//	|  \	|		 |  \	 |			 |  \	 |
		//	|	\	|		 |	 \	 |			 |	 \	 |
		//	|	 \	| 		 |	  \	 | 			 |	  \	 | 
		//	|	  \	|		 |	   \ |			 |	   \ |
		//	2-------1		 1-------2			 3-------2
		//	   ??				CCW				    CW

		//polygon angles should add up to 360 degrees (2 PI)

		float sumAngle{ 0.f };

		Elite::Vector2 firstSegment{};
		Elite::Vector2 prevSegment{};
		Elite::Vector2 currSegment{};
		auto it = shape.begin();
		auto next = std::next(it);
		
		firstSegment = prevSegment = Elite::Vector2{ next->x - it->x, next->y - it->y };

		while ( next != shape.end())
		{
			it = next;
			next = std::next(it);
			if (next != shape.end())
			{
				currSegment = Elite::Vector2{ next->x - it->x, next->y - it->y };
			}
			else
			{
				currSegment = firstSegment;
			}
			
			sumAngle += Elite::AngleBetween(currSegment, -prevSegment);
			prevSegment = currSegment;
		}
		if (sumAngle <= 0)
			return Winding::CW;
		return Winding::CCW;
	}
	/*! Based on 3 points see if point is convex or not. */
	constexpr float IsConvex(const Vector2& tip, const Vector2& prev, const Vector2& next)
	{
		//Formula: d=(x−x1)(y2−y1)−(y−y1)(x2−x1)
		//http://math.stackexchange.com/questions/274712/calculate-on-which-side-of-a-straight-line-is-a-given-point-located
		//    tip
		// 	   /\
		//    /  \
		//   /    \
		// prev   next
		const float d = (tip.x - prev.x) * (next.y - prev.y) - (tip.y - prev.y) * (next.x - prev.x);
		return d > 0;
	}
	/*! Check if a single point is inside the triangle's bounding box. This is a quick overlap test. */
	constexpr bool PointInTriangleBoundingBox(const Vector2& p, const Vector2& tip, const Vector2& prev, const Vector2& next)
	{
		const float xMin = std::min(tip.x, std::min(prev.x, next.x)) - FLT_EPSILON;
		const float xMax = std::max(tip.x, std::max(prev.x, next.x)) + FLT_EPSILON;
		const float yMin = std::min(tip.y, std::min(prev.y, next.y)) - FLT_EPSILON;
		const float yMax = std::max(tip.y, std::max(prev.y, next.y)) + FLT_EPSILON;
		return !(p.x < xMin || xMax < p.x || p.y < yMin || yMax < p.y);
	}
	/*! Square Distance of a point to a line. Used to deal with floating point errors when checking if point is on a line. */
	inline float DistanceSquarePointToLine(const Vector2& p1, const Vector2& p2, const Vector2& point)
	{
		//http://totologic.blogspot.be/2014/01/accurate-point-in-triangle-test.html
		const float p1p2_squareDistance = DistanceSquared(p1, p2);
		const float dp = ((point.x - p1.x)*(p2.x - p1.x) + (point.y - p1.y)*(p2.y - p1.y)) / p1p2_squareDistance;
		if (dp < 0)
			return DistanceSquared(p1, point);
		if (dp <= 1)
		{
			const float pp1_squareDistance = DistanceSquared(point, p1);
			return pp1_squareDistance - dp * dp * p1p2_squareDistance;
		}
		return DistanceSquared(p2, point);
	}
	/*! Check if a single point is inside the triangle - Barycentric Technique*/
	constexpr bool IsPointInTriangle(const Vector2& point, const Vector2& tri1, const Vector2& tri2, const Vector2& tri3)
	{
		const float denominator = ((tri2.y - tri3.y) * (tri1.x - tri3.x) + (tri3.x - tri2.x) * (tri1.y - tri3.y));
		const float a = ((tri2.y - tri3.y) * (point.x - tri3.x) + (tri3.x - tri2.x) * (point.y - tri3.y)) / denominator;
		const float b = ((tri3.y - tri1.y) * (point.x - tri3.x) + (tri1.x - tri3.x) * (point.y - tri3.y)) / denominator;
		const float c = 1 - a - b;
		return 0 <= a && a <= 1 && 0 <= b && b <= 1 && 0 <= c && c <= 1;
	};
	constexpr bool PointInTriangle(const Vector2& point, const Vector2& tip, const Vector2& prev, const Vector2& next, bool onLineAllowed = false)
	{
		//Do bounding box test first
		if (!PointInTriangleBoundingBox(point, tip, prev, next))
			return false;

		//Reference: http://www.blackpawn.com/texts/pointinpoly/default.html
		//Compute direction vectors
		const Vector2 v0 = prev - tip;
		const Vector2 v1 = next - tip;
		const Vector2 v2 = point - tip;

		//Compute dot products
		const float dot00 = Dot(v0, v0);
		const float dot01 = Dot(v0, v1);
		const float dot02 = Dot(v0, v2);
		const float dot11 = Dot(v1, v1);
		const float dot12 = Dot(v1, v2);

		// Compute barycentric coordinates
		const float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
		const float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		const float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		// Check if point is not in triangle (== can be on edge/line)
		if (u < 0 || v < 0 || u > 1 || v > 1 || (u + v) > 1)
		{
			if (onLineAllowed)
			{
				//Check special case where these barycentric coordinates are not enough for on line detection!
				if (DistanceSquarePointToLine(tip, next, point) <= FLT_EPSILON ||
					DistanceSquarePointToLine(next, prev, point) <= FLT_EPSILON ||
					DistanceSquarePointToLine(prev, tip, point) <= FLT_EPSILON)
					return true;
			}
			return false;
		}
		return true;
	}
	/*! Check if point is on a line */
	inline auto IsPointOnLine(const Vector2& lineStart, const Vector2& lineEnd, const Vector2& point)
	{
		//Direction vector
		Vector2 line = lineEnd - lineStart;
		line.Normalize();

		//Projection
		const Vector2 w = point - lineStart;
		const float proj = Dot(w, line);
		if (proj < 0) //Not on line
			return false;

		const float vsq = Dot(line, line);
		if (proj > vsq) //Not on line
			return false;

		return true;
	}
	/*! Project point on line segment. If not on line, remap to fit on line segment (with or without extra offset)*/
	inline Vector2 ProjectOnLineSegment(const Vector2& segmentStart, const Vector2& segmentEnd, const Vector2& point, float offset = 0.0f)
	{
		//Shorten segment based on offset, before doing actual calculations
		const Vector2 toStartDirection = (segmentStart - segmentEnd).GetNormalized();
		const Vector2 vEnd = segmentEnd + (toStartDirection * offset);
		const Vector2 toEndDirection = (segmentEnd - segmentStart).GetNormalized();
		const Vector2 vStart = segmentStart + (toEndDirection * offset);
		//Create shorten segment
		const Vector2 line = vEnd - vStart;

		//Projection
		const Vector2 w = point - vStart;
		const float proj = Dot(w, line);
		const float offsetPercentage = offset / line.Magnitude();

		//Not on line or on vertex, get closest point and offset
		if (proj <= 0)
			return vStart;
		else
		{
			const float vsq = Dot(line, line);
			//Again not on line or on vertex, but the other side
			if (proj >= vsq)
				return vEnd;
			else
				return vStart + (proj / vsq) * line;
		}
	}
	inline bool IsSegmentIntersectingWithCircle(Vector2 startSegment, Vector2 endSegment, Vector2 circleCenter, float circleRadius)
	{
		const Vector2 closestPoint = ProjectOnLineSegment(startSegment, endSegment, circleCenter);
		const Vector2 distanceToCircleCenter = circleCenter - closestPoint;

		if (distanceToCircleCenter.MagnitudeSquared() <= (circleRadius * circleRadius))
		{
			return true;
		}

		return false;
	}
}
#endif
