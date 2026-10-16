#include "Player.h"
#include "../Utility/Input.h"
#include "../Utility/Game.h"
#include "../Utility/Map.h"
#include "../Utility/Camera.h"
#include "../Utility/AnimationLoader.h"
#include "../Utility/Application.h"
#include "../Utility/Sound/SoundManager.h"
#include "Effect/EffekseerEffect.h"
#include "GameManager.h"
#include "Chest.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	constexpr float kGravity = 0.30f;					// プレイヤーにかかる重力
	constexpr float kGroundY = 570.0f;				// 床の座標
	constexpr float kJumpPower = -15.0f;				// ジャンプ時の上に上がる力
	constexpr float kMissJumpPower = kJumpPower * 1.01f;	// ミスしたときの上に上がる力
	constexpr float kNormalMoveSpeed = 3.5f;					// 左右に動くスピード
	constexpr float kUpGravityScale = 1.5f;				// 上ボタンを押したときの重力倍率
	constexpr float kDownGravityScale = 0.5f;			// 下ボタンを押したときの重力倍率

	constexpr int	kGraphWidth = 45;					// プレイヤー画像の幅
	constexpr int	kGraphHeight = 45;					// プレイヤー画像の高さ

	constexpr float kGraphScale = 1.40625f;	// 画像の拡大率

	constexpr float kPlayerWidth = 35.0f;				// プレイヤーの実際の幅
	constexpr float kPlayerHeight = 35.0f;				// プレイヤーの実際の高さ

	constexpr int	kChestChipNo = 1;					// マップの宝箱の番号
	constexpr float kMapColMargin = 0.5f;				// マップとの当たり判定のマージン
	constexpr float kChestConfirmRange = 1.0f;			// マップから宝箱を見る範囲

	constexpr int	kEntryTextDispTime = 60;					// 登場テキストを表示する時間
	constexpr float	kEntryMoveSpeed = 0.05f;				// プレイヤー登場のスピード

	constexpr int	kMissFreezeTime = 40;					// ミスしたときに止まるフレーム数
	constexpr int	kMissEndTime = 180;					// ミス処理が終わるフレーム数

	constexpr int	kJumpAddScore = 10;					// ジャンプしたときの加算スコア

	constexpr int	kPowerUpLevelOne = 1;					// パワーアップ1段階目
	constexpr int	kJumpLimitNumLevelOne = 20;					// パワーアップが解除されるまでのジャンプ回数

	constexpr int	kPowerUpLevelMax = 2;					// パワーアップ最大値
	constexpr int	kJumpLimitNumLevelMax = 25;					// パワーアップが解除されるまでのジャンプ回数

	constexpr int	kAttackableTime = 600;				// 攻撃可能時間

	constexpr float kEntryEndXOffset = 100.0f;			// プレイヤー登場終了位置のXオフセット

	constexpr int	kAutoWaitFrame = 300;			// プレイヤーが自動で動くときに待つ時間

	constexpr float kAppearSEVolume = 100.0f; // プレイヤーが出現するときのSEのボリューム
	constexpr float kEntrySEVolume = 100.0f; // プレイヤーが登場するときのSEのボリューム

	constexpr float kMissBGMFadeInFrame = 1.0f; // ミスになった時にフェードしながら流すフレーム
	constexpr float kMissBGMFadeOutFrame = 2.0f; // ミスが終わった時にフェードしながら流すフレーム
}

Player::Player(std::weak_ptr<Map> map, std::weak_ptr<GameManager> gameManager) :
	Actor(Types::ActorType::Player),
	m_direction{},
	m_velocity{},
	m_entryEndPos{},
	m_barrierPos{},
	m_graphHandle(-1),
	m_frameCount(0),
	m_jumpCount(0),
	m_attackCount(0),
	m_autoTimeCount(0),
	m_level(0),
	m_prevPosY(0.0f),
	m_missStartY(0.0f),
	m_isGround(false),
	m_isHover(false),
	m_isMiss(false),
	m_isLevelDown(false),
	m_isJumpStart(false),
	m_isEntryStart(false),
	m_isPlayAppearSE(false),
	m_isPlayMissBGM(false),
	m_isFreeze(false),
	m_isAttackable(false),
	m_isWalk(false),
	m_isTurn(false),
	m_pMap(map),
	m_pGameManager(gameManager),
	m_update(&Player::EntryUpdate),
	m_draw(&Player::EntryDraw)
{
	LoadSounds();
}

Player::Player(std::weak_ptr<Map> map) : 
	Actor(Types::ActorType::Player),
	m_direction{},
	m_velocity{},
	m_entryEndPos{},
	m_barrierPos{},
	m_graphHandle(-1),
	m_frameCount(0),
	m_jumpCount(0),
	m_attackCount(0),
	m_autoTimeCount(0),
	m_level(0),
	m_prevPosY(0.0f),
	m_missStartY(0.0f),
	m_isGround(false),
	m_isHover(false),
	m_isMiss(false),
	m_isLevelDown(false),
	m_isJumpStart(false),
	m_isEntryStart(false),
	m_isPlayAppearSE(false),
	m_isPlayMissBGM(false),
	m_isFreeze(false),
	m_isAttackable(false),
	m_isWalk(false),
	m_isTurn(false),
	m_pMap(map),
	m_pGameManager{},
	m_update(&Player::AutoMoveUpdate),
	m_draw(&Player::AutoMoveDraw)
{
	LoadSounds();
}

