#include "RandomLayer.hpp"
#include <Geode/modify/LevelBrowserLayer.hpp>

class $modify(MyLevelBrowserLayer, LevelBrowserLayer) {

	struct Fields {
		bool m_isRandomTab = false;
	};

    void setRefreshButton(CCNode* target, SEL_MenuHandler selector) {
        if (auto menu = getChildByID("refresh-menu")) {
			if (auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("refresh-button"))) {
                btn->m_pListener = target;
                btn->m_pfnSelector = selector;
            }
		}
    }

	void setRandomTab(){
		m_fields->m_isRandomTab = true;

		if (auto menu = getChildByID("search-menu")) {
			menu->setVisible(false);
		}
		if (auto menu = getChildByID("prev-page-menu")) {
			menu->setVisible(false);
		}
		if (auto menu = getChildByID("next-page-menu")) {
			menu->setVisible(false);
		}
		if (auto menu = getChildByID("page-menu")) {
			menu->setVisible(false);
		}
		if (auto label = getChildByID("level-count-label")) {
			label->setVisible(false);
		}
		if (m_fields->m_isRandomTab) {
			if (m_list) {
				if (auto label = typeinfo_cast<CCLabelBMFont*>(m_list->getChildByID("title"))){
					label->setString("Random Levels");
				}
			}
		}
		
		auto bg = CCSprite::create("background.png"_spr);
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
		auto winSize = CCDirector::get()->getWinSize();

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

		auto bg = typeinfo_cast<CCSprite*>(getChildByID("bg-overlay"_spr));

		auto width = static_cast<CCFloat*>(bg->getUserObject("width"))->getValue();
		auto height = static_cast<CCFloat*>(bg->getUserObject("height"))->getValue();

		auto rect = bg->getTextureRect();

		static float dX;
		static float dY;

		rect.origin.x = dX;
		rect.origin.y = dY;

		dX = rect.origin.x - 2 * dt;
		dY = rect.origin.y + 6 * dt;

		if (dX >= std::abs(width)) {
			dX = 0;
		}

		if (dY >= std::abs(height)) {
			dY = 0;
		}

		rect.origin = CCPoint{dX, dY};

        bg->setTextureRect(rect);
	}

	void setupLevelBrowser(cocos2d::CCArray* p0){
		LevelBrowserLayer::setupLevelBrowser(p0);
		if (m_fields->m_isRandomTab) {
			if (m_list) {
				if (auto label = typeinfo_cast<CCLabelBMFont*>(m_list->getChildByID("title"))) {
					label->setString("Random Levels");
				}
			}
		}
	}
};

Toggle* createToggle(ZStringView text, CCNode* target, SEL_MenuHandler callback, ZStringView savedValue) {
    auto menu = Toggle::create();
    menu->ignoreAnchorPointForPosition(false);
    menu->setContentSize({180, 30});
    menu->setLayout(RowLayout::create()->setGap(0)->setAxisAlignment(AxisAlignment::Start)->setAutoScale(false));

    auto toggle = CCMenuItemToggler::createWithStandardSprites(target, callback, 0.5f);
    toggle->toggle(Mod::get()->getSavedValue<bool>(savedValue));
    auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    label->setAnchorPoint({0, 0.5f});
    label->setScale(0.4f);

    menu->addChild(toggle);
    menu->addChild(label);

    menu->updateLayout();

    menu->m_toggler = toggle;
    menu->m_label = label;

    return menu;
}

std::unordered_map<int, std::string> RandomLayer::parseLevel(ZStringView str) {
    std::unordered_map<int, std::string> result;

    size_t start = 0;
    int delimiterCount = 0;

    for (size_t i = 0; i <= str.length(); ++i) {
        if (i == str.length() || str.view()[i] == ':') {
            delimiterCount++;

            if (delimiterCount == 2 || i == str.length()) {
                std::string_view segment = str.view().substr(start, i - start);
                
                size_t delimPos = segment.find(':');
                if (delimPos != std::string::npos) {

                    int first = utils::numFromString<int>(segment.substr(0, delimPos)).unwrapOr(0);
                    std::string_view second = segment.substr(delimPos + 1);

                    result[first] = second;
                }

                start = i + 1;
                delimiterCount = 0;
            }
        }
    }

    return result;
}

