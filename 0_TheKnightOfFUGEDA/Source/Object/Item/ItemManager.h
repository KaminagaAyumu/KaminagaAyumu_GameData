#pragma once
#include "../../Utility/CSV/ItemResource.h"
#include <list>
#include <memory>

class Item;

/// <summary>
/// アイテムを管理するクラス
/// </summary>
class ItemManager
{
public:

	ItemManager();
	virtual ~ItemManager();

	void Init(int stageNo);
	void Update();
	void End();

	bool IsItemGetAll()const;

	/// <summary>
	/// 現在のフレーム内で獲得されたアイテムの数を返す
	/// </summary>
	/// <returns></returns>
	const int GetAcquisitionCountThisFrame() const { return m_acquisitionCountThisFrame; }

private:

	// アイテムのコンテナ
	std::list<std::weak_ptr<Item>> m_pItems;

	// アイテムのデータを管理するCSVデータクラス(現状位置のみ)
	ItemResource m_itemResource;

	// 現在のフレーム内で取得した数
	int m_acquisitionCountThisFrame;
};

