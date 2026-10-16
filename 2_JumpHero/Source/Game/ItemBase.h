#pragma once
#include "Actor.h"
#include <functional>

class Input;
class GameManager;

/// <summary>
/// アイテムの基底クラス
/// </summary>
class ItemBase : public Actor
{
public:
	ItemBase();
	virtual ~ItemBase();

	virtual void Init()abstract;
	virtual void Update(Input&)abstract;
	virtual void Draw()abstract;

	/// <summary>
	/// アイテムの存在フラグ
	/// </summary>
	/// <returns>true : 存在する false : 存在しない</returns>
	bool IsExist() const { return m_isExist; }

	/// <summary>
	/// アイテムが取得された際の処理
	/// </summary>
	/// <param name="gameManager">ゲームマネージャーのポインタ</param>
	virtual void OnCollected(std::weak_ptr<GameManager> gameManager)abstract;

protected:

	// 画像ハンドル
	int m_graphHandle;

	// 存在フラグ
	bool m_isExist;

	/// <summary>
	/// 当たっているかどうか
	/// </summary>
	/// <param name="info"></param>
	virtual void IsCollision(const Types::CollisionInfo& info) abstract;


};

