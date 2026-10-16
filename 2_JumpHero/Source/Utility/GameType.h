#pragma once

class Actor;

// オブジェクト(Actor)の区別をつける列挙体など
namespace Types
{
	/// <summary>
	/// フォントのサイズなどをまとめる列挙体
	/// </summary>
	enum class FontType : int
	{
		Small, // 小サイズ
		Midium, // 中サイズ
		Large, // 大きいサイズ
		Header, // 見出しサイズ
		Default
	};

	enum class ImageType : int
	{
		TitleLogo,
		TutorialIcon,
		Stage1Icon,
		Stage2Icon,
		Stage3Icon,
		TitleIcon,
		ExitIcon,
		PlayerIcon,
		ChangeCoinIcon,
		MedalIcon,
		AButtonIcon,
		ImageNum,
	};

	/// <summary>
	/// オブジェクトが何なのかを説明するための列挙体
	/// </summary>
	enum class ActorType
	{
		Player,
		Enemy,
		Chest,
		Item
	};

	/// <summary>
	/// 当たり判定が何なのかを説明するための列挙体
	/// </summary>
	enum class ColliderType
	{
		Rect,
		Circle
	};

	/// <summary>
	/// 当たり判定の内容を格納する構造体
	/// </summary>
	struct CollisionInfo
	{
		ActorType otherType; // 当たったオブジェクトの種類
		ColliderType ownColliderType; // 自身の当たり判定のタイプ
		ColliderType otherColliderType; // 当たったオブジェクトの当たり判定のタイプ
	};

	/// <summary>
	/// アイテムの種類
	/// </summary>
	enum class ItemType
	{
		Balloon, // 風船
		UpgradeMedal, // 強化メダル
		LifeUp, // 1UP
		Coin, // スコア加算用コイン
		ChangeToCoin, // 敵をコインに変える
		AttackItem, // 攻撃アイテム
	};

	/// <summary>
	/// エフェクトの種類
	/// </summary>
	enum class EffectType : int
	{
		CoinGet, // コイン取得時のエフェクト
		Impact, // 爆発エフェクト
		Star, // 星エフェクト
		PowerUp1, // パワーアップ時のエフェクト
		PowerUp2, // パワーアップ時のエフェクト
		Jump, // ジャンプ時のエフェクト
		ChangeCoin, // 敵をコインに変えた際のエフェクト
		ChangeCoinStart, // 敵をコインに変えた際のエフェクト
		BalloonGet,
		Test,

		EffectNum
	};

	/// <summary>
	/// 背景の種類
	/// </summary>
	enum class BgType : int
	{
		Loop, // 自動でループする背景
		ScrollXY, // カメラのXY座標のスクロールをする背景
		ScrollX, // カメラのX座標のスクロールをする背景
	};
}