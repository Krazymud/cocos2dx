#pragma execution_character_set("utf-8")
#include "HitBrick.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#define database UserDefault::getInstance()
#define SHIP 1
#define BALL 2
#define PLAYER 3
#define BOX 4

USING_NS_CC;
using namespace CocosDenshion;

void HitBrick::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* HitBrick::createScene() {
  srand((unsigned)time(NULL));
  auto scene = Scene::createWithPhysics();

  scene->getPhysicsWorld()->setAutoStep(true);

  // Debug 模式
  // scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
  scene->getPhysicsWorld()->setGravity(Vec2(0, -300.0f));
  auto layer = HitBrick::create();
  layer->setPhysicsWorld(scene->getPhysicsWorld());
  layer->setJoint();
  scene->addChild(layer);
  return scene;
}

// on "init" you need to initialize your instance
bool HitBrick::init() {
  //////////////////////////////
  // 1. super init first
  if (!Layer::init()) {
    return false;
  }
  visibleSize = Director::getInstance()->getVisibleSize();


  auto edgeSp = Sprite::create();  //创建一个精灵
  auto boundBody = PhysicsBody::createEdgeBox(visibleSize, PhysicsMaterial(0.0f, 1.0f, 0.0f), 3);  //edgebox是不受刚体碰撞影响的一种刚体，我们用它来设置物理世界的边界
  edgeSp->setPosition(visibleSize.width / 2, visibleSize.height / 2);  //位置设置在屏幕中央
  edgeSp->setPhysicsBody(boundBody);
  addChild(edgeSp);


  preloadMusic(); // 预加载音效

  addSprite();    // 添加背景和各种精灵
  addListener();  // 添加监听器 
  addPlayer();    // 添加球与板
  BrickGeneraetd();  // 生成砖块

  addTouchListener();
  schedule(schedule_selector(HitBrick::update), 0.01f, kRepeatForever, 0.1f);
  schedule(schedule_selector(HitBrick::move), 0.005f, kRepeatForever, 0.1f);
  schedule(schedule_selector(HitBrick::show), 0.1f, kRepeatForever, 0.1f);
  onBall = true;

  spFactor = 0;
  return true;
}

// 关节连接，固定球与板子
// Todo
void HitBrick::setJoint() {
	joint1 = PhysicsJointPin::construct(
		player->getPhysicsBody(), ball->getPhysicsBody(),
		player->getAnchorPoint() + Vec2(0, player->getBoundingBox().size.height / 2),
		ball->getAnchorPoint() - Vec2(0, ball->getBoundingBox().size.height / 2)
	);
	m_world->addJoint(joint1);
}



// 预加载音效
void HitBrick::preloadMusic() {
  auto sae = SimpleAudioEngine::getInstance();
  sae->preloadEffect("gameover.mp3");
  sae->preloadBackgroundMusic("bgm.mp3");
  sae->playBackgroundMusic("bgm.mp3", true);
}

// 添加背景和各种精灵
void HitBrick::addSprite() {
  // add background
  auto bgSprite = Sprite::create("bg.png");
  bgSprite->setPosition(visibleSize / 2);
  bgSprite->setScale(visibleSize.width / bgSprite->getContentSize().width, visibleSize.height / bgSprite->getContentSize().height);
  this->addChild(bgSprite, 0);


  // add ship
  ship = Sprite::create("ship.png");
  ship->setScale(visibleSize.width / ship->getContentSize().width * 0.97, 1.2f);
  ship->setPosition(visibleSize.width / 2, 0);
  auto shipbody = PhysicsBody::createBox(ship->getContentSize(), PhysicsMaterial(100.0f, 0.0f, 1.0f));
  shipbody->setCategoryBitmask(0xFFFFFFFF);
  shipbody->setCollisionBitmask(0xFFFFFFFF);
  shipbody->setContactTestBitmask(0xFFFFFFFF);
  ship->setTag(SHIP);
  shipbody->setDynamic(false);  // ??????岻?????????, ????????????б??
  ship->setPhysicsBody(shipbody);
  this->addChild(ship, 1);

  // add sun and cloud
  auto sunSprite = Sprite::create("sun.png");
  sunSprite->setPosition(rand() % (int)(visibleSize.width - 200) + 100, 550);
  this->addChild(sunSprite);
  auto cloudSprite1 = Sprite::create("cloud.png");
  cloudSprite1->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
  this->addChild(cloudSprite1);
  auto cloudSprite2 = Sprite::create("cloud.png");
  cloudSprite2->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
  this->addChild(cloudSprite2);
}

