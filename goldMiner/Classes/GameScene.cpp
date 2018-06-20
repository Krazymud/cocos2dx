#include "GameScene.h"

USING_NS_CC;

Scene* GameSence::createScene()
{
	return GameSence::create();
}

// on "init" you need to initialize your instance
bool GameSence::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}
	auto parent = this;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg = Sprite::create("level-background-0.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(bg, 0);

	stoneLayer = Layer::create();
	this->addChild(stoneLayer);
	stoneLayer->setAnchorPoint(Vec2::ZERO);
	auto stone = Sprite::create("stone.png");
	stone->setPosition(Vec2(560 + origin.x, 480 + origin.y));
	this->stone = stone;
	stoneLayer->addChild(stone);

	const char *shoot = "Shoot";
	MenuItemFont::setFontSize(70);
	auto shootItem = MenuItemFont::create(shoot,
										CC_CALLBACK_1(GameSence::shootMenuCallback, this));
	auto shootMenu = Menu::create(shootItem, NULL);
	shootMenu->setPosition(Vec2(visibleSize.width - 160 + origin.x, visibleSize.height - 160 + origin.y));
	this->addChild(shootMenu, 1);

	mouseLayer = Layer::create();
	this->addChild(mouseLayer);
	mouseLayer->setAnchorPoint(Vec2::ZERO);
	auto mouse = Sprite::createWithSpriteFrameName("mouse-0.png");
	Animate* mouseAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
	mouse->runAction(RepeatForever::create(mouseAnimate));
	mouse->setPosition(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);
	this->mouse = mouse;
	mouseLayer->addChild(mouse);
	
	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameSence::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

bool GameSence::onTouchBegan(Touch *touch, Event *unused_event) {
	auto location = touch->getLocation();
	auto cheese = Sprite::create("cheese.png");
	cheese->setPosition(location);
	stoneLayer->addChild(cheese);
	auto fadeOut = FadeOut::create(5.0f);
	cheese->runAction(fadeOut);
	auto moveTo = MoveTo::create(1, location);
	mouse->runAction(moveTo);
	return true;
}

void GameSence::shootMenuCallback(Ref* pSender) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	int width = visibleSize.width;
	int height = visibleSize.height;
	Vec2 mousePos = mouse->getPosition();
	Vec2 stonePos = stone->getPosition();
	// shoot a stone
	auto shootStone = Sprite::create("stone.png");
	shootStone->setPosition(stonePos);
	mouseLayer->addChild(shootStone, 1);
	auto moveTo = MoveTo::create(0.5, mousePos);
	auto fadeOut = FadeOut::create(0.3f);
	auto seq = Sequence::create(moveTo, fadeOut, nullptr);
	shootStone->runAction(seq);
	// leave a diamond
	auto diamond = Sprite::create("diamond.png");
	Animate* diamondAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("diamondAnimation"));
	diamond->runAction(RepeatForever::create(diamondAnimate));
	diamond->setPosition(mousePos);
	stoneLayer->addChild(diamond);
	// mouse runaway
	int x = (int)(rand_0_1() * width);
	int y = (int)(rand_0_1() * height);
	moveTo = MoveTo::create(1.5, Vec2(x, y));
	mouse->runAction(moveTo);
}
