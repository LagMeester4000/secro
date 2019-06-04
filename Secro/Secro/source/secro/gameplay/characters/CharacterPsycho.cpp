#include "CharacterPsycho.h"
#include <Box2D.h>

secro::CharacterPsycho::CharacterPsycho()
{
}

secro::CharacterPsycho::CharacterPsycho(Level * level, HitboxManager * hitboxManager, b2Body * body, std::shared_ptr<Controller> controller)
	: PlayerGraphicsCharacter(level, hitboxManager, body, controller)
{
}

void secro::CharacterPsycho::init()
{
	PlayerGraphicsCharacter::init();
}

void secro::CharacterPsycho::loadAnimations()
{
	loadAnimation("content/characters/psycho/BigBoi.png", 1, true, 1.f, animStand);
	loadAnimation("content/characters/psycho/BigBoi_jump.png", 1, true, 1.f, animInAir);
	loadAnimation("content/characters/psycho/BigBoi_crouch.png", 1, true, 1.f, animJumpSquat);
	loadAnimation("content/characters/psycho/BigBoi_run.png", 4, true, 0.1f, animRun);
	loadAnimation("content/characters/psycho/BigBoi_run.png", 4, true, 0.13f, animWalk);
	loadAnimation("content/characters/psycho/BigBoi_DTilt.png", 5, false, 0.05f, animDTilt);
	loadAnimation("content/characters/psycho/BigBoi_UTilt.png", 4, false, 0.05f, animUTilt);
	loadAnimation("content/characters/psycho/BigBoi_FTilt.png", 4, false, 0.05f, animFTilt);
	loadAnimation("content/characters/psycho/BigBoi_FTilt.png", 4, false, 0.05f, animJab);
	loadAnimation("content/characters/psycho/BigBoi_NAir.png", 5, false, 0.05f, animNAir);
	loadAnimation("content/characters/psycho/BigBoi_FAir.png", 11, false, 0.05f, animFAir);
	loadAnimation("content/characters/psycho/BigBoi_DAir.png", 9, false, 0.05f, animDAir);
	loadAnimation("content/characters/psycho/BigBoi_BAir.png", 7, false, 0.05f, animBAir);
	loadAnimation("content/characters/psycho/BigBoi_UAir.png", 5, false, 0.05f, animUAir);

	//need to be done still
	loadAnimation("content/characters/psycho/BigBoi_jump.png", 1, true, 1.f, animHitstun);
	loadAnimation("content/characters/psycho/BigBoi_jump.png", 1, true, 1.f, animTechLeft);
	loadAnimation("content/characters/psycho/BigBoi_jump.png", 1, true, 1.f, animTechRight);
	loadAnimation("content/characters/psycho/BigBoi_jump.png", 1, true, 1.f, animLandingLag);
	loadAnimation("content/characters/psycho/BigBoi_run.png", 1, true, 1.f, animDash);
	loadAnimation("content/characters/psycho/BigBoi_jump.png", 1, true, 1.f, animFreeFall);
	loadAnimation("content/characters/psycho/BigBoi_jump.png", 1, true, 1.f, animGrab);
}

void secro::CharacterPsycho::setupAttacks(AttackCollection & atts)
{
	atts.loadAttack("D_NAir.json", PlayerState::ANAir);
	atts.loadAttack("P_UAir.json", PlayerState::AUAir);
	atts.loadAttack("P_FAir.json", PlayerState::AFAir);
	atts.loadAttack("BAir.json", PlayerState::ABAir);
	atts.loadAttack("P_DAir.json", PlayerState::ADAir);

	atts.loadAttack("D_Jab.json", PlayerState::AJab);
	atts.loadAttack("D_UTilt.json", PlayerState::AUTilt);
	atts.loadAttack("FTilt.json", PlayerState::AFTilt);
	atts.loadAttack("DTilt.json", PlayerState::ADTilt);

	atts.loadAttack("Grab.json", PlayerState::AGrab);
	atts.loadAttack("Grab.json", PlayerState::AGrabAir);

	atts.loadAttack("FalShine.json", PlayerState::SpecialD);
}

void secro::CharacterPsycho::setupStates(StateMachine & sm)
{
	PlayerGraphicsCharacter::setupStates(sm);

	sm.addCondition(PlayerState::Jump, PlayerState::SpecialU, std::bind(&CharacterPsycho::ConditionUpSpecial, this, std::placeholders::_1));
	sm.addCondition(PlayerState::Stand, PlayerState::SpecialU, std::bind(&CharacterPsycho::ConditionUpSpecial, this, std::placeholders::_1));
	sm.addCondition(PlayerState::Run, PlayerState::SpecialU, std::bind(&CharacterPsycho::ConditionUpSpecial, this, std::placeholders::_1));
	sm.addCondition(PlayerState::Dash, PlayerState::SpecialU, std::bind(&CharacterPsycho::ConditionUpSpecial, this, std::placeholders::_1));
	sm.addCondition(PlayerState::Walk, PlayerState::SpecialU, std::bind(&CharacterPsycho::ConditionUpSpecial, this, std::placeholders::_1));

	sm.addSetState(PlayerState::SpecialU, std::bind(&CharacterPsycho::BeginUpSpecial, this, std::placeholders::_1));
	sm.addCondition(PlayerState::SpecialU, PlayerState::Jump, [&](float f) 
	{
		return getMovementState() == MovementState::InAir;
	});
	sm.addCondition(PlayerState::SpecialU, PlayerState::Stand, [&](float f)
	{
		return getMovementState() == MovementState::OnGround;
	});
}

void secro::CharacterPsycho::update(float deltaTime)
{
	PlayerGraphicsCharacter::update(deltaTime);

	if (getMovementState() == MovementState::OnGround)
	{
		CanNeutralSpecial = true;
	}
}

bool secro::CharacterPsycho::ConditionUpSpecial(float deltaTime)
{
	return input->getMovementDirection() == Direction::Up && input->specialPressed() && CanNeutralSpecial;
}

void secro::CharacterPsycho::BeginUpSpecial(float deltaTime)
{
	CanNeutralSpecial = false;
	physicsBody->SetLinearVelocity({ 0.f, -UpSpecialSpeed });
}
