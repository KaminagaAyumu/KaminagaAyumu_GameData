#pragma once
#include "Vector2.h"
#include "Dxlib.h"
/// <summary>
/// 演算用の幾何学クラス
/// </summary>
class Geometry
{
public:
	/// <summary>
	/// 指定された値の絶対値を返す(float)
	/// </summary>
	/// <param name="value">絶対値を求める対象</param>
	/// <returns>指定された値の絶対値</returns>
	static float Abs(float value) 
	{
		if (value < 0)
		{
			return -value;
		}
		return value;
	}
	/// <summary>
	/// 点と線分の距離の2乗を返す(計算はこっちのほうが軽い)
	/// </summary>
	/// <param name="point">点の位置</param>
	/// <param name="startP">線分の開始位置</param>
	/// <param name="endP">線分の終了位置</param>
	/// <returns></returns>
	static float DistancePointToSegmentMag(const Vector2& point, const Vector2& pointOne, const Vector2& pointTwo)
	{
		//点と最も近い端をstrtPにする。
		Vector2 startP = pointOne;
		Vector2 endP = pointTwo;
		if ((point - pointOne).Magnitude() > (point - pointTwo).Magnitude()) { startP = pointTwo; endP = pointOne; }

		// 線分ベクトルの作成
		Vector2 lineVec = endP - startP;
		//DrawFormatString(50, 40, GetColor(255, 255, 255), "lineVec:(%f,%f)",lineVec.X,lineVec.Y);

		//始点からポイントまでのベクトルの作成
		Vector2 pointVec = point - startP;
		//DrawFormatString(50, 60, GetColor(255, 255, 255), "pointVec:(% f, % f)", pointVec.X, pointVec.Y);

		//なす角が90度以上かを確認
		float dotNum = Vector2::Dot(lineVec,pointVec);
		//DrawFormatString(50, 80, GetColor(255, 255, 255), "dot:%f", dotNum);

		//90度以上なら、startPとpointの単純な距離が線分との一番近い距離
		//90度以上なら、内積の結果は0以下である。
		if (dotNum <= 0) { return (point - startP).Magnitude();}

		// 線分を軸にした射影ベクトルの作成
		Vector2 projectVec = Vector2::Project(pointVec, lineVec);
		//DrawFormatString(50, 100, GetColor(255, 255, 255), "projectVec:(% f, % f)", projectVec.X, projectVec.Y);

		// 点から線分への垂線となるベクトルの作成
		Vector2 ansVec = projectVec - pointVec;
		//DrawFormatString(50, 120, GetColor(255, 255, 255), "anstVec:(% f, % f)", ansVec.X, ansVec.Y);

		return ansVec.Magnitude();
	}
	/// <summary>
	/// 点と線分の距離を返す(計算はこっちのほうが重い)
	/// </summary>
	/// <param name="point">点の位置</param>
	/// <param name="startP">線分の開始位置</param>
	/// <param name="endP">線分の終了位置</param>
	/// <returns></returns>
	static float DistancePointToSegment(const Vector2& point, const Vector2& pointOne, const Vector2& pointTwo)
	{
		//点と最も近い端をstrtPにする。
		Vector2 startP = pointOne;
		Vector2 endP = pointTwo;
		if ((point - pointOne).Magnitude() > (point - pointTwo).Magnitude()) { startP = pointTwo; endP = pointOne; }

		// 線分ベクトルの作成
		Vector2 lineVec = endP - startP;
		//DrawFormatString(50, 40, GetColor(255, 255, 255), "lineVec:(%f,%f)", lineVec.X, lineVec.Y);

		//始点からポイントまでのベクトルの作成
		Vector2 pointVec = point - startP;
		//DrawFormatString(50, 60, GetColor(255, 255, 255), "pointVec:(% f, % f)", pointVec.X, pointVec.Y);

		//なす角が90度以上かを確認
		float dotNum = Vector2::Dot(lineVec, pointVec);
		//DrawFormatString(50, 80, GetColor(255, 255, 255), "dot:%f", dotNum);

		//90度以上なら、startPとpointの単純な距離が線分との一番近い距離
		//90度以上なら、内積の結果は0以下である。
		if (dotNum <= 0) { return (point - startP).Magnitude(); }

		// 線分を軸にした射影ベクトルの作成
		Vector2 projectVec = Vector2::Project(pointVec, lineVec);
		//DrawFormatString(50, 100, GetColor(255, 255, 255), "projectVec:(% f, % f)", projectVec.X, projectVec.Y);

		// 点から線分への垂線となるベクトルの作成
		Vector2 ansVec = projectVec - pointVec;
		//DrawFormatString(50, 120, GetColor(255, 255, 255), "anstVec:(% f, % f)", ansVec.X, ansVec.Y);

		return ansVec.Length();
	}
};
