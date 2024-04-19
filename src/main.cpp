#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <random>

using namespace geode::prelude;

class $modify(MyLevelSearchLayer, LevelSearchLayer) {

	bool init(int p0) {
		if (!LevelSearchLayer::init(p0)) {
			return false;
		}

		if(!p0){

			auto followedButton = this->getChildByIDRecursive("followed-button");

			auto winSize = CCDirector::sharedDirector()->getWinSize();

			auto followedSprite = CCSprite::createWithSpriteFrameName("gj_heartOn_001.png");

			auto followedButtonNew = CCMenuItemSpriteExtra::create(followedSprite, this, menu_selector(LevelSearchLayer::onFollowed));

			auto bottomLeftMenu = this->getChildByIDRecursive("bottom-left-menu");

			auto quickSearchMenu = this->getChildByIDRecursive("quick-search-menu");

			auto randomButtonSprite = SearchButton::create("GJ_longBtn04_001.png", "Random", 0.5f, "GJ_sFollowedIcon_001.png");
			
			auto randomSprite = CCSprite::create("random.png"_spr);

			auto oldSprite = dynamic_cast<CCSprite*>(randomButtonSprite->getChildren()->objectAtIndex(1));
			oldSprite->setVisible(false);

			randomSprite->setPosition(oldSprite->getPosition());

			randomButtonSprite->addChild(randomSprite);

			auto randomTabButton = CCMenuItemSpriteExtra::create(randomButtonSprite, this, menu_selector(MyLevelSearchLayer::onRandom));

			if(followedButton){
				followedButton->setVisible(false);
				randomTabButton->setPosition(followedButton->getPosition());
			}

			if(quickSearchMenu){
				quickSearchMenu->addChild(randomTabButton);
				quickSearchMenu->setZOrder(1);
			}
			if(bottomLeftMenu){
				bottomLeftMenu->addChild(followedButtonNew);
				bottomLeftMenu->updateLayout();
			}

		}
		return true;
	}

	void onRandom(CCObject* obj){
		std::ostringstream query;
		bool addSeparator = false;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(128, 150000000);

		for(int n = 0; n < 50; n++){
			if(addSeparator) query << ",";
			query << distr(gen);
			addSeparator = true;
		}

		m_searchInput->onClickTrackNode(false);

		auto searchObject = GJSearchObject::create(SearchType::Type19, query.str());
		auto browserLayer = LevelBrowserLayer::scene(searchObject);
		auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
		CCDirector::sharedDirector()->pushScene(transitionFade);
	}

};
