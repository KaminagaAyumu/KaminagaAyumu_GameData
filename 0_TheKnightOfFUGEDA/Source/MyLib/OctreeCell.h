#pragma once
#include <memory>
#include "ObjectForTree.h"

namespace MyLib
{

	/// <summary>
	/// 3D空間を8分割する際のセルを表すクラス
	/// </summary>
	class OctreeCell : public std::enable_shared_from_this<OctreeCell>
	{
	public:

		/// <summary>
		/// オブジェクトをセルに追加する
		/// </summary>
		/// <param name="pOFT"></param>
		/// <returns></returns>
		bool Push(std::weak_ptr<ObjectForTree> pOFT);

		/// <summary>
		/// 最新のオブジェクトへのポインタを取得する
		/// </summary>
		/// <returns></returns>
		std::weak_ptr<ObjectForTree> GetLatestOFT() const { return m_pLatestOFT; }

		void SetLatestOFT(std::weak_ptr<ObjectForTree> pOFT);

	private:
		std::weak_ptr<ObjectForTree> m_pLatestOFT; // 最新のオブジェクトへの弱参照
	};
}