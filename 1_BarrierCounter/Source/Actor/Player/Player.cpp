#include "Player.h"
#include "../Model.h"
#include "../Enemy/EnemyBase.h"
#include "../../Geometry/Vector3.h"
#include "../../Geometry/Vector2.h"
#include "../../Geometry/Matrix4x4.h"
#include "../../Effect/EffectManager.h"
#include "../../Effect/Effect.h"
#include "DxLib.h"
#include "../../MyLib/MyLib.h"
#include "../../Common/Input.h"
#include "../../Sound/SoundManager.h"
#include "../../Common/UI/UIManager.h"
#include "../../Common/UI/UIImage.h"
#include <cmath>

namespace
{
	// 初期座標(当たり判定含む)
	const Vector3 kFirstPos = { 0.0f,100.0f,-3000.0f };
	// プレイヤーのモデルのオフセット
	const Vector3 kModelOffset = { 0.0f, 100.0f,0.0f };

	// モデルの最初の向き
	const Vector3 kFirstModelDir = { 0.0f,0.0f,-1.0f };

	// プレイヤーの速度
	constexpr float kDefaultSpeed = 20.0f;
	// プレイヤーの当たり判定の半径
	constexpr float kDefaultRadius = 100.0f;

	constexpr int kWalkAnimNum = 11; // 移動アニメーション番号
	constexpr int kAvoidAnimNum = 7; // 回避アニメーション番号

	// アニメーションのブレンド時間
	constexpr int kAnimBlendTime = 20;

	// 無敵時間
	constexpr int kInvincibleTime = 120;
	// 無敵時間の点滅の周期
	constexpr int kFlashCycle = 7;

	// 回避時間
	constexpr int kAvoidTime = 30;

	// ノックバック速度
	constexpr float kKnockBackSpeed = 7.0f;

	// 最大体力
	constexpr int kMaxLife = 3;

	// 体力UIの左端位置
	const Vector2Int kLifeUILeft = { 60, 60 };
	// 体力UIのスケール
	const Vector2 kLifeUIScale = { 0.3f,0.3f };
	// 体力UI同士の間隔
	constexpr int kLifeUIMargin = 100;
	// 体力UIの拡縮範囲
	constexpr float kLifeUIShrinkScale = 0.05f;
	// 体力UIの拡縮スピード
	constexpr float kLifeUIShrinkSpeed = 0.1f;

	// バリアを表示する際のオフセット
	const Vector3 kBarrierOffset = { 0.0f, 100.0f,0.0f };
}

Player::Player() :
	Actor(ObjectTag::Player, MyLib::ColliderBase::ColliderType::Sphere),
	m_modelHandle(-1),
	m_isMoving(false),
	m_isDamage(false),
	m_isInvincible(false),
	m_isAvoid(false),
	m_invincibleTime(0),
	m_avoidTime(0),
	m_life(0),
	m_lifeUIAnimCount(0),
	m_update(&Player::IdleUpdate)
{
	// 球の当たり判定を取得
	auto collider = dynamic_pointer_cast<MyLib::SphereCollider>(m_pColliderData);
	// 球の当たり判定の半径を設定
	collider->m_radius = kDefaultRadius;
}

Player::~Player()
{
}

