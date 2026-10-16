#pragma once
#include "../Component.h"

namespace MyLib
{
	class LockOnController : public Component
	{
	public:

		LockOnController();
		virtual ~LockOnController();


		void Init(std::weak_ptr<MyLib::GameObject> parent) override;
		void Start() override;
		void Update() override;
		void End() override;



	private:

		


	};
}