Player::~Player()
{
	DeleteGraph(m_graphHandle);
}

void Player::Init()
{
	auto pMap = m_pMap.lock();
	m_pos = pMap->GetStartPosToMap();
	m_entryEndPos = m_pos + Vector2{ kEntryEndXOffset,0.0f };
	m_direction = {};
	m_velocity = {};
	m_barrierPos = {};
	m_graphHandle = LoadGraph(L"data/img/player.png");
	m_colCircle = { m_pos,kPlayerWidth * 0.5f };
	m_colRect = { m_pos,kPlayerWidth,kPlayerHeight };
	m_frameCount = 0;
	m_jumpCount = 0;
	m_attackCount = 0;
	m_autoTimeCount = 0;
	m_level = 0;
	m_isGround = false;
	m_isHover = false;
	m_isMiss = false;
	m_isLevelDown = false;
	m_isJumpStart = false;
	m_isEntryStart = false;
	m_isPlayAppearSE = false;
	m_isPlayMissBGM = false;
	m_isFreeze = false;
	m_isAttackable = false;
	m_isWalk = false;
	m_isTurn = false;
	m_update = &Player::EntryUpdate;
	m_draw = &Player::EntryDraw;
	AnimationLoader::LoadAnimationData(L"data/animation/player.csv", m_graphHandle, m_animations);
	
	m_animations["Idle"].SetScale(kGraphScale);
	m_animations["Jump"].SetScale(kGraphScale);
	m_animations["Walk"].SetScale(kGraphScale);
	m_animations["Fall"].SetScale(kGraphScale);

	ChangeAnimation(m_animations["Idle"]);
}

void Player::InitMap(std::weak_ptr<Map> map)
{
	m_pMap = map;
}

void Player::InitAuto()
{
	auto pMap = m_pMap.lock();
	m_pos = pMap->GetStartPosToMap();
	m_entryEndPos = m_pos + Vector2{ kEntryEndXOffset,0.0f };
	m_direction = {};
	m_velocity = {};
	m_barrierPos = {};
	m_graphHandle = LoadGraph(L"data/img/player.png");
	m_colCircle = { m_pos,kPlayerWidth * 0.5f };
	m_colRect = { m_pos,kPlayerWidth,kPlayerHeight };
	m_frameCount = 0;
	m_jumpCount = 0;
	m_attackCount = 0;
	m_autoTimeCount = 0;
	m_level = 0;
	m_isGround = false;
	m_isHover = false;
	m_isMiss = false;
	m_isLevelDown = false;
	m_isJumpStart = false;
	m_isEntryStart = false;
	m_isPlayAppearSE = false;
	m_isPlayMissBGM = false;
	m_isFreeze = false;
	m_isAttackable = false;
	m_isWalk = false;
	m_isTurn = false;
	m_update = &Player::AutoMoveUpdate;
	m_draw = &Player::AutoMoveDraw;
	AnimationLoader::LoadAnimationData(L"data/animation/player.csv", m_graphHandle, m_animations);

	m_animations["Idle"].SetScale(kGraphScale);
	m_animations["Jump"].SetScale(kGraphScale);
	m_animations["Walk"].SetScale(kGraphScale);
	m_animations["Fall"].SetScale(kGraphScale);

	ChangeAnimation(m_animations["Idle"]);
}

void Player::Update(Input& input)
{
	if (m_isFreeze)
	{
		return;
	}

	m_attackCount--;
	if (m_attackCount < 0)
	{
		m_attackCount = 0;
		m_isAttackable = false;
	}

	CheckAnimation();
	m_currentAnim.Update();

	(this->*m_update)(input);
}

void Player::Draw()
{
	(this->*m_draw)();
}

bool Player::IsMiss() const
{
	return (m_update == &Player::MissUpdate);
}

void Player::IsCollision(const Types::CollisionInfo& info)
{
	// 敵と衝突したらミス処理を行う
	if (info.otherType == Types::ActorType::Enemy && !m_isMiss)
	{
#ifdef _DEBUG
		printfDx(L"Player : 敵と衝突しました\n");
#endif 

		//MissStart();
	}

	// 宝箱と衝突した時
	if (info.otherType == Types::ActorType::Chest)
	{
#ifdef _DEBUG
		printfDx(L"Player : 宝箱と衝突しました\n");
#endif 
		

	}
}

void Player::CheckPowerDown()
{
	if (m_jumpCount <= 0 && !m_isLevelDown) // ジャンプカウンタが0の時
	{
#ifdef _DEBUG
		printfDx(L"レベルが下がった\n");
#endif
		m_level--; // レベルダウン
		if (m_level == kPowerUpLevelOne) // レベルダウンした結果レベルが1になったら
		{
			m_jumpCount = kJumpLimitNumLevelOne;

			// エフェクトが存在している場合は
			if (auto effect = m_pPowerUpEffect.lock())
			{
				// エフェクトを消す
				effect->StopEffect();
			}
			auto gameManager = m_pGameManager.lock();
			// エフェクトを1段階目のものにする
			m_pPowerUpEffect = gameManager->RequestCreateEffect(Types::EffectType::PowerUp1, m_pos, weak_from_this(), true);
		}
		else // レベルが0になったら
		{
			auto gameManager = m_pGameManager.lock();
			// エフェクトが存在している場合は
			if (auto effect = m_pPowerUpEffect.lock())
			{
				// エフェクトを消す
				effect->StopEffect();
			}
			else
			{
				// 何もしない(通常起こらない現象)
			}
			m_jumpCount = 0; // ジャンプカウンタをそれ以上下がらないようにする
			m_isLevelDown = true; // レベルが0になったのでここを通らないようにする
		}
	}
}