void Player::Init()
{
	// Collidableの初期化処理を呼ぶ
	Collidable::Init();

	m_modelHandle = MV1LoadModel(L"Data/Model/player.mv1");

	m_model = std::make_shared<Model>();

	m_model->SetModelHandle(MV1DuplicateModel(m_modelHandle));

	m_model->Init();
	m_model->SetAnimation(3, 0.5f, 0, true);

	m_rigidbody.SetPos(kFirstPos);

	// モデルの座標をセット
	m_model->SetPos(kFirstPos - kModelOffset);

	// 最初の向きを設定
	m_lastMoveDir = kFirstModelDir;

	// 体力の初期化
	m_life = kMaxLife;

	// 体力UIのアニメーション時間を初期化
	m_lifeUIAnimCount = 0;

	// 初期状態は待機状態にする
	m_update = &Player::IdleUpdate;

	// サウンドを登録
	auto& soundManager = SoundManager::GetInstance();
	soundManager.LoadSoundClip("Walk", L"Data/Sound/SE/walk.mp3", SoundBus::SE, 1.0f, true); // 歩き時のSE
	soundManager.LoadSoundClip("Avoid", L"Data/Sound/SE/avoid.mp3", SoundBus::SE, 1.0f, false); // 回避時のSE
	soundManager.LoadSoundClip("Damage", L"Data/Sound/SE/damage.mp3", SoundBus::SE, 1.0f, false); // ダメージ時のSE

	// エフェクトマネージャーのインスタンスを取得
	auto& effectManager = EffectManager::GetInstance();
	// エフェクトを登録
	effectManager.LoadEffect(L"avoid_1.efk");
	effectManager.LoadEffect(L"hit.efk");
	
	// UI管理クラスのインスタンスを取得
	auto& uiManager = UIManager::GetInstance();
	// UI画像をロード
	uiManager.LoadGraphHandle(L"life_full.png");
	uiManager.LoadGraphHandle(L"life_empty.png");

	// UIを生成
	for (int i = 0; i < kMaxLife; i++)
	{
		// 初期座標をセット
		Vector2Int pos = { kLifeUILeft.x + kLifeUIMargin * i, kLifeUILeft.y };

		// 体力UIのImageを作成
		auto lifeUI = uiManager.CreateImage(pos, L"life_full.png");
		// 体力UIが取得できたら初期状態の設定をする
		if (auto sharedLife = lifeUI.lock())
		{
			// 初期スケールの設定をする
			sharedLife->SetScale(kLifeUIScale);
			// レイヤーの設定をする
			sharedLife->SetLayer(UIBase::Layer::InGame);
		}
		// 体力UIリストにpush_backしておく
		m_pLifeUI.push_back(lifeUI);
	}
}

void Player::End()
{
	// Collidableの終了処理を呼ぶ
	Collidable::End();

	m_model->End();
	MV1DeleteModel(m_modelHandle);

	// 登録したサウンドを開放
	auto& soundManager = SoundManager::GetInstance();
	soundManager.DeleteSoundClip("Walk");
	soundManager.DeleteSoundClip("Avoid");
	soundManager.DeleteSoundClip("Damage");

	// エフェクトマネージャーのインスタンスを取得
	auto& effectManager = EffectManager::GetInstance();
	// 登録したエフェクトを開放
	effectManager.DeleteEffect(L"avoid_1.efk");
	effectManager.DeleteEffect(L"hit.efk");

	// UIマネージャーのインスタンスを取得
	auto& uiManager = UIManager::GetInstance();
	// 登録したUI画像を開放
	uiManager.DeleteGraphHandle(L"life_full.png");
	uiManager.DeleteGraphHandle(L"life_empty.png");
}

void Player::Update()
{
	// 体力のアニメーションのカウントを進める
	m_lifeUIAnimCount++;

	// モデルの点滅用(今後消す可能性あり)
	bool isModelEnable = true;

	// 無敵状態の場合
	if (m_isInvincible)
	{
		// 無敵時間を減少
		m_invincibleTime--;

		if (m_invincibleTime % kFlashCycle == 0)
		{
			isModelEnable = !isModelEnable;
		}

		m_model->SetEnable(isModelEnable);

		// 無敵時間が0以下になったら
		if (m_invincibleTime <= 0)
		{
			m_invincibleTime = 0;
			m_isInvincible = false;
			m_model->SetEnable(true);
		}
	}

	// コントローラーの入力を取得
	auto& input = Input::GetInstance();

	// スティックの入力を取得
	auto stick = input.GetStickData();

	if (stick.leftStick.Length() != 0.0f)
	{
		m_isMoving = true;
	}
	else
	{
		m_isMoving = false;
	}

	// 体力UIの参照がある場合
	if (!m_pLifeUI.empty())
	{
		// アニメーション対象の体力UIを取得
		if (auto animLife = m_pLifeUI.back().lock())
		{
			animLife->SetScale(Vector2(kLifeUIScale.x + kLifeUIShrinkScale * sinf(m_lifeUIAnimCount * kLifeUIShrinkSpeed), kLifeUIScale.y + kLifeUIShrinkScale * sinf(m_lifeUIAnimCount * kLifeUIShrinkSpeed)));
		}
	}

	// 回避中ではない時に回避ボタンが押されたら
	if (input.IsTriggered("Avoid") && !m_isAvoid && !m_isDamage)
	{
		// エフェクトをすでに持っている場合
		if (auto effect = m_pEffect.lock())
		{
			// 現在のエフェクトを止める
			effect->StopEffect();
		}
		m_pEffect = EffectManager::GetInstance().CreateEffect(L"avoid_1.efk", m_rigidbody.GetPos());
		// 回避SEを再生
		SoundManager::GetInstance().Play("Avoid", 1.0f, true);
		// 回避スタート
		m_model->SetAnimation(kAvoidAnimNum, 1.5f, kAnimBlendTime, false);
		m_isAvoid = true;
		m_avoidTime = kAvoidTime;
		// 回避処理に移行
		m_update = &Player::AvoidUpdate;
	}

	// 体力が0になった場合
	if (m_life <= 0)
	{
		// 死亡処理に移行
		m_update = &Player::DeadUpdate;
	}


	// ステートごとの更新処理を行う
	(this->*m_update)();

	// モデルの更新
	UpdateModel();
}

