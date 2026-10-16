#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <deque>
#include "../Utility/GameType.h"
#include "../Utility/Geometry.h"

// プロトタイプ宣言
class Actor;
class Player;
class Camera;
class ChestManager;
class ItemManager;
class ItemBase;
class EnemyManager;
class Input;
class Map;
class EffectManager;
class EffekseerEffect;
class UIManager;
class UIGauge;


/// <summary>
/// ゲーム内の状態を管理するクラス
/// オブジェクト管理クラスの実体もここで持つ
/// </summary>
class GameManager : public std::enable_shared_from_this<GameManager>
{
public:
	GameManager();
	virtual ~GameManager();

	/// <summary>
	/// ゲーム開始時の初期化処理
	/// </summary>
	/// <param name="isTutorial">チュートリアルかどうかを判別する(trueならチュートリアル)</param>
	void Init(std::weak_ptr<Map> map, std::weak_ptr<Camera> camera, std::weak_ptr<EffectManager> effectManager, std::weak_ptr<UIManager> uiManager, std::vector<std::weak_ptr<Actor>>& actors, bool isTutorial);
	
	/// <summary>
	/// ミニゲーム開始時の初期化処理
	/// </summary>
	/// <param name="map">現在のシーンのマップ</param>
	/// <param name="camera">現在のシーンのカメラ</param>
	/// <param name="effectManager">現在のシーンのエフェクトマネージャー</param>
	/// <param name="actors">現在のシーンのアクターコンテナ</param>
	void MiniGameInit(std::weak_ptr<Map> map, std::weak_ptr<Camera> camera, std::weak_ptr<EffectManager> effectManager, std::weak_ptr<UIManager> uiManager, std::vector<std::weak_ptr<Actor>>& actors);
	void Update(Input& input);
	void Draw() const;

	/// <summary>
	/// ゲームシーンが持っているオブジェクトコンテナに派生クラスを入れる
	/// </summary>
	/// <param name="actors">ゲームシーン内オブジェクトコンテナ</param>
	void PushActors(std::vector<std::weak_ptr<Actor>>& actors);

	/// <summary>
	/// スコアを加算する
	/// </summary>
	/// <param name="score">加算するスコアの値</param>
	void AddScore(int score);

	/// <summary>
	/// 敵をアイテムに変える
	/// </summary>
	void ChangeEnemyToCoin();

	/// <summary>
	/// ゴールのアクティブ状態をtrueにする
	/// </summary>
	void ActiveGoal(){ m_isOpenGoal = true; }

	/// <summary>
	/// 当たり判定を行わないかどうか
	/// </summary>
	/// <returns>true : 行わない false : 行う</returns>
	bool IsSkipCollision()const;

	/// <summary>
	/// クリア状態になっているか
	/// </summary>
	/// <returns>true : クリア状態 false : クリア状態ではない</returns>
	bool IsClear()const;

	/// <summary>
	/// ゲームオーバーかどうか
	/// </summary>
	/// <returns>true : ゲームオーバー false : ゲームオーバー状態ではない</returns>
	bool IsGameOver()const;

	/// <summary>
	/// アイテムを取得した際、アイテムに対応したラムダ式を行う処理
	/// </summary>
	/// <param name="type">アイテムの種類</param>
	/// <param name="pos">アイテムの座標</param>
	void OnItemCollected(const Types::ItemType& type, const Position2& pos);

	/// <summary>
	/// アイテムを生成するという通知をItemManagerに送る関数
	/// </summary>
	/// <param name="pos">生成座標</param>
	void DropItem(const Position2& pos);

	/// <summary>
	/// アイテムを指定して生成する通知をItemManagerに送る関数
	/// </summary>
	/// <param name="pos">生成座標</param>
	/// <param name="type">アイテムの種類</param>
	void DropItem(const Position2& pos, const Types::ItemType& type);

	/// <summary>
	/// 空中で動くアイテムを生成
	/// </summary>
	/// <param name="pos">生成座標</param>
	void SpawnFloatingItem(const Position2& pos);

