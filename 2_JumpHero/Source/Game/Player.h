#pragma once
#include "Actor.h"
#include <memory>
#include <unordered_map>
#include <string>

class Input;
class Map;
class GameManager;
class EffekseerEffect;

/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public Actor
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="map">マップのデータ</param>
	/// <param name="gameManager">ゲームマネージャーのデータ</param>
	Player(std::weak_ptr<Map> map, std::weak_ptr<GameManager> gameManager);

	/// <summary>
	/// 自動で動くときに呼ぶコンストラクタ
	/// </summary>
	/// <param name="map">マップのデータ</param>
	Player(std::weak_ptr<Map> map);

	// デストラクタ
	virtual ~Player();

	void Init() override;
	void InitMap(std::weak_ptr<Map> map);
	void InitAuto(); // 自動で動くときの初期化
	void Update(Input&) override;
	void Draw() override;

	/// <summary>
	/// ミスしているかどうかを返す
	/// </summary>
	/// <returns>true: ミス状態 false: ミスしていない</returns>
	bool IsMiss() const;

	/// <summary>
	/// 地面についているかどうかを返す
	/// </summary>
	/// <returns>true: 地面についている false: 地面についていない</returns>
	bool IsGround() const { return m_isGround; }

	/// <summary>
	/// プレイヤーの登場が終わったかどうかを判定
	/// </summary>
	/// <returns>true : 終わった false : 終わっていない</returns>
	bool IsEntryEnd()const;

	/// <summary>
	/// ミスした瞬間の処理
	/// </summary>
	void MissStart();

	/// <summary>
	/// プレイヤーをパワーアップする
	/// </summary>
	/// <returns>true : パワーアップできる false : パワーアップできない</returns>
	bool PowerUp();

	/// <summary>
	/// パワーアップ状態かどうか
	/// </summary>
	/// <returns>true : パワーアップ状態 false : パワーアップしていない</returns>
	bool IsPowerUp();

	/// <summary>
	/// 攻撃可能かどうか
	/// </summary>
	/// <returns>true : 攻撃可能 false : 攻撃できない</returns>
	bool IsAttackable() const { return m_isAttackable; }

	/// <summary>
	/// 攻撃可能状態になる
	/// </summary>
	void AttackableStart();

	void DebugClear(const Position2& pos);

	/// <summary>
	/// プレイヤーが動くかどうかのフラグを変える
	/// </summary>
	void FreezeChange();

	/// <summary>
	/// バリアを設定する
	/// </summary>
	/// <param name="barrier">バリアの情報</param>
	void SetBarrier(const ActivePosition2& barrier);

private:
	// マップとの当たり判定を行う際の構造体
	struct ContactFrags
	{
		bool isHitLeft = false;
		bool isHitRight = false;
		bool isHitCeil = false;
		bool isHitGround = false;
	};

	Vector2 m_direction; // 進む方向
	Vector2 m_velocity; // 進む速度
	Position2 m_entryEndPos;
	ActivePosition2 m_barrierPos; // バリア

	int m_graphHandle; // プレイヤーのグラフィックハンドル

	int m_frameCount; // フレームカウンタ
	int m_jumpCount; // ジャンプカウンタ(プレイヤーの強化状態で使用)
	int m_attackCount; // 攻撃カウンタ(プレイヤーの強化状態で使用)
	int m_autoTimeCount; // 自動で動いているときに使うタイムカウンタ
	int m_level; // プレイヤーの強化状態
	float m_prevPosY; // 前フレームのY座標
	float m_missStartY; // ミスになった際のY座標
	bool m_isGround; // 地面についているか
	bool m_isHover; // ジャンプ中にボタンを押したかどうか
	bool m_isMiss; // ミスになったかどうか
	bool m_isLevelDown; // レベルダウンしたかどうか
	bool m_isJumpStart; // ジャンプ開始したかどうか
	bool m_isEntryStart; // 登場開始したかどうか
	bool m_isPlayAppearSE; // 登場時のSEを鳴らしたかどうか
	bool m_isPlayMissBGM; // ミス時のBGMを鳴らしたかどうか
	bool m_isFreeze; // 止まっているかどうか
	bool m_isAttackable; // 攻撃可能かどうか
	bool m_isWalk; // 歩いているかどうか
	bool m_isTurn; // 反転しているかどうか

	std::unordered_map<std::string, Animation> m_animations; // アニメーション

	// マップのポインタ
	std::weak_ptr<Map> m_pMap;

	// ゲームマネージャーのポインタ
	std::weak_ptr<GameManager> m_pGameManager;

	std::weak_ptr<EffekseerEffect> m_pPowerUpEffect; // パワーアップ時のみエフェクトを表示できるように取得

	/// <summary>
	/// 当たっているかどうか
	/// </summary>
	/// <param name="info"></param>
	void IsCollision(const Types::CollisionInfo& info) override;

	/// <summary>
	/// ジャンプ開始時の処理
	/// </summary>
	void JumpStart();

	/// <summary>
	/// プレイヤーのパワーダウンを判定する
	/// </summary>
	void CheckPowerDown();

	/// <summary>
	/// プレイヤーの位置更新とマップとの当たり判定を行う
	/// </summary>
	void MoveOperation(Input& input);

	/// <summary>
	/// マップの番号から宝箱に当たっているかどうかを判定
	/// </summary>
	void CheckHitToChest(Input& input);

	/// <summary>
	/// プレイヤーの状態をチェックしてアニメーションを変更する
	/// </summary>
	void CheckAnimation();

	/// <summary>
	/// サウンドをロード確認する
	/// </summary>
	void LoadSounds();

	// 更新処理用関数群
	void EntryUpdate(Input&); // 登場状態の時
	void JumpUpdate(Input&); // ジャンプ状態の時
	void GroundUpdate(Input&); // 床についている時
	void MissUpdate(Input&); // ミス処理の時
	void AutoMoveUpdate(Input&); // 自動で動く時
	// 更新処理用関数ポインタの型定義
	using UpdateFunc_t = void(Player::*)(Input&);
	UpdateFunc_t m_update;

	// 描画処理用関数群
	void EntryDraw(); // 登場状態の時
	void JumpDraw(); // ジャンプ状態の時
	void GroundDraw(); // 床についている時
	void MissDraw(); // ミス処理の時
	void AutoMoveDraw(); // 自動で動く時
	// 描画処理用関数ポインタの型定義
	using DrawFunc_t = void(Player::*)();
	DrawFunc_t m_draw;

	/// <summary>
	/// プレイヤーが宝箱の上にいるかどうかを判定する
	/// </summary>
	/// <param name="chestPos">プレイヤーが乗っている宝箱の座標</param>
	/// <returns>true : 宝箱の上にいる false : 宝箱の上にいない</returns>
	bool IsOnChestTop(Position2Int& chestPos);

	/// <summary>
	/// 宝箱を横から開けられるかどうかを判定する
	/// </summary>
	/// <returns>true : 横から開けられる false : 横から開けられない</returns>
	bool IsOpenChestX()const;

};

