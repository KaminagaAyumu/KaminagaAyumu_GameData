#include <memory>
#include "TransformEnemy.h"
#include "../Utility/Camera.h"
#include "../Utility/Map.h"
#include "../Utility/Application.h"
#include "../Utility/Sound/SoundManager.h"
#include "GameManager.h"
#include "Player.h"
#include <algorithm>
#include "DxLib.h"

namespace
{
	// 描画関連
	constexpr float kEnemyWidth				= 30.0f;	// 敵の実際の幅
	constexpr float kEnemyHeight			= 30.0f;	// 敵の実際の高さ

	constexpr float kEnemyCoinWidth			= 30.0f;	// 敵がコイン化した際の幅
	constexpr float kEnemyCoinHeight		= 30.0f;	// 敵がコイン化した際の高さ

	constexpr int	kEnemyCoinAddScore		= 1000;		// 敵コインを取得した際のスコア

	// 更新処理関連
	constexpr float kGravity				= 0.5f;		// 敵にかかる重力
	constexpr float kNormalMoveSpeed		= 0.9f;		// 通常時の左右移動の速さ
	constexpr float kSeekerMoveSpeed		= 0.8f;		// プレイヤーを追い続ける敵の移動の速さ

	constexpr int	kAppearTime				= 90;		// 敵の出現までの時間
	constexpr int	kFormChangeWaitTime		= 180;		// 敵の変身までの時間
	constexpr int	kFormChangeTime			= 30;		// 敵の変身準備までの時間
	constexpr int	kDeadTime				= 300;		// 敵がやられて消えるまでの時間
	constexpr float kDeadMoveSpeed			= 2.0f;		// 敵がやられた際の動くスピード

	constexpr float kMaxDirectionValue		= 1.0f;		// 向きの最大値(大きさ)
	constexpr float kDirectionMagnification = 0.01f;	// 向きの倍率

	constexpr int	kTurnMaxCount			= 2;        // 方向転換できる最大回数

	constexpr float kMapColMargin			= 0.1f;		// マップとの当たり判定の際のマージン
	constexpr float kFootTileMargin			= 0.5f;		// 自分の足元のタイルを見る際のマージン
	constexpr float kProbeYDist				= 1.0f;		// Y座標を探す距離

	// 演出関連
	constexpr float	kItemWarningRate		= 0.20f;	// アイテム化が終わりそうなことを示す時間の割合
	constexpr float kWarningFrashCycle		= 0.20f;	// 点滅の周期
	constexpr int	kMaxFadeRate			= 255;		// フェード率の最大値

	// アニメーション関連
	constexpr int	kGraphWidth				= 64;		// 敵画像1枚の幅
	constexpr int	kGraphHeight			= 64;		// 敵画像1枚の高さ
	constexpr float	kGraphScale				= 0.625;	// 敵画像の拡大率(サイズが40*40になる)
	constexpr float kCoinScale				= 0.25;		// コイン画像の拡大率(サイズが16*16になる)
}

TransformEnemy::TransformEnemy(const Position2& pos, Player* player, std::weak_ptr<Map> map, EnemyForm changeForm, std::unordered_map<std::string, Animation>& animations) :
	EnemyBase(player, map),
	m_updateFunc(&TransformEnemy::AppearUpdate),
	m_drawFunc(&TransformEnemy::AppearDraw),
	m_currentForm(EnemyForm::Normal),
	m_nextForm(changeForm),
	m_frameCount(0),
	m_moveCount(0),
	m_itemFormTime(0),
	m_maxItemFormTime(0),
	m_turnCount(0),
	m_prevPosY(0.0f),
	m_isGround(false),
	m_isRightDirection(true),
	m_isUpDirection(true),
	m_velocity{}
{
	m_direction = {};
	m_pos = pos;
	m_colRect = { m_pos,kEnemyWidth,kEnemyHeight };
	m_colCircle = { m_pos,kEnemyWidth / 2 };
	// アニメーションの設定
	m_animations = animations;

	// アニメーションの拡大率を設定
	SetAnimScale();

	ChangeAnimation(m_animations["Appear"]);

	// 敵が使うSEをロード確認
	Application::GetInstance().GetSoundManager()->LoadSoundClip("coinSE", L"data/sound/SE/coinSE.wav", SoundBus::SE, 1.0f, false);
	Application::GetInstance().GetSoundManager()->LoadSoundClip("enemyEntrySE", L"data/sound/SE/enemyEntry.wav", SoundBus::SE, 1.0f, false);
	Application::GetInstance().GetSoundManager()->LoadSoundClip("enemyAppearSE", L"data/sound/SE/enemyAppear.wav", SoundBus::SE, 1.0f, false);
	Application::GetInstance().GetSoundManager()->LoadSoundClip("seekerSE", L"data/sound/SE/enemySeeker.wav", SoundBus::SE, 1.0f, false);
	Application::GetInstance().GetSoundManager()->LoadSoundClip("fireBallSE", L"data/sound/SE/enemyFireBall.wav", SoundBus::SE, 1.0f, false);
	Application::GetInstance().GetSoundManager()->LoadSoundClip("skullSE", L"data/sound/SE/enemySkull.wav", SoundBus::SE, 1.0f, false);

	// 登場時のSEを鳴らす
	Application::GetInstance().GetSoundManager()->Play("enemyEntrySE", 1.0f, true);
}