// 添加监听器
void HitBrick::addListener() {
  auto keyboardListener = EventListenerKeyboard::create();
  keyboardListener->setEnabled(true);
  keyboardListener->onKeyPressed = CC_CALLBACK_2(HitBrick::onKeyPressed, this);
  keyboardListener->onKeyReleased = CC_CALLBACK_2(HitBrick::onKeyReleased, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

  auto contactListener = EventListenerPhysicsContact::create();
  contactListener->onContactBegin = CC_CALLBACK_1(HitBrick::onConcactBegin, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

// 创建角色
void HitBrick::addPlayer() {



  player = Sprite::create("bar.png");
  int xpos = visibleSize.width / 2;

  player->setScale(0.1f, 0.1f);
  player->setPosition(Vec2(xpos, ship->getContentSize().height - player->getContentSize().height*0.1f));
  player->setTag(PLAYER);
  // 设置板的刚体属性
  // Todo
  auto playerbody = PhysicsBody::createBox(player->getContentSize(), PhysicsMaterial(1.0f, 1.0f, 0.0f));
  playerbody->setCategoryBitmask(0xFFFFFFFF);
  playerbody->setCollisionBitmask(0xFFFFFFFF);
  playerbody->setContactTestBitmask(0xFFFFFFFF);
  playerbody->setDynamic(false);
  player->setPhysicsBody(playerbody);

  this->addChild(player, 2);
  
  ball = Sprite::create("ball.png");
  ball->setPosition(Vec2(xpos, player->getPosition().y + ball->getContentSize().height*0.1f));
  ball->setScale(0.1f, 0.1f);
  ball->setTag(BALL);
  // 设置球的刚体属性
  // Todo
  auto ballbody = PhysicsBody::createBox(ball->getContentSize(), PhysicsMaterial(1.0f, 1.0f, 0.0f));
  ballbody->setCategoryBitmask(0xFFFFFFFF);
  ballbody->setCollisionBitmask(0xFFFFFFFF);
  ballbody->setContactTestBitmask(0xFFFFFFFF);
  ballbody->setDynamic(true);
  ballbody->setRotationEnable(true);
  ballbody->setGravityEnable(false);
  ball->setPhysicsBody(ballbody);
  addChild(ball, 3);
  
}

// 实现简单的蓄力效果
// Todo
void HitBrick::update(float dt) {
	if (spHolded == true) {
		spFactor++;
	}
}


void HitBrick::show(float dt) {
	emitter = ParticleFire::create();
	emitter->setDuration(0.1f);
	emitter->setPosition(ball->getPosition());
	addChild(emitter);
}

void HitBrick::move(float dt) {
	auto a = player->getPhysicsBody()->getVelocity();
	log("%f", a.x);
	if (isLPressed == true) {
		if (player->getPosition().x - 65 > 0) {
			player->setPosition(Vec2(player->getPosition().x - 2, player->getPosition().y));
			player->getPhysicsBody()->setVelocity(Vec2(-100, 0));
		}
	}
	else if (isRPressed == true) {
		if (player->getPosition().x + 65 < visibleSize.width) {
			player->setPosition(Vec2(player->getPosition().x + 2, player->getPosition().y));
			player->getPhysicsBody()->setVelocity(Vec2(100, 0));
		}
	}
}


// Todo
void HitBrick::BrickGeneraetd() {

for (int i = 0; i < 3; i++) {
	int cw = 0;
	while (cw <= visibleSize.width) {
		auto box = Sprite::create("box.png");
		box->setTag(BOX);
		// 为砖块设置刚体属性
		// Todo
		auto boxbody = PhysicsBody::createBox(box->getContentSize(), PhysicsMaterial(1.0f, 1.0f, 0.0f));
		boxbody->setCategoryBitmask(0xFFFFFFFF);
		boxbody->setCollisionBitmask(0xFFFFFFFF);
		boxbody->setContactTestBitmask(0xFFFFFFFF);
		boxbody->setDynamic(false);
		box->setPhysicsBody(boxbody);
		box->setPosition(Vec2(cw + box->getContentSize().width / 2, visibleSize.height - (i + 1) * box->getContentSize().height));
		addChild(box, 3);
		cw += box->getContentSize().width + 5;
	}

 }

}


// 左右
void HitBrick::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		isLPressed = true;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		// 左右移动
		// Todo
		isRPressed = true;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE: // 开始蓄力
		spHolded = true;
		spFactor = 0;
		break;
	default:
		break;
	}
}

// 释放按键
void HitBrick::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	spFactor = (spFactor > 100) ? 100 : spFactor;
	double speed = 1000.0f * (spFactor / 100.0f);
	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		isLPressed = false;
		player->getPhysicsBody()->setVelocity(Vec2::ZERO);
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		// 停止运动
		// Todo
		player->getPhysicsBody()->setVelocity(Vec2::ZERO);
		isRPressed = false;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:   // 蓄力结束，小球发射
		m_world->removeJoint(joint1);
		ball->getPhysicsBody()->setVelocity(Vec2(0, speed));
		spHolded = false;
		spFactor = 0;
		break;
	default:
		break;
	}
}

