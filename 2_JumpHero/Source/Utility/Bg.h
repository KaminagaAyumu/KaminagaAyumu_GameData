#pragma once
#include "Geometry.h"
#include "GameType.h"
#include <functional>

// プロトタイプ宣言
class Camera;

/// <summary>
/// 背景画像を表示するためのクラス
/// </summary>
class Bg
{
public:
	Bg();
	virtual ~Bg();

	void Init();

	void Update();

	void Draw();

	void Draw(std::weak_ptr<Camera> camera);

	/// <summary>
	/// 背景の描画の仕方を設定する
	/// </summary>
	/// <param name="type">背景タイプ</param>
	void SetBgType(Types::BgType type);

	/// <summary>
	/// レイヤーを使う場合にこれを呼ぶ
	/// </summary>
	void SetLayer();

	/// <summary>
	/// 動かない背景を表示する際に呼ぶ
	/// </summary>
	/// <param name="pathNo">背景の番号</param>
	void LoadNormalBg(int pathNo);

private:

	struct BgLayer
	{
		// 画像ハンドル
		int handle = -1;

		// 画像のサイズ
		Size size = {};

		// 初期位置
		Position2 basePos = {};

		// カメラのオフセット
		Position2 offset = {};

		// スクロールスピード
		Position2 speed = {};

		// 視差→背景のスクロール率
		Position2 parallax = {};

		/// <summary>
		/// 初期化処理(※引数と変数の名前が一致したため引数には_を付けている)
		/// </summary>
		/// <param name="_handle">画像ハンドル</param>
		/// <param name="_speed">スクロールスピード</param>
		/// <param name="_parallax">スクロール割合</param>
		/// <param name="_base">初期座標</param>
		void Init(int _handle, const Position2& _speed, const Position2& _parallax, Position2 _base);
	};

	int m_bgHandle;
	int m_normalBgHandle; // スクロールしない背景画像のハンドル
	Position2 m_pos;
	std::vector<int> m_bgHandles;

	std::vector<BgLayer> m_layers;

	using UpdateFunc_t = void (Bg::*)();
	UpdateFunc_t m_updateFunc;

	using DrawFunc_t = void (Bg::*)(std::shared_ptr<Camera>);
	DrawFunc_t m_drawFunc;

	/// <summary>
	/// 斜めにループして背景を表示する更新処理
	/// </summary>
	void LoopUpdate();

	/// <summary>
	/// カメラを基準にスクロールして背景を表示する更新処理
	/// </summary>
	void ScrollUpdate();

	/// <summary>
	/// カメラを基準にX座標のみスクロールして背景を表示する更新処理
	/// </summary>
	void ScrollXUpdate();

	/// <summary>
	/// 斜めにループして背景を表示する描画処理
	/// </summary>
	void LoopDraw(std::shared_ptr<Camera>);

	/// <summary>
	/// カメラのスクロールから背景を表示する描画処理
	/// </summary>
	/// <param name="camera">カメラのポインタ</param>
	void ScrollDraw(std::shared_ptr<Camera> camera);

	/// <summary>
	/// カメラのX座標のスクロールを行う描画処理
	/// </summary>
	/// <param name="camera">カメラのポインタ</param>
	void ScrollXDraw(std::shared_ptr<Camera> camera);

	/// <summary>
	/// 背景画像を描画する処理
	/// </summary>
	/// <param name="handle"></param>
	/// <param name="size"></param>
	/// <param name="pos"></param>
	void DrawTile(int handle, const Size& size, const Position2Int& pos);

	/// <summary>
	/// スクロールする背景をレイヤーで分けずに描画する処理
	/// </summary>
	/// <param name="camera"></param>
	void DrawScrollSingle(std::shared_ptr<Camera> camera);
	
	/// <summary>
	/// X座標のみスクロールする背景をレイヤーで分けずに描画する処理
	/// </summary>
	/// <param name="camera"></param>
	void DrawScrollSingleX(std::shared_ptr<Camera> camera);



};

