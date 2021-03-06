#include "Character.h"

#include "Animator.h"
#include "Collider.h"

Character::Character(GameObject& associated, std::string name) : Component(associated) {
	hp = 0;
	speed = 0;
	directionAngle = 0;
	action = IDLE;
	direction = "SE";
	associated.AddComponentAsFirst(new Collider(associated));
	associated.AddComponentAsFirst(new Animator(associated, this, name));
}

Character::~Character() {

}

void Character::SetHealth(int hp) {
	Character::hp = hp;
}

void Character::SetSpeed(int speed) {
	Character::speed = speed;
}

void Character::SetAngleDirection(float directionAngle) {
	Character::directionAngle = directionAngle;
}

void Character::SetAction(std::string action) {
	Character::action = action;
}

void Character::SetDirection(std::string direction) {
	Character::direction = direction;
}

void Character::Damage(int damage) {
	SetHealth(GetHealth()-damage);
	if(GetHealth() < 1)
		associated.RequestDelete();
}

void Character::Update(float dt) {

}

bool Character::Is(std::string type) {
	return (type == "Character");
}

int Character::GetHealth() {
	return hp;
}

int Character::GetSpeed() {
	return speed;
}

float Character::GetAngleDirection() {
	return directionAngle;
}

std::string Character::GetAction() {
	return action;
}

std::string Character::GetDirection() {
	return direction;
}

std::string Character::GetSprite() {
	return action+direction;
}
