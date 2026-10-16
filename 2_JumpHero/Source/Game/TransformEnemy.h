#pragma once
#include "EnemyBase.h"
#include <unordered_map>
#include <string>

class Player;
class Map;
class Input;

/// <summary>
/// 変身する敵クラス
/// </summary>
class TransformEnemy : public EnemyBase
{
public:

	/// <summary>
	/// 敵の状態を表す列挙型
	/// </summary>
	enum class EnemyForm
	{
		Normal, // 原作のミイラ
		PlayerSeeker, // 原作の鳥
		FireBall, // 原作の火の玉
		Skull, // 原作のどくろ
		Item, // アイテム化状態
	};


	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">敵の初期位置</param>
	/// <param name="player">プレイヤーのポインタ</param>
	/// <param name="map">マップのポインタ</param>
	/// <param name="animations">アニメーションのデータ</param>
	TransformEnemy(const Position2& pos,Player* player, std::weak_ptr<Map> map, EnemyForm changeForm, std::unordered_map<std::string, Animation>& animations);
	
	// デストラクタ(今は特に使わないのでdefault)
	virtual ~TransformEnemy() = default;
	virtual void Init() override;
	virtual void Update(Input&) override;
	virtual void Draw() override;

	/// <summary>
	/// 敵をアイテムに変える
	/// </summary>
	/// <param name="time">アイテムに変える時間</param>
	virtual void ChangeToItem(int time)override;

	/// <summary>
	/// 敵コインが取得された際の処理
	/// </summary>
	/// <param name="gameManager">ゲームマネージャーのポインタ</param>
	virtual void OnCollected(std::weak_ptr<GameManager> gameManager)override;

private:

	// 当たり判定用の構造体
	// どこから当たったかを保存する
	struct ContactFrags
	{
		bool isHitLeft = false;
		bool isHitRight = false;
		bool isHitCeil = false;
		bool isHitGround = false;
	};

	virtual void IsCollision(const Types::CollisionInfo& info) override;

	// 更新処理用関数群
	using UpdateFunc_t = void(TransformEnemy::*)(Input&); // 更新処理用関数ポインタの型定義
	UpdateFunc_t m_updateFunc; // 現在の更新処理用関数ポインタ
	void AppearUpdate(Input&); // 出現の際の更新処理
	void NormalUpdate(Input&); // デフォルト状態の更新処理
	void TransformUpdate(Input&); // 変身中の更新処理
	void SeekerUpdate(Input&); // プレイヤーを追い続ける更新処理
	void FireBallUpdate(Input&); // プレイヤーの縦座標を追い続ける更新処理
	void SkullUpdate(Input&); // プレイヤーの横座標を追い続ける更新処理
	void ItemUpdate(Input&); // 敵がアイテム化中の更新処理
	void DeadUpdate(Input&); // 敵が倒された後の更新処理

	// 描画処理用関数群
	using DrawFunc_t = void(TransformEnemy::*)(); // 関数の形式自体は更新処理と同じなので別名とする
	DrawFunc_t m_drawFunc; // 現在の描画処理用関数ポインタ
	void AppearDraw();
	void NormalDraw(); // デフォルト状態の描画処理
	void TransformDraw(); // 変身中の描画処理
	void SeekerDraw(); // プレイヤーを追い続ける時の描画処理
	void FireBallDraw(); // プレイヤーの縦座標を追い続ける時の描画処理
	void SkullDraw(); // プレイヤーの横座標を追い続けるときの描画処理
	void ItemDraw(); // 敵がアイテム化中の描画処理

	/// <summary>
	/// アニメーションのスケールをまとめて設定する
	/// </summary>
	void SetAnimScale();

	/// <summary>
	/// 現在の状態をチェックしてアニメーションを変更する
	/// </summary>
	void CheckAnimation();

	/// <summary>
	/// 当たり判定を行うかどうか
	/// </summary>
	/// <returns>true : 行う false : 行わない</returns>
	bool IsCanCollision()const;

	/// <summary>
	/// アイテム状態かどうか
	/// </summary>
	/// <returns>true : アイテム状態 false : アイテム状態ではない</returns>
	bool IsItemMode()const;

	/// <summary>
	/// 移動の際角で折り返すかどうか
	/// </summary>
	/// <returns>true : 折り返す false : 折り返さない</returns>
	bool IsFlipCorner()const;

	/// <summary>
	/// 座標更新関連の処理
	/// </summary>
	void MoveOperation();

	/// <summary>
	/// 変身後の座標更新関連の処理
	/// </summary>
	void TransformMoveOperation(const Position2& steer);

	// 敵の状態管理用
	EnemyForm m_currentForm; // 現在の敵の姿
	EnemyForm m_nextForm; // 変わる姿

	Animation m_lastAnim; // 1つ前の状態のアニメーションを保存する

	// 時間カウンタ(状態が変わるごとにリセットする)
	int m_frameCount;

	// 移動用カウンタ
	int m_moveCount;

	// アイテム化状態の時間
	int m_itemFormTime;
	// 最大時間(残り時間を判定する用)
	int m_maxItemFormTime;

	// 敵が折り返した回数をカウントする
	int m_turnCount;

	// 前フレームのY座標
	float m_prevPosY;

	// 床にいるかどうか
	bool m_isGround;

	// 右向きかどうか
	bool m_isRightDirection;

	// 上向きかどうか
	bool m_isUpDirection;

	// 移動の力
	Vector2 m_velocity;

	// アニメーション関連
	std::unordered_map<std::string, Animation> m_animations; // アニメーション格納用マップ

};

