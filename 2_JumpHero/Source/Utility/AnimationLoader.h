#pragma once


/// <summary>
/// ファイルからアニメーションデータをロードするクラス
/// </summary>
class AnimationLoader
{
public:
	/// <summary>
	/// アニメーションをファイルから読み込んで格納する
	/// </summary>
	/// <param name="path">指定のアニメーションデータ</param>
	/// <param name="graphHandle">アニメーションの画像ハンドル</param>
	/// <param name="outAnimations">返すアニメーション</param>
	/// <returns></returns>
	static bool LoadAnimationData(const std::wstring& path, int graphHandle, std::unordered_map<std::string, Animation>& outAnimations);

};

