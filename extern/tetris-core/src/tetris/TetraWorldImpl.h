#pragma once

#include "TetraBlocks.h"

#include <random>

class TetraGenerator {
    mutable std::mt19937 gen;
    mutable std::uniform_int_distribution<> dis;

public:
    TetraGenerator();
    TetraType getRnd() const;
};

class TetraBoard : public TetraWorldInterface,
                   public std::enable_shared_from_this<TetraBoard> {
    const uint mMaxX;
    const uint mMaxY;
    std::vector<std::vector<std::optional<TetraBlock>>> mBoard;

    TetraGenerator mGenerator{};
    std::shared_ptr<TetraGroup> mCurrentTetraGroup{};

    std::shared_ptr<TetraGroup> generate();

public:
    TetraBoard(uint x, uint y);
    ~TetraBoard() override = default;

    void initialize();
    bool canExist(int x, int y) const override;
    void worldStep(const WorldStepParams &params) override;
    void inputReceived(const std::vector<TetraInput> &inputs) override;
    std::vector<TetraBlock> getCurrentWorld() override;
};
