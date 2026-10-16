#include "PlayerController.h"
#include "../../../State/Player/PlayerStateIdle.h"
#include "../../../State/Player/PlayerStateFreeze.h"
#include "../../../State/Player/PlayerStateDamage.h"
#include "../../../State/Player/PlayerStateDash.h"
#include "../../../State/Player/PlayerStateGuard.h"
#include "../../../State/Player/PlayerStateWalk.h"
#include "../../../State/Player/PlayerStateRun.h"
#include "../../../State/Player/PlayerStateAttack.h"
#include "../../../State/Player/PlayerStateDead.h"
#include "../../../State/Player/PlayerStateDie.h"
#include "../../../../Geometry/Vector3.h"
#include "../../Transform.h"
#include "../../EffectComponent.h"
#include "../../../MyMath.h"
#include "../../Draw/Drawable3D.h"
#include "../../Animator.h"
#include "../../../GameObject.h"
#include "../../Collision/Collidable.h"
#include "../../../Collider/SphereCollider.h"
#include "../../../Collider/CapsuleCollider.h"
#include "../../../Collider/BoxCollider.h"
#include "../../../../Utility/File/FileManager.h"
#include "../../../../Utility/File/File.h"
#include "../../../../Utility/Input.h"
#include "../../../../Common/Effect/EffectManager.h"
#include "../../../../Common/Sound/SoundManager.h"
#include <vector>

namespace
{
	// 初期位置
	const Vector3 kFirstPos = { 0.0f, 1.0f, 0.0f };

	// モデルのサイズ
	const Vector3 kModelScale = { 0.01f, 0.01f, 0.01f };

	// 剣モデルのサイズ
	const Vector3 kSwordModelScale = { 0.6f, 0.6f, 0.6f };
	
	// 盾モデルのサイズ
	const Vector3 kShieldModelScale = { 0.4f, 0.4f, 0.4f };
	
	// モデルの表示オフセット
	const Vector3 kModelOffset = { 0.0f, -1.0f, 0.0f };

	// 剣の表示オフセット
	const Vector3 kSwordOffset = { 0.0f, 40.0f, 0.0f };
	
	// 盾の表示オフセット
	const Vector3 kShieldOffset = { 10.0f, -30.0f, -15.0f };
	
	// 盾を構えているときの表示オフセット
	const Vector3 kShieldGuardOffset = { 0.0f, 0.0f, -10.0f };
	
	// 攻撃の当たり判定のオフセット
	const Vector3 kAttackColOffset = { 0.0f, 0.0f, -1.0f };

	// ガードの当たり判定のオフセット
	const Vector3 kGuardColOffset = { -0.25f, 0.0f, -0.5f };

	// プレイヤー自身の当たり判定の半径
	constexpr float kPlayerColRadius = 0.25f;

	// 攻撃の当たり判定の半径
	constexpr float kAttackColRadius = 1.0f;

	// ガードの当たり判定の半径
	constexpr float kGuardColRadius = 0.35f;

	// 剣の回転
	// 剣を持っているようにするための回転角度
	constexpr float kSwordGripRadian = MyLib::ToRadian(270.0f);
	// 剣の向きを変える回転角度
	constexpr float kSwordOffsetRadian = MyLib::ToRadian(180.0f);
	
	// 盾の回転
	// 盾を構えているようにするための回転角度
	constexpr float kShieldAttachRadian = MyLib::ToRadian(30.0f);
	// 盾の向きを変える回転角度
	constexpr float kShieldOffsetRadian = MyLib::ToRadian(180.0f);

	// 盾(実際に構えているとき)の回転
	constexpr float kShieldGuardAttachRadian = MyLib::ToRadian(90.0f);

	constexpr float kShieldGuardOffsetRadian = MyLib::ToRadian(180.0f);

	constexpr float kLockOnTurnSpeed = 0.2f;

	constexpr float kLockOnFaceMinDist = 0.01f;

