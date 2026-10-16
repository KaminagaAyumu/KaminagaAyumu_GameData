#pragma once
#include "../Utility/Geometry.h"
#include <unordered_map>

class Map;

/// <summary>
/// 座標情報をレジストリ(キーと値)で管理するクラス
/// </summary>
class PositionRegistry
{
public:

	PositionRegistry();
	virtual ~PositionRegistry();

	/// <summary>
	/// 座標情報を初期化する
	/// </summary>
	/// <param name="map">マップのポインタ</param>
	/// <returns>true : 初期化に成功 false : 初期化に失敗</returns>
	bool InitPositions(std::shared_ptr<Map> map);

	/// <summary>
	/// イベント発火エリアの座標を取得する
	/// </summary>
	/// <param name="id">指定ID</param>
	/// <returns>イベント発火エリアの座標</returns>
	const Position2 GetAreaPos(int id)const;
	
	/// <summary>
	/// 宝箱の座標を取得する
	/// </summary>
	/// <param name="id">指定ID</param>
	/// <returns>宝箱の座標</returns>
	const Position2 GetChestPos(int id)const;
	
	/// <summary>
	/// バリアの座標を取得する
	/// </summary>
	/// <param name="id">指定ID</param>
	/// <returns>バリアの座標</returns>
	const ActivePosition2 GetBarrierPos(int id)const;

	/// <summary>
	/// 指定したバリアのアクティブ状態をセットする
	/// </summary>
	/// <param name="id">指定ID</param>
	/// <param name="isActive">アクティブかどうか</param>
	/// <returns>true : セットが出来た false : セットができなかった</returns>
	bool SetBarrierActive(int id, bool isActive);
	
	/// <summary>
	/// カメラのターゲット座標を取得する
	/// </summary>
	/// <param name="id">カメラID</param>
	/// <returns>ターゲット座標</returns>
	const Position2 GetCameraPos(int id)const;
	
	/// <summary>
	/// 敵のスポーン座標を取得する
	/// </summary>
	/// <returns>スポーン座標の配列</returns>
	const std::vector<Position2>& GetSpawnPosAll()const;

private:
	std::unordered_map<int, Position2> m_areaPos;
	std::unordered_map<int, Position2> m_chestPos;
	std::unordered_map<int, ActivePosition2> m_barrierPos;
	std::unordered_map<int, Position2> m_cameraPos;
	std::vector<Position2> m_spawnPos;

};

