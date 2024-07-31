#pragma once

#include <vector>

#pragma mark TetraInput --------------------------------------------------------------------------------------------------------------------
enum TetraInput {
    moveLeft,
    moveRight,
    moveDown,
    spinLeft,
    spinRight,
    hardPush,
};

#pragma mark TetraInput --------------------------------------------------------------------------------------------------------------------
enum TetraType {
    TypeZee,
    TypeReverseZee,
    TypeBlock,
    TypeEl,
    TypeReverseEl,
    TypeTee,
    TypeLine,
};

#pragma mark TetraBlock --------------------------------------------------------------------------------------------------------------------
class TetraBlock {
    int mX;
    int mY;

public:
    TetraBlock(int x, int y) :
        mX{x}, mY{y} {}

    [[nodiscard]] int getX() const {
        return mX;
    }
    [[nodiscard]] int getY() const {
        return mY;
    }
    [[nodiscard]] std::pair<int, int> getPair() const {
        return {mX, mY};
    }
};

#pragma mark TetraWorldInterface -----------------------------------------------------------------------------------------------------------
struct WorldStepParams {
    const bool mHardPush;
};
class TetraWorldInterface {
public:
    virtual ~TetraWorldInterface() = default;
    virtual void worldStep(const WorldStepParams &params) = 0;
    virtual void inputReceived(const std::vector<TetraInput> &inputs) = 0;
    [[nodiscard]] virtual bool canExist(int x, int y) const = 0;
    [[nodiscard]] virtual std::vector<TetraBlock> getCurrentWorld() = 0;
};

#pragma mark TetraGroup --------------------------------------------------------------------------------------------------------------------
class TetraGroup {
public:
    virtual ~TetraGroup() = default;
    [[nodiscard]] virtual bool tryInput(const TetraInput &input) = 0;
    [[nodiscard]] virtual std::vector<TetraBlock> getGroupCoordinates() const = 0;
};
