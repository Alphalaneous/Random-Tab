#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <random>
#include <alphalaneous.pages_api/include/PageMenu.h>
#include "RandomLayer.hpp"

using namespace geode::prelude;

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

			auto oldSprite = typeinfo_cast<CCSprite*>(randomButtonSprite->getChildren()->objectAtIndex(1));
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
            quickSearchMenu->setPosition({quickSearchMenu->getPosition().x, winSize.height/2 + 28});
            quickSearchMenu->setLayout(layout);

            static_cast<PageMenu*>(quickSearchMenu)->setPaged(9, PageOrientation::HORIZONTAL, 422);
		}
		return true;
	}

	void onRandom(CCObject* obj){
		std::ostringstream query;
		bool addSeparator = false;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(128, 150000000);

		for(int n = 0; n < 10000; n++){
			if(addSeparator) query << ",";
			query << distr(gen);
			addSeparator = true;
		}

		m_searchInput->onClickTrackNode(false);
		auto transitionFade = CCTransitionFade::create(0.5, RandomLayer::scene());
		CCDirector::sharedDirector()->pushScene(transitionFade);
	}

};