void Player::Draw() const
{
	m_model->Draw();
}

void Player::OnCollide(std::shared_ptr<Collidable> collider)
{
	// 無敵状態、回避状態ではない場合
	if (!m_isInvincible && !m_isAvoid)
	{
		// 敵を取得する
		if (auto enemy = dynamic_pointer_cast<EnemyBase>(collider))
		{
			// 敵が通常の状態なら
			if (enemy->IsAlive())
			{
				// ダメージを受けたとする
				m_isDamage = true;
			}
			return; // 敵との判定の場合ここで処理を抜ける
		}

		// 敵の弾と当たった場合はすでにダメージを受けていなければヒットエフェクトを生成
		if (collider->GetTag() == ObjectTag::EnemyBullet && !m_isDamage)
		{
			m_pEffect = EffectManager::GetInstance().CreateEffect(L"hit.efk", m_rigidbody.GetPos());
		}

		// ダメージを受けたとする
		m_isDamage = true;
	}
}

void Player::GetCameraDir(const Vector3 dir)
{
	m_cameraDir = dir;
}

bool Player::IsDamage() const
{
	return m_isDamage;
}

bool Player::IsAvoid() const
{
	return m_update == &Player::AvoidUpdate;
}

bool Player::IsDead() const
{
	return m_update == &Player::DeadUpdate;
}

bool Player::IsDied() const
{
	return m_update == &Player::DieUpdate;
}

void Player::IdleUpdate()
{
	// 移動速度を0にする
	m_rigidbody.SetVelocity(Vector3::Zero());

	// コントローラーの入力を取得
	auto& input = Input::GetInstance();

	// スティックの入力を取得
	auto stick = input.GetStickData();

	// ダメージを受けている場合
	if (m_isDamage)
	{
		// ダメージ処理を行う
		OnDamage();
		return;
	}

	// スティックの入力がある場合
	if (m_isMoving)
	{
		// 歩きSEを再生
		SoundManager::GetInstance().Play("Walk", 1.0f, true);
		m_model->SetAnimation(kWalkAnimNum, 0.5f, kAnimBlendTime, true);
		// 移動時の更新処理に変更
		m_update = &Player::WalkUpdate;
	}

}

void Player::WalkUpdate()
{
	// スティックの入力を取得
	auto stick = Input::GetInstance().GetStickData();

	// コントローラーの入力を取得して移動できるようにする
	Vector3 moveDir = m_cameraDir;
	// Y方向を初期化
	moveDir.y = 0;

	// 正規化して正面方向を出す
	Vector3 forward = moveDir.Normalized();
	m_forward = forward;
	// 外積を使ってmoveDirを正面とした右方向のベクトルを出す
	Vector3 right = Vector3::Cross(Vector3::Up(), forward);
	m_right = right;
	// 右方向のベクトルを正規化
	right.Normalize();

	// 正面入力の作成
	Vector3 inputForward = forward * stick.leftStick.y * kDefaultSpeed;

	// 左右入力の作成
	Vector3 inputSide = right * stick.leftStick.x * kDefaultSpeed;

	// 入力の合成
	Vector3 input = inputForward + inputSide;

	// 速度ベクトルにセット
	m_rigidbody.SetVelocity(input);

	if (input.Length() != 0.0f)
	{
		// 動いている間は向きを取得
		m_lastMoveDir = input.Normalized();
	}

	// ダメージを受けている場合
	if (m_isDamage)
	{
		// ダメージ処理を行う
		OnDamage();
		return;
	}

	// 移動入力されていない場合
	if (!m_isMoving)
	{
		// 歩きSEを停止
		SoundManager::GetInstance().Stop("Walk");
		// 待機状態に変更
		m_model->SetAnimation(3, 0.5f, kAnimBlendTime, true);
		m_update = &Player::IdleUpdate;
	}
}