	/// <summary>
	/// 敵の変身先を指定して生成する通知をEnemyManagerに送る関数
	/// </summary>
	/// <param name="pos">生成座標</param>
	/// <param name="formNum">敵の変身先</param>
	/// <note>敵のステートは敵クラス内に書いてしまっているため、一旦番号を変換する形で指定</note>
	void SpawnEnemy(const Position2& pos, int formNum);

	/// <summary>
	/// プレイヤーを強化する
	/// </summary>
	void PowerUpPlayer();

	/// <summary>
	/// プレイヤーのフリーズ状態を変える
	/// </summary>
	void ChangeFreezePlayer();

	/// <summary>
	/// 座標指定した場所の宝箱を開ける
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	void OpenChestToPosition(int x, int y);

	/// <summary>
	/// プレイヤーが指定の座標を超えたかどうかを判定する
	/// </summary>
	/// <param name="area">指定エリア</param>
	/// <returns>true : 超えている false : 超えていない</returns>
	bool IsExceededPlayer(const Position2& area);

	/// <summary>
	/// プレイヤーがパワーアップしているかどうかを判定
	/// </summary>
	/// <returns>true : パワーアップしている false : パワーアップしていない</returns>
	bool IsPowerUpPlayer();

	/// <summary>
	/// 指定された風船の数を取得したかどうかを判定
	/// </summary>
	/// <param name="balloonNum">風船の数</param>
	/// <param name="isAll">すべての風船と判定するかどうか</param>
	/// <returns>true : 取得した false : 取得していない</returns>
	bool IsGetBalloon(int balloonNum, bool isAll);

	/// <summary>
	/// プレイヤーが進めないバリアの座標をセットする
	/// </summary>
	/// <param name="barrier">バリアの座標</param>
	void SetBarrierPlayer(const ActivePosition2& barrier);

	/// <summary>
	/// ゲーム内に存在するオブジェクトの数を取得
	/// </summary>
	/// <returns>ゲーム内のオブジェクトの数</returns>
	const size_t GetActorNum()const;

	/// <summary>
	/// プレイヤーを取得させる
	/// </summary>
	/// <returns>プレイヤーポインタ</returns>
	/// <note>ゲームマネージャー自身が管理しているが、他でも参照したいクラスが存在するため作っている</note>
	Player* GetPlayer()const { return m_pPlayer.get(); }

	/// <summary>
	/// プレイヤーの座標を返す
	/// </summary>
	/// <returns>プレイヤーの座標</returns>
	const Position2& GetPlayerPos()const;

	/// <summary>
	/// スコアを取得(描画用)
	/// </summary>
	/// <returns>演出用のスコア</returns>
	const int GetScore()const { return m_score; }

	/// <summary>
	/// 残機を取得(描画用)
	/// </summary>
	/// <returns>残機</returns>
	const int GetLife()const { return m_life; }

	/// <summary>
	/// 現在のスコアを取得する
	/// リザルト用
	/// </summary>
	/// <returns>スコア</returns>
	const int GetCurrentScore()const { return m_currentScore; }

	/// <summary>
	/// ゲーム内の風船の総数を取得する
	/// </summary>
	/// <returns>風船の総数</returns>
	const int GetTotalBalloonNum()const { return m_totalBalloonNum; }

	/// <summary>
	/// 持っているメダルの数を取得する
	/// </summary>
	/// <returns>持っているメダルの数</returns>
	const int GetMedalNum()const { return m_medalNum; }

	/// <summary>
	/// 風船をどれだけ取得したかの割合を返す
	/// </summary>
	/// <returns>0.0f~1.0fの割合</returns>
	float GetBalloonCounterRate()const;

	/// <summary>
	/// ミニゲーム時の風船をどれだけ取得したかの割合を返す
	/// </summary>
	/// <returns>0.0f~1.0fの割合</returns>
	float GetMiniGameBalloonCounterRate()const;

	/// <summary>
	/// 敵のコイン化時間の割合を返す
	/// </summary>
	/// <returns>0.0f~1.0fの割合</returns>
	float GetChangeToCoinTimeRate()const;

