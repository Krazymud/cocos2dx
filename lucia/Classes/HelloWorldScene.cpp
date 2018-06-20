#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#pragma execution_character_set("utf-8")
#define database UserDefault::getInstance()
USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
	
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	if (!database->getBoolForKey("isExist")) {
		database->setBoolForKey("isExist", true);
	}
	if (!database->getIntegerForKey("highest")) {
		database->setIntegerForKey("highest", kill);
	}
	
	TMXTiledMap* tmx = TMXTiledMap::create("test.tmx");
	tmx->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	tmx->setScale(Director::getInstance()->getContentScaleFactor());
	this->addChild(tmx, 0);

	//����һ����ͼ
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//����ͼ�������ص�λ�и�����ؼ�֡
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//ʹ�õ�һ֡��������
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player, 3);

	//hp��
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//ʹ��hp������progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 0);

	// ��̬����
	idle.reserve(1);
	idle.pushBack(frame0);

	// ��������
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
		attack.pushBack(frame);
	}
	attack.pushBack(SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113))));

	// ���Է��չ�������
	// ��������(֡����22֡���ߣ�90����79��
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}
	// Todo

	// �˶�����(֡����8֡���ߣ�101����68��
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	run.reserve(4);
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}
	// Todo
	
	auto itemW = MenuItemLabel::create(Label::createWithTTF("W", "fonts/arial.ttf", 36), CC_CALLBACK_1(HelloWorld::move, this, 'W'));
	itemW->setPosition(Vec2(origin.x + 60, origin.y + 80));
	auto itemA = MenuItemLabel::create(Label::createWithTTF("A", "fonts/arial.ttf", 36), CC_CALLBACK_1(HelloWorld::move, this, 'A'));
	itemA->setPosition(Vec2(origin.x + 20, origin.y + 40));
	auto itemS = MenuItemLabel::create(Label::createWithTTF("S", "fonts/arial.ttf", 36), CC_CALLBACK_1(HelloWorld::move, this, 'S'));
	itemS->setPosition(Vec2(origin.x + 60, origin.y + 40));
	auto itemD = MenuItemLabel::create(Label::createWithTTF("D", "fonts/arial.ttf", 36), CC_CALLBACK_1(HelloWorld::move, this, 'D'));
	itemD->setPosition(Vec2(origin.x + 100, origin.y + 40));
	auto itemX = MenuItemLabel::create(Label::createWithTTF("X", "fonts/arial.ttf", 36), CC_CALLBACK_1(HelloWorld::die, this));
	itemX->setPosition(Vec2(origin.x + visibleSize.width - 30, origin.y + 80));
	auto itemY = MenuItemLabel::create(Label::createWithTTF("Y", "fonts/arial.ttf", 36), CC_CALLBACK_1(HelloWorld::hit, this));
	itemY->setPosition(Vec2(origin.x + visibleSize.width - 30, origin.y + 40));
	time = Label::createWithTTF("180", "fonts/arial.ttf", 36);
	dtime = 180;
	time->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 40));
	this->addChild(time, 1);
	
	Label* h = Label::createWithTTF("Highest Score:", "fonts/arial.ttf", 36);
	h->setPosition(Vec2(origin.x + visibleSize.width / 2 - 140, origin.y + visibleSize.height - 110));
	this->addChild(h, 1);

	std::string highest = Value(database->getIntegerForKey("highest")).asString();
	highest_score = Label::createWithTTF(highest, "fonts/arial.ttf", 36);
	highest_score->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 110));
	this->addChild(highest_score, 1);

	killed = Label::createWithTTF("0", "fonts/arial.ttf", 36);
	killed->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 75));
	this->addChild(killed, 1);

	auto menu = Menu::create(itemW, itemA, itemS, itemD, itemX, itemY, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu);
	schedule(schedule_selector(HelloWorld::updateCustom), 1.0f, kRepeatForever, 0);
	schedule(schedule_selector(HelloWorld::createMonster), 1.5f, kRepeatForever, 0);
	schedule(schedule_selector(HelloWorld::updateMonster), 0.1f, kRepeatForever, 0);
    return true;
}