void Player::MoveOperation(Input& input)
{
	float dx = 0.0f; // X軸の未来の移動量
	// 左右移動の処理
	const bool movingLeft = input.IsPressed("Left");
	const bool movingRight = input.IsPressed("Right");

	if (movingLeft || movingRight)
	{
		m_isWalk = true;
	}
	else
	{
		m_isWalk = false;
	}

	if (movingLeft) // 左ボタンが押されている時
	{
		dx -= kNormalMoveSpeed; // 左に移動
		m_isTurn = true;
	}
	if (movingRight) // 右ボタンが押されている時
	{
		dx += kNormalMoveSpeed; // 右に移動
		m_isTurn = false;
	}

	// 重力の処理
	float gravity = kGravity; // 基本の重力

	if (m_velocity.y < 0.0f) // 上に移動している時
	{
		if (input.IsPressed("Down")) // 下ボタンが押されたとき
		{
			gravity *= kUpGravityScale; // 重力を強くする
		}
		if (input.IsPressed("Up")) // 上ボタンが押されたとき
		{
			gravity *= kDownGravityScale; // 重力を弱くする
		}
	}

	// 重力を速度に加える
	m_velocity.y += gravity;


	float dy = m_velocity.y; // Y軸の未来の移動量
	ContactFrags frags; // 当たり判定の結果を格納する構造体

	// X軸の当たり判定
	Position2 tryPosX = m_pos;
	// 未来の位置を計算
	tryPosX.x += dx;

	// マップを使えるようにlockする
	auto pMap = m_pMap.lock();

	// 当たり判定用の矩形を作成して移動可能範囲を取得
	Rect2D rectX(tryPosX, kPlayerWidth, kPlayerHeight - kMapColMargin);
	Rect2D rangeX = pMap->GetCanMoveRange(rectX);

	// プレイヤーの左右端の座標を取得
	float leftX = tryPosX.x - kPlayerWidth * 0.5f;
	float rightX = tryPosX.x + kPlayerWidth * 0.5f;

	if (leftX < rangeX.GetLeft()) // 左端が移動可能範囲を超えた場合
	{
		// 位置を移動可能範囲内に修正
		tryPosX.x = rangeX.GetLeft() + kPlayerWidth * 0.5f + kMapColMargin;
		frags.isHitLeft = true; // 左に当たったフラグを立てる
		// 速度を抑える(問題があれば0にする)
		dx = tryPosX.x - m_pos.x;
	}
	else if (rightX > rangeX.GetRight()) // 右端が移動可能範囲を超えた場合
	{
		// 位置を移動可能範囲内に修正
		tryPosX.x = rangeX.GetRight() - kPlayerWidth * 0.5f - kMapColMargin;
		frags.isHitRight = true; // 右に当たったフラグを立てる
		// 速度を抑える(問題があれば0にする)
		dx = tryPosX.x - m_pos.x;
	}

	// 修正した座標を実際の座標に反映
	m_pos.x = tryPosX.x;

	// Y軸の当たり判定
	Position2 tryPosY = m_pos;
	// 未来の位置を計算
	tryPosY.y += dy;

	// 当たり判定用の矩形を作成して移動可能範囲を取得
	Rect2D rectY(tryPosY, kPlayerWidth, kPlayerHeight);
	Rect2D rangeY = pMap->GetCanMoveRange(rectY);

	// プレイヤーの上下端の座標を取得
	float topY = tryPosY.y - kPlayerHeight * 0.5f;
	float bottomY = tryPosY.y + kPlayerHeight * 0.5f;

	// 貫通しない床に当たったかどうか
	bool isHitNormalFloor = false;

	// 落下している時に床に当たった場合
	if (dy > 0.0f && bottomY > rangeY.GetBottom())
	{
		// 位置を移動可能範囲内に修正
		tryPosY.y = rangeY.GetBottom() - kPlayerHeight * 0.5f;
		frags.isHitGround = true; // 床に当たったフラグを立てる
		isHitNormalFloor = true; // 貫通しない床に当たったとする
		m_velocity.y = 0.0f; // Y方向の速度を0にする
		dy = tryPosY.y - m_pos.y; // 移動量を修正
		m_frameCount = 0; // 時間経過をリセット
	}
	else if (dy < 0.0f && topY < rangeY.GetTop()) // 上昇している時に天井に当たった場合
	{
		// 位置を移動可能範囲内に修正
		tryPosY.y = rangeY.GetTop() + kPlayerHeight * 0.5f;
		frags.isHitCeil = true; // 天井に当たったフラグを立てる
		m_velocity.y = 0.0f; // Y方向の速度を0にする
		dy = tryPosY.y - m_pos.y; // 移動量を修正
		// 再びジャンプボタンを押した際と同じ処理をする
		m_isHover = true;
		m_frameCount = 0;
	}

	// 下からのみ貫通できる床との判定
	if (!isHitNormalFloor && dy > 0.0f) // 地面との判定を行わなかった際に落下中なら
	{
		const float oldBottom = m_prevPosY + kPlayerHeight * 0.5f; // 前回の下端のY座標

		const float tileSize = pMap->GetTileSize(); // マップのタイルサイズを取得
		int leftTileX = pMap->WorldPosToMapPos(m_pos.x - kPlayerWidth * 0.5f, tileSize); // プレイヤーの左端のマップ座標X
		int rightTileX = pMap->WorldPosToMapPos(m_pos.x + kPlayerWidth * 0.5f, tileSize); // プレイヤーの右端のマップ座標X
		int footTileY = pMap->WorldPosToMapPos(bottomY + kMapColMargin, tileSize); // プレイヤーの下端のマップ座標Y

		bool hitOneWay = false; // 片面通行の床に当たったかどうか
		float candidateTop = pMap->GetMapHeight() * tileSize; // 候補となる床の上端のY座標(仮)

		for (int x = leftTileX; x <= rightTileX; x++) // プレイヤーの幅が大きい時を考慮
		{
			int chipNo = pMap->GetMapChipNum(x, footTileY);
			if (pMap->IsOnlyTopTile(chipNo))
			{
				float tileTop = footTileY * tileSize; // タイルの上端のY座標

				if (oldBottom <= tileTop) // 前回の下端が床の上にあった場合
				{
					if (tileTop < candidateTop)
					{
						candidateTop = min(candidateTop, tileTop);
						// 一方通行床に当たったフラグを立てる
						hitOneWay = true;
					}
				}
			}
		}
		if (hitOneWay)
		{
			tryPosY.y = candidateTop - kPlayerHeight * 0.5f;
			frags.isHitGround = true; // 床に当たったフラグを立てる
			m_velocity.y = 0.0f; // Y方向の速度を0にする
			dy = tryPosY.y - m_pos.y; // 移動量を修正
			m_frameCount = 0; // 時間経過をリセット
		}
	}
	

	// 実際の座標に反映
	m_pos.y = tryPosY.y;

	// 状態の更新
	m_isGround = frags.isHitGround;
	if (m_isGround) // 地面についている時
	{
		m_update = &Player::GroundUpdate; // 更新処理を地面についている時に
		m_draw = &Player::GroundDraw; // 描画処理を地面についている時に
	}
	else // 空中にいる時
	{
		m_update = &Player::JumpUpdate; // 更新処理をジャンプ状態に
		m_draw = &Player::JumpDraw; // 描画処理をジャンプ状態に
	}

	if (m_barrierPos.isActive)
	{
		if (m_pos.x >= m_barrierPos.pos.x)
		{
			m_pos.x = m_barrierPos.pos.x;
		}
	}

	m_colCircle.pos = m_pos; // 円の座標更新
	m_colRect.pos = m_pos; // 矩形の座標更新
	m_prevPosY = m_pos.y; // 前回のY座標を更新

	CheckHitToChest(input);
}

