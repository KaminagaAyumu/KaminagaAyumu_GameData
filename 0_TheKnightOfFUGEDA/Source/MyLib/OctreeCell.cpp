#include "OctreeCell.h"

bool MyLib::OctreeCell::Push(std::weak_ptr<ObjectForTree> pOFT)
{
	// セルをlockして取得する
	std::shared_ptr<ObjectForTree> pSharedOFT = pOFT.lock();

	// オブジェクトが有効でない場合falseとする
	if (!pSharedOFT) { return  false; }

	// オブジェクトがすでにこのセルに属している場合は追加しない
	if (pSharedOFT->GetOctreeCell().lock() == shared_from_this()) { return false; }

	// オブジェクトの前後のリンクを更新する
	pSharedOFT->SetNext(m_pLatestOFT);

	if (std::shared_ptr<ObjectForTree> pSharedLatest = m_pLatestOFT.lock())
	{
		pSharedLatest->SetPrev(pOFT);
	}
	m_pLatestOFT = pOFT;
	pSharedOFT->SetOctreeCell(shared_from_this());
	return true;
}

void MyLib::OctreeCell::SetLatestOFT(std::weak_ptr<ObjectForTree> pOFT)
{
	if (std::shared_ptr<ObjectForTree> pSharedOFT = pOFT.lock())
	{
		m_pLatestOFT = pSharedOFT;
	}
	else
	{
		m_pLatestOFT.reset();
	}
}