void Player::DamageUpdate()
{
	// 現在の方向から逆方向に移動
	m_rigidbody.SetVelocity(m_knockBack * kKnockBackSpeed);

	// アニメーションが終了したとき
	if (m_model->GetAnimationEnd())
	{
		m_isDamage = false;
		m_isInvincible = true;
		m_invincibleTime = kInvincibleTime;
		if (m_isMoving)
		{
			// 歩きSEを再生
			SoundManager::GetInstance().Play("Walk", 1.0f, true);
			m_model->SetAnimation(kWalkAnimNum, 0.5f, kAnimBlendTime, true);
			m_update = &Player::WalkUpdate;
		}
		else
		{
			m_model->SetAnimation(3, 0.5f, kAnimBlendTime, true);
			m_update = &Player::IdleUpdate;
		}
	}
}

void Player::DeadUpdate()
{
	// その場から動かない
	m_rigidbody.SetVelocity(Vector3::Zero());

	// 死亡アニメーションが終わったら
	if (m_model->GetAnimationEnd())
	{
		m_update = &Player::DieUpdate;
	}
}

void Player::DieUpdate()
{
	// その場から動かない
	m_rigidbody.SetVelocity(Vector3::Zero());
}

void Player::AvoidUpdate()
{
	// その場から動かない
	m_rigidbody.SetVelocity(Vector3::Zero());

	// 回避時間を減少
	m_avoidTime--;

	// 回避時間が0になったら
	if (m_avoidTime <= 0)
	{
		m_avoidTime = 0;
		m_isAvoid = false;
		
		// ダメージを受けている場合
		if (m_isDamage)
		{
			// ダメージ処理を行う
			OnDamage();
			return;
		}

		if (m_isMoving)
		{
			// 歩きSEを再生
			SoundManager::GetInstance().Play("Walk", 1.0f, true);
			m_model->SetAnimation(kWalkAnimNum, 0.5f, kAnimBlendTime, true);
			m_update = &Player::WalkUpdate;
		}
		else
		{
			m_model->SetAnimation(3, 0.5f, kAnimBlendTime, true);
			m_update = &Player::IdleUpdate;
		}
	}
}

void Player::UpdateModel()
{
	// モデルの更新処理を行う
	m_model->Update();

	// モデルの座標をセット
	m_model->SetPos(m_rigidbody.GetPos() - kModelOffset);

	// 最後に動いた時の向きにする
	m_model->SetDir(m_lastMoveDir);

	if (auto effect = m_pEffect.lock())
	{
		effect->SetPos(m_rigidbody.GetPos() + kBarrierOffset);
	}
}

void Player::OnDamage()
{
	// ダメージSEを再生
	SoundManager::GetInstance().Play("Damage", 1.0f, true);
	m_model->SetAnimation(0, 0.5f, kAnimBlendTime, false);
	// あらかじめ現在の方向を取得しておく
	m_knockBack = m_rigidbody.GetDir();
	// 現在の方向から反転させる
	m_knockBack *= -1;
	m_update = &Player::DamageUpdate;
	// 体力を減らす
	m_life--;

	// 体力が0以上の場合
	if (m_life >= 0)
	{
		// 体力UIの右端を取得
		if (auto lifeUI = m_pLifeUI.back().lock())
		{
			// 体力画像を空のものにする
			lifeUI->SetHandle(UIManager::GetInstance().GetGraphHandle(L"life_empty.png"));
			// スケールを初期のものにする
			lifeUI->SetScale(kLifeUIScale);
			// 参照から削除
			m_pLifeUI.pop_back();
		}
	}

}