void TransformEnemy::Init()
{
	m_colRect = { m_pos,kEnemyWidth,kEnemyHeight };
	m_colCircle = { m_pos,kEnemyWidth / 2 };
}

void TransformEnemy::Update(Input& input)
{
	if (IsItemMode())
	{
		// フレーム数の更新を行わない
	}
	else
	{
		m_frameCount++; // フレーム数は常に更新し続ける
	}

	CheckAnimation(); // アニメーションのチェック

	m_currentAnim.Update(); // アニメーションの更新

	(this->*m_updateFunc)(input);
}

void TransformEnemy::Draw()
{
	(this->*m_drawFunc)();
}

void TransformEnemy::ChangeToItem(int time)
{
	m_colRect = { m_pos,kEnemyCoinWidth,kEnemyCoinHeight };
	m_colCircle = { m_pos,kEnemyCoinWidth / 2 };
	m_updateFunc = &TransformEnemy::ItemUpdate;
	m_drawFunc = &TransformEnemy::ItemDraw;
	m_itemFormTime = time;
	m_maxItemFormTime = time;
	m_lastAnim = m_currentAnim; // 前のフォームのアニメーションを保存
	return; // 念のためreturn
}

void TransformEnemy::OnCollected(std::weak_ptr<GameManager> gameManager)
{
	// コイン取得時のSEを鳴らす
	Application::GetInstance().GetSoundManager()->Play("coinSE", 1.0f, true);
	auto manager = gameManager.lock();
	manager->AddScore(kEnemyCoinAddScore);
	manager->RequestCreateEffect(Types::EffectType::CoinGet, m_pos,true);
}

void TransformEnemy::IsCollision(const Types::CollisionInfo& info)
{
	if (info.otherType == Types::ActorType::Player) // プレイヤーに当たっている時
	{
		if (IsCanCollision()) // 当たり判定を行えるなら
		{
			// プレイヤーが攻撃可能状態なら
			if (m_pPlayer->IsAttackable())
			{
				// 敵が吹っ飛ぶ処理を行う予定
				m_updateFunc = &TransformEnemy::DeadUpdate;
			}
			else // プレイヤーが攻撃可能状態でないなら
			{
				// ミス状態にになっていない時
				if (!m_pPlayer->IsMiss())
				{
					m_pPlayer->MissStart(); // プレイヤーをミスにさせる
				}
			}
		}
		else if (IsItemMode()) // 敵がアイテム化している場合
		{
			m_isDead = true;
		}
	}
}

void TransformEnemy::AppearUpdate(Input&)
{
	// 一定時間たったら出現するようにする
	if (m_frameCount >= kAppearTime)
	{
		// 出現時のSEに変更
		Application::GetInstance().GetSoundManager()->Play("enemyAppearSE", 1.0f, true);
		// 敵の最初の状態の処理に変更
		m_updateFunc = &TransformEnemy::NormalUpdate;
		m_drawFunc = &TransformEnemy::NormalDraw;
		m_frameCount = 0; // フレームカウントをリセット
		return; // 念のためreturn
	}
}

void TransformEnemy::NormalUpdate(Input&)
{
	// 時間経過で敵の姿が変わる(今後別の条件を増やす予定あり)
	if (m_frameCount >= kFormChangeWaitTime)
	{
		// 変身時のSEを鳴らす(登場時と同じ)
		Application::GetInstance().GetSoundManager()->Play("enemyEntrySE", 1.0f, true);
		// 敵の姿を変える間の処理に変更
		m_updateFunc = &TransformEnemy::TransformUpdate;
		m_drawFunc = &TransformEnemy::TransformDraw;
		m_frameCount = 0; // フレームカウントをリセット
		return; // 念のためreturn
	}

	MoveOperation();

}