void Player::CheckHitToChest(Input& input)
{
	// 宝箱の位置を調べる
	bool isRightSide = false;
	bool isLeftSide = false;
	// 宝箱が見つかった際に見つかった座標を指定できるようにするための変数
	int chestX = -1;
	int chestY = -1;
	
	// マップを使えるようにlockする
	auto pMap = m_pMap.lock();

	float tileSize = pMap->GetTileSize(); // マップのタイルサイズを取得

	const float top = m_pos.y - kPlayerHeight * 0.5f; // プレイヤーの上端のY座標
	const float bottom = m_pos.y + kPlayerHeight * 0.5f; // プレイヤーの下端のY座標
	const float left = m_pos.x - kPlayerWidth * 0.5f; // プレイヤーの左端のX座標
	const float right = m_pos.x + kPlayerWidth * 0.5f; // プレイヤーの右端のX座標

	int topY = pMap->WorldPosToMapPos(top, tileSize); // プレイヤーの上端のマップ座標Y
	int bottomY = pMap->WorldPosToMapPos(bottom, tileSize); // プレイヤーの下端のマップ座標Y
	int leftX = pMap->WorldPosToMapPos(left, tileSize); // プレイヤーの左端のマップ座標X
	int rightX = pMap->WorldPosToMapPos(right, tileSize); // プレイヤーの右端のマップ座標X

	const float leftProbe = left - kChestConfirmRange; // プレイヤーの左端の少し左のX座標
	int leftProbeX = pMap->WorldPosToMapPos(leftProbe, tileSize);
	
	for (int y = topY; y <= bottomY; y++)
	{
		// int chipNo = pMap->GetMapChipNum(leftProbeX,y);
		int chipNo = pMap->GetPositioningData(leftProbeX, y);
		if(pMap->IsChestTile(chipNo))
		{
			isLeftSide = true;
			
#ifdef _DEBUG
			printfDx(L"Player: 宝箱の右側にいる\n");
#endif 
			if(chestX != -1 && chestY != -1)
			{
				// すでに宝箱が見つかっている場合は何もしない
				break;
			}
			chestX = leftProbeX;
			chestY = y;
			if(IsOpenChestX()) // 宝箱が横から開けられる時
			{
				auto gameManager = m_pGameManager.lock();
				// 宝箱を開ける
				gameManager->OpenChestToPosition(chestX, chestY);
			}
			break;
		}
	}

	const float rightProbe = right + kChestConfirmRange; // プレイヤーの右端の少し右のX座標
	int rightProbeX = pMap->WorldPosToMapPos(rightProbe, tileSize);

	for (int y = topY; y <= bottomY; y++)
	{
		//int chipNo = pMap->GetMapChipNum(rightProbeX, y);
		int chipNo = pMap->GetPositioningData(rightProbeX, y);
		if (pMap->IsChestTile(chipNo))
		{
			isRightSide = true;
			if(chestX != -1 && chestY != -1)
			{
				// すでに宝箱が見つかっている場合は何もしない
				break;
			}
			chestX = rightProbeX;
			chestY = y;
#ifdef _DEBUG
			printfDx(L"Player: 宝箱の左側にいる\n");
#endif 
			if (IsOpenChestX()) // 宝箱が横から開けられる時
			{
				auto gameManager = m_pGameManager.lock();
				// 宝箱を開ける
				gameManager->OpenChestToPosition(chestX, chestY);
			}
			break;
		}
	}
}

