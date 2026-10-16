#pragma once
#include "Drawable.h"
#include "../Transform.h"
#include "../Rigidbody.h"
#include "../../../Common/Model.h"
#include <memory>
#include <vector>
#include <string>

namespace MyLib
{
	/// <summary>
	/// 3D描画クラス
	/// </summary>
	class Drawable3D : public Drawable
	{
	public:
		/// <summary>
		/// コンストラクタ
		/// 必ずレイヤー情報をセットする
		/// </summary>
		explicit Drawable3D(DrawLayer layer);

		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~Drawable3D() = default;

		void Init(std::weak_ptr<MyLib::GameObject> parent) override;

		void Start() override;

		/// <summary>
		/// モデルを追加する
		/// </summary>
		/// <param name="slot">追加するスロット</param>
		/// <param name="handle">モデルのハンドル</param>
		/// <returns>モデルのポインタ</returns>
		std::shared_ptr<Model> AddModel(Model::ModelSlot slot, int handle);

		/// <summary>
		/// 指定スロットのモデルを取得する
		/// </summary>
		/// <param name="slot">モデルのスロット</param>
		/// <returns>指定モデル</returns>
		std::shared_ptr<Model> GetModel(Model::ModelSlot slot);

		const int GetModelHandle(Model::ModelSlot slot)const;

		/// <summary>
		/// モデルの表示状態を変更する
		/// </summary>
		/// <param name="slot">モデルのスロット</param>
		/// <param name="isEnable">true : 表示状態 false : 非表示</param>
		void SetEnable(Model::ModelSlot slot, bool isEnable);

		/// <summary>
		/// モデルの表示状態を取得する
		/// </summary>
		/// <param name="slot"></param>
		/// <returns>true : 表示されている false : 表示されていない</returns>
		bool IsEnable(Model::ModelSlot slot);

		/// <summary>
		/// モデルのオフセットをセットする
		/// </summary>
		/// <param name="offset"></param>
		void SetModelOffset(Model::ModelSlot slot, std::weak_ptr<MyLib::Transform> offset);

		void SetAnchor(Model::ModelSlot slot, Model::AnchorFunc func);

		void SetCastShadow(bool isCastShadow) { m_isCastShadow = isCastShadow; }

		void SetReceiveShadow(bool isReceiveShadow) { m_isReceiveShadow = isReceiveShadow; }

		void End();

		void Update();

		void Draw() const override;

		float GetDepth()override;

		/// <summary>
		/// シャドウマップを行うかどうか(影を落とす側)
		/// </summary>
		/// <returns></returns>
		bool IsCastShadow()override { return m_isCastShadow; }

		/// <summary>
		/// シャドウマップを行うかどうか(影を落とされる側)
		/// </summary>
		/// <returns></returns>
		bool IsReceiveShadow()override { return m_isReceiveShadow; }

	private:

		Matrix4x4 GetLocalMatrix(std::shared_ptr<Model> model) const;

	private:
		// モデルクラス(今後はこちらを使いたい)
		std::vector<std::shared_ptr<Model>> m_pModels;

		// 位置、スケールデータ
		std::weak_ptr<MyLib::Transform> m_pTransform;

		// 影を落とすかどうか
		bool m_isCastShadow;

		// 影を落とされるかどうか
		bool m_isReceiveShadow;
	};

}