void TransformEnemy::TransformUpdate(Input&)
{
	// 少し待ってから姿が変わるようにしている
	if (m_frameCount >= kFormChangeTime)
	{
		// 最初に受け取った変身先によって処理を変更する
		switch (m_nextForm)
		{
		case EnemyForm::PlayerSeeker:
			// 変身時のSEを鳴らす
			Application::GetInstance().GetSoundManager()->Play("seekerSE", 1.0f, true);
			m_updateFunc = &TransformEnemy::SeekerUpdate;
			m_drawFunc = &TransformEnemy::SeekerDraw;
			m_currentForm = m_nextForm; // 現在の状態を更新
			break;
		case EnemyForm::FireBall:
			// 変身時のSEを鳴らす
			Application::GetInstance().GetSoundManager()->Play("fireBallSE", 1.0f, true);
			m_updateFunc = &TransformEnemy::FireBallUpdate;
			m_drawFunc = &TransformEnemy::FireBallDraw;
			m_currentForm = m_nextForm; // 現在の状態を更新
			break;
		case EnemyForm::Skull:
			// 変身時のSEを鳴らす
			Application::GetInstance().GetSoundManager()->Play("skullSE", 1.0f, true);
			m_updateFunc = &TransformEnemy::SkullUpdate;
			m_drawFunc = &TransformEnemy::SkullDraw;
			m_currentForm = m_nextForm; // 現在の状態を更新
			break;
		default:
			break;
		}

		m_frameCount = 0; // フレームカウントをリセット
		return; // 念のためreturn
	}
}

void TransformEnemy::SeekerUpdate(Input&)
{
	// 移動方向をプレイヤーに向かうようにする
	m_direction = m_pPlayer->GetPos() - m_pos;
	// プレイヤーからどちらの軸の座標のほうが遠いか計測
	if (fabsf(m_direction.x) > fabsf(m_direction.y))
	{
		// x軸のほうが遠い時
		m_direction.y = 0.0f;
	}
	else
	{
		// y軸のほうが遠い時
		m_direction.x = 0.0f;
	}
	// 向きを正規化する
	m_direction.Normalize();

	Position2 steer = m_direction * kSeekerMoveSpeed;
	TransformMoveOperation(steer);
	if ((m_pPlayer->GetPos().x - m_pos.x) > 0.0f)
	{
		m_isRightDirection = true;
	}
	else
	{
		m_isRightDirection = false;
	}
}

void TransformEnemy::FireBallUpdate(Input&)
{
	// プレイヤーの縦方向だけ追い続ける
	float toPlayerY = m_pPlayer->GetPos().y - m_pos.y; // 敵からプレイヤーに向かうベクトルのY座標
	float yComp = std::clamp(toPlayerY * kDirectionMagnification, -kMaxDirectionValue, kMaxDirectionValue); // -1から1までの間で向きを変える
	m_direction.x = m_isRightDirection ? kMaxDirectionValue : -kMaxDirectionValue;
	m_direction.y = yComp;
	m_direction.Normalize();

	Position2 steer = m_direction * kSeekerMoveSpeed;
	TransformMoveOperation(steer);
}

void TransformEnemy::SkullUpdate(Input&)
{
	// プレイヤーの横方向だけ追い続ける
	float toPlayerX = m_pPlayer->GetPos().x - m_pos.x; // 敵からプレイヤーに向かうベクトルのX座標
	float xComp = std::clamp(toPlayerX * kDirectionMagnification, -kMaxDirectionValue, kMaxDirectionValue); // -1から1までの間で向きを変える
	m_direction.y = kMaxDirectionValue;
	m_direction.x = xComp;
	m_direction.Normalize();

	Position2 steer = m_direction * kSeekerMoveSpeed;
	TransformMoveOperation(steer);

	if ((m_pPlayer->GetPos().x - m_pos.x) > 0.0f)
	{
		m_isRightDirection = true;
	}
	else
	{
		m_isRightDirection = false;
	}
}

