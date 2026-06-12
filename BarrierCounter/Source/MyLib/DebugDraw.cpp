#include "../Geometry/Vector3.h"
#include <list>
#include "DebugDraw.h"
#include "DxLib.h"

using namespace MyLib;

DebugDraw& DebugDraw::GetInstance()
{
	static DebugDraw instance;
	return instance;
}

void DebugDraw::Clear()
{
	m_lineInfos.clear();
	m_sphereInfos.clear();
}

void DebugDraw::Draw()
{
	for (const auto& item : m_lineInfos)
	{
		DxLib::DrawLine3D(item.start.ToDxLibVector(), item.end.ToDxLibVector(), item.color);
	}
	for (const auto& item : m_sphereInfos)
	{
		DxLib::DrawSphere3D(item.center.ToDxLibVector(), item.radius, 8, item.color, item.color, false);
	}
}

void MyLib::DebugDraw::GetPrintData(const std::string str)
{
	
}

void DebugDraw::DrawLine(const Vector3& start, const Vector3& end, unsigned int color)
{
	LineInfo newInfo;
	newInfo.start = start;
	newInfo.end = end;
	newInfo.color = color;
	m_lineInfos.push_back(newInfo);
}

void MyLib::DebugDraw::DrawSphere(const Vector3& center, float radius, unsigned int color)
{
	SphereInfo newInfo;
	newInfo.center = center;
	newInfo.radius = radius;
	newInfo.color = color;
	m_sphereInfos.push_back(newInfo);
}

DebugDraw::DebugDraw()
{
}