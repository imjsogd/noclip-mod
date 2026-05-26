#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

// ─── Global state ────────────────────────────────────────────────────────────
static bool g_noclipEnabled = false;

// ─── Helper: get PlayLayer safely ────────────────────────────────────────────
static PlayLayer* getPlayLayer() {
    return PlayLayer::get();
}

// ─── Cheat indicator node ─────────────────────────────────────────────────────
// A small red dot drawn at the bottom-center of the screen.
class CheatDot : public CCNode {
public:
    static CheatDot* create() {
        auto ret = new CheatDot();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool init() {
        if (!CCNode::init()) return false;

        // Draw a filled red circle using CCDrawNode
        auto draw = CCDrawNode::create();
        // Draw a solid red circle, radius 8
        draw->drawDot(CCPoint(0, 0), 8.f, ccc4f(1.f, 0.05f, 0.05f, 0.92f));
        this->addChild(draw);

        // "NOCLIP" label in tiny red text
        auto label = CCLabelBMFont::create("NOCLIP", "bigFont.fnt");
        label->setScale(0.28f);
        label->setColor(ccc3(255, 60, 60));
        label->setPosition(0, -16.f);
        this->addChild(label);

        return true;
    }
};

// ─── PlayLayer hooks ──────────────────────────────────────────────────────────
class $modify(NoclipPlayLayer, PlayLayer) {

    struct Fields {
        CCNode* cheatDot = nullptr;
    };

    // Called when the layer initialises (level loads)
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        // Add cheat dot (hidden by default, shown when noclip on)
        auto dot = CheatDot::create();
        dot->setID("noclip-cheat-dot"_spr);
        dot->setVisible(g_noclipEnabled);

        // Position: bottom center of screen
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        dot->setPosition(CCPoint(winSize.width / 2.f, 22.f));
        dot->setZOrder(9999);
        this->addChild(dot);

        m_fields->cheatDot = dot;
        return true;
    }

    // Called every frame — keep dot visibility in sync
    void update(float dt) {
        PlayLayer::update(dt);

        if (m_fields->cheatDot) {
            m_fields->cheatDot->setVisible(g_noclipEnabled);
        }

        // ── Anti-cheat: force death at 97-99% ──────────────────────────────
        // If noclip is on and the player is in that range, kill them.
        if (g_noclipEnabled && m_player1) {
            float pct = this->getCurrentPercent();
            if (pct >= 97.f && pct < 100.f) {
                // Temporarily turn off noclip so destroyPlayer actually fires,
                // then restore — gives a clean death at that %
                g_noclipEnabled = false;
                this->destroyPlayer(m_player1, m_player1);
                // Don't re-enable; the death ends the run naturally.
            }
        }
    }

    // ── Noclip core: skip death while enabled ─────────────────────────────
    void destroyPlayer(PlayerObject* player, GameObject* object) {
        if (g_noclipEnabled) {
            // Block the death — do nothing
            return;
        }
        PlayLayer::destroyPlayer(player, object);
    }

    // ── Block level completion while noclip is on ─────────────────────────
    void levelComplete() {
        if (g_noclipEnabled) {
            // Kill the player instead of completing the level
            g_noclipEnabled = false;
            if (m_player1) {
                PlayLayer::destroyPlayer(m_player1, m_player1);
            }
            return;
        }
        PlayLayer::levelComplete();
    }
};

// ─── PauseLayer hook — adds the toggle button ────────────────────────────────
class $modify(NoclipPauseLayer, PauseLayer) {

    // Helper to rebuild the button label
    static std::string buttonLabel() {
        return g_noclipEnabled
            ? "Noclip: <cr>ON</c>"
            : "Noclip: <cg>OFF</c>";
    }

    void customSetupButtons() {
        // Find the left-side button menu in the pause layer
        // Geode gives nodes IDs; the left column is "left-button-menu"
        CCMenu* leftMenu = typeinfo_cast<CCMenu*>(
            this->getChildByID("left-button-menu")
        );

        // Fallback: search for any menu on the left side
        if (!leftMenu) {
            // Try to find by position heuristic
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            CCObject* obj;
            CCARRAY_FOREACH(this->getChildren(), obj) {
                auto node = static_cast<CCNode*>(obj);
                if (auto menu = typeinfo_cast<CCMenu*>(node)) {
                    if (menu->getPositionX() < winSize.width * 0.35f) {
                        leftMenu = menu;
                        break;
                    }
                }
            }
        }

        // Build button sprite
        auto makeBtnSprite = [](const std::string& text) -> ButtonSprite* {
            auto spr = ButtonSprite::create(
                text.c_str(), 120, true, "bigFont.fnt", "GJ_button_04.png", 30.f, 0.55f
            );
            return spr;
        };

        auto btnSpr = makeBtnSprite(buttonLabel());
        btnSpr->setID("noclip-btn-sprite"_spr);

        auto btn = CCMenuItemSpriteExtra::create(
            btnSpr,
            this,
            menu_selector(NoclipPauseLayer::onNoclipToggle)
        );
        btn->setID("noclip-toggle-btn"_spr);

        if (leftMenu) {
            btn->setZOrder(10);
            leftMenu->addChild(btn);
            leftMenu->updateLayout();
        } else {
            // Absolute fallback: create our own menu
            auto menu = CCMenu::create();
            menu->setID("noclip-menu"_spr);
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            menu->setPosition(winSize.width * 0.15f, winSize.height * 0.5f);
            menu->addChild(btn);
            this->addChild(menu, 10);
        }
    }

    bool init(bool isPaused) {
        if (!PauseLayer::init(isPaused)) return false;
        this->customSetupButtons();
        return true;
    }

    void onNoclipToggle(CCObject* sender) {
        g_noclipEnabled = !g_noclipEnabled;

        // Update the button label live
        auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(sender);
        if (btn) {
            auto spr = typeinfo_cast<ButtonSprite*>(btn->getNormalImage());
            if (spr) {
                spr->setString(buttonLabel().c_str());
            }
        }

        // Update cheat dot visibility immediately if in PlayLayer
        if (auto pl = PlayLayer::get()) {
            if (auto dot = pl->getChildByID("noclip-cheat-dot"_spr)) {
                dot->setVisible(g_noclipEnabled);
            }
        }

        log::info("[NoclipMod] Noclip toggled: {}", g_noclipEnabled ? "ON" : "OFF");
    }
};