void Player::CheckAnimation()
{
	if (m_isGround) // 地面についている時
	{
		if (m_isWalk)
		{
			ChangeAnimation(m_animations["Walk"]);
		}
		else
		{
			ChangeAnimation(m_animations["Idle"]);
		}
	}
	else
	{
		ChangeAnimation(m_animations["Fall"]);
	}
}

void Player::LoadSounds()
{
	// サウンドマネージャーを取得
	auto soundManager = Application::GetInstance().GetSoundManager();
	soundManager->LoadSoundClip("jumpSE", L"data/sound/SE/jumpSE.wav", SoundBus::SE, 1.0f, false);
	soundManager->LoadSoundClip("jumpSE1", L"data/sound/SE/jumpSE_1.wav", SoundBus::SE, 1.0f, false);
	soundManager->LoadSoundClip("appearSE", L"data/sound/SE/playerAppear.mp3", SoundBus::SE, kAppearSEVolume, false);
	soundManager->LoadSoundClip("entrySE", L"data/sound/SE/playerEntry.mp3", SoundBus::SE, kEntrySEVolume, false);
	soundManager->LoadSoundClip("playerFloatSE", L"data/sound/SE/playerFloat.wav", SoundBus::SE, 1.0f, false);
	soundManager->LoadSoundClip("missSE", L"data/sound/SE/missSE.wav", SoundBus::SE, 1.0f, false);
	soundManager->LoadSoundClip("missBGM", L"data/sound/BGM/missBGM.wav", SoundBus::BGM, 1.0f, false);
}

void Player::EntryUpdate(Input&)
{
	m_frameCount++;
	if (!m_isEntryStart)
	{
		auto manager = m_pGameManager.lock();
		manager->CreateReadyGoText();
		Application::GetInstance().GetSoundManager()->Play("entrySE", 1.0f, true);
		m_isEntryStart = true;
	}

	if (m_frameCount < kEntryTextDispTime) // テキストを表示している間は動かない
	{
		return;
	}

	if (!m_isPlayAppearSE)
	{
		Application::GetInstance().GetSoundManager()->Play("appearSE", 1.0f, true);
		m_isPlayAppearSE = true;
	}

	// 位置を初期位置から登場終了位置まで線形補完で動かす
	m_pos = Geometry::LerpVec2(m_pos, m_entryEndPos, kEntryMoveSpeed);

	if (m_pos.x >= m_entryEndPos.x - 1.0f) // 終了位置と大体同じになったら終了
	{
		m_pos = m_entryEndPos; // プレイヤーの位置を終了位置に動かす
		m_frameCount = 0; // フレームカウントをリセット
		m_update = &Player::JumpUpdate; // 更新処理をジャンプ状態に
		m_draw = &Player::JumpDraw; // 描画処理をジャンプ状態に
		return;
	}
}

void Player::JumpUpdate(Input& input)
{
	if (m_level > 0) // レベルが上がっている時
	{
		CheckPowerDown(); // プレイヤーのパワーダウンをするかどうか判定する
	}

	if (input.IsTriggered("PowerUp")) // パワーアップボタンが押されたとき
	{
		auto gameManager = m_pGameManager.lock();
		// パワーアップ処理を行う
		gameManager->PowerUpPlayer();
	}

	if (input.IsTriggered("Jump")) // 再びジャンプボタンが押されたら
	{
		Application::GetInstance().GetSoundManager()->Play("playerFloatSE", 1.0f, true);
		m_isHover = true;
		m_velocity.y = 0.0f;
		m_frameCount = 0;
	}

	MoveOperation(input); // 移動処理を行う
}

void Player::GroundUpdate(Input& input)
{
	if (input.IsTriggered("PowerUp")) // パワーアップボタンが押されたとき
	{
		auto gameManager = m_pGameManager.lock();
		// パワーアップ処理を行う
		gameManager->PowerUpPlayer();
	}
	if (input.IsTriggered("Jump") && m_isGround) // ジャンプボタンが押されたとき
	{
		JumpStart(); // ジャンプする際の処理を行う
		Position2Int chestPos;
		if (IsOnChestTop(chestPos)) // 宝箱の上にいる時
		{
			auto gameManager = m_pGameManager.lock();
			// 宝箱を開ける
			gameManager->OpenChestToPosition(chestPos.x, chestPos.y);
		}
		return;
	}
	else
	{
		m_isJumpStart = false;
	}

	MoveOperation(input); // 移動処理を行う
}