	// 無敵時間
	constexpr int kInvincibleFrame = 120;
	// 無敵時間中のモデルの点滅周期
	constexpr int kFlashCycle = 4;

	// 体力
	constexpr int kPlayerMaxHP = 5;

	// 先行入力を保存するフレーム数
	constexpr int kInputBufferFrame = 5;

	// ジャストガードになるフレーム数
	constexpr int kJustGuardFrame = 10;

	std::vector<std::string> kBufferName = { "AButton", "BButton", "Guard" };
}

MyLib::PlayerController::PlayerController() : 
	m_invincibleFrame(0),
	m_guardingFrame(0),
	m_hp(kPlayerMaxHP),
	m_isGuard(false)
{
}

MyLib::PlayerController::~PlayerController()
{
}

void MyLib::PlayerController::Init(std::weak_ptr<MyLib::GameObject> parent)
{
	m_stateMachine.Init(this);

	// 最初の状態を待機状態にする
	m_stateMachine.ChangeState<MyLib::PlayerStateIdle>();

	// 親オブジェクトを取得
	std::shared_ptr<MyLib::GameObject> pParent = parent.lock();

	m_pTransform = pParent->GetComponent<MyLib::Transform>();	// Transformコンポーネントの弱参照を取得

	m_pDrawable3D = pParent->GetComponent<MyLib::Drawable3D>();	// Drawable3Dコンポーネントの弱参照を取得

	m_pRigidbody = pParent->GetComponent<MyLib::Rigidbody>();	// Rigidbodyコンポーネントの弱参照を取得

	m_pAnimator = pParent->GetComponent<MyLib::Animator>();		// Animatorコンポーネントの弱参照を取得

	m_pEffectComponent = pParent->GetComponent<MyLib::EffectComponent>();	// エフェクトコンポーネントの弱参照を取得

	std::shared_ptr<MyLib::Transform> pTransform = pParent->GetComponent<MyLib::Transform>().lock();	// Transformコンポーネントを取得
	std::shared_ptr<MyLib::Drawable3D> pDrawable = pParent->GetComponent<MyLib::Drawable3D>().lock();	// Drawable3Dコンポーネントを取得
	std::shared_ptr<MyLib::Rigidbody> pRigidbody = pParent->GetComponent<MyLib::Rigidbody>().lock();	// Rigidbodyコンポーネントを取得
	std::shared_ptr<MyLib::Collidable> pCollidable = pParent->GetComponent<MyLib::Collidable>().lock();	// Collidableコンポーネントを取得

	// モデルのスケールを設定する
	pTransform->SetScale(kModelScale);

	// モデルの初期位置を設定する
	pTransform->SetPos(kFirstPos);

	FileManager& fileManager = FileManager::GetInstance();
	std::shared_ptr<File> playerModelH = fileManager.GetModel(L"Data/File/Model/player.mv1", false);

	std::shared_ptr<File> swordModelH = fileManager.GetModel(L"Data/File/Model/sword.mv1", false);
	
	std::shared_ptr<File> shieldModelH = fileManager.GetModel(L"Data/File/Model/shield.mv1", false);

	// モデルをセットする
	std::shared_ptr<Model> bodyModel = pDrawable->AddModel(Model::ModelSlot::Main, playerModelH->GetHandle());
	std::shared_ptr<Model> swordModel = pDrawable->AddModel(Model::ModelSlot::Weapon, swordModelH->GetHandle());
	std::shared_ptr<Model> shieldModel = pDrawable->AddModel(Model::ModelSlot::Shield, shieldModelH->GetHandle());
	//bodyModel->SetEnable(false);
	// -----モデル自体のオフセットを設定する-----
	m_pModelOffset = std::make_shared<MyLib::Transform>();
	m_pModelOffset->SetPos(kModelOffset);

	// モデルのオフセットを適用
	pDrawable->SetModelOffset(Model::ModelSlot::Main, m_pModelOffset);
	// -----モデル自体のオフセットを設定する-----


	// -----剣のオフセットを設定する-----
	int handFrame = MV1SearchFrame(bodyModel->GetModelHandle(), L"mixamorig:RightHandMiddle3");
	m_pSwordOffset = std::make_shared<MyLib::Transform>();

	// 剣の向きをプレイヤーが持っているようにするための回転
	Quaternion attachRotation = Quaternion::AngleAxis(kSwordGripRadian, Vector3::Forward());
	// 剣の回転(デフォルトの向きだと剣の持ち手と切る部分が逆になっているため180度回転)
	Quaternion offsetRotation = Quaternion::AngleAxis(kSwordOffsetRadian, Vector3::Right());

	Quaternion swordOffset = offsetRotation * attachRotation;
	swordOffset.Normalize();
	m_pSwordOffset->SetPos(kSwordOffset);
	m_pSwordOffset->SetScale(kSwordModelScale);
	m_pSwordOffset->SetRotation(swordOffset);
	
	pDrawable->SetModelOffset(Model::ModelSlot::Weapon, m_pSwordOffset);
	pDrawable->SetAnchor(Model::ModelSlot::Weapon, [bodyModel, handFrame]()
		{
			return Matrix4x4::ToMatrix(MV1GetFrameLocalWorldMatrix(bodyModel->GetModelHandle(), handFrame));
		});
	// -----剣のオフセットを設定する-----

	// -----盾のオフセットを設定する-----
	int neckFrame = MV1SearchFrame(bodyModel->GetModelHandle(), L"mixamorig:Neck");
	m_pShieldOffset = std::make_shared<MyLib::Transform>();

	// 盾の向きを背中につけているようにするため少し斜めにする
	attachRotation = Quaternion::AngleAxis(kShieldAttachRadian, Vector3::Back());
	// 盾の回転(デフォルトの向きだと持つ部分と盾の向きが逆になっているため180度回転)
	offsetRotation = Quaternion::AngleAxis(kShieldOffsetRadian, Vector3::Up());
	Quaternion shieldOffset = offsetRotation * attachRotation;
	m_pShieldOffset->SetPos(kShieldOffset);
	m_pShieldOffset->SetScale(kShieldModelScale);
	m_pShieldOffset->SetRotation(shieldOffset);

	pDrawable->SetModelOffset(Model::ModelSlot::Shield, m_pShieldOffset);
	pDrawable->SetAnchor(Model::ModelSlot::Shield, [bodyModel, neckFrame]()
		{
			return Matrix4x4::ToMatrix(MV1GetFrameLocalWorldMatrix(bodyModel->GetModelHandle(), neckFrame));
		});

	// -----盾のオフセットを設定する-----

	// プレイヤーで使用するサウンドをロード
	auto& soundManager = SoundManager::GetInstance();
	soundManager.LoadSoundClip("Hit", L"Data/File/Sound/SE/player_hit.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Rolling", L"Data/File/Sound/SE/rolling.mp3", SoundBus::SE, 1.0f, false);
	soundManager.LoadSoundClip("Walk", L"Data/File/Sound/SE/walk.mp3", SoundBus::SE, 1.0f, true);
	soundManager.LoadSoundClip("Run", L"Data/File/Sound/SE/run.mp3", SoundBus::SE, 1.0f, true);
	
	// 重力をかける
	pRigidbody->SetIsGravity(true);

	// -----当たり判定関連-----
	//pCollidable->AddCollider(std::make_shared<MyLib::CapsuleCollider>(MyLib::ColliderBase::ObjectTag::Player, 0.5f, 1.0f, Vector3::Zero(), false));
	pCollidable->AddCollider(std::make_shared<MyLib::SphereCollider>(MyLib::ColliderBase::ObjectTag::Player, kPlayerColRadius, false));
	// 攻撃の当たり判定を追加
	auto attackCol = std::make_shared<MyLib::SphereCollider>(MyLib::ColliderBase::ObjectTag::PlayerAttach, kAttackColRadius, true, "PlayerAttack");
	attackCol->SetLocalOffset(kAttackColOffset);
	attackCol->SetEnable(false);
	m_pAttackCollider = attackCol;
	pCollidable->AddCollider(attackCol);

	// ガードの当たり判定を追加
	auto guardCol = std::make_shared<MyLib::SphereCollider>(MyLib::ColliderBase::ObjectTag::PlayerAttach, kGuardColRadius, true, "PlayerGuard");
	guardCol->SetLocalOffset(kGuardColOffset);
	guardCol->SetEnable(false);
	m_pGuardCollider = guardCol;
	pCollidable->AddCollider(guardCol);

	guardCol->SetContext([this]() -> MyLib::ColliderBase::HitContext
		{
			MyLib::ColliderBase::HitContext context;
			context.isJustGuard = IsJustGuard();
			return context;
		});

	pCollidable->SetOnCollide([&](const MyLib::CollisionInfo& info)
		{
			if (m_stateMachine.IsCheckState<MyLib::PlayerStateFreeze>() || m_hp <= 0) return;

			// 無敵時間ではない状態でプレイヤーの本体に当たった場合
			if (info.myCollider->GetTag() != MyLib::ColliderBase::ObjectTag::PlayerAttach && !IsInvincible() && !IsGuarding())
			{
				if (info.otherCollider->GetName() == "EnemyAttack")
				{
					auto context = info.otherCollider->GetHitContext();
					// スタン状態でない場合はダメージを受ける
					if (!context.isStunned)
					{
						m_stateMachine.ChangeState<MyLib::PlayerStateDamage>();
						m_invincibleFrame = kInvincibleFrame;
						soundManager.Play("Hit", 1.0f, true);
						m_hp--;
					}
					return;
				}
			}
		});

	pCollidable->SetOnCollideEnter([&](const MyLib::CollisionInfo& info)
		{
			if (m_stateMachine.IsCheckState<MyLib::PlayerStateFreeze>() || m_hp <= 0) return;

			// 無敵時間ではない状態でプレイヤーの本体に当たった場合
			if (info.myCollider->GetTag() != MyLib::ColliderBase::ObjectTag::PlayerAttach && !IsInvincible())
			{
				if (info.otherCollider->GetTag() == MyLib::ColliderBase::ObjectTag::EnemyBullet && !(info.otherCollider->GetName() == "ReflectBullet"))
				{
					m_stateMachine.ChangeState<MyLib::PlayerStateDamage>();
					m_invincibleFrame = kInvincibleFrame;
					soundManager.Play("Hit", 1.0f, true);
					m_hp--;
					return;
				}
			}

			if (info.myCollider->GetName() == "PlayerGuard")
			{
				if(info.otherCollider->GetTag() == MyLib::ColliderBase::ObjectTag::EnemyBullet)
				{
					auto pEffectComponent = m_pEffectComponent.lock();

					pEffectComponent->PlayEffect(L"shield.efk", kGuardColOffset);
				}
			}
		});
	// -----当たり判定関連-----


	// 攻撃のデータを読み込む
	if (m_attackResource.Load(L"Data/File/CSV/player_attack.csv"))
	{
		m_attackResource.ConvertAttackData();
	}

	// エフェクトを登録
	auto& effectManager = EffectManager::GetInstance();
	effectManager.LoadEffect(L"playerHit.efk");
	effectManager.LoadEffect(L"slash.efk");
	effectManager.LoadEffect(L"shield.efk");
	effectManager.LoadEffect(L"footSmoke.efk");
	effectManager.LoadEffect(L"rolling.efk");
}

