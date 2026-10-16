#pragma once
#include "SceneBase.h"
#include <vector>
#include <string>
#include <functional>

/// <summary>
/// デバッグ用に使用するシーン
/// </summary>
class DebugScene : public SceneBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="controller">シーン遷移に必要なコントローラー</param>
	explicit DebugScene(SceneController& controller);
	// デストラクタ
	virtual ~DebugScene();

	virtual void Init()override;
	virtual void End()override;
	virtual void Update()override;
	virtual void Draw() const override;

private:

	struct SelectData
	{
		std::wstring text;
		std::function<void()> onSelect;
	};

	int m_cursor;

	std::vector<SelectData> m_selectScenes;

};