void Player::MissUpdate(Input&)
{
	m_frameCount++;
	// 一定時間たつまで座標の更新を行わない(止まる)
	if (m_frameCount <= kMissFreezeTime)
	{
		return;
	}

	// ミス用のBGMを流していないなら流す
	if (!m_isPlayMissBGM)
	{
		Application::GetInstance().GetSoundManager()->BeginTemporaryBGM("missBGM", kMissBGMFadeInFrame);
		m_isPlayMissBGM = true;
	}

	// 重力とジャンプの速度を加える
	m_velocity.y += kGravity;

	m_pos.y += m_velocity.y;

	// プレイヤーの中心Y座標がミス開始座標から画面の高さ分下に落ちたら
	if (m_pos.y >= m_missStartY + Game::kScreenHeight)
	{
		// 床にいるフラグをtrueにする
		m_isGround = true;
	}

	// 地面についたかつミス状態が終わる時間になったら
	if (m_frameCount >= kMissEndTime && m_isGround)
	{
		Init(); // 初期化処理を行う
		Application::GetInstance().GetSoundManager()->EndTemporaryBGM(kMissBGMFadeOutFrame); // BGMをもとに戻す
		return;
	}
	m_colRect.pos = m_pos; // 矩形の座標更新
	m_colCircle.pos = m_pos; // 円の座標更新
}

void Player::AutoMoveUpdate(Input&)
{
	m_autoTimeCount++;

	if (m_autoTimeCount >= kAutoWaitFrame)
	{
		// 歩いているフラグをtrueに変更
		m_isWalk = true;
	}

	//if (ジャンプの条件)
	//{
	//	m_direction = { 0.0f,1.0f }; // ジャンプの方向を上向きにする
	//	m_velocity = { 0.0f, kJumpPower }; // ジャンプの力を加える
	//	m_isHover = false; // 空中で浮いたかどうかをリセット
	//	m_isGround = false; // ジャンプしたので地面についていないとする
	//	m_isJumpStart = true; // ジャンプ開始フラグをtrueにする
	//	m_frameCount = 0; // 時間経過をリセット
	//}

	float dx = 0.0f; // X軸の未来の移動量

	if (m_isWalk)
	{
		if (m_isTurn)
		{
			dx -= kNormalMoveSpeed; // 左に移動
		}
		else
		{
			dx += kNormalMoveSpeed; // 右に移動
		}
	}

	// 重力の処理
	float gravity = kGravity; // 基本の重力

	if (m_velocity.y < 0.0f) // 上に移動している時
	{
		//if (input.IsPressed("Down")) // 下ボタンが押されたとき
		//{
		//	gravity *= kUpGravityScale; // 重力を強くする
		//}
		//if (input.IsPressed("Up")) // 上ボタンが押されたとき
		//{
		//	gravity *= kDownGravityScale; // 重力を弱くする
		//}
	}

	// 重力を速度に加える
	m_velocity.y += gravity;


	float dy = m_velocity.y; // Y軸の未来の移動量
	ContactFrags frags; // 当たり判定の結果を格納する構造体

	// X軸の当たり判定
	Position2 tryPosX = m_pos;
	// 未来の位置を計算
	tryPosX.x += dx;

	// マップを使えるようにlockする
	auto pMap = m_pMap.lock();

	// 当たり判定用の矩形を作成して移動可能範囲を取得
	Rect2D rectX(tryPosX, kPlayerWidth, kPlayerHeight - kMapColMargin);
	Rect2D rangeX = pMap->GetCanMoveRange(rectX);

	// プレイヤーの左右端の座標を取得
	float leftX = tryPosX.x - kPlayerWidth * 0.5f;
	float rightX = tryPosX.x + kPlayerWidth * 0.5f;

	if (leftX < rangeX.GetLeft()) // 左端が移動可能範囲を超えた場合
	{
		// 位置を移動可能範囲内に修正
		tryPosX.x = rangeX.GetLeft() + kPlayerWidth * 0.5f + kMapColMargin;
		frags.isHitLeft = true; // 左に当たったフラグを立てる
		// 速度を抑える(問題があれば0にする)
		dx = tryPosX.x - m_pos.x;
	}
	else if (rightX > rangeX.GetRight()) // 右端が移動可能範囲を超えた場合
	{
		// 位置を移動可能範囲内に修正
		tryPosX.x = rangeX.GetRight() - kPlayerWidth * 0.5f - kMapColMargin;
		frags.isHitRight = true; // 右に当たったフラグを立てる
		// 速度を抑える(問題があれば0にする)
		dx = tryPosX.x - m_pos.x;
	}

	// 修正した座標を実際の座標に反映
	m_pos.x = tryPosX.x;

	// Y軸の当たり判定
	Position2 tryPosY = m_pos;
	// 未来の位置を計算
	tryPosY.y += dy;

	// 当たり判定用の矩形を作成して移動可能範囲を取得
	Rect2D rectY(tryPosY, kPlayerWidth, kPlayerHeight);
	Rect2D rangeY = pMap->GetCanMoveRange(rectY);

	// プレイヤーの上下端の座標を取得
	float topY = tryPosY.y - kPlayerHeight * 0.5f;
	float bottomY = tryPosY.y + kPlayerHeight * 0.5f;

	// 貫通しない床に当たったかどうか
	bool isHitNormalFloor = false;

	// 落下している時に床に当たった場合
	if (dy > 0.0f && bottomY > rangeY.GetBottom())
	{
		// 位置を移動可能範囲内に修正
		tryPosY.y = rangeY.GetBottom() - kPlayerHeight * 0.5f;
		frags.isHitGround = true; // 床に当たったフラグを立てる
		isHitNormalFloor = true; // 貫通しない床に当たったとする
		m_velocity.y = 0.0f; // Y方向の速度を0にする
		dy = tryPosY.y - m_pos.y; // 移動量を修正
		m_frameCount = 0; // 時間経過をリセット
	}
	else if (dy < 0.0f && topY < rangeY.GetTop()) // 上昇している時に天井に当たった場合
	{
		// 位置を移動可能範囲内に修正
		tryPosY.y = rangeY.GetTop() + kPlayerHeight * 0.5f;
		frags.isHitCeil = true; // 天井に当たったフラグを立てる
		m_velocity.y = 0.0f; // Y方向の速度を0にする
		dy = tryPosY.y - m_pos.y; // 移動量を修正
		// 再びジャンプボタンを押した際と同じ処理をする
		m_isHover = true;
		m_frameCount = 0;
	}

	// 下からのみ貫通できる床との判定
	if (!isHitNormalFloor && dy > 0.0f) // 地面との判定を行わなかった際に落下中なら
	{
		const float oldBottom = m_prevPosY + kPlayerHeight * 0.5f; // 前回の下端のY座標

		const float tileSize = pMap->GetTileSize(); // マップのタイルサイズを取得
		int leftTileX = pMap->WorldPosToMapPos(m_pos.x - kPlayerWidth * 0.5f, tileSize); // プレイヤーの左端のマップ座標X
		int rightTileX = pMap->WorldPosToMapPos(m_pos.x + kPlayerWidth * 0.5f, tileSize); // プレイヤーの右端のマップ座標X
		int footTileY = pMap->WorldPosToMapPos(bottomY + kMapColMargin, tileSize); // プレイヤーの下端のマップ座標Y

		bool hitOneWay = false; // 片面通行の床に当たったかどうか
		float candidateTop = pMap->GetMapHeight() * tileSize; // 候補となる床の上端のY座標(仮)

		for (int x = leftTileX; x <= rightTileX; x++) // プレイヤーの幅が大きい時を考慮
		{
			int chipNo = pMap->GetMapChipNum(x, footTileY);
			if (pMap->IsOnlyTopTile(chipNo))
			{
				float tileTop = footTileY * tileSize; // タイルの上端のY座標

				if (oldBottom <= tileTop) // 前回の下端が床の上にあった場合
				{
					if (tileTop < candidateTop)
					{
						candidateTop = min(candidateTop, tileTop);
						// 一方通行床に当たったフラグを立てる
						hitOneWay = true;
					}
				}
			}
		}
		if (hitOneWay)
		{
			tryPosY.y = candidateTop - kPlayerHeight * 0.5f;
			frags.isHitGround = true; // 床に当たったフラグを立てる
			m_velocity.y = 0.0f; // Y方向の速度を0にする
			dy = tryPosY.y - m_pos.y; // 移動量を修正
			m_frameCount = 0; // 時間経過をリセット
		}
	}
	

	// 実際の座標に反映
	m_pos.y = tryPosY.y;

	// 状態の更新
	m_isGround = frags.isHitGround;

	if (frags.isHitLeft)
	{
		m_isTurn = false;
	}
	if (frags.isHitRight)
	{
		m_isTurn = true;
	}

	if (m_barrierPos.isActive)
	{
		if (m_pos.x >= m_barrierPos.pos.x)
		{
			m_pos.x = m_barrierPos.pos.x;
		}
	}

	m_colCircle.pos = m_pos; // 円の座標更新
	m_colRect.pos = m_pos; // 矩形の座標更新
	m_prevPosY = m_pos.y; // 前回のY座標を更新
}