void TransformEnemy::ItemUpdate(Input&)
{
	// 前回のフォームのアニメーションを更新
	m_lastAnim.Update();
	// アイテム化時間を減らしていって、時間に達したら
	if (--m_itemFormTime <= 0)
	{
		// 元の状態によって処理を変更する
		// ※出現や変身の途中でItemに遷移した際の処理をどうするか未定
		switch (m_currentForm)
		{
		case EnemyForm::Normal:
			m_updateFunc = &TransformEnemy::NormalUpdate;
			m_drawFunc = &TransformEnemy::NormalDraw;
			break;
		case EnemyForm::PlayerSeeker:
			m_updateFunc = &TransformEnemy::SeekerUpdate;
			m_drawFunc = &TransformEnemy::SeekerDraw;
			break;
		case EnemyForm::FireBall:
			m_updateFunc = &TransformEnemy::FireBallUpdate;
			m_drawFunc = &TransformEnemy::FireBallDraw;
			break;
		case EnemyForm::Skull:
			m_updateFunc = &TransformEnemy::SkullUpdate;
			m_drawFunc = &TransformEnemy::SkullDraw;
			break;
		default:
			break;
		}

		// 当たり判定を元に戻す
		m_colRect = { m_pos,kEnemyWidth,kEnemyHeight };
		m_colCircle = { m_pos,kEnemyWidth / 2 };

		m_itemFormTime = 0; // アイテム化時間を0にする
		m_maxItemFormTime = 0; // アイテム化最大時間を0にする
		return; // 念のためreturn
	}
}

void TransformEnemy::DeadUpdate(Input&)
{
	m_frameCount++;
	// 一定時間経ったら消える
	if (m_frameCount >= kDeadTime)
	{
		m_isDead = true; // 敵を消す
		return; // 念のためreturn
	}
	m_pos.y -= kDeadMoveSpeed; // 上に飛んでいく
}

void TransformEnemy::AppearDraw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);
	m_currentAnim.Draw({ drawX, drawY }, m_isRightDirection);

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void TransformEnemy::NormalDraw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);

	m_currentAnim.Draw({ drawX, drawY }, m_isRightDirection);

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void TransformEnemy::TransformDraw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);

	m_currentAnim.Draw({ drawX, drawY }, m_isRightDirection);
}

void TransformEnemy::SeekerDraw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);

	m_currentAnim.Draw({ drawX, drawY }, m_isRightDirection);

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void TransformEnemy::FireBallDraw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);

	m_currentAnim.Draw({ drawX, drawY }, m_isRightDirection);

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void TransformEnemy::SkullDraw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);

	m_currentAnim.Draw({ drawX, drawY }, m_isRightDirection);
	
#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void TransformEnemy::ItemDraw()
{
	auto camera = m_pCamera.lock();
	int drawX = static_cast<int>(m_pos.x - camera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - camera->scroll.y);

	// 残り時間が75%消費されたら
	if (m_itemFormTime < m_maxItemFormTime * kItemWarningRate)
	{
		// フェード率の計算 開始時: 0.0f  終了時: 1.0f
		auto sinRate = 1.0f - sinf(m_itemFormTime / (m_maxItemFormTime * kItemWarningRate * kWarningFrashCycle) * DX_PI_F * 2);
		//auto rate = static_cast<float>(m_itemFormTime) / (m_maxItemFormTime * kItemWarningRate) / 2;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * sinRate));
		m_lastAnim.Draw({ drawX, drawY }, m_isRightDirection);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		sinRate = sinf(m_itemFormTime / (m_maxItemFormTime * kItemWarningRate * kWarningFrashCycle) * DX_PI_F * 2);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(kMaxFadeRate * sinRate));
		m_currentAnim.Draw({ drawX, drawY }, m_isRightDirection);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else // それ以前なら
	{
		m_currentAnim.Draw({ drawX, drawY }, m_isRightDirection);
	}

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif

}

void TransformEnemy::SetAnimScale()
{
	m_animations["NormalWalk"].SetScale(kGraphScale);
	m_animations["NormalFall"].SetScale(kGraphScale);
	m_animations["Appear"].SetScale(kGraphScale);
	m_animations["Seeker"].SetScale(kGraphScale);
	m_animations["Skull"].SetScale(kGraphScale);
	m_animations["FireBall"].SetScale(kGraphScale);
	m_animations["Coin"].SetScale(kCoinScale);
}

