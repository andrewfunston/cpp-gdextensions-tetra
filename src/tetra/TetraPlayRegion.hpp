#pragma once

#include "util/io.hpp"
#include "util/scene.hpp"

#include <godot_cpp/classes/os.hpp>
#include "godot_cpp/classes/input.hpp"
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/display_server.hpp>
#include "godot_cpp/variant/utility_functions.hpp"

#include "util/engine.hpp"

#include "../extern/tetris-core/src/tetris/Tetra.h"
#include "../extern/tetris-core/src/tetris/TetraBlocks.h"
#include "../extern/tetris-core/src/tetris/TetraWorldImpl.h"
#include "util/input.hpp"

#pragma mark TetraDisplayBlock ---------------------------------------------------------------------------------------------------------
class TetraDisplayBlock : public godot::Sprite2D {
    GDCLASS(TetraDisplayBlock, godot::Sprite2D);

public:
    TetraDisplayBlock() {
    }
    ~TetraDisplayBlock() override {
    }

    void _ready() override {
        if (rl::engine::editor_active()) {
            return;
        }
        godot::Ref<godot::Resource> textureRef = godot::ResourceLoader::get_singleton()->load("res://assets/icon.svg");
        set_texture(textureRef);
    }
    void _process(double delta_time) override {
        if (rl::engine::editor_active()) {
            return;
        }
    }
    void _enter_tree() override {
    }
    void _exit_tree() override {
    }

protected:
    static void _bind_methods() {
    }
};

#pragma mark TetraPlayRegion -----------------------------------------------------------------------------------------------------------
class TetraPlayRegion : public godot::Node2D {
    GDCLASS(TetraPlayRegion, godot::Node2D);

    static constexpr int sMaxX{10};
    static constexpr int sMaxY{18};

    std::shared_ptr<TetraBoard> mTetraBoard{};

    TetraDisplayBlock *mBlocks[sMaxX][sMaxY];

    float mScaleToSet = 1.0;

    bool mInitialized = false;
    bool mRotateLeft = false;
    bool mRotateRight = false;

    double mCumulativeTime = 0.0;

public:
    TetraPlayRegion() {
        godot::UtilityFunctions::print("TetraPlayRegion created");
        // rl::resource::preload::packed_scene<rl::Level> level{path::scene::Level1};
    }
    ~TetraPlayRegion() override {
        godot::UtilityFunctions::print("TetraPlayRegion destroyed");
    }

    void _ready() override {
        if (rl::engine::editor_active()) {
            return;
        }

        // godot::DisplayServer::get_singleton().screen_get_size()
        // godot::OS::get_singleton().
        mTetraBoard = std::make_shared<TetraBoard>(sMaxX, sMaxY);
        mTetraBoard->initialize();

        for (int x = 0; x < (int)sMaxX; x++) {
            for (int y = 0; y < (int)sMaxY; y++) {
                auto block = memnew(TetraDisplayBlock);
                add_child(block);
                mBlocks[x][y] = block;
            }
        }
    }

    bool initializeBlocks() {
        for (int x = 0; x < (int)sMaxX; x++) {
            for (int y = 0; y < (int)sMaxY; y++) {
                if (!mBlocks[x][y]->is_node_ready()) {
                    return false;
                }
            }
        }

        auto viewportSize = get_viewport()->get_visible_rect().size;

        godot::UtilityFunctions::print("TetraPlayRegion screen ", viewportSize.height, viewportSize.width);

        float requestedWidth = viewportSize.width / (float)sMaxX;
        auto scale = requestedWidth / mBlocks[0][0]->get_texture()->get_size().width;
        float halfDistance = requestedWidth / 2;

        for (int x = 0; x < sMaxX; x++) {
            for (int y = 0; y < sMaxY; y++) {
                const auto &b = mBlocks[x][y];
                b->set_scale(godot::Vector2{scale, scale});
                auto vec = godot::Vector2{(halfDistance + requestedWidth * x),
                                          (halfDistance + requestedWidth * y)};
                b->set_position(vec);

                godot::UtilityFunctions::print("TetraPlayRegion screen ", x, ",", y, " ", vec.x, ",", vec.y);
            }
        }

        return true;
    }

    void _input(const godot::Ref<godot::InputEvent> &event) override {
        const auto &inputEvent = godot::Object::cast_to<const godot::InputEvent>(*event);
        mRotateLeft = inputEvent->is_action_pressed("tetra_rotate_left");
        mRotateRight = inputEvent->is_action_pressed("tetra_rotate_right");
    }

    void _process(double delta_time) override {
        if (rl::engine::editor_active()) {
            return;
        }

        if (!mInitialized) {
            mInitialized = initializeBlocks();
        }

        mCumulativeTime += delta_time;

        bool screenNeedsUpdated = false;
        std::vector<TetraInput> inputs{};
        auto godotInput = godot::Input::get_singleton();
        if (godotInput->is_key_pressed(godot::Key::KEY_LEFT)) {
            inputs.emplace_back(TetraInput::moveLeft);
        }
        if (godotInput->is_key_pressed(godot::Key::KEY_RIGHT)) {
            inputs.emplace_back(TetraInput::moveRight);
        }
        if (godotInput->is_key_pressed(godot::Key::KEY_DOWN)) {
            inputs.emplace_back(TetraInput::moveDown);
        }
        if(mRotateLeft) {
            inputs.emplace_back(TetraInput::spinLeft);
            mRotateLeft = false;
        }
        if(mRotateRight) {
            inputs.emplace_back(TetraInput::spinRight);
            mRotateRight = false;
        }

        if (!inputs.empty()) {
            screenNeedsUpdated = true;
            mTetraBoard->inputReceived(inputs);
        }

        if (mCumulativeTime >= .5) {
            mCumulativeTime = 0.0;
            mTetraBoard->worldStep(WorldStepParams{false});
            screenNeedsUpdated = true;
        }

        if (screenNeedsUpdated) {

            for (int x = 0; x < sMaxX; x++) {
                for (int y = 0; y < sMaxY; y++) {
                    const auto &b = mBlocks[x][y];
                    b->set_visible(false);
                }
            }
            for (const auto &block : mTetraBoard->getCurrentWorld()) {
                mBlocks[block.getX()][block.getY()]->set_visible(true);
            }
        }
    }

protected:
    static void _bind_methods() {
    }
};