void Player::EntryDraw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);
	m_currentAnim.Draw({ drawX, drawY }, m_isTurn);
	
#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void Player::JumpDraw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);
	
	m_currentAnim.Draw({ drawX, drawY }, m_isTurn);

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void Player::GroundDraw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);
	
	m_currentAnim.Draw({ drawX, drawY }, m_isTurn);

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void Player::MissDraw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);
	
	m_currentAnim.Draw({ drawX, drawY }, m_isTurn);

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void Player::AutoMoveDraw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);
	
	m_currentAnim.Draw({ drawX, drawY }, m_isTurn);

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

bool Player::IsOnChestTop(Position2Int& chestPos)
{
	// 宝箱が見つかった際に見つかった座標を指定できるようにするための変数
	int chestX = -1;
	int chestY = -1;

	// マップを使えるようにlockする
	auto pMap = m_pMap.lock();

	float tileSize = pMap->GetTileSize(); // マップのタイルサイズを取得

	const float bottom = m_pos.y + kPlayerHeight * 0.5f; // プレイヤーの下端のY座標
	const float left = m_pos.x - kPlayerWidth * 0.5f; // プレイヤーの左端のX座標
	const float right = m_pos.x + kPlayerWidth * 0.5f; // プレイヤーの右端のX座標

	int bottomY = pMap->WorldPosToMapPos(bottom, tileSize); // プレイヤーの下端のマップ座標Y
	int leftX = pMap->WorldPosToMapPos(left, tileSize); // プレイヤーの左端のマップ座標X
	int rightX = pMap->WorldPosToMapPos(right, tileSize); // プレイヤーの右端のマップ座標X

	// プレイヤーの下端の少し下のマップチップを取得
	const float foot = bottom + kChestConfirmRange; // プレイヤーの下端の少し下のY座標
	int footY = pMap->WorldPosToMapPos(foot, tileSize); // プレイヤーの下端の少し下のマップ座標Y

	for (int x = leftX; x <= rightX; x++) // プレイヤーの幅が大きい時を考慮
	{
		int chipNo = pMap->GetPositioningData(x, footY);
		if (pMap->IsChestTile(chipNo))
		{
			chestX = x;
			chestY = footY;
			chestPos = { chestX,chestY };
#ifdef _DEBUG
			printfDx(L"Player: 宝箱の上にいる\n");
#endif
			return true;
		}
	}
	return false;
}

