#pragma once
#include "BulletBase.h"
#include <memory>

class Model;
class Effect;
class EnemyBase;
class Collidable;

class EnemyBullet : public BulletBase
{
public:
	EnemyBullet();
	virtual ~EnemyBullet();


	void Init()override;
	void End()override;
	void Update()override;
	void Draw()const override;

	void OnCollide(std::shared_ptr<Collidable> collider)override;

	void SetModel(int handle);

	/// <summary>
	/// 情報をセットする
	/// </summary>
	/// <param name="pos">生成座標</param>
	/// <param name="dir">進む方向</param>
	/// <param name="owner">生成した敵</param>
	void SetInfo(const Vector3& pos, const Vector3& dir, std::shared_ptr<Collidable> owner);

private:

	// 更新の関数ポインタ名を定義
	using UpdateFunc_t = void (EnemyBullet::*)();
	// 更新処理
	UpdateFunc_t m_update;

	// モデルクラス
	std::shared_ptr<Model> m_pModel;

	// この弾を発射した敵を取得する
	std::weak_ptr<EnemyBase> m_pOwnerEnemy;

	// 使用するエフェクトを更新するために取得する
	std::weak_ptr<Effect> m_pEffect;

	// 弾の向き
	Vector3 m_dir;

private:

	/// <summary>
	/// 反射を始める際の処理
	/// </summary>
	void StartReflect();

	void NormalUpdate(); // 通常の更新
	void AvoidedUpdate(); // 回避された後の更新

	/// <summary>
	/// 回避されたかどうか
	/// </summary>
	/// <returns></returns>
	bool IsAvoided();

};

