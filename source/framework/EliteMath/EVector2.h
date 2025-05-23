/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// EVector2.h: Vector2D struct
/*=============================================================================*/
#ifndef ELITE_MATH_VECTOR2
#define	ELITE_MATH_VECTOR2
namespace Elite
{
#define ZeroVector2 Vector2()
#define UnitVector2 Vector2(1.f,1.f)

	//Vector 2D
	struct Vector2 final
	{
		//=== Datamembers ===
		float x = 0.0f;
		float y = 0.0f;

		//=== Constructors ===
		Vector2() = default;
		Vector2(float _x, float _y) :x(_x), y(_y) {};

		//=== Vector Conversions Functions ===
#ifdef USE_BOX2D
		explicit Vector2(const b2Vec2& v) : x(v.x), y(v.y) {};
		Vector2& operator=(const b2Vec2& v) { x = v.x; y = v.y; return *this; }
		operator b2Vec2() const
		{return {x, y};};
#endif

		//=== Arithmetic Operators ===
		inline Vector2 operator-(const Vector2& v) const
		{ return Vector2(x - v.x, y - v.y);	}
		inline Vector2 operator-() const
		{ return Vector2(-x, -y); }
		inline Vector2 operator*(float scale) const
		{ return Vector2(x * scale, y * scale);	}
		inline Vector2 operator/(float scale) const
		{
			const float revScale = 1.0f / scale;
			return Vector2(x * revScale, y * revScale);
		}

		//=== Compound Assignment Operators ===
		inline Vector2& operator+=(const Vector2& v)
		{ x += v.x; y += v.y; return *this;	}
		inline Vector2& operator-=(const Vector2& v)
		{ x -= v.x; y -= v.y; return *this;	}
		inline Vector2& operator*=(float scale)
		{ x *= scale; y *= scale; return *this;	}
		inline Vector2& operator/=(float scale)
		{
			const float revScale = 1.0f / scale;
			x *= revScale; y *= revScale; return *this;
		}

		//=== Relational Operators ===
		inline bool operator==(const Vector2& v) const /*Check if both components are equal*/
		{ return AreEqual(x, v.x) && AreEqual(y, v.y);	}
		inline bool operator!=(const Vector2& v) const /*Check if one or both components are NOT equal*/
		{ return !(*this == v);	}

		//=== Member Access Operators ===
		inline float operator[](unsigned int i) const
		{
			return ((i == 0) ? x : y);
			//if (i >= 0 && i < 2)
			//	return ((i == 0) ? x : y);
			//throw; /*TODO : specify error thrown;*/
		}
		inline float& operator[](unsigned int i)
		{
			return ((i == 0) ? x : y);
			//if (i >= 0 && i < 2)
			//	return ((i == 0) ? x : y);
			//throw; /*TODO : specify error thrown;*/
		}

		//=== Internal Vector Functions ===
		inline float Dot(const Vector2& v) const
		{ return x * v.x + y * v.y;	}

		inline float Cross(const Vector2& v) const
		{ return x * v.y - y * v.x;	}

		inline Vector2 GetAbs() const
		{ return Vector2(abs(x), abs(y)); }

		inline float MagnitudeSquared() const
		{ return x*x + y*y;	}

		inline float Magnitude() const
		{ return sqrtf(MagnitudeSquared()); }

		inline float Normalize()
		{
			const float m = Magnitude();
			if (AreEqual(m, 0.f))
			{
				*this = ZeroVector2;
				return 0.f;
			}

			const float invM = 1.f / m;
			x *= invM;
			y *= invM;

			return m;
		}

		inline Vector2 GetNormalized() const /*! Returns a normalized copy of this vector. This vector does not change.*/
		{
			Vector2 v = Vector2(*this);
			v.Normalize();
			return v;
		}

		inline auto DistanceSquared(const Vector2& v) const
		{ return Square(v.x - x) + Square(v.y - y);	}

		inline auto Distance(const Vector2& v) const
		{ return sqrtf(DistanceSquared(v)); }

		inline Vector2 Clamp(float max) const
		{
			float scale = max / Magnitude();
			scale = scale < 1.f ? scale : 1.f;
			return *this * scale;
		}
	};

	//=== Global Vector Operators ===
#pragma region GlobalVectorOperators
	inline Vector2 operator+(const Vector2& v, const Vector2& v2)
	{ return Vector2(v.x + v2.x, v.y + v2.y); }

	inline Vector2 operator* (float s, const Vector2& v)
	{ return Vector2(s * v.x, s * v.y); }

	inline Vector2 operator*(const Vector2& a, const Vector2& b)
	{ return Vector2(a.x*b.x, a.y*b.y); }

	inline Vector2 operator/ (float s, const Vector2& v)
	{
		const float revScale = 1.0f / s;
		return Vector2(revScale * v.x, revScale * v.y);
	}

	inline std::ostream& operator<<(std::ostream& os, const Vector2& rhs)
	{
		os << "(" << rhs.x << ", " << rhs.y << " )";
		return os;
	}
#pragma endregion //GlobalVectorOperators

	//=== Global Vector Functions ===
#pragma region GlobalVectorFunctions
	inline float Dot(const Vector2& v1, const Vector2& v2)
	{ return v1.Dot(v2); }

	inline float Cross(const Vector2& v1, const Vector2& v2)
	{ return v1.Cross(v2); }

	inline Vector2 GetAbs(const Vector2& v)
	{ return v.GetAbs(); }

	inline Vector2 Abs(Vector2& v) /*! Make absolute Vector2 of this Vector2 */
	{ v = v.GetAbs(); }

	inline void Normalize(Vector2& v)
	{ v.Normalize(); }

	inline Vector2 GetNormalized(const Vector2& v)
	{ return v.GetNormalized(); }

	inline float DistanceSquared(const Vector2& v1, const Vector2& v2)
	{ return v1.DistanceSquared(v2); }

	inline float Distance(const Vector2& v1, const Vector2& v2)
	{ return v1.Distance(v2); }

	inline Vector2 Clamp(const Vector2& v, float max)
	{
		float scale = max / v.Magnitude();
		scale = scale < 1.f ? scale : 1.f;
		return v * scale;
	}
#pragma endregion //GlobalVectorFunctions

#pragma region ExtraFunctions
	/*! Random Vector2 */
	inline Vector2 RandomVector2(float max = 1.f)
	{
		return{ RandomBinomial(max), RandomBinomial(max) };
	}
	inline Vector2 RandomVector2(float min, float max)
	{
		return{ RandomFloat(min, max), RandomFloat(min, max) };
	}

	/*Creates a normalized vector from an angle in radians.  */
	inline Vector2 OrientationToVector(float orientation)
	{
		return Vector2(cos(orientation), sin(orientation));
	}

	/*Calculates the orientation angle from a vector*/
	inline float VectorToOrientation(const Vector2& vector)
	{
		return atan2f(vector.y, vector.x);
	}

	/*! Get Angle Between 2 vectors*/
	inline float AngleBetween(const Elite::Vector2& v1, const Elite::Vector2& v2) {
		float x = v1.Dot(v2);
		float y = v1.Cross(v2);
		return atan2(y, x);
	}

#pragma endregion //ExtraFunctions
}
#endif