void TransformEnemy::CheckAnimation()
{
	// 現在の更新処理に応じてアニメーションを変更する
	// 出現中、変身中は出現アニメーション
	if (m_updateFunc == &TransformEnemy::AppearUpdate || m_updateFunc == &TransformEnemy::TransformUpdate)
	{
		ChangeAnimation(m_animations["Appear"]);
	}
	// 通常状態の時
	else if (m_updateFunc == &TransformEnemy::NormalUpdate)
	{
		// 地面にいるかどうかでアニメーションを変更
		if (m_isGround)
		{
			ChangeAnimation(m_animations["NormalWalk"]);
		}
		else
		{
			ChangeAnimation(m_animations["NormalFall"]);
		}
	}
	else if (m_updateFunc == &TransformEnemy::SeekerUpdate)
	{
		ChangeAnimation(m_animations["Seeker"]);
	}
	else if (m_updateFunc == &TransformEnemy::SkullUpdate)
	{
		ChangeAnimation(m_animations["Skull"]);
	}
	else if (m_updateFunc == &TransformEnemy::FireBallUpdate)
	{
		ChangeAnimation(m_animations["FireBall"]);
	}
	else if (m_updateFunc == &TransformEnemy::ItemUpdate)
	{
		ChangeAnimation(m_animations["Coin"]);
	}
}

bool TransformEnemy::IsCanCollision() const
{
	// 出現中、変身中、アイテム化中、やられ中は当たり判定を行わない
	if (m_updateFunc == &TransformEnemy::AppearUpdate || m_updateFunc == &TransformEnemy::TransformUpdate || m_updateFunc == &TransformEnemy::ItemUpdate || m_updateFunc == &TransformEnemy::DeadUpdate)
	{
		return false;
	}
	// それ以外は当たり判定可能
	return true;
}

bool TransformEnemy::IsItemMode() const
{
	// 更新処理がアイテム状態の場合
	return m_updateFunc == &TransformEnemy::ItemUpdate;
}

bool TransformEnemy::IsFlipCorner() const
{
	// 方向転換回数が最大回数以下ならtrueを返す
	return m_turnCount <= kTurnMaxCount;
}