std::unordered_map<int, std::unordered_map<int, std::vector<int>>> RandomLayer::parse(ZStringView str) {
    std::unordered_map<int, std::unordered_map<int, std::vector<int>>> result;

    size_t start = 0;
    int delimiterCount = 0;

    for (size_t i = 0; i <= str.length(); ++i) {
        if (i == str.length() || str.view()[i] == ',') {
            delimiterCount++;

            if (delimiterCount == 2 || i == str.length()) {
                std::string_view segment = str.view().substr(start, i - start);
                
                size_t delimPos = segment.find(',');
                if (delimPos != std::string::npos) {

                    std::string_view first = segment.substr(0, delimPos);
                    std::string_view second = segment.substr(delimPos + 1);

                    int ID = utils::numFromString<int>(first).unwrapOr(0);
                    int difficulty = utils::numFromString<int>(second).unwrapOr(0);
                    int internalDifficulty = 0;
                    switch (difficulty)
                    {
                    case 1:
                        internalDifficulty = 7;
                        break;
                    case 2:
                        internalDifficulty = 1;
                        break;
                    case 3:
                        internalDifficulty = 2;
                        break;
                    case 4:
                    case 5:
                        internalDifficulty = 3;
                        break;
                    case 6:
                    case 7: 
                        internalDifficulty = 4;
                        break;
                    case 8:
                    case 9:
                        internalDifficulty = 5;
                        break;
                    case 10:
                        internalDifficulty = 6;
                        break;
                    default:
                        break;
                    }
                    result[betweenWhich(ID)][internalDifficulty].push_back(ID);
                }

                start = i + 1;
                delimiterCount = 0;
            }
        }
    }

    return result;
}

int RandomLayer::betweenWhich(int id) {
    for (const auto& [k, v] : m_versionData) {
        if (id >= v.first && id <= v.second) {
            return k;
        }
    }
    return -1;
};

