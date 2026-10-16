#pragma once
#include "../Utility/Geometry.h"
#include "../Utility/GameType.h"
#include "../Utility/Animation.h"
#include <memory>

class Input;
class Camera;

/// <summary>
/// ゲーム中に存在するオブジェクトの基底クラス
/// </summary>
class Actor : public std::enable_shared_from_this<Actor>
{
public:

	/// <summary>
	/// コンストラクタ
	/// オブジェクトのタイプを必ず設定させるようにする
	/// </summary>
	/// <param name="t">オブジェクトの種類名</param>
	Actor(Types::ActorType t);
	virtual ~Actor(); // デストラクタ

	// abstractを使って純粋仮想関数として実装
	virtual void Init()		abstract;   // 初期化処理
	virtual void Update(Input&)	abstract;   // 更新処理
	virtual void Draw()		abstract;   // 描画処理

	void SetCamera(std::weak_ptr<Camera> camera); // カメラの参照をセットする関数

	//////////////////////////////////////////////
	         //////当たり判定用関数//////
	//////////////////////////////////////////////

	/// <summary>
	/// 座標の情報を返す
	/// </summary>
	/// <returns>オブジェクトが持つ座標の情報</returns>
	const Position2& GetPos() const { return m_pos; }

	/// <summary>
	/// 円の当たり判定の情報を返す
	/// </summary>
	/// <returns>オブジェクトが持つ円の情報</returns>
	const Circle2D& GetColCircle() const { return m_colCircle; }

	/// <summary>
	/// 矩形の当たり判定の情報を返す
	/// </summary>
	/// <returns>オブジェクトが持つ矩形の情報</returns>
	const Rect2D& GetColRect() const { return m_colRect; }

	/// <summary>
	/// オブジェクトの種類を返す
	/// </summary>
	/// <returns>オブジェクトの種類</returns>
	Types::ActorType GetType() const { return m_type; }

	/// <summary>
	/// 衝突が発生したかどうかを確認する純粋仮想関数
	/// </summary>
	/// <param name="info">衝突したActorの情報、どれと何の当たり判定で当たったか</param>
	virtual void IsCollision(const Types::CollisionInfo& info) abstract;

	//////////////////////////////////////////////
	//////////////////////////////////////////////


protected:
	Position2 m_pos; // 中心位置
	Circle2D m_colCircle; // 当たり判定用の円
	Rect2D m_colRect; // 当たり判定用の矩形
	Types::ActorType m_type; // オブジェクトのタイプ

	std::weak_ptr<Camera> m_pCamera; // カメラへの参照
	Animation m_currentAnim; // 現在のアニメーション

	/// <summary>
	/// アニメーションを変更する
	/// 注意:現在のアニメーションと同じならば変更しない
	/// </summary>
	/// <param name="anim"></param>
	void ChangeAnimation(const Animation& anim);

};

