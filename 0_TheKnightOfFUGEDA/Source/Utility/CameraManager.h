#pragma once
#include <memory>
#include <list>
#include "../MyLib/Component/Camera/CameraComponent.h"

/// <summary>
/// カメラを管理するクラス
/// </summary>
class CameraManager
{
public:

	virtual ~CameraManager();

	/// <summary>
	/// インスタンスを取得する
	/// </summary>
	/// <returns></returns>
	static CameraManager& GetInstance();

	void Entry(std::shared_ptr<MyLib::CameraComponent> camera);
	void Exit(std::shared_ptr<MyLib::CameraComponent> camera);

	void Init();
	void Update();
	void End();

	

private:

	// 登録するカメラクラス
	std::list<std::shared_ptr<MyLib::CameraComponent>> m_pCameras;


private:

	/// <summary>
	/// コンストラクタ
	/// シングルトンクラスのためprivateで宣言する
	/// ※宣言の際にcppファイルの一番上に配置しています
	/// </summary>
	CameraManager();
	CameraManager(const CameraManager&) = delete; // コピーコンストラクタを作れないようにする
	void operator=(const CameraManager&) = delete; // 代入演算子も使えないようにする

};