bool Player::IsOpenChestX() const
{
	// レベルが1以上なら宝箱が横から開けられる
	return m_level >= kPowerUpLevelOne;
}

bool Player::IsPowerUp()
{
	// レベルが1以上ならパワーアップしている
	return m_level >= kPowerUpLevelOne;
}

void Player::AttackableStart()
{
	m_isAttackable = true;
	m_attackCount = kAttackableTime;
}

void Player::DebugClear(const Position2& pos)
{
	m_pos = pos;
}

void Player::FreezeChange()
{
	m_isFreeze = !m_isFreeze;
}

void Player::SetBarrier(const ActivePosition2& barrier)
{
	m_barrierPos = barrier;
}

void Player::JumpStart()
{
	m_direction = { 0.0f,1.0f }; // ジャンプの方向を上向きにする
	m_velocity = { 0.0f, kJumpPower }; // ジャンプの力を加える
	m_isHover = false; // 空中で浮いたかどうかをリセット
	m_isGround = false; // ジャンプしたので地面についていないとする
	m_isJumpStart = true; // ジャンプ開始フラグをtrueにする
	m_frameCount = 0; // 時間経過をリセット
	m_jumpCount--; // ジャンプ回数を減らす
	auto gameManager = m_pGameManager.lock();
	gameManager->AddScore(kJumpAddScore); // スコアを加算
	gameManager->RequestCreateEffect(Types::EffectType::Jump, m_pos + Position2{0,kPlayerHeight / 2},true);
	Application::GetInstance().GetSoundManager()->Play("jumpSE", 1.0f, true);
	Application::GetInstance().GetSoundManager()->Play("jumpSE1", 1.0f, true);
	m_update = &Player::JumpUpdate; // 更新処理をジャンプ状態に
	m_draw = &Player::JumpDraw; // 描画処理をジャンプ状態に
}

bool Player::IsEntryEnd() const
{
	// 初期状態がentryなのでそれ以外の時は終わっているとする
	return !(m_update == &Player::EntryUpdate);
}

void Player::MissStart()
{
	m_direction = { 0.0f,1.0f }; // ジャンプの方向を上向きにする
	m_velocity = { 0.0f, kMissJumpPower }; // 普通のジャンプより高い力を加える
	m_isHover = false; // 空中で浮いたかどうかをリセット
	m_isGround = false; // 一応ジャンプするので地面についていないとする
	m_isMiss = true; // ミスフラグをtrueにする
	m_isPlayMissBGM = false; // ミスのBGMを鳴らしたかどうかをリセット
	m_frameCount = 0; // 時間経過をリセット
	m_missStartY = m_pos.y; // ミスした際の座標を取得
	auto gameManager = m_pGameManager.lock();
	gameManager->MissStart(); // 残機を減らす処理を呼ぶ
	Application::GetInstance().GetSoundManager()->Play("missSE", 1.0f, true);
	m_update = &Player::MissUpdate; // 更新処理をミス状態に
	m_draw = &Player::MissDraw;// 描画処理をミス状態に
}

bool Player::PowerUp()
{
	// レベルが最大値だったら
	if (m_level >= kPowerUpLevelMax)
	{
#ifdef _DEBUG
		printfDx(L"強化状態はマックスです\n");
#endif
		return false; // パワーアップできないのでfalseを返す
	}
#ifdef _DEBUG
	printfDx(L"プレイヤーの強化に成功\n");
#endif

	m_level++; // レベルを1増やす
	if (m_level == kPowerUpLevelOne) // 1段階目なら
	{
		auto gameManager = m_pGameManager.lock();
		if (auto effect = m_pPowerUpEffect.lock())
		{
			// 何もしない(通常起こらない現象)
		}
		else
		{
			m_pPowerUpEffect = gameManager->RequestCreateEffect(Types::EffectType::PowerUp1, m_pos, weak_from_this(), true);
		}
		m_jumpCount = kJumpLimitNumLevelOne; // 20回ジャンプするまでパワーアップ継続
	}
	else if (m_level == kPowerUpLevelMax) // 2段階目なら
	{
		m_jumpCount = kJumpLimitNumLevelMax; // 25回ジャンプするまでパワーアップ継続
		auto gameManager = m_pGameManager.lock();
		gameManager->ChangeEnemyToCoin(); // 敵をアイテムに変える処理を呼ぶ

		if (auto effect = m_pPowerUpEffect.lock())
		{
			effect->StopEffect();
		}

		// エフェクトを2段階目のものにする
		m_pPowerUpEffect = gameManager->RequestCreateEffect(Types::EffectType::PowerUp2, m_pos, weak_from_this(), true);

	}
	m_isLevelDown = false; // レベルが下がったかどうかの判定を可能にする

	return true; // パワーアップできたのでtrueを返す
}
