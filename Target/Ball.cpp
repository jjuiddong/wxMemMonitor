
#include "stdafx.h"
#include "Ball.h"


float rand_rng() // 0.00 ~ 1.00f
{
	return (rand() % 100)/100.f;
}

CBall::CBall(POINT pos, int radian) : 
	m_Radius(radian)
,	m_Color(YELLOW)
{
	m_Pos = Vector2((float)pos.x, (float)pos.y);
	m_Velocity = Vector2(
		(rand_rng()*0.4f - 0.2f),
		(rand_rng()*0.4f - 0.2f) );
}

void CBall::Move(const RECT &windowRect, const std::vector<CBall*> &balls,
				 int elapse_time)
{
	float x = m_Velocity.x * elapse_time;
	float y = m_Velocity.y * elapse_time;
	m_Pos.x += x;
	m_Pos.y += y;

	if (m_Velocity.x < 0 && m_Pos.x - m_Radius  < 0 )
		m_Velocity.x = -m_Velocity.x;

	if (m_Velocity.x > 0 && m_Pos.x + m_Radius  > windowRect.right)
		m_Velocity.x = -m_Velocity.x;	

	if (m_Velocity.y < 0 && m_Pos.y - m_Radius < 0 )
		m_Velocity.y = -m_Velocity.y;

	if (m_Velocity.y > 0 &&  m_Pos.y + m_Radius  > windowRect.bottom)
		m_Velocity.y = -m_Velocity.y;

	// check collision 
	Vector2 curDir = m_Velocity;
	curDir.normalize();
	for (size_t i=0; i < balls.size(); ++i)
	{
		if (this != balls[ i])
		{
			Vector2 dist = balls[ i]->m_Pos - m_Pos;
			if ((m_Radius+balls[ i]->m_Radius) >= dist.length()-0.1f)
			{// collision
				Vector2 dir = m_Pos - balls[ i]->m_Pos;
				dir.normalize();
				// 위치보정 (충돌 위치)
				m_Pos = dir * (m_Radius + balls[ i]->m_Radius) + balls[ i]->m_Pos;
				m_Velocity = dir * m_Velocity.length();
			}
		}
	}
}
