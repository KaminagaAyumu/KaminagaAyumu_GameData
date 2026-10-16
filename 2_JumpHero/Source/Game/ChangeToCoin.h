#pragma once
#include "ItemBase.h"

class Map; // 浮遊状態で使う

/// <summary>
/// アイテム : 敵をコインに変えるアイテムクラス
/// </summary>
class ChangeToCoin : public ItemBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">生成位置</param>
	/// <param name="handle">画像ハンドル</param>
	ChangeToCoin(const Position2& pos, int handle);
	virtual ~ChangeToCoin() = default;

	virtual void Init()override;
	virtual void Update(Input&)override;
	virtual void Draw()override;


	void SetFloating(std::weak_ptr<Map> map);

	/// <summary>
	/// アイテムが取得された際の処理
	/// </summary>
	/// <param name="gameManager">ゲームマネージャーのポインタ</param>
	virtual void OnCollected(std::weak_ptr<GameManager> gameManager)override;

private:

	float m_startY; // 初期Y座標

	std::weak_ptr<Map> m_pMap; // 浮遊状態の更新処理を行うために取得する

	Vector2 m_velocity;

	/// <summary>
	/// 出現中かどうか
	/// </summary>
	/// <returns>true : 出現中 false : 出現が終わっている</returns>
	bool IsAppear();

	void DropUpdate(); // 宝箱から出現したときの更新処理
	void FloatingUpdate(); // 浮遊状態の更新処理

	// 更新処理用関数ポインタの型定義
	using UpdateFunc_t = void(ChangeToCoin::*)();
	UpdateFunc_t m_updateFunc;

	virtual void IsCollision(const Types::CollisionInfo& info) override;
};

