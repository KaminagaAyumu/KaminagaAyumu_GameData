#pragma once
#include "EnemyBase.h"

/// <summary>
/// 動かない敵クラス
/// </summary>
class StayEnemy : public EnemyBase
{
	StayEnemy(const Position2& pos, Player* player, std::weak_ptr<Map> map);

	virtual ~StayEnemy() = default;
	virtual void Init() override;
	virtual void Update(Input&) override;
	virtual void Draw() override;

	virtual void ChangeToItem(int time) override;

private:

	virtual void IsCollision(const Types::CollisionInfo& info) override;

	// 更新処理用関数群
	using UpdateFunc_t = void(StayEnemy::*)(Input&); // 更新処理用関数ポインタの型定義
	UpdateFunc_t m_updateFunc; // 現在の更新処理用関数ポインタ
	void NormalUpdate(Input&); // 通常時の更新処理
	void ItemUpdate(Input&);   // アイテム変化時の更新処理
	void DeadUpdate(Input&);   // 倒された後の更新処理

	// 描画処理用関数群
	using DrawFunc_t = void(StayEnemy::*)(); // 描画処理用関数ポインタの型定義
	DrawFunc_t m_drawFunc; // 現在の描画処理用関数ポインタ
	void NormalDraw(); // 通常時の描画処理
	void ItemDraw();   // アイテム変化時の描画処理

	// 時間カウンタ(状態が変わるごとにリセットする)
	int m_frameCount;

	// アイテム化状態の時間
	int m_itemTime;

	// アイテム化の最大時間
	int m_maxItemTime;

};

