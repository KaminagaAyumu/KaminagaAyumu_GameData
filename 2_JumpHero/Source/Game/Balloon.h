#pragma once
#include "ItemBase.h"


/// <summary>
/// アイテム : 風船クラス
/// </summary>
class Balloon : public ItemBase
{
public:
	
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">生成位置</param>
	/// <param name="handle">画像ハンドル</param>
	Balloon(const Position2& pos, int handle);
	virtual ~Balloon() = default;

	virtual void Init()override;
	virtual void Update(Input&)override;
	virtual void Draw()override;

	/// <summary>
	/// アイテムが取得された際の処理
	/// </summary>
	/// <param name="gameManager">ゲームマネージャーのポインタ</param>
	virtual void OnCollected(std::weak_ptr<GameManager> gameManager)override;

private:

	virtual void IsCollision(const Types::CollisionInfo& info) override;
};