bool RandomLayer::init() {
    if (!CCLayer::init()) return false;

    auto winSize = CCDirector::get()->getWinSize();

    m_waitAlert = geode::createQuickPopup("Finding Level", "Please wait...", "Cancel", nullptr, [this] (FLAlertLayer*, bool confirm) {
        m_cancelled = true;
        for (const auto& [k, v] : m_listeners) {
            m_listeners[k].cancel();
            m_listeners.erase(k);
        }
    }, false);

    unsigned long fileSize = 0;
    unsigned char* buffer = CCFileUtils::get()->getFileData("LevelStats.dat", "rb", &fileSize);    

    if (buffer && fileSize != 0) {
        std::string data = std::string(reinterpret_cast<char*>(buffer), fileSize);
        m_ratedLevels = parse(data);
    }

    m_versionButtons = CCArray::create();
    m_difficultyButtons = CCArray::create();

    m_filteredVersions = Mod::get()->getSavedValue<std::vector<int>>("version-filter");
    m_filteredDifficulties = Mod::get()->getSavedValue<std::vector<int>>("difficulty-filter");

    auto backMenu = CCMenu::create();
    backMenu->setContentSize({48, 48});
    backMenu->ignoreAnchorPointForPosition(false);
    backMenu->setAnchorPoint({0, 1});
    backMenu->setPosition({0, winSize.height});

    auto backSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    auto backBtn = CCMenuItemSpriteExtra::create(backSprite, this, menu_selector(RandomLayer::onBack));
    backBtn->setPosition(backMenu->getContentSize()/2);

    backMenu->addChild(backBtn);

    auto clearFiltersMenu = CCMenu::create();
    clearFiltersMenu->setContentSize({48, 48});
    clearFiltersMenu->ignoreAnchorPointForPosition(false);
    clearFiltersMenu->setAnchorPoint({1, 1});
    clearFiltersMenu->setPosition({winSize.width, winSize.height});

    auto clearSprite = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    clearSprite->setScale(0.8f);
    auto clearBtn = CCMenuItemSpriteExtra::create(clearSprite, this, menu_selector(RandomLayer::onClearFilters));
    clearBtn->setPosition(clearFiltersMenu->getContentSize()/2);
    
    clearFiltersMenu->addChild(clearBtn);

    auto background = CCSprite::create("GJ_gradientBG.png");
    background->setPosition({-5, -5});

    float scaleX = (winSize.width + 10) / background->getContentWidth();
    float scaleY = (winSize.height + 10) / background->getContentHeight();

    background->setScaleX(scaleX);
    background->setScaleY(scaleY);
    background->setZOrder(-3);
    background->setColor({0, 102, 255});
    background->setAnchorPoint({0, 0});

    auto logo = CCSprite::create("random-tab-logo.png"_spr);
    logo->setPosition({winSize.width/2, winSize.height - 40});
    addChild(logo);

    auto bottomLeftCorner = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    bottomLeftCorner->setPosition({-1, -1});
    bottomLeftCorner->setAnchorPoint({0, 0});

    auto bottomRightCorner = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    bottomRightCorner->setPosition({winSize.width + 1, -1});
    bottomRightCorner->setAnchorPoint({1, 0});
    bottomRightCorner->setFlipX(true);

    auto topLeftCorner = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    topLeftCorner->setPosition({-1, winSize.height + 1});
    topLeftCorner->setAnchorPoint({0, 1});
    topLeftCorner->setFlipY(true);

    auto topRightCorner = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    topRightCorner->setPosition({winSize.width + 1, winSize.height + 1});
    topRightCorner->setAnchorPoint({1, 1});
    topRightCorner->setFlipX(true);
    topRightCorner->setFlipY(true);

    auto searchBG = geode::NineSlice::create("square02b_001.png");
    searchBG->setContentSize({365, 50});
    searchBG->setPosition({winSize.width/2, winSize.height/2 + 60});
    searchBG->setColor({0, 56, 141});

    addChild(searchBG);

    auto searchMenu = CCMenu::create();
    searchMenu->setContentSize({365, 50});
    searchMenu->setPosition({winSize.width/2, winSize.height/2 + 60});
    searchMenu->ignoreAnchorPointForPosition(false);
    searchMenu->setLayout(RowLayout::create()->setGap(10));
    searchMenu->setZOrder(1);

    addChild(searchMenu);

    auto listButtonSpr = CCSprite::createWithSpriteFrameName("GJ_longBtn03_001.png");
    auto listButtonLabel = CCLabelBMFont::create("Randomize Them!", "bigFont.fnt");
    listButtonLabel->setPosition(listButtonSpr->getContentSize()/2);
    listButtonLabel->setScale(0.5f);
    listButtonLabel->setPositionY(listButtonLabel->getPositionY() + 1);
    listButtonSpr->addChild(listButtonLabel);

    m_listButton = CCMenuItemSpriteExtra::create(listButtonSpr, this, menu_selector(RandomLayer::onRandomList));
    listButtonSpr->setCascadeColorEnabled(true);
    listButtonSpr->setCascadeOpacityEnabled(true);

    auto randomLevelSpr = CCSprite::createWithSpriteFrameName("GJ_longBtn03_001.png");
    auto randomLevelLabel = CCLabelBMFont::create("I'm Feeling Lucky!", "bigFont.fnt");
    randomLevelLabel->setPosition(randomLevelSpr->getContentSize()/2);
    randomLevelLabel->setScale(0.5f);
    randomLevelLabel->setPositionY(randomLevelLabel->getPositionY() + 1);
    randomLevelSpr->addChild(randomLevelLabel);

    auto randomLevelButton = CCMenuItemSpriteExtra::create(randomLevelSpr, this, menu_selector(RandomLayer::onRandomLevel));
    randomLevelSpr->setCascadeColorEnabled(true);
    randomLevelSpr->setCascadeOpacityEnabled(true);

    searchMenu->addChild(m_listButton);
    searchMenu->addChild(randomLevelButton);

    searchMenu->updateLayout();

    auto filtersLabel = CCLabelBMFont::create("Filters", "bigFont.fnt");
    filtersLabel->setPosition({winSize.width/2, winSize.height/2 + 20});
    filtersLabel->setScale(0.5f);

    auto toggles = CCNode::create();
    toggles->setContentSize({360, 30});
    toggles->setPosition({winSize.width/2, winSize.height/2 - 10});
    toggles->ignoreAnchorPointForPosition(false);
    toggles->setAnchorPoint({0.5f, 0.5f});
    toggles->setLayout(RowLayout::create()->setGap(0)->setAxisAlignment(AxisAlignment::Start)->setAutoScale(false));
    toggles->setZOrder(1);

    m_ratedToggle = createToggle("Rated Levels", this, menu_selector(RandomLayer::onRatedToggle), "rated-filter");
    m_completedToggle = createToggle("Completed Levels", this, menu_selector(RandomLayer::onCompletedToggle), "completed-filter");
    toggles->addChild(m_ratedToggle);

    m_ratedOnly = Mod::get()->getSavedValue<bool>("rated-filter");
    m_completedOnly = Mod::get()->getSavedValue<bool>("completed-filter");
    //toggles->addChild(m_completedToggle);

    auto togglesBG = geode::NineSlice::create("square02b_001.png");
    togglesBG->setContentSize({365, 36});
    togglesBG->setPosition({winSize.width/2, winSize.height/2 - 10});
    togglesBG->setColor({0, 46, 117});

    toggles->updateLayout();

    addChild(toggles);

    m_difficultiesMenu = CCMenu::create();
    m_difficultiesMenu->setContentSize({365, 50});
    m_difficultiesMenu->setPosition({winSize.width/2, winSize.height/2 - 60});
    m_difficultiesMenu->ignoreAnchorPointForPosition(false);
    m_difficultiesMenu->setLayout(RowLayout::create()->setGap(20));
    m_difficultiesMenu->setZOrder(1);

    auto difficultiesBG = geode::NineSlice::create("square02b_001.png");
    difficultiesBG->setContentSize({365, 50});
    difficultiesBG->setPosition({winSize.width/2, winSize.height/2 - 60});
    difficultiesBG->setColor({0, 36, 91});

    auto versionsMenu = CCMenu::create();
    versionsMenu->setContentSize({350, 50});
    versionsMenu->setPosition({winSize.width/2, winSize.height/2 - 120});
    versionsMenu->ignoreAnchorPointForPosition(false);
    versionsMenu->setLayout(RowLayout::create()->setGap(5)->setGrowCrossAxis(true)->setAutoScale(false));
    versionsMenu->setZOrder(1);

    versionsMenu->addChild(createVersionButton(" 1.0 ", 0));
    versionsMenu->addChild(createVersionButton(" 1.1 ", 1));
    versionsMenu->addChild(createVersionButton(" 1.2 ", 2));
    versionsMenu->addChild(createVersionButton(" 1.3 ", 3));
    versionsMenu->addChild(createVersionButton(" 1.4 ", 4));
    versionsMenu->addChild(createVersionButton(" 1.5 ", 5));
    versionsMenu->addChild(createVersionButton(" 1.6 ", 6));
    versionsMenu->addChild(createVersionButton(" 1.7 ", 7));
    versionsMenu->addChild(createVersionButton(" 1.8 ", 8));
    versionsMenu->addChild(createVersionButton(" 1.9 ", 9));
    versionsMenu->addChild(createVersionButton(" 2.0 ", 10));
    versionsMenu->addChild(createVersionButton(" 2.1 ", 11));
    m_twoPointTwoButton = createVersionButton(" 2.2 ", 12);
    versionsMenu->addChild(m_twoPointTwoButton);
    m_randomButton = createVersionButton(" Random ", -2);
    versionsMenu->addChild(m_randomButton);

    versionsMenu->updateLayout();

    auto versionsBG = geode::NineSlice::create("square02b_001.png");
    versionsBG->setContentSize({365, 50});
    versionsBG->setPosition({winSize.width/2, winSize.height/2 - 120});
    versionsBG->setColor({0, 31, 79});

    m_difficultiesMenu->addChild(createDifficultyButton("difficulty_01_btn_001.png", 1));
    m_difficultiesMenu->addChild(createDifficultyButton("difficulty_02_btn_001.png", 2));
    m_difficultiesMenu->addChild(createDifficultyButton("difficulty_03_btn_001.png", 3));
    m_difficultiesMenu->addChild(createDifficultyButton("difficulty_04_btn_001.png", 4));
    m_difficultiesMenu->addChild(createDifficultyButton("difficulty_05_btn_001.png", 5));
    m_difficultiesMenu->addChild(createDifficultyButton("difficulty_06_btn_001.png", 6));
    m_difficultiesMenu->addChild(createDifficultyButton("difficulty_auto_btn_001.png", 7));
    
    m_difficultiesMenu->updateLayout();

    addChild(m_difficultiesMenu);
    addChild(versionsMenu);

    addChild(filtersLabel);
    addChild(difficultiesBG);
    addChild(versionsBG);
    addChild(togglesBG);

    addChild(bottomLeftCorner);
    addChild(bottomRightCorner);
    addChild(topLeftCorner);
    addChild(topRightCorner);

    addChild(background);
    addChild(backMenu);
    addChild(clearFiltersMenu);

    if (!m_ratedOnly) {
        m_difficultiesMenu->setOpacity(127);
        m_difficultiesMenu->setColor({125, 125, 125});
        m_difficultiesMenu->setEnabled(false);
    }

    if (!m_filteredDifficulties.empty()) {
        m_twoPointTwoButton->setColor({125, 50, 50});
        m_twoPointTwoButton->setEnabled(false);
    }

    ratedAnd2p2Check();

    setKeypadEnabled(true);

    return true;
}