void HelloWorld::updateCustom(float dt) {
	if (dtime > 0) {
		dtime--;
	}
	time->setString(Value(dtime).asString());
}
void HelloWorld::createMonster(float dt) {
	auto factory = Factory::getInstance();
	auto monster = factory->createMonster();
	float x = random(origin.x, visibleSize.width);
	float y = random(origin.y, visibleSize.height);
	monster->setPosition(x, y);
	addChild(monster);
	auto luciaPos = player->getPosition();
	factory->moveMonster(luciaPos, 0.5f);
}
void HelloWorld::updateMonster(float dt) {
	auto factory = Factory::getInstance();
	Rect playerRect = player->getBoundingBox();
	auto mons = factory->collider(playerRect);
	if (mons) {
		factory->removeMonster(mons);
		auto progressTo = ProgressTo::create(1.5, pT->getPercentage() - 20);
		pT->runAction(progressTo);
		
	}
	if (pT->getPercentage() == 0) {
		auto animation = Animation::createWithSpriteFrames(dead, 0.1f);
		auto animate = Animate::create(animation);
		player->runAction(animate);
		unscheduleAllCallbacks();
		isDead = true;
		killed->setString("GameOver!");
		int highest = database->getIntegerForKey("highest");
		if (highest < kill) {
			database->setIntegerForKey("highest", kill);
			highest_score->setString(Value(kill).asString());
		}
	}
}
void HelloWorld::move(Ref* pSender, char cid) {
	if (isDead) {
		return;
	}
	if (player->getNumberOfRunningActions() == 0) {
		int offset_x = 0, offset_y = 0;
		switch (cid) {
			case 'W':
				offset_y = 30;
				break;
			case 'A':
				if (lastCid != 'A') {
					player->setFlipX(true);
				}
				lastCid = 'A';
				offset_x = -30;
				break;
			case 'S':
				offset_y = -30;
				break;
			case 'D':
				if (lastCid != 'D') {
					player->setFlipX(false);
				}
				lastCid = 'D';
				offset_x = 30;
				break;
		}
		auto luciaPos = player->getPosition();
		if (luciaPos.y + offset_y < visibleSize.height && luciaPos.y + offset_y > 0 &&
			luciaPos.x + offset_x < visibleSize.width && luciaPos.x + offset_x > 0) {
			auto moveTo = MoveTo::create(0.3, Vec2(luciaPos.x + offset_x, luciaPos.y + offset_y));
			player->runAction(moveTo);
		}
			auto animation = Animation::createWithSpriteFrames(run, 0.3f / 4.0f);
			auto animate = Animate::create(animation);
			player->runAction(animate);
	}
}
void HelloWorld::die(Ref* pSender) {
}
void HelloWorld::hit(Ref* pSender) {
	if (isDead) {
		return;
	}
	if (player->getNumberOfRunningActions() == 0) {
		auto animation = Animation::createWithSpriteFrames(attack, 0.1f);
		auto animate = Animate::create(animation);
		player->runAction(animate);
		Rect playerRect = player->getBoundingBox();
		Rect attackRect = Rect(playerRect.getMinX() - 40, playerRect.getMinY(), playerRect.getMaxX() - playerRect.getMinX() + 80,
			playerRect.getMaxY() - playerRect.getMinY());
		auto factory = Factory::getInstance();
		auto mons = factory->collider(attackRect);
		if (mons) {
			factory->removeMonster(mons);
			auto progressTo = ProgressTo::create(1.5, pT->getPercentage() + 20);
			pT->runAction(progressTo);
			kill++;
			database->setIntegerForKey("killed", kill);
			killed->setString(Value(kill).asString());
		}
	}
}
