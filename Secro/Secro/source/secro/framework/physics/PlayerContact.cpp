#include "PlayerContact.h"

void secro::PlayerContact::BeginContact(b2Contact * contact)
{
	
}

void secro::PlayerContact::EndContact(b2Contact * contact)
{
	while (contact != nullptr)
	{
		b2Body* dynBody = nullptr;
		if (contact->GetFixtureA()->GetBody()->GetType() == b2_dynamicBody)
			dynBody = contact->GetFixtureA()->GetBody();
		else if (contact->GetFixtureB()->GetBody()->GetType() == b2_dynamicBody)
			dynBody = contact->GetFixtureB()->GetBody();
		else
			continue;

		auto* manifold = contact->GetManifold();
		auto& normal = manifold->localNormal;
		auto y = normal.y;
		if (abs(y) < 0.8f)
		{
			auto vel = dynBody->GetLinearVelocity();
			if (normal.x > 0.f)
			{
				normal = { 1.f, 0.f };
				dynBody->SetLinearVelocity({ 0.f, vel.y });
			}
			else if (normal.x < 0.f)
			{
				normal = { -1.f, 0.f };
				dynBody->SetLinearVelocity({ 0.f, vel.y });
			}
		}

		//next
		contact = contact->GetNext();
	}
}
