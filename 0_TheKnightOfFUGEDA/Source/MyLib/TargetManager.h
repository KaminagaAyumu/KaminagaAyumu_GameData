#pragma once
#include <list>

namespace MyLib
{
	class Targetable;
	/// <summary>
	/// ターゲットを管理するクラス
	/// </summary>
	class TargetManager
	{
	public:

		virtual ~TargetManager();

		static TargetManager& GetInstance();

		void Register(Targetable* target);
		void UnRegister(Targetable* target);

	private:
		TargetManager();
		TargetManager(const TargetManager&) = delete;
		void operator=(const TargetManager&) = delete;

		std::list<Targetable*> m_pTargets;
	};
}

