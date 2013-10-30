//------------------------------------------------------------------------
// Name:    Ball.h
// Author:  jjuiddong
// Date:    2/1/2013
// 
// 
//------------------------------------------------------------------------
#ifndef __BALL_H__
#define __BALL_H__

#include <math.h>
#include <vector>
#include "../wxMemMonitorLib/wxMemMonitor.h"
using std::string;

struct Vector2
{
	float x, y;
	Vector2() {}
	Vector2(float _x, float _y) : x(_x), y(_y) {}
	Vector2& operator=(const Vector2 &rhs)
	{
		if (this != &rhs)
		{
			x = rhs.x;
			y = rhs.y;
		}
		return *this;
	}
	Vector2 operator-(const Vector2 &rhs) const
	{
		Vector2 v;
		v.x = x - rhs.x;
		v.y = y - rhs.y;
		return v;
	}
	Vector2 operator+(const Vector2 &rhs) const
	{
		Vector2 v;
		v.x = x + rhs.x;
		v.y = y + rhs.y;
		return v;
	}
	Vector2 operator*(const Vector2 &rhs) const
	{
		Vector2 v;
		v.x = x * rhs.x;
		v.y = y * rhs.y;
		return v;
	}
	template<class T>
	Vector2 operator*(const T &f) const
	{
		Vector2 v;
		v.x = x * f;
		v.y = y * f;
		return v;
	}
	float length() { return (float)sqrt(x*x + y*y); }
	void normalize()
	{
		const float L = length();
		x /= L;
		y /= L;
	}
	void interpol( const Vector2 &from, const Vector2 &to, float f )
	{
		*this = from*(1.f-f) + to*f;
		normalize();
	}

};

DECLARE_TYPE_NAME(CBall)
class CBall : public memmonitor::Monitor<CBall, TYPE_NAME(CBall)>
{
public:
	CBall() {}
	CBall(const string &name, POINT pos, int radian);
	virtual ~CBall() {}

	enum Color
	{
		YELLOW,
		GREEN,
		BLUE,
		RED
	};

protected:
	string m_Name;
	Vector2 m_Pos;
	Vector2 m_Velocity;
	int m_Radius;
	Color m_Color;

public:
	void Move(const RECT &windowRect, const std::vector<CBall*> &balls, 
		int elapse_time );
	Color GetColor() { return m_Color; }
	RECT GetRect() 
	{ 
		RECT r;
		::SetRect(&r, (int)m_Pos.x-m_Radius, (int)m_Pos.y-m_Radius, 
			(int)m_Pos.x+m_Radius, (int)m_Pos.y+m_Radius);
		return r;
	};
	float Power()
	{
		return (m_Velocity.length() * m_Velocity.length()) * m_Radius;
	}

};

#endif // __BALL_H__
