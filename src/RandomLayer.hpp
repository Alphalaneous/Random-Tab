#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class Toggle : public CCMenu {
    public:
    CCMenuItemToggler* m_toggler;
    CCLabelBMFont* m_label;

    static Toggle* create() {
        Toggle* ret = new Toggle();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
    
        delete ret;
        return nullptr;
    }
};

class RandomLayer : public CCLayer {
    public:

    //these aren't 100% accurate, feel free to pr accurate values
    std::unordered_map<int, std::pair<int, int>> m_versionData = {
        {0,  {128,      3785     }},
        {1,  {3786,     13519    }},
        {2,  {13520,    66513    }},
        {3,  {66514,    122780   }},
        {4,  {122781,   184402   }},
        {5,  {184403,   422703   }},
        {6,  {422704,   835700   }},
        {7,  {835701,   1628620  }},
        {8,  {1628621,  2804784  }},
        {9,  {2804785,  11040708 }},
        {10, {11040709, 28360553 }},
        {11, {28360554, 97454811 }},
        {12, {97454812, 130000000}}
    };


    static RandomLayer* create();
    static CCScene* scene();

    bool init();
    void keyBackClicked();
    void onBack(CCObject* object);

    CCMenuItemSpriteExtra* createVersionButton(ZStringView label, int version);
    CCMenuItemSpriteExtra* createDifficultyButton(ZStringView texture, int difficulty);

    void onVersionButton(CCObject* object);
    void onDifficultyButton(CCObject* object);
    void onRandomList(CCObject* object);
    void onRandomListReplace(CCObject* object);
    void onRandomLevel(CCObject* object);
    void onClearFilters(CCObject* object);
    void onRatedToggle(CCObject* object);
    void onCompletedToggle(CCObject* object);
    void goToRandomLevel(float dt);

    bool isVersionSelected(int version);
    bool isDifficultySelected(int difficulty);
    void removeVersion(int version);
    void removeDifficulty(int difficulty);
    void ratedAnd2p2Check();

    std::string getRandomIDsList(int amountOverride = 0, bool allow2p2 = false);
    int getRandomID();
    void makeSearchFor(ZStringView ids, int type, std::function<void(GJGameLevel*)>&& onLoad);

    int getRandomFromFilters(int versionOverride, bool randomOverride = false);
    int betweenWhich(int id);
    std::unordered_map<int, std::unordered_map<int, std::vector<int>>> parse(ZStringView str);
    std::unordered_map<int, std::string> parseLevel(ZStringView str);
    GJGameLevel* levelFromData(ZStringView data);

    Ref<CCArray> m_versionButtons;
    Ref<CCArray> m_difficultyButtons;
    CCMenuItemSpriteExtra* m_twoPointTwoButton;
    CCMenuItemSpriteExtra* m_randomButton;
    CCMenuItemSpriteExtra* m_listButton;
    CCMenu* m_difficultiesMenu;
    Ref<FLAlertLayer> m_waitAlert;
    bool m_cancelled;

    StringMap<async::TaskHolder<web::WebResponse>> m_listeners;

    Toggle* m_ratedToggle;
    Toggle* m_completedToggle;

    std::unordered_map<int, std::unordered_map<int, std::vector<int>>> m_ratedLevels;
    std::vector<int> m_filteredVersions;
    std::vector<int> m_filteredDifficulties;
    bool m_ratedOnly;
    bool m_completedOnly;

};