void TransformEnemy::MoveOperation()
{
	float dx = 0.0f; // X軸の移動量
	const bool movingLeft = !m_isRightDirection;
	const bool movingRight = m_isRightDirection;
	if (movingLeft)
	{
		dx -= kNormalMoveSpeed;
	}
	if (movingRight)
	{
		dx += kNormalMoveSpeed;
	}

	m_velocity.y += kGravity; // 重力をかける

	float dy = m_velocity.y; // Y軸の移動量
	ContactFrags frags;
	const float margin = kMapColMargin;

	// X軸の当たり判定
	Position2 tryPosX = m_pos;
	tryPosX.x += dx;

	Rect2D rectX(tryPosX, kEnemyWidth, kEnemyHeight - margin);
	
	// マップを使えるようにlockしておく
	auto pMap = m_pMap.lock();

	Rect2D rangeX = pMap->GetCanMoveRange(rectX);

	float leftX = tryPosX.x - kEnemyWidth * 0.5f;
	float rightX = tryPosX.x + kEnemyWidth * 0.5f;

	if (leftX < rangeX.GetLeft())
	{
		tryPosX.x = rangeX.GetLeft() + kEnemyWidth * 0.5f + margin;
		frags.isHitLeft = true;
		// 速度を抑える(問題があれば0にする)
		dx = tryPosX.x - m_pos.x;
	}
	else if (rightX > rangeX.GetRight())
	{
		tryPosX.x = rangeX.GetRight() - kEnemyWidth * 0.5f - margin;
		frags.isHitRight = true;
		// 速度を抑える(問題があれば0にする)
		dx = tryPosX.x - m_pos.x;
	}

	// 実際の座標に反映
	m_pos.x = tryPosX.x;

	// Y軸の当たり判定
	Position2 tryPosY = m_pos;
	tryPosY.y += dy;

	Rect2D rectY(tryPosY, kEnemyWidth, kEnemyHeight);
	Rect2D rangeY = pMap->GetCanMoveRange(rectY);

	float topY = tryPosY.y - kEnemyHeight * 0.5f;
	float bottomY = tryPosY.y + kEnemyHeight * 0.5f;

	// 貫通しない床に当たったかどうか
	bool isHitNormalFloor = false;

	// 落下している時に床に当たった場合
	if (dy > 0.0f && bottomY > rangeY.GetBottom())
	{
		tryPosY.y = rangeY.GetBottom() - kEnemyHeight * 0.5f;
		frags.isHitGround = true;
		isHitNormalFloor = true; // 貫通しない床に当たった
		m_velocity.y = 0.0f; // Y方向の速度を0にする
		dy = tryPosY.y - m_pos.y; // 移動量を修正
		m_moveCount = 0; // 時間経過をリセット
	}
	else if (dy < 0.0f && topY < rangeY.GetTop()) // 上昇している時に天井に当たった場合
	{
		tryPosY.y = rangeY.GetTop() + kEnemyHeight * 0.5f;
		frags.isHitCeil = true;
		m_velocity.y = 0.0f; // Y方向の速度を0にする
		dy = tryPosY.y - m_pos.y; // 移動量を修正
	}

	// 下からのみ貫通できる床との判定
	if (!isHitNormalFloor && dy > 0.0f) // 通常の地面との判定を行わなかった際に落下中なら
	{
		const float oldBottom = m_prevPosY + kEnemyHeight * 0.5f; // 前回の下端のY座標

		const float tileSize = pMap->GetTileSize(); // マップのタイルサイズを取得
		int leftTileX = pMap->WorldPosToMapPos(m_pos.x - kEnemyWidth * 0.5f, tileSize); // プレイヤーの左端のマップ座標X
		int rightTileX = pMap->WorldPosToMapPos(m_pos.x + kEnemyWidth * 0.5f, tileSize); // プレイヤーの右端のマップ座標X
		int footTileY = pMap->WorldPosToMapPos(bottomY + kFootTileMargin, tileSize); // プレイヤーの下端のマップ座標Y

		bool hitOneWay = false; // 片面通行の床に当たったかどうか
		float candidateTop = pMap->GetMapHeight() * tileSize; // 候補となる床の上端のY座標(仮)

		for (int x = leftTileX; x <= rightTileX; x++) // 敵の幅が大きい時を考慮
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
			tryPosY.y = candidateTop - kEnemyHeight * 0.5f;
			frags.isHitGround = true; // 床に当たったフラグを立てる
			m_velocity.y = 0.0f; // Y方向の速度を0にする
			dy = tryPosY.y - m_pos.y; // 移動量を修正
			m_moveCount = 0; // 時間経過をリセット
		}
	}


	// 実際の座標に反映
	m_pos.y = tryPosY.y;

	m_colCircle.pos = m_pos; // 円の座標更新
	m_colRect.pos = m_pos; // 矩形の座標更新
	m_prevPosY = m_pos.y; // 前回のY座標を更新

	// 状態の更新
	m_isGround = frags.isHitGround;

	// 方向転換の判定
	if (m_isGround && IsFlipCorner()) // 地面にいて、方向転換可能な場合
	{
		// 前方のX座標を見る
		const float aheadX = m_isRightDirection ? (m_pos.x + kEnemyWidth * 0.5f) : (m_pos.x - kEnemyWidth * 0.5f);
		const float aheadY = m_pos.y + kEnemyHeight * 0.5f; // 足元のY座標
		const float probeY = aheadY + kProbeYDist; // 足元より少し下のY座標
		const float tileSize = pMap->GetTileSize(); // マップチップのサイズ
		int tx = pMap->WorldPosToMapPos(aheadX, tileSize); // X座標のマップ位置
		int ty = pMap->WorldPosToMapPos(probeY, tileSize); // Y座標のマップ位置

		int chipNo = pMap->GetMapChipNum(tx, ty); // マップチップ番号を取得
		if (pMap->IsSpaceTile(chipNo)) // 前方のマップチップが空白の場合
		{
			// 足元に地面がないので方向転換
			m_isRightDirection = !m_isRightDirection;
			m_turnCount++; // 方向転換カウンタを増加
		}
	}

	// 横方向の壁衝突判定
	if(frags.isHitLeft || frags.isHitRight)
	{
		// 方向転換
		m_isRightDirection = !m_isRightDirection;
		m_turnCount++; // 方向転換カウンタを増加
	}
}