void MyLib::PlayerController::Start()
{
}

void MyLib::PlayerController::Update()
{
	UpdateInputBuffer();

	// 無敵時間中の処理
	if (IsInvincible())
	{
		// 無敵時間のフレームを減少
		m_invincibleFrame--;

		std::shared_ptr<MyLib::Drawable3D> pDrawable = m_pDrawable3D.lock();

		// 点滅の周期のフレームに応じて表示非表示を切り替える
		if (m_invincibleFrame % kFlashCycle == 0)
		{
			pDrawable->SetEnable(Model::ModelSlot::Main, !(pDrawable->IsEnable(Model::ModelSlot::Main)));
		}
	}

	Input& input = Input::GetInstance();
	m_isGuard = input.IsPressed("Guard");

	m_stateMachine.Update();

	if (m_stateMachine.IsCheckState<PlayerStateFreeze>())
	{
		return;
	}

	if (m_hp <= 0)
	{
		m_stateMachine.ChangeState<PlayerStateDead>();
		return;
	}

	if (IsGuarding())
	{
		m_guardingFrame++;
	}
	else
	{
		m_guardingFrame = 0;
	}

	if (auto target = m_pLockOnTarget.lock())
	{
		auto pTransform = m_pTransform.lock();

		auto pRigidbody = m_pRigidbody.lock();
		if (m_stateMachine.IsCheckState<PlayerStateDash>())
		{
			pRigidbody->SetIsApplyDirection(true);
		}
		else
		{
			pRigidbody->SetIsApplyDirection(false);
		}

		Vector3 toTarget = target->GetPos() - pTransform->GetPos();
		toTarget.y = 0.0f;

		if (toTarget.Length() > kLockOnFaceMinDist)
		{
			toTarget.Normalize();

			Quaternion targetRot = Quaternion::LookRotation(-toTarget, Vector3::Up());

			Quaternion currentRot = pTransform->GetRotation();

			pTransform->SetRotation(Quaternion::Slerp(currentRot, targetRot, kLockOnTurnSpeed));
		}

	}
	else
	{
		auto pRigidbody = m_pRigidbody.lock();
		pRigidbody->SetIsApplyDirection(true);
	}

	// FIXME : 関数化を行うべき、ここでやるべきことなのかも不明
	if (m_isGuard)
	{
		std::shared_ptr<MyLib::Drawable3D> pDrawable = m_pDrawable3D.lock();	// Drawable3Dコンポーネントを取得

		std::shared_ptr<Model> bodyModel = pDrawable->GetModel(Model::ModelSlot::Main);

		// -----盾のオフセットを設定する-----
		int handFrame = MV1SearchFrame(bodyModel->GetModelHandle(), L"mixamorig:LeftHand");

		// デフォルトの向きだと盾の向きが逆になるため90度回転させる
		Quaternion attachRotation = Quaternion::AngleAxis(kShieldGuardAttachRadian, Vector3::Back());
		// 盾の回転(デフォルトの向きだと持つ部分と盾の向きが逆になっているため180度回転)
		Quaternion offsetRotation = Quaternion::AngleAxis(kShieldGuardOffsetRadian, Vector3::Up());
		Quaternion shieldOffset = offsetRotation * attachRotation;
		m_pShieldOffset->SetPos(kShieldGuardOffset);
		m_pShieldOffset->SetScale(kShieldModelScale);
		m_pShieldOffset->SetRotation(shieldOffset);

		pDrawable->SetModelOffset(Model::ModelSlot::Shield, m_pShieldOffset);
		pDrawable->SetAnchor(Model::ModelSlot::Shield, [bodyModel, handFrame]()
			{
				return Matrix4x4::ToMatrix(MV1GetFrameLocalWorldMatrix(bodyModel->GetModelHandle(), handFrame));
			});

		// -----盾のオフセットを設定する-----
	}
	else
	{
		std::shared_ptr<MyLib::Drawable3D> pDrawable = m_pDrawable3D.lock();	// Drawable3Dコンポーネントを取得

		std::shared_ptr<Model> bodyModel = pDrawable->GetModel(Model::ModelSlot::Main);

		// -----盾のオフセットを設定する-----
		int neckFrame = MV1SearchFrame(bodyModel->GetModelHandle(), L"mixamorig:Neck");

		// 盾の向きを背中につけているようにするため少し斜めにする
		Quaternion attachRotation = Quaternion::AngleAxis(kShieldAttachRadian, Vector3::Back());
		// 盾の回転(デフォルトの向きだと持つ部分と盾の向きが逆になっているため180度回転)
		Quaternion offsetRotation = Quaternion::AngleAxis(kShieldOffsetRadian, Vector3::Up());
		Quaternion shieldOffset = offsetRotation * attachRotation;
		m_pShieldOffset->SetPos(kShieldOffset);
		m_pShieldOffset->SetScale(kShieldModelScale);
		m_pShieldOffset->SetRotation(shieldOffset);

		pDrawable->SetModelOffset(Model::ModelSlot::Shield, m_pShieldOffset);
		pDrawable->SetAnchor(Model::ModelSlot::Shield, [bodyModel, neckFrame]()
			{
					return Matrix4x4::ToMatrix(MV1GetFrameLocalWorldMatrix(bodyModel->GetModelHandle(), neckFrame));
			});

		// -----盾のオフセットを設定する-----
	}
}