// 碰撞检测
// Todo
bool HitBrick::onConcactBegin(PhysicsContact & contact) {
  auto c1 = contact.getShapeA(), c2 = contact.getShapeB();
  auto sp1 = c1->getBody()->getNode();
  auto sp2 = c2->getBody()->getNode();
  if (sp1 && sp2) {
	  if (sp1->getTag() == BOX && sp2->getTag() == BALL) {
		  sp1->removeFromParentAndCleanup(true);
	  }
	  else if (sp2->getTag() == BOX && sp1->getTag() == BALL) {
		  sp2->removeFromParentAndCleanup(true);
	  }
	  else if ((sp1->getTag() == BALL && sp2->getTag() == SHIP) || (sp2->getTag() == BALL && sp1->getTag() == SHIP)) {
		  GameOver();
	  }
  }
  return true;
}


void HitBrick::GameOver() {

	_eventDispatcher->removeAllEventListeners();
	unscheduleAllCallbacks();
	Director::getInstance()->getRunningScene()->getPhysicsWorld()->setGravity(Vec2(0, -10000.0f));
	ball->getPhysicsBody()->setGravityEnable(true);
	ball->getPhysicsBody()->setVelocity(Vec2(0, 0));
	player->getPhysicsBody()->setVelocity(Vec2(0, 0));
	
  SimpleAudioEngine::getInstance()->stopBackgroundMusic("bgm.mp3");
  SimpleAudioEngine::getInstance()->playEffect("gameover.mp3", false);

  auto label1 = Label::createWithTTF("Game Over~", "fonts/STXINWEI.TTF", 60);
  label1->setColor(Color3B(0, 0, 0));
  label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
  this->addChild(label1);

  auto label2 = Label::createWithTTF("重玩", "fonts/STXINWEI.TTF", 40);
  label2->setColor(Color3B(0, 0, 0));
  auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(HitBrick::replayCallback, this));
  Menu* replay = Menu::create(replayBtn, NULL);
  replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
  this->addChild(replay);

  auto label3 = Label::createWithTTF("退出", "fonts/STXINWEI.TTF", 40);
  label3->setColor(Color3B(0, 0, 0));
  auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(HitBrick::exitCallback, this));
  Menu* exit = Menu::create(exitBtn, NULL);
  exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
  this->addChild(exit);
}

// 继续或重玩按钮响应函数
void HitBrick::replayCallback(Ref * pSender) {
  Director::getInstance()->replaceScene(HitBrick::createScene());
}

// 退出
void HitBrick::exitCallback(Ref * pSender) {
  Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  exit(0);
#endif
}

// 添加触摸事件监听器
void HitBrick::addTouchListener() {
	// Todo
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(HitBrick::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(HitBrick::onTouchEnded, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(HitBrick::onTouchMoved, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

// 鼠标点击发射炮弹
bool HitBrick::onTouchBegan(Touch *touch, Event *event) {
	if (touch->getLocation().getDistance(player->getPosition()) <= 30) {
		isClick = true;
	}
	return true;
}

void HitBrick::onTouchEnded(Touch *touch, Event *event) {
	isClick = false;
}

// 当鼠标按住飞船后可控制飞船移动 (加分项)
void HitBrick::onTouchMoved(Touch *touch, Event *event) {
	// Todo
	Vec2 delta = touch->getDelta();
	auto moveBy = MoveBy::create(0.1, delta);
	player->runAction(moveBy);
}