void TransformEnemy::TransformMoveOperation(const Position2& steer)
{
	float dx = steer.x; // X軸の移動量

	float dy = steer.y;
	m_velocity.y = 0.0f;

	
	ContactFrags frags;
	const float margin = kMapColMargin;

	// X軸の当たり判定
	Position2 tryPosX = m_pos;
	tryPosX.x += dx;

	Rect2D rectX(tryPosX, kEnemyWidth, kEnemyHeight);

	// マップを使えるようにlockしておく
	auto pMap = m_pMap.lock();

	Rect2D rangeX = pMap->GetCanMoveRange(rectX);

	float leftX = tryPosX.x - kEnemyWidth * 0.5f;
	float rightX = tryPosX.x + kEnemyWidth * 0.5f;

	if (leftX < rangeX.GetLeft())
	{
		tryPosX.x = rangeX.GetLeft() + kEnemyWidth * 0.5f + margin;
		frags.isHitLeft = true;
		
		dx = fabsf(dx);
	}
	else if (rightX > rangeX.GetRight())
	{
		tryPosX.x = rangeX.GetRight() - kEnemyWidth * 0.5f - margin;
		frags.isHitRight = true;
		
		dx = -fabsf(dx);
	}

	// 実際の座標に反映
	m_pos.x = tryPosX.x;

	// Y軸の当たり判定
	Position2 tryPosY = m_pos;
	tryPosY.y += dy;

	Rect2D rectY(tryPosY, kEnemyWidth, kEnemyHeight);
	Rect2D rangeY = pMap->GetCanMoveRange(rectY);

	float topY = tryPosY.y - kEnemyHeight * 0.5f;
	float bottomY = tryPosY.y + kEnemyHeight * 0.5f;

	// 貫通しない床に当たったかどうか
	bool isHitNormalFloor = false;

	// 落下している時に床に当たった場合
	if (dy > 0.0f && bottomY > rangeY.GetBottom())
	{
		tryPosY.y = rangeY.GetBottom() - kEnemyHeight * 0.5f;
		frags.isHitGround = true;
		m_velocity.y = 0.0f; // Y方向の速度を0にする
		isHitNormalFloor = true; // 貫通しない床に当たった
		dy = tryPosY.y - m_pos.y; // 移動量を修正
		m_moveCount = 0; // 時間経過をリセット
	}
	else if (dy < 0.0f && topY < rangeY.GetTop()) // 上昇している時に天井に当たった場合
	{
		tryPosY.y = rangeY.GetTop() + kEnemyHeight * 0.5f;
		frags.isHitCeil = true;
		m_velocity.y = 0.0f; // Y方向の速度を0にする
		dy = tryPosY.y - m_pos.y; // 移動量を修正
	}

	// 下からのみ貫通できる床との判定
	if (!isHitNormalFloor && dy > 0.0f) // 通常の地面との判定を行わなかった際に落下中なら
	{
		const float oldBottom = m_prevPosY + kEnemyHeight * 0.5f; // 前回の下端のY座標

		const float tileSize = pMap->GetTileSize(); // マップのタイルサイズを取得
		int leftTileX = pMap->WorldPosToMapPos(m_pos.x - kEnemyWidth * 0.5f, tileSize); // プレイヤーの左端のマップ座標X
		int rightTileX = pMap->WorldPosToMapPos(m_pos.x + kEnemyWidth * 0.5f, tileSize); // プレイヤーの右端のマップ座標X
		int footTileY = pMap->WorldPosToMapPos(bottomY + kFootTileMargin, tileSize); // プレイヤーの下端のマップ座標Y

		bool hitOneWay = false; // 片面通行の床に当たったかどうか
		float candidateTop = pMap->GetMapHeight() * tileSize; // 候補となる床の上端のY座標(仮)

		for (int x = leftTileX; x <= rightTileX; x++) // 敵の幅が大きい時を考慮
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
			tryPosY.y = candidateTop - kEnemyHeight * 0.5f;
			frags.isHitGround = true; // 床に当たったフラグを立てる
			m_velocity.y = 0.0f; // Y方向の速度を0にする
			dy = tryPosY.y - m_pos.y; // 移動量を修正
			m_moveCount = 0; // 時間経過をリセット
		}
	}

	// 実際の座標に反映
	m_pos.y = tryPosY.y;

	m_colCircle.pos = m_pos; // 円の座標更新
	m_colRect.pos = m_pos; // 矩形の座標更新
	m_prevPosY = m_pos.y; // 前フレームのY座標更新

	// 状態の更新
	m_isGround = frags.isHitGround;

	// 横方向の壁衝突判定
	if (frags.isHitLeft || frags.isHitRight)
	{
		// 方向転換
		m_isRightDirection = !m_isRightDirection;
		m_turnCount++; // 方向転換カウンタを増加
	}

}
