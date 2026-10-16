#pragma once
#include "../MyLib/Component/Transform.h"
#include <functional>
#include "DxLib.h"

class Vector3;
class Matrix4x4;

/// <summary>
/// モデル制御クラス
/// </summary>
class Model
{
public:

	enum class ModelSlot
	{
		Main,
		Weapon,
		Shield
	};


public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Model();
	virtual ~Model();

	void Init();
	void End();
	void Update();
	void Draw(const Matrix4x4& worldMatrix) const;

	/// <summary>
	/// モデルのハンドルをセットする
	/// </summary>
	/// <param name="handle">モデルのハンドル</param>
	void SetModelHandle(int handle) { m_modelHandle = handle; }

	/// <summary>
	/// モデルハンドルを取得する
	/// </summary>
	/// <returns></returns>
	const int GetModelHandle() const { return m_modelHandle; }

	/// <summary>
	/// モデルの表示状態をセットする
	/// </summary>
	/// <param name="isEnable"></param>
	void SetEnable(bool isEnable) { m_isEnable = isEnable; }

	/// <summary>
	/// モデルの表示状態を取得する
	/// </summary>
	/// <returns></returns>
	bool IsEnable()const { return m_isEnable; }

	/// <summary>
	/// モデルのスロットをセットする
	/// </summary>
	/// <param name="slot">モデルのスロット</param>
	void SetSlot(ModelSlot slot) { m_slot = slot; }

	/// <summary>
	/// モデルのスロットを取得する
	/// </summary>
	/// <returns>モデルのスロット</returns>
	ModelSlot GetSlot() const { return m_slot; }

	/// <summary>
	/// このモデルのレイヤー番号を取得する
	/// </summary>
	/// <param name="layer"></param>
	void SetModelLayer(int layer) { m_modelLayer = layer; }
	int GetModelLayer() const { return m_modelLayer; }

	void SetLocalOffset(std::weak_ptr<MyLib::Transform> offset) { m_localOffset = offset; }
	std::weak_ptr<MyLib::Transform> GetLocalOffset() const { return m_localOffset; }

	/// <summary>
	/// モデルのアンカー(行列)をセットする関数
	/// </summary>
	using AnchorFunc = std::function<Matrix4x4()>;
	void SetAnchor(AnchorFunc func) { m_anchor = std::move(func); }
	void ClearAnchor() { m_anchor = nullptr; }
	bool IsUseAnchor() const { return static_cast<bool>(m_anchor); }
	Matrix4x4 GetAnchorMatrix() const { return m_anchor(); }

private:
	// モデルハンドル
	int m_modelHandle;

	// 表示しているかどうか
	bool m_isEnable;

	// モデルの表示する位置
	ModelSlot m_slot;

	// モデル内でのレイヤー
	int m_modelLayer;

	// ローカルオフセット
	std::weak_ptr<MyLib::Transform> m_localOffset;

	// 行列を取得して描画を行う際の関数
	AnchorFunc m_anchor;
};