void MyLib::PlayerController::End()
{
	m_stateMachine.End();
	// エフェクトを登録
	auto& effectManager = EffectManager::GetInstance();
	effectManager.DeleteEffect(L"playerHit.efk");
	effectManager.DeleteEffect(L"slash.efk");
	effectManager.DeleteEffect(L"shield.efk");
	effectManager.DeleteEffect(L"footSmoke.efk");
	effectManager.DeleteEffect(L"rolling.efk");

	auto& soundManager = SoundManager::GetInstance();
	soundManager.DeleteSoundClip("Hit");
	soundManager.DeleteSoundClip("Rolling");
	soundManager.DeleteSoundClip("Walk");
	soundManager.DeleteSoundClip("Run");
}

void MyLib::PlayerController::SetCanAct(bool canAct)
{
	if (canAct)
	{
		m_stateMachine.ChangeState<MyLib::PlayerStateIdle>();
	}
	else
	{
		m_stateMachine.ChangeState<MyLib::PlayerStateFreeze>();
	}
}

bool MyLib::PlayerController::IsBuffered(const std::string& name) const
{
	// 現在の入力バッファに対象のキーが存在するかを確認
	auto it = m_inputBufferFrames.find(name);

	// 入力バッファにキーが存在し、先行入力と判定される場合trueを返す
	return it != m_inputBufferFrames.end() && it->second > 0;
}

