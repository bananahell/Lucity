#include "Book.h"
#include "Game.h"
#include "GameData.h"
#include "InputManager.h"

#include "BookAnimator.h"
#include "Attack.h"
#include "Character.h"

#include <math.h>

Book::Book(GameObject& associated, GameObject& player) : Component(associated) {
	Book::player = Game::GetInstance().GetCurrentState().GetObjectPtr(&player, "MAIN");
	attackMode = "bubbles";
	action = "idle";
	direction = "SE";
	arc = 0;
	attackCD = 0.4;
	attacking = false;
	executeCD = 0.2;
	executing = false;

	associated.AddComponentAsFirst(new BookAnimator(associated, this));
}

Book::~Book() {

}

void Book::Start() {
	GameData::book = Game::GetInstance().GetCurrentState().GetObjectPtr(&associated, "MAIN");
}

void Book::SetArc(float arc) {
	if(arc > 360)
		arc -= 360;
	else if(arc < 0)
		arc += 360;

	Book::arc = arc;

	if(GetArc() >= 0 && GetArc() < 90)
		SetDirection("SE");
	else if(GetArc() >= 90 && GetArc() < 180)
		SetDirection("SW");
	else if(GetArc() >= 180 && GetArc() < 270)
		SetDirection("NW");
	else if(GetArc() >= 270 && GetArc() < 360)
		SetDirection("NE");
}

void Book::SetAction(std::string action) {
	Book::action = action;
}

void Book::SetDirection(std::string direction) {
	Book::direction = direction;
}

void Book::SetAttackMode(std::string attackMode) {
	Book::attackMode = attackMode;
}

void Book::Update(float dt) {
	if(!GameData::paused) {
		attackT.Update(dt);
		channelT.Update(dt);
		executeT.Update(dt);
		attacking = Attacking();
		
		if(!player.expired()) {
			Character* character = (Character*) player.lock()->GetComponent("Character");
			if(character)
				SetArc(character->GetAngleDirection());
			associated.box.SetCenter(player.lock()->box.GetCenter()+(Vec2(Vec2::Cos(arc), Vec2::Sin(arc))*30));
		}

		if(InputManager::GetMouseWheel())
			ModeSwitch(InputManager::GetMouseWheel());

		if(GetAction() == "idle") {
			if(attacking) {
				if(GetAttackMode() == "bubbles")
					channelCD = 0.2;
				else if(GetAttackMode() == "fireball")
					channelCD = 1;
				else if(GetAttackMode() == "mysticbolt")
					channelCD = 0.6;

				channelT.Restart();
				SetAction("channel");
			}
		}
		else if(GetAction() == "channel") {
			if(channelT.Get() > channelCD) {
				if(GetAttackMode() == "bubbles") {
					executing = true;
					executeT.Restart();
				}
				else if(GetAttackMode() == "fireball") {
					GameObject* go = new GameObject();
					go->AddComponent(new Attack(*go, "Player", "fireball", associated.box.GetCenter(), 1, GetArc(), 400, 1));
					Game::GetInstance().GetCurrentState().AddObject(go, "MAIN");
				}
				else if(GetAttackMode() == "mysticbolt") {
					GameObject* go = new GameObject();
					go->AddComponent(new Attack(*go, "Player", "mysticbolt", associated.box.GetCenter(), 2, GetArc(), 400, 0));
					Game::GetInstance().GetCurrentState().AddObject(go, "MAIN");
				}

				if(attacking) {
					channelT.Restart();
					SetAction("channel");
				}
				else {
					attackT.Restart();
					SetAction("close");
				}
			}
		}
		else if(GetAction() == "close") {
			if(attackT.Get() > attackCD) {
				SetAction("idle");
			}
		}

		if(executing) {
			if(executeT.Get() > executeCD) {
				executing = false;
			}
			else {
				float timeOS = pow(-1,rand()%2)*(rand()%31)/100;
				float directionOS = pow(-1,rand()%2)*(rand()%45);
				GameObject* go = new GameObject();
				go->AddComponent(new Attack(*go, "Player", "bubbles", associated.box.GetCenter(), 0.5 + timeOS, GetArc()+ directionOS, 200, 1));
				Game::GetInstance().GetCurrentState().AddObject(go, "MAIN");
			}
		}
	}
}

bool Book::Is(std::string type) {
	return (type == "Book");
}

bool Book::Attacking() {
	if(InputManager::IsMouseDown(RIGHT_MOUSE_BUTTON)) {
		return true;
	}
	else if(InputManager::IsKeyDown(SDLK_1)) {
		if(GetAction() == "channel" && GetAttackMode() != "bubbles")
			SetAction("close");
		SetAttackMode("bubbles");
		return true;
	}
	else if(InputManager::IsKeyDown(SDLK_2)) {
		if(GetAction() == "channel" && GetAttackMode() != "fireball")
			SetAction("close");
		SetAttackMode("fireball");
		return true;
	}
	else if(InputManager::IsKeyDown(SDLK_3)) {
		if(GetAction() == "channel" && GetAttackMode() != "mysticbolt")
			SetAction("close");
		SetAttackMode("mysticbolt");
		return true;
	}
	else
		return false;
}

void Book::ModeSwitch(int scroll) {
	if(GetAttackMode() == "bubbles") {
		if(scroll > 0)
			SetAttackMode("fireball");
		else if(scroll < 0)
			SetAttackMode("mysticbolt");
	}
	else if(GetAttackMode() == "fireball") {
		if(scroll > 0)
			SetAttackMode("mysticbolt");
		else if(scroll < 0)
			SetAttackMode("bubbles");
	}
	else if(GetAttackMode() == "mysticbolt") {
		if(scroll > 0)
			SetAttackMode("bubbles");
		else if(scroll < 0)
			SetAttackMode("fireball");
	}

	if(GetAction() == "channel")
		SetAction("close");
}

float Book::GetArc() {
	return arc;
}

std::string Book::GetSprite() {
	return action+direction;
}

std::string Book::GetAction() {
	return action;
}

std::string Book::GetDirection() {
	return direction;
}

std::string Book::GetAttackMode() {
	return attackMode;
}
