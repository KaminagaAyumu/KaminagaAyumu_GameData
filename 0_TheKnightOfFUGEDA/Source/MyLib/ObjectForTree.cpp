#include "ObjectForTree.h"
#include "OctreeCell.h"

void MyLib::ObjectForTree::RemoveFromCell()
{
	if (m_pCell.expired()) { return; }

	std::shared_ptr<OctreeCell> pCell = m_pCell.lock();

	if (std::shared_ptr<ObjectForTree> prev = m_pPrev.lock())
	{
		prev->SetNext(m_pNext);
	}
	else if (pCell)
	{
		// 自分がセルの先頭ノードだった場合、セルの先頭を次のノードに変更させる
		pCell->SetLatestOFT(m_pNext);
	}

	if (std::shared_ptr<ObjectForTree> next = m_pNext.lock())
	{
		next->SetPrev(m_pPrev);
	}

	// 自分自身のポインタも初期化する
	m_pPrev.reset();
	m_pNext.reset();
	m_pCell.reset();
}