void MyLib::PlayerController::ClearBuffer(const std::string& name)
{
	auto it = m_inputBufferFrames.find(name);
	if (it != m_inputBufferFrames.end())
	{
		m_inputBufferFrames[name] = 0;
	}
}

bool MyLib::PlayerController::IsJustGuard()
{
	// ガード中のフレームが1～指定のフレーム数以内ならジャストガードとする
	return m_guardingFrame != 0 && m_guardingFrame < kJustGuardFrame;
}

bool MyLib::PlayerController::IsPlayerDie()
{
	return m_stateMachine.IsCheckState<PlayerStateDie>();
}

bool MyLib::PlayerController::IsLockOn()
{
	return !m_pLockOnTarget.expired();
}

int MyLib::PlayerController::GetMaxLife()
{
	return kPlayerMaxHP;
}

void MyLib::PlayerController::UpdateInputBuffer()
{
	Input& input = Input::GetInstance();

	for (const auto& name : kBufferName)
	{
		// 押された瞬間から先行入力のフレームを設定する
		if (input.IsTriggered(name))
		{
			m_inputBufferFrames[name] = kInputBufferFrame;
		}
		// 先行入力している間はフレームを減少する
		else if (m_inputBufferFrames[name] > 0)
		{
			m_inputBufferFrames[name]--;
		}
	}
}

bool MyLib::PlayerController::IsGuarding()
{
	return m_stateMachine.IsCheckState<PlayerStateGuard>() || (m_stateMachine.IsCheckState<PlayerStateWalk>() && m_isGuard) || (m_stateMachine.IsCheckState<PlayerStateRun>() && m_isGuard) || (m_stateMachine.IsCheckState<PlayerStateIdle>() && m_isGuard);
}
