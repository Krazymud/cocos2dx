#pragma once
#include "cocos2d.h"
#include "Monster.h"
using namespace cocos2d;

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
        
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
	void move(Ref* pSender, char cid);
	void die(Ref* pSender);
	void hit(Ref* pSender);
	void updateCustom(float dt);
	void updateMonster(float dt);
	void createMonster(float dt);
private:
	cocos2d::Sprite* player;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	cocos2d::Label* killed;
	cocos2d::Label* highest_score;
	char lastCid = 'D';
	bool isDead = false;
	int kill = 0;
	int dtime;
	cocos2d::ProgressTimer* pT;
};
