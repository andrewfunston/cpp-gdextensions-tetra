#pragma once

#include <vector>

#include "Tetra.h"

#pragma mark GroupBase ---------------------------------------------------------------------------------------------------------------------
class GroupBase : public TetraGroup {
protected:
    const std::shared_ptr<TetraWorldInterface> mCurrentChecker;

    int mCurrentCenterX;
    int mCurrentCenterY;

    bool tryMovement(int xDelta, int yDelta, const std::vector<TetraBlock> &blocks);
    virtual bool tryRotation(bool left) = 0;

public:
    GroupBase(std::shared_ptr<TetraWorldInterface> currentChecker,
              int centerX, int centerY);
    virtual ~GroupBase() = default;

    bool tryInput(const TetraInput &input) override;
};

#pragma mark TwoStateGroupBase -------------------------------------------------------------------------------------------------------------
class TwoStateGroupBase : public GroupBase {
protected:
    enum State { normal,
                 rotated };

    State mCurrentState{normal};

public:
    TwoStateGroupBase(std::shared_ptr<TetraWorldInterface> currentChecker);
};
#pragma mark FourStateGroupBase ------------------------------------------------------------------------------------------------------------
class FourStateGroupBase : public GroupBase {
protected:
    enum State { normal,
                 left,
                 right,
                 invert };

    State mCurrentState{normal};

    virtual std::vector<TetraBlock> getAsNormal() const = 0;
    virtual std::vector<TetraBlock> getAsLeft() const = 0;
    virtual std::vector<TetraBlock> getAsRight() const = 0;
    virtual std::vector<TetraBlock> getAsInvert() const = 0;

public:
    FourStateGroupBase(std::shared_ptr<TetraWorldInterface> currentChecker);
    bool tryRotation(bool rotateLeft) override;
    std::vector<TetraBlock> getGroupCoordinates() const override;
};

#pragma mark ReverseZee --------------------------------------------------------------------------------------------------------------------
class ReverseZee : public TwoStateGroupBase {
protected:
    bool tryRotation(bool left) override;

public:
    ReverseZee(std::shared_ptr<TetraWorldInterface> currentChecker);
    std::vector<TetraBlock> getGroupCoordinates() const override;
};
#pragma mark Zee ---------------------------------------------------------------------------------------------------------------------------
class Zee : public TwoStateGroupBase {
protected:
    bool tryRotation(bool left) override;

public:
    Zee(std::shared_ptr<TetraWorldInterface> currentChecker);
    std::vector<TetraBlock> getGroupCoordinates() const override;
};

#pragma mark Block -------------------------------------------------------------------------------------------------------------------------
class Block : public GroupBase {
protected:
    bool tryRotation(bool left) override;

public:
    Block(std::shared_ptr<TetraWorldInterface> currentChecker);
    std::vector<TetraBlock> getGroupCoordinates() const override;
};

#pragma mark Line -----------------------------------------------------------------------------------------------------------------------------
class Line : public TwoStateGroupBase {
protected:
    bool tryRotation(bool left) override;

public:
    Line(std::shared_ptr<TetraWorldInterface> currentChecker);
    std::vector<TetraBlock> getGroupCoordinates() const override;
};

#pragma mark Tee ------------------------------------------------------------------------------------------------------------------------------
class Tee : public FourStateGroupBase {
protected:
    std::vector<TetraBlock> getAsNormal() const override;
    std::vector<TetraBlock> getAsLeft() const override;
    std::vector<TetraBlock> getAsRight() const override;
    std::vector<TetraBlock> getAsInvert() const override;

public:
    Tee(std::shared_ptr<TetraWorldInterface> currentChecker);
};

#pragma mark El -------------------------------------------------------------------------------------------------------------------------------
class El : public FourStateGroupBase {
protected:
    std::vector<TetraBlock> getAsNormal() const override;
    std::vector<TetraBlock> getAsLeft() const override;
    std::vector<TetraBlock> getAsRight() const override;
    std::vector<TetraBlock> getAsInvert() const override;

public:
    El(std::shared_ptr<TetraWorldInterface> currentChecker);
};

#pragma mark ReverseEl ------------------------------------------------------------------------------------------------------------------------
class ReverseEl : public FourStateGroupBase {
protected:
    std::vector<TetraBlock> getAsNormal() const override;
    std::vector<TetraBlock> getAsLeft() const override;
    std::vector<TetraBlock> getAsRight() const override;
    std::vector<TetraBlock> getAsInvert() const override;

public:
    ReverseEl(std::shared_ptr<TetraWorldInterface> currentChecker);
};