CCMenuItemSpriteExtra* RandomLayer::createVersionButton(ZStringView label, int version) {

    auto versionLabel = CCLabelBMFont::create(label.c_str(), "bigFont.fnt");
    versionLabel->setScale(0.5);

    auto btn = CCMenuItemSpriteExtra::create(versionLabel, this, menu_selector(RandomLayer::onVersionButton));
    if (!isVersionSelected(version)) {
        btn->setColor({125, 125, 125});
    }

    btn->setTag(version);

    m_versionButtons->addObject(btn);

    return btn;
}

CCMenuItemSpriteExtra* RandomLayer::createDifficultyButton(ZStringView texture, int difficulty) {

    auto difficultySprite = CCSprite::createWithSpriteFrameName(texture.c_str());
    difficultySprite->setScale(0.8f);

    auto btn = CCMenuItemSpriteExtra::create(difficultySprite, this, menu_selector(RandomLayer::onDifficultyButton));
    if (!isDifficultySelected(difficulty)) {
        btn->setColor({125, 125, 125});
    }
    btn->setTag(difficulty);

    m_difficultyButtons->addObject(btn);

    return btn;
}

bool RandomLayer::isVersionSelected(int version) {
    return std::find(m_filteredVersions.begin(), m_filteredVersions.end(), version) != m_filteredVersions.end();
}

bool RandomLayer::isDifficultySelected(int difficulty) {
    return std::find(m_filteredDifficulties.begin(), m_filteredDifficulties.end(), difficulty) != m_filteredDifficulties.end();
}

void RandomLayer::removeVersion(int version) {
    m_filteredVersions.erase(std::remove(m_filteredVersions.begin(), m_filteredVersions.end(), version), m_filteredVersions.end());
}

void RandomLayer::removeDifficulty(int difficulty) {
    m_filteredDifficulties.erase(std::remove(m_filteredDifficulties.begin(), m_filteredDifficulties.end(), difficulty), m_filteredDifficulties.end());
}

int RandomLayer::getRandomFromFilters(int versionOverride, bool randomOverride) {
    if (m_ratedOnly && !randomOverride) {
        std::vector<int> validLevels;

        auto insertLevels = [&](int version) {
            if (!m_filteredDifficulties.empty() && m_ratedOnly) {
                for (int difficulty : m_filteredDifficulties) {
                    const auto& levels = m_ratedLevels[version][difficulty];
                    validLevels.insert(validLevels.end(), levels.begin(), levels.end());
                }
            } 
            else {
                for (const auto& [difficulty, levels] : m_ratedLevels[version]) {
                    validLevels.insert(validLevels.end(), levels.begin(), levels.end());
                }
            }
        };

        if (versionOverride != -1) {
            insertLevels(versionOverride);
        }
        else if (!m_filteredVersions.empty()) {
            for (int version : m_filteredVersions) {
                insertLevels(version);
            }
        }
        else {
            for (const auto& [version, _] : m_ratedLevels) {
                insertLevels(version);
            }
        }

        if (validLevels.empty()) {
            return 0;
        }
        return validLevels[utils::random::generate(0, validLevels.size() - 1)];

    }
    else {
        std::vector<std::pair<int, int>> ranges;

        if (versionOverride != -1) {
            ranges.push_back(m_versionData[versionOverride]);
        }
        else {
            for (int version : m_filteredVersions) {
                ranges.push_back(m_versionData[version]);
            }
        }
        int randomRange = 0;
        int randomID = 0;
        if (m_filteredVersions.size() > 1) {
            int randomRange = utils::random::generate(0, ranges.size() - 1);
            std::pair<int, int> range = ranges[randomRange];
            randomID = utils::random::generate(range.first, range.second);
        }
        else if (m_filteredVersions.size() == 1) {
            randomID = utils::random::generate(ranges[0].first, ranges[0].second);
        }
        if (m_filteredVersions.size() == 0) {
            randomID = utils::random::generate(128, 130000000);
        }

        return randomID;
    }
}

int RandomLayer::getRandomID() {
    int versionOverride = -1;
    if (isVersionSelected(-2)) {
        versionOverride = utils::random::generate(0, m_versionData.size() - 2);
    }

    if (isVersionSelected(12)) {
        return 0;
    }
    else {
        return getRandomFromFilters(versionOverride);
    }
}

std::string RandomLayer::getRandomIDsList(int amountOverride, bool allow2p2) {
    std::ostringstream query;
    bool addSeparator = false;
    int versionOverride = -1;
    int offset = 2;
    if (allow2p2) offset = 1;
    if (isVersionSelected(-2)) {
        versionOverride = utils::random::generate(0, m_versionData.size() - offset);
    }

    int amount = 100;
    if (m_ratedOnly) amount = 11;
    if (amountOverride != 0) amount = amountOverride;

    for (int n = 0; n < amount; n++) {
        if (addSeparator) query << ",";
        query << getRandomFromFilters(versionOverride, allow2p2);
        addSeparator = true;
    }

    return query.str();
}

void RandomLayer::onRandomListReplace(CCObject* object) {
    auto searchObject = GJSearchObject::create(SearchType::Type19, getRandomIDsList());

    auto browserLayer = static_cast<MyLevelBrowserLayer*>(LevelBrowserLayer::create(searchObject));
    browserLayer->setRandomTab();
    browserLayer->setRefreshButton(this, menu_selector(RandomLayer::onRandomListReplace));

    auto scene = CCScene::create();
    scene->addChild(browserLayer);

    CCDirector::get()->replaceScene(scene);
}

void RandomLayer::onRandomList(CCObject* object) {
    auto searchObject = GJSearchObject::create(SearchType::Type19, getRandomIDsList());

    auto browserLayer = static_cast<MyLevelBrowserLayer*>(LevelBrowserLayer::create(searchObject));
    browserLayer->setRandomTab();
    browserLayer->setRefreshButton(this, menu_selector(RandomLayer::onRandomListReplace));

    auto scene = CCScene::create();
    scene->addChild(browserLayer);

	auto transitionFade = CCTransitionFade::create(0.5, scene);
    CCDirector::get()->pushScene(transitionFade);
}

GJGameLevel* RandomLayer::levelFromData(ZStringView data) {
    if (data == "-1") return nullptr;

    auto parts = utils::string::split(data, "#");
    std::string levelsStr = parts[0];
    std::string creatorsStr = "";
    if (parts.size() > 1) creatorsStr = parts[1];

    std::unordered_map<int, std::pair<std::string, int>> accountInformation;

    auto creatorsData = utils::string::split(creatorsStr, "|");

    for (const std::string& creatorPart : creatorsData) {
        auto creatorData = utils::string::split(creatorPart, ":");
        int userID = utils::numFromString<int>(creatorData[0]).unwrapOr(0);
        std::string userName = creatorData[1];
        int accountID = utils::numFromString<int>(creatorData[2]).unwrapOr(0);
        accountInformation[userID] = {userName, accountID};
    }

    auto levels = utils::string::split(levelsStr, "|");
    if (levels.size() == 0) return nullptr;
    std::string firstLevel = levels[0];
    auto levelData = parseLevel(firstLevel);
    
    GJGameLevel* level = nullptr;
    auto levelObject = GameLevelManager::get()->m_onlineLevels->objectForKey(levelData[1]);

    if (levelObject) level = static_cast<GJGameLevel*>(levelObject);
    else level = GJGameLevel::create();

    level->m_levelID = utils::numFromString<int>(levelData[1]).unwrapOr(0);
    level->m_levelName = levelData[2];
    level->m_levelDesc = levelData[3];
    level->m_levelVersion = utils::numFromString<int>(levelData[5]).unwrapOr(0);
    level->m_userID = utils::numFromString<int>(levelData[6]).unwrapOr(0);
    level->m_creatorName = accountInformation[level->m_userID].first;
    level->m_accountID = accountInformation[level->m_userID].second;
    level->m_downloads = utils::numFromString<int>(levelData[10]).unwrapOr(0);
    level->m_gameVersion = utils::numFromString<int>(levelData[13]).unwrapOr(0);
    level->m_likes = utils::numFromString<int>(levelData[14]).unwrapOr(0);
    level->m_levelLength = utils::numFromString<int>(levelData[15]).unwrapOr(0);
    level->m_stars = utils::numFromString<int>(levelData[18]).unwrapOr(0);
    level->m_featured = utils::numFromString<int>(levelData[19]).unwrapOr(0);
    level->m_autoLevel = utils::numFromString<int>(levelData[20]).unwrapOr(0);
    level->m_audioTrack = utils::numFromString<int>(levelData[12]).unwrapOr(0);
    level->m_songID = utils::numFromString<int>(levelData[35]).unwrapOr(0);
    level->m_coins = utils::numFromString<int>(levelData[37]).unwrapOr(0);
    level->m_coinsVerified = utils::numFromString<int>(levelData[38]).unwrapOr(0);
    level->m_starsRequested = utils::numFromString<int>(levelData[39]).unwrapOr(0);
    level->m_isEpic = utils::numFromString<int>(levelData[42]).unwrapOr(0);
    level->m_demonDifficulty = utils::numFromString<int>(levelData[43]).unwrapOr(0);
    level->m_songIDs = levelData[52];
    level->m_sfxIDs = levelData[53];

    return level;
}

void RandomLayer::makeSearchFor(ZStringView ids, int type, std::function<void(GJGameLevel*)>&& onLoad) {
    auto req = web::WebRequest();
    req.bodyString(fmt::format("str={}&type={}&secret=Wmfd2893gb7", ids, type));
    req.userAgent("");
    req.header("Content-Type", "application/x-www-form-urlencoded");

    m_listeners[ids].spawn(
        req.post("http://www.boomlings.com/database/getGJLevels21.php"),
        [this, onLoad, ids] (web::WebResponse value) {
            if (value.ok()) {
                auto strRes = value.string();
                if (!strRes) return;
                onLoad(levelFromData(strRes.unwrap()));
            }
        }
    );
}

void RandomLayer::goToRandomLevel(float dt) {
    if (m_cancelled) {
        m_cancelled = false;
        return;
    }
    if (!m_waitAlert->getParent()) {
        m_waitAlert->show();
    }
    if ((isVersionSelected(12) || isVersionSelected(-2)) && m_ratedOnly) {
        auto ids = getRandomIDsList(1000, true);
        makeSearchFor(ids, 10, [this, ids] (GJGameLevel* level) {
            if (level) {
                m_waitAlert->removeFromParent();
                auto lel = LevelInfoLayer::create(level, false);
                if (lel->m_level->m_levelString.size() == 0) {
                    lel->downloadLevel();
                }
                auto scene = CCScene::create();
                scene->addChild(lel);

                auto transitionFade = CCTransitionFade::create(0.5, scene);
                CCDirector::get()->pushScene(transitionFade);
            }
            else {
                scheduleOnce(schedule_selector(RandomLayer::goToRandomLevel), 1);
            }
            m_listeners.erase(ids);
        });
    }
    else {
        auto ids = getRandomIDsList();
        makeSearchFor(ids, 19, [this, ids] (GJGameLevel* level) {
            if (level) {
                m_waitAlert->removeFromParent();
                auto lel = LevelInfoLayer::create(level, false);
                if (lel->m_level->m_levelString.size() == 0) {
                    lel->downloadLevel();
                }                
                auto scene = CCScene::create();
                scene->addChild(lel);

                auto transitionFade = CCTransitionFade::create(0.5, scene);
                CCDirector::get()->pushScene(transitionFade);
            }
            else {
                scheduleOnce(schedule_selector(RandomLayer::goToRandomLevel), 1);
            }
            m_listeners.erase(ids);
        });
    }
}

void RandomLayer::onRandomLevel(CCObject* object) {
    goToRandomLevel(0);
}

void RandomLayer::onClearFilters(CCObject* object) {

    geode::createQuickPopup("Clear Filters", "Do you want to <cr>reset</c> all your <cy>filters</c>?", "No", "Yes", [this] (FLAlertLayer*, bool confirmed) {
        if (confirmed) {
            m_filteredVersions.clear();
            m_filteredDifficulties.clear();
            m_ratedOnly = false;
            m_completedOnly = false;
            for (auto vBtn : CCArrayExt<CCMenuItemSpriteExtra*>(m_versionButtons)) {
                vBtn->setColor({125, 125, 125});
            }
            for (auto vBtn : CCArrayExt<CCMenuItemSpriteExtra*>(m_difficultyButtons)) {
                vBtn->setColor({125, 125, 125});
            }
            m_ratedToggle->m_toggler->toggle(false);
            //m_completedToggle->m_toggler->toggle(false);

            m_difficultiesMenu->setOpacity(127);
            m_difficultiesMenu->setColor({125, 125, 125});
            m_difficultiesMenu->setEnabled(false);

            Mod::get()->setSavedValue("rated-filter", m_ratedOnly);
            Mod::get()->setSavedValue("completed-filter", m_completedOnly);
            Mod::get()->setSavedValue("version-filter", m_filteredVersions);
            Mod::get()->setSavedValue("difficulty-filter", m_filteredDifficulties);
        }
    }, true);
}

void RandomLayer::ratedAnd2p2Check() {
    if (m_ratedOnly && isVersionSelected(12)) {
        m_listButton->setEnabled(false);
        m_listButton->setColor({125, 125, 125});
        m_listButton->setOpacity(100);
    }
    else {
        m_listButton->setEnabled(true);
        m_listButton->setColor({255, 255, 255});
        m_listButton->setOpacity(255);
    }

    if (isVersionSelected(-2)) {
        m_difficultiesMenu->setOpacity(125);
        m_difficultiesMenu->setColor({125, 125, 125});
        m_difficultiesMenu->setEnabled(false);
    }
}


void RandomLayer::onRatedToggle(CCObject* object) {
    CCMenuItemToggler* toggler = static_cast<CCMenuItemToggler*>(object);
    m_ratedOnly = !toggler->isOn();

    if (m_ratedOnly) {
        m_difficultiesMenu->setOpacity(255);
        m_difficultiesMenu->setColor({255, 255, 255});
        m_difficultiesMenu->setEnabled(true);
        if (!m_filteredDifficulties.empty()) {
            m_twoPointTwoButton->setColor({125, 50, 50});
            m_twoPointTwoButton->setEnabled(false);
        }
        else if (isVersionSelected(12)) {
            m_twoPointTwoButton->setColor({255, 255, 255});
        }
        else {
            m_twoPointTwoButton->setColor({125, 125, 125});
        }
    }
    else {
        m_difficultiesMenu->setOpacity(127);
        m_difficultiesMenu->setColor({125, 125, 125});
        m_difficultiesMenu->setEnabled(false);
        if (isVersionSelected(12)) {
            m_twoPointTwoButton->setColor({255, 255, 255});
        }
        else {
            m_twoPointTwoButton->setColor({125, 125, 125});
        }
        m_twoPointTwoButton->setEnabled(true);
    }
    ratedAnd2p2Check();
    Mod::get()->setSavedValue("rated-filter", m_ratedOnly);
}

void RandomLayer::onCompletedToggle(CCObject* object) {
    auto toggler = static_cast<CCMenuItemToggler*>(object);
    m_completedOnly = !toggler->isOn();
    Mod::get()->setSavedValue("completed-filter", m_completedOnly);
}

void RandomLayer::onVersionButton(CCObject* object) {

    auto btn = static_cast<CCMenuItemSpriteExtra*>(object);

    m_difficultiesMenu->setOpacity(255);
    m_difficultiesMenu->setColor({255, 255, 255});
    m_difficultiesMenu->setEnabled(true);

    if (isVersionSelected(btn->getTag())) {
        btn->setColor({125, 125, 125});
        removeVersion(btn->getTag());
    }
    else {
        if (btn->getTag() == -2) {
            m_difficultiesMenu->setOpacity(127);
            m_difficultiesMenu->setColor({125, 125, 125});
            m_difficultiesMenu->setEnabled(false);
            m_filteredVersions.clear();
            for (auto vBtn : CCArrayExt<CCMenuItemSpriteExtra*>(m_versionButtons)) {
                vBtn->setColor({125, 125, 125});
            }
            if (!m_filteredDifficulties.empty() && m_ratedOnly) {
                m_twoPointTwoButton->setColor({125, 50, 50});
            }
        }
        else {
            m_randomButton->setColor({125, 125, 125});
            removeVersion(-2);
        }
        m_filteredVersions.push_back(btn->getTag());
        btn->setColor({255, 255, 255});
    }
    ratedAnd2p2Check();
    Mod::get()->setSavedValue("version-filter", m_filteredVersions);
}

void RandomLayer::onDifficultyButton(CCObject* object) {

    auto btn = static_cast<CCMenuItemSpriteExtra*>(object);

    if (isDifficultySelected(btn->getTag())) {
        btn->setColor({125, 125, 125});
        removeDifficulty(btn->getTag());
    }
    else {
        m_filteredDifficulties.push_back(btn->getTag());
        btn->setColor({255, 255, 255});
    }

    if (!m_filteredDifficulties.empty()) {
        m_twoPointTwoButton->setColor({125, 50, 50});
        m_twoPointTwoButton->setEnabled(false);
        removeVersion(12);
    }
    else {
        m_twoPointTwoButton->setColor({125, 125, 125});
        m_twoPointTwoButton->setEnabled(true);
    }

    Mod::get()->setSavedValue("difficulty-filter", m_filteredDifficulties);
}

void RandomLayer::keyBackClicked() {
    onBack(nullptr);
}

void RandomLayer::onBack(CCObject* object) {
    CCDirector::get()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

CCScene* RandomLayer::scene() {
    auto layer = RandomLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}


RandomLayer* RandomLayer::create() {
    RandomLayer* ret = new RandomLayer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
