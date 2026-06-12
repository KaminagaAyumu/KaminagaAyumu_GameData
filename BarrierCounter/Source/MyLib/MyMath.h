#pragma once

namespace MyLib
{
	/// <summary>
	/// 余りを正の数にして返す
	/// </summary>
	/// <param name="left">左辺</param>
	/// <param name="right">右辺</param>
	/// <returns>余りの数(正の数)</returns>
	int RemainderToNaturalNumber(int left, int right)
	{
		if (right <= 0) return 0;
		// 左辺と右辺の余りを求める
		int ret = left % right;

		// 余りが負の数だったら右辺の値を足した数を返す
		if (ret < 0)
		{
			return ret + right;
		}

		// ここを通ったら正の数の場合なのでそのまま返す
		return ret;
	}

}
