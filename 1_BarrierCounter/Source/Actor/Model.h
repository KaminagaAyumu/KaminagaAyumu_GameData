#pragma once

class Vector3;
class Animation;

class Model
{
public:

	Model();
	virtual ~Model();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();
	void End();
	void Update();
	void Draw();

	/// <summary>
	/// モデルの座標をセットする
	/// </summary>
	/// <param name="pos"></param>
	void SetPos(const Vector3& pos);
	/// <summary>
	/// モデルの正面の向きをセットする
	/// </summary>
	/// <param name="dir"></param>
	void SetDir(const Vector3& dir);
	/// <summary>
	/// モデルのスケールをセットする
	/// </summary>
	/// <param name="scale"></param>
	void SetScale(const Vector3& scale);

	/// <summary>
	/// アニメーションをセットする
	/// ※モデルをセットした後に呼ぶ
	/// </summary>
	/// <param name="animNo"></param>
	/// <param name="blendTime"></param>
	void SetAnimation(int animNo, float animSpeed, int blendTime, bool isLoop);

	/// <summary>
	/// モデルのハンドルをセットする
	/// </summary>
	/// <param name="handle">モデルのハンドル</param>
	void SetModelHandle(int handle) { m_modelHandle = handle; }

	/// <summary>
	/// モデルの表示状態をセットする
	/// </summary>
	/// <param name="isEnable"></param>
	void SetEnable(bool isEnable) { m_isEnable = isEnable; }

	/// <summary>
	/// アニメーションが終わったかどうかを取得
	/// </summary>
	bool GetAnimationEnd()const;

private:
	// モデルハンドル
	int m_modelHandle;

	// 表示しているかどうか
	bool m_isEnable;

	// モデルのアニメーション
	std::shared_ptr<Animation> m_pAnimation;

};

