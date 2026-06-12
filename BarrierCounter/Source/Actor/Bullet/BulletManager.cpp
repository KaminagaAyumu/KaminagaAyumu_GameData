#include "../../Geometry/Vector3.h"
#include "BulletManager.h"
#include "DxLib.h"
#include "EnemyBullet.h"

BulletManager::BulletManager() : 
	m_modelHandle(-1)
{
}

BulletManager::~BulletManager()
{
}

void BulletManager::Init()
{
	// 弾のモデルをロード
	m_modelHandle = MV1LoadModel(L"Data/Model/enemyBullet.mv1");

	// 弾リストを初期化
	m_pBullets.clear();
}

void BulletManager::End()
{
	m_modelHandle = MV1DeleteModel(m_modelHandle);
	// 残っている弾がある場合はすべての弾の終了処理を行う
	for (auto& bullet : m_pBullets)
	{
		bullet->End();
	}
}

void BulletManager::Update()
{
	// 弾の更新
	for (auto& bullet : m_pBullets)
	{
		bullet->Update();
		// 弾が死んだとき
		if (bullet->IsDead())
		{
			// 弾の終了処理を行う
			bullet->End();
		}
	}

	// 弾の中で死んだものを探す
	m_pBullets.remove_if([](std::shared_ptr<BulletBase> bullet)
		{
			// 弾が消えた判定の時に削除
			return bullet->IsDead();
		});

}

void BulletManager::Draw() const
{
	// 弾の描画
	for (const auto& bullet : m_pBullets)
	{
		bullet->Draw();
	}
}

void BulletManager::CreateBullet(const Vector3& pos, const Vector3& dir, std::shared_ptr<MyLib::Collidable> owner)
{
	auto bullet = std::make_shared<EnemyBullet>();
	bullet->Init();
	bullet->SetModel(MV1DuplicateModel(m_modelHandle));
	bullet->SetInfo(pos, dir, owner);
	m_pBullets.push_back(bullet);
}
