#pragma once
#include <memory>

namespace MyLib
{
	class OctreeCell;
	class Collidable;

	/// <summary>
	/// オブジェクトをツリー構造で管理するためのクラス
	/// </summary>
	class ObjectForTree
	{
	public:

		// ゲッター群
		std::weak_ptr<Collidable> GetOwner() const { return m_pOwner; }
		std::weak_ptr<OctreeCell> GetOctreeCell() const { return m_pCell; }
		std::weak_ptr<ObjectForTree> GetPrev() const { return m_pPrev; }
		std::weak_ptr<ObjectForTree> GetNext() const { return m_pNext; }

		// セッター群
		void SetOwner(std::weak_ptr<Collidable> pOwner) { m_pOwner = pOwner; }
		void SetOctreeCell(std::weak_ptr<OctreeCell> pCell) { m_pCell = pCell; }
		void SetPrev(std::weak_ptr<ObjectForTree> pPrev) { m_pPrev = pPrev; }
		void SetNext(std::weak_ptr<ObjectForTree> pNext) { m_pNext = pNext; }

		/// <summary>
		/// セルからオブジェクトを削除する
		/// </summary>
		void RemoveFromCell();
		
	private:
		std::weak_ptr<Collidable> m_pOwner; // 当たり判定への弱参照
		std::weak_ptr<OctreeCell> m_pCell; // オブジェクトが属するセルへの弱参照
		std::weak_ptr<ObjectForTree> m_pPrev; // 前のオブジェクトへの弱参照
		std::weak_ptr<ObjectForTree> m_pNext; // 次のオブジェクトへの弱参照
	};
}



