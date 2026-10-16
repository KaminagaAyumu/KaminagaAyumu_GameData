#pragma once
#include "Geometry.h"
#include <functional>

/// <summary>
/// ターゲットを基準にして描画位置を設定するカメラクラス
/// </summary>
class Camera
{
public:
	Camera(const Size& size);
	virtual ~Camera();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();

	/// <summary>
	/// ターゲットの座標を基準にして画面のスクロール量を計算する処理
	/// </summary>
	void Update();

	Vector2 scroll; // 画面上のオブジェクトのスクロール量


	/// <summary>
	/// カメラが見るターゲットをセットする
	/// </summary>
	/// <param name="pos">ターゲットの座標</param>
	void SetTarget(const Position2& pos);

	/// <summary>
	/// カメラが見るターゲットを関数からセットする
	/// </summary>
	/// <param name="provider">ターゲットを返す関数</param>
	void SetTargetProvider(std::function<Position2()> provider);

	/// <summary>
	/// 現在の画面の表示範囲を取得
	/// 画面外処理などに使用
	/// </summary>
	/// <returns>画面の表示範囲</returns>
	Rect2D GetNowScreenArea()const;

	/// <summary>
	/// カメラの補正が終わったかどうか
	/// </summary>
	/// <returns>true : 終わった false : 終わっていない</returns>
	bool IsLerpEnd();

private:
	/// <summary>
	/// カメラのモード
	/// </summary>
	enum class Mode
	{
		None, // 見るものがない
		Fixed,// 指定された座標を見る
		Provider // 指定された座標関数を見る
	};

	Mode m_mode; // カメラのモード

	Position2 m_pos; // カメラの座標
	Size m_mapSize; // マップのサイズ
	Position2 m_fixedTarget; // 直接指定する用のターゲット
	std::function<Position2()> m_targetProvider; // ターゲットを示す関数を取得するためのもの

	/// <summary>
	/// 現在のカメラのターゲットを取得する
	/// </summary>
	/// <returns>カメラのターゲット</returns>
	Position2 GetCurrentTarget();

};

