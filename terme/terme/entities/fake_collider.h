#pragma once

#include "terme/entities/collider.h"

#include <cassert>

namespace terme
{
	class FakeCollider : public terme::Collider
	{
	public:
		FakeCollider() : terme::Collider(0, 0) {}
	private:
		bool CanExitScreenSpace() const override { assert(false); return true; }
		double GetGravityScale() const override { assert(false); return 0; }
		void InitModel() override {}
	};
}