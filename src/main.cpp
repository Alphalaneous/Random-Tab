#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <random>
#include <alphalaneous.pages_api/include/PageMenu.h>

using namespace geode::prelude;

class $modify(MyLevelBrowserLayer, LevelBrowserLayer) {

	struct Fields {
		bool m_isRandomTab = false;
	};

	void setRandomTab(){
		m_fields->m_isRandomTab = true;

		if(auto menu = getChildByID("search-menu")){
			menu->setVisible(false);
		}
		if(auto menu = getChildByID("prev-page-menu")){
			menu->setVisible(false);
		}
		if(auto menu = getChildByID("next-page-menu")){
			menu->setVisible(false);
		}
		if(auto menu = getChildByID("page-menu")){
			menu->setVisible(false);
		}
		if(auto label = getChildByID("level-count-label")){
			label->setVisible(false);
		}
		if(m_fields->m_isRandomTab){
			if(m_list){
				if(auto label = typeinfo_cast<CCLabelBMFont*>(m_list->getChildByID("title"))){
					label->setString("Random Levels");
				}
			}
		}
		
		CCSprite* bg = CCSprite::create("background.png"_spr);
		bg->setUserObject("width", CCFloat::create(bg->getContentSize().width));
		bg->setUserObject("height", CCFloat::create(bg->getContentSize().height));
		bg->setID("bg-overlay"_spr);
		bg->setScale(0.8f);
		bg->setOpacity(25);
        bg->setBlendFunc({ GL_ONE, GL_ONE_MINUS_CONSTANT_ALPHA });
		ccTexParams params = {GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT};
        bg->getTexture()->setTexParameters(&params);
        
        bg->setZOrder(-1);
        bg->setAnchorPoint({0, 0});
		bg->setRotation(10);
		CCSize winSize = CCDirector::get()->getWinSize();

		bg->setPosition({-winSize.width/2, -winSize.height/2});

        auto rect = bg->getTextureRect();
        rect.size = rect.size * (CCPoint(winSize) / CCPoint(bg->getScaledContentSize()) * 2);
        rect.origin = CCPoint{0, 0};
        bg->setTextureRect(rect);
		addChild(bg);

		animateBG(0);
		schedule(schedule_selector(MyLevelBrowserLayer::animateBG));
	}

	void animateBG(float dt){

		CCSprite* bg = typeinfo_cast<CCSprite*>(getChildByID("bg-overlay"_spr));

		auto width = static_cast<CCFloat*>(bg->getUserObject("width"))->getValue();
		auto height = static_cast<CCFloat*>(bg->getUserObject("height"))->getValue();

		auto rect = bg->getTextureRect();

		static float dX;
		static float dY;

		rect.origin.x = dX;
		rect.origin.y = dY;

		dX = rect.origin.x - 2 * dt;
		dY = rect.origin.y + 6 * dt;

		if(dX >= std::abs(width)) {
			dX = 0;
		}

		if(dY >= std::abs(height)) {
			dY = 0;
		}

		rect.origin = CCPoint{dX, dY};

        bg->setTextureRect(rect);
	}

	void onRefresh(cocos2d::CCObject* sender){

		if(m_fields->m_isRandomTab){
			onRandom(sender);
		}
		else{
			LevelBrowserLayer::onRefresh(sender);
		}
	}

	void onRandom(CCObject* sender){
		std::ostringstream query;
		bool addSeparator = false;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(128, 150000000);

		for(int n = 0; n < 100; n++){
			if(addSeparator) query << ",";
			query << distr(gen);
			addSeparator = true;
		}

		auto searchObject = GJSearchObject::create(SearchType::Type19, query.str());

		auto browserLayer = static_cast<MyLevelBrowserLayer*>(LevelBrowserLayer::create(searchObject));
		browserLayer->setRandomTab();

		auto scene = CCScene::create();
		scene->addChild(browserLayer);
		CCDirector::sharedDirector()->replaceScene(scene);
	}

	void setupLevelBrowser(cocos2d::CCArray* p0){
		LevelBrowserLayer::setupLevelBrowser(p0);
		if(m_fields->m_isRandomTab){
			if(m_list){
				if(auto label = typeinfo_cast<CCLabelBMFont*>(m_list->getChildByID("title"))){
					label->setString("Random Levels");
				}
			}
		}
	}

};

class $modify(MyLevelSearchLayer, LevelSearchLayer) {
	
	bool init(int p0) {
		if (!LevelSearchLayer::init(p0)) {
			return false;
		}

		if(!p0){

			auto winSize = CCDirector::sharedDirector()->getWinSize();

			auto quickSearchMenu = this->getChildByIDRecursive("quick-search-menu");

			auto randomButtonSprite = SearchButton::create("GJ_longBtn04_001.png", "Random", 0.5f, "GJ_sFollowedIcon_001.png");
			
			auto randomSprite = CCSprite::create("random.png"_spr);

			auto oldSprite = dynamic_cast<CCSprite*>(randomButtonSprite->getChildren()->objectAtIndex(1));
			oldSprite->setVisible(false);

			randomSprite->setPosition(oldSprite->getPosition());

			randomButtonSprite->addChild(randomSprite);

			auto randomTabButton = CCMenuItemSpriteExtra::create(randomButtonSprite, this, menu_selector(MyLevelSearchLayer::onRandom));

			if(quickSearchMenu){
				quickSearchMenu->addChild(randomTabButton);
			}

			RowLayout* layout = RowLayout::create();
            layout->setGrowCrossAxis(true);
            layout->setCrossAxisOverflow(false);
            layout->setAxisAlignment(AxisAlignment::Center);
            layout->setCrossAxisAlignment(AxisAlignment::Center);
            layout->ignoreInvisibleChildren(true);

            quickSearchMenu->setContentSize({365, 116});
            quickSearchMenu->ignoreAnchorPointForPosition(false);
			
            CCSize winSize = CCDirector::get()->getWinSize();
            quickSearchMenu->setPosition({quickSearchMenu->getPosition().x, winSize.height/2 + 28});

            PageMenu* menuPage = PageMenu::create(typeinfo_cast<CCMenu*>(quickSearchMenu), layout, 9);

            addChild(menuPage);
		}
		return true;
	}

	void onRandom(CCObject* obj){
		std::ostringstream query;
		bool addSeparator = false;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(128, 150000000);

		for(int n = 0; n < 100; n++){
			if(addSeparator) query << ",";
			query << distr(gen);
			addSeparator = true;
		}

		m_searchInput->onClickTrackNode(false);

		auto searchObject = GJSearchObject::create(SearchType::Type19, query.str());

		auto browserLayer = static_cast<MyLevelBrowserLayer*>(LevelBrowserLayer::create(searchObject));
		browserLayer->setRandomTab();

		auto scene = CCScene::create();
		scene->addChild(browserLayer);
		auto transitionFade = CCTransitionFade::create(0.5, scene);
		CCDirector::sharedDirector()->pushScene(transitionFade);
	}

};