	/// <summary>
	/// プレイヤーがミスしたときに行う処理
	/// 残機を減らしてミスのテキストを表示する
	/// </summary>
	void MissStart();

	/// <summary>
	/// アイテムを取得するイベントを設定
	/// </summary>
	/// <param name="type">どのアイテムか</param>
	void NotifyItemEvent(Types::ItemType type);

	/// <summary>
	/// 指定されたアイテムが取得されたかどうか
	/// </summary>
	/// <param name="type"></param>
	/// <returns></returns>
	bool IsItemPicked(Types::ItemType itemType);

	/// <summary>
	/// エフェクトマネージャーにエフェクトの生成を指示する
	/// </summary>
	/// <param name="effectType">エフェクトの内容</param>
	/// <param name="pos">表示する座標</param>
	void RequestCreateEffect(Types::EffectType effectType, const Position2& pos, bool isUseCamera);

	/// <summary>
	/// エフェクトマネージャーにエフェクトの生成を指示する(座標を返すプロバイダ付き)
	/// </summary>
	/// <param name="effectType">エフェクトの内容</param>
	/// <param name="pos">表示する座標</param>
	/// <param name="provider">座標を返すプロバイダ</param>
	std::weak_ptr<EffekseerEffect> RequestCreateEffect(Types::EffectType effectType, const Position2& pos, std::weak_ptr<Actor> provider, bool isUseCamera);

	/// <summary>
	/// プレイヤーが登場する際のテキストを出す
	/// </summary>
	void CreateReadyGoText();

	/// <summary>
	/// 最後のステージの場合の処理を行う
	/// </summary>
	void SetLastStageMode();

private:
	int m_frameCount; // フレームカウンタ

	// ゲーム内データ関連
	int m_score; // 表示用のスコア
	int m_currentScore; // ゲーム内のスコア
	int m_life; // 残機数
	int m_medalNum; // 強化メダルの所持数
	int m_balloonNum; // 風船をどれだけ取ったか
	int m_balloonCounter; // 風船の数のカウンタ
	int m_totalBalloonNum; // そのステージに存在する風船の総数
	int m_changeToCoinDropBalloonNum; // 敵をコインに変えるアイテムを生成する状態かどうかを判別するために使う
	float m_miniGameBalloonCounterRate; // ミニゲーム内の風船を取った割合を保存する
	bool m_isMiniGame; // ミニゲーム中かどうか
	bool m_isTutorial; // チュートリアルかどうか
	bool m_isOpenGoal; // クリアしたかどうか
	bool m_isItemGaugeMax; // 敵をコインに変えるアイテムを落とすゲージがマックスになったかどうか
	bool m_isChangeToCoin; // 現在敵をコインに変えているかどうか

	std::unordered_map<Types::ItemType, std::function<void(const Position2& pos)>> m_itemCollectFunc;

	/// <summary>
	/// アイテムが取得される時のイベント
	/// </summary>
	struct ItemPickUpEvent
	{
		Types::ItemType type; // どのアイテムか
		int frameCount; // どの順序かを確認するためにフレーム数を格納
	};

	std::deque<ItemPickUpEvent> m_itemPickEvent; // アイテムを取得したかどうかのイベント

	// 各オブジェクト管理クラスの参照用
	std::weak_ptr<Map> m_pMap; // マップはゲームシーンにshared_ptrで持たせるのでweak_ptr
	std::weak_ptr<EffectManager> m_pEffectManager;
	std::weak_ptr<UIManager> m_pUIManager;
	std::weak_ptr<UIGauge> m_pCoinGauge; // 敵をコインに変えている時間を表示するゲージを管理する
	std::shared_ptr<Player> m_pPlayer;
	std::unique_ptr<ChestManager> m_pChestManager;
	std::unique_ptr<ItemManager> m_pItemManager;
	std::unique_ptr<EnemyManager> m_pEnemyManager;

	/// <summary>
	/// 敵をコインに変えるアイテムを生成するかどうか
	/// </summary>
	/// <returns>true : 生成する false : 生成しない</returns>
	bool IsDropChangeToCoin();

};

