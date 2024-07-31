#include "TetraBlocks.h"

#pragma mark GroupBase ---------------------------------------------------------------------------------------------------------------------
GroupBase::GroupBase(std::shared_ptr<TetraWorldInterface> currentChecker, int centerX, int centerY) :
    mCurrentChecker(std::move(currentChecker)),
    mCurrentCenterX(centerX),
    mCurrentCenterY(centerY) {}
bool GroupBase::tryInput(const TetraInput &input) {
    switch (input) {
        case moveDown:
            return tryMovement(0, 1, getGroupCoordinates());
        case moveLeft:
            return tryMovement(-1, 0, getGroupCoordinates());
        case moveRight:
            return tryMovement(1, 0, getGroupCoordinates());
        case spinLeft:
            return tryRotation(true);
        case spinRight:
            return tryRotation(false);
        case hardPush:
            // We don't handle this!
            return true;
    }
    return false;
}
bool GroupBase::tryMovement(int xDelta, int yDelta, const std::vector<TetraBlock> &blocks) {
    for (const auto &coord : blocks) {
        const auto &[x, y] = coord.getPair();
        if (!mCurrentChecker->canExist(x + xDelta, y + yDelta)) {
            return false;
        }
    }
    mCurrentCenterX += xDelta;
    mCurrentCenterY += yDelta;
    return true;
}

#pragma mark ZeeBae ------------------------------------------------------------------------------------------------------------------------
TwoStateGroupBase::TwoStateGroupBase(std::shared_ptr<TetraWorldInterface> currentChecker) :
    GroupBase(std::move(currentChecker), 4, 0) {}

#pragma mark FourStateGroupBase ------------------------------------------------------------------------------------------------------------
FourStateGroupBase::FourStateGroupBase(std::shared_ptr<TetraWorldInterface> currentChecker) :
    GroupBase(std::move(currentChecker), 4, 0) {}
bool FourStateGroupBase::tryRotation(bool rotateLeft) {
    switch (mCurrentState) {
        case normal: {
            auto retVal = tryMovement(0, 0, rotateLeft ? getAsLeft() : getAsRight());
            if (retVal) {
                mCurrentState = rotateLeft ? left : right;
            }
            return retVal;
        }
        case left: {
            auto retVal = tryMovement(0, 0, rotateLeft ? getAsInvert() : getAsNormal());
            if (retVal) {
                mCurrentState = rotateLeft ? invert : normal;
            }
            return retVal;
        }
        case right: {
            auto retVal = tryMovement(0, 0, rotateLeft ? getAsNormal() : getAsInvert());
            if (retVal) {
                mCurrentState = rotateLeft ? normal : invert;
            }
            return retVal;
        }
        case invert: {
            auto retVal = tryMovement(0, 0, rotateLeft ? getAsRight() : getAsLeft());
            if (retVal) {
                mCurrentState = rotateLeft ? right : left;
            }
            return retVal;
        }
        default:
            return false;
    }
}
std::vector<TetraBlock> FourStateGroupBase::getGroupCoordinates() const {
    switch (mCurrentState) {
        case normal:
            return getAsNormal();
        case left:
            return getAsLeft();
        case right:
            return getAsRight();
        case invert:
            return getAsInvert();
        default:
            return {};
    }
}

#pragma mark ReverseZee --------------------------------------------------------------------------------------------------------------------
ReverseZee::ReverseZee(std::shared_ptr<TetraWorldInterface> currentChecker) :
    TwoStateGroupBase(std::move(currentChecker)) {}
std::vector<TetraBlock> ReverseZee::getGroupCoordinates() const {
    if (mCurrentState == normal) {
        return {
            {mCurrentCenterX, mCurrentCenterY},
            {mCurrentCenterX + 1, mCurrentCenterY},
            {mCurrentCenterX, mCurrentCenterY + 1},
            {mCurrentCenterX - 1, mCurrentCenterY + 1}};
    }
    return {
        {mCurrentCenterX, mCurrentCenterY - 1},
        {mCurrentCenterX, mCurrentCenterY},
        {mCurrentCenterX + 1, mCurrentCenterY},
        {mCurrentCenterX + 1, mCurrentCenterY + 1}};
}
bool ReverseZee::tryRotation(bool) {
    // with zee/reverse zee we ignore the rotation variable.
    if (mCurrentState == normal) {
        if (mCurrentChecker->canExist(mCurrentCenterX, mCurrentCenterY - 1) &&
            mCurrentChecker->canExist(mCurrentCenterX + 1, mCurrentCenterY + 1)) {
            mCurrentState = rotated;
            return true;
        }
    } else {
        if (mCurrentChecker->canExist(mCurrentCenterX, mCurrentCenterY + 1) &&
            mCurrentChecker->canExist(mCurrentCenterX - 1, mCurrentCenterY + 1)) {
            mCurrentState = normal;
            return true;
        }
    }
    return false;
}

#pragma mark Zee ---------------------------------------------------------------------------------------------------------------------------
Zee::Zee(std::shared_ptr<TetraWorldInterface> currentChecker) :
    TwoStateGroupBase(std::move(currentChecker)) {}
std::vector<TetraBlock> Zee::getGroupCoordinates() const {
    if (mCurrentState == normal) {
        return {
            {mCurrentCenterX - 1, mCurrentCenterY},
            {mCurrentCenterX, mCurrentCenterY},
            {mCurrentCenterX, mCurrentCenterY + 1},
            {mCurrentCenterX + 1, mCurrentCenterY + 1}};
    }
    return {
        {mCurrentCenterX, mCurrentCenterY - 1},
        {mCurrentCenterX, mCurrentCenterY},
        {mCurrentCenterX - 1, mCurrentCenterY},
        {mCurrentCenterX - 1, mCurrentCenterY + 1}};
}
bool Zee::tryRotation(bool) {
    // with zee/reverse zee we ignore the rotation variable.
    if (mCurrentState == normal) {
        if (mCurrentChecker->canExist(mCurrentCenterX, mCurrentCenterY - 1) &&
            mCurrentChecker->canExist(mCurrentCenterX - 1, mCurrentCenterY + 1)) {
            mCurrentState = rotated;
            return true;
        }
    } else {
        if (mCurrentChecker->canExist(mCurrentCenterX, mCurrentCenterY + 1) &&
            mCurrentChecker->canExist(mCurrentCenterX + 1, mCurrentCenterY + 1)) {
            mCurrentState = normal;
            return true;
        }
    }
    return false;
}

#pragma mark Block --------------------------------------------------------------------------------------------------------------------------
Block::Block(std::shared_ptr<TetraWorldInterface> currentChecker) :
    GroupBase(std::move(currentChecker), 4, 0) {}
bool Block::tryRotation(bool left) {
    // blocks don't rotate, dummy.
    return true;
}
std::vector<TetraBlock> Block::getGroupCoordinates() const {
    return {
        {mCurrentCenterX, mCurrentCenterY},
        {mCurrentCenterX + 1, mCurrentCenterY},
        {mCurrentCenterX, mCurrentCenterY + 1},
        {mCurrentCenterX + 1, mCurrentCenterY + 1}};
}

#pragma mark El -----------------------------------------------------------------------------------------------------------------------------
Line::Line(std::shared_ptr<TetraWorldInterface> currentChecker) :
    TwoStateGroupBase(std::move(currentChecker)) {}
bool Line::tryRotation(bool) {
    // with el we ignore the rotation variable.
    if (mCurrentState == normal) {
        if (mCurrentChecker->canExist(mCurrentCenterX, mCurrentCenterY - 1) &&
            mCurrentChecker->canExist(mCurrentCenterX, mCurrentCenterY - 2) &&
            mCurrentChecker->canExist(mCurrentCenterX, mCurrentCenterY + 1)) {
            mCurrentState = rotated;
            return true;
        }
    } else {
        if (mCurrentChecker->canExist(mCurrentCenterX - 1, mCurrentCenterY) &&
            mCurrentChecker->canExist(mCurrentCenterX + 1, mCurrentCenterY) &&
            mCurrentChecker->canExist(mCurrentCenterX + 2, mCurrentCenterY)) {
            mCurrentState = normal;
            return true;
        }
    }
    return false;
}
std::vector<TetraBlock> Line::getGroupCoordinates() const {
    if (mCurrentState == normal) {
        return {
            {mCurrentCenterX - 1, mCurrentCenterY},
            {mCurrentCenterX, mCurrentCenterY},
            {mCurrentCenterX + 1, mCurrentCenterY},
            {mCurrentCenterX + 2, mCurrentCenterY}};
    }
    return {
        {mCurrentCenterX, mCurrentCenterY - 2},
        {mCurrentCenterX, mCurrentCenterY - 1},
        {mCurrentCenterX, mCurrentCenterY},
        {mCurrentCenterX, mCurrentCenterY + 1}};
}

#pragma mark Tee ------------------------------------------------------------------------------------------------------------------------------
Tee::Tee(std::shared_ptr<TetraWorldInterface> currentChecker) :
    FourStateGroupBase(std::move(currentChecker)) {}
std::vector<TetraBlock> Tee::getAsNormal() const {
    return {
        {mCurrentCenterX, mCurrentCenterY},
        {mCurrentCenterX - 1, mCurrentCenterY},
        {mCurrentCenterX + 1, mCurrentCenterY},
        {mCurrentCenterX, mCurrentCenterY + 1}};
}
std::vector<TetraBlock> Tee::getAsLeft() const {
    return {
        {mCurrentCenterX, mCurrentCenterY},
        {mCurrentCenterX, mCurrentCenterY - 1},
        {mCurrentCenterX, mCurrentCenterY + 1},
        {mCurrentCenterX + 1, mCurrentCenterY}};
}
std::vector<TetraBlock> Tee::getAsRight() const {
    return {
        {mCurrentCenterX, mCurrentCenterY},
        {mCurrentCenterX, mCurrentCenterY - 1},
        {mCurrentCenterX, mCurrentCenterY + 1},
        {mCurrentCenterX - 1, mCurrentCenterY}};
}
std::vector<TetraBlock> Tee::getAsInvert() const {
    return {
        {mCurrentCenterX, mCurrentCenterY},
        {mCurrentCenterX, mCurrentCenterY - 1},
        {mCurrentCenterX + 1, mCurrentCenterY},
        {mCurrentCenterX - 1, mCurrentCenterY}};
}

#pragma mark El -------------------------------------------------------------------------------------------------------------------------------
El::El(std::shared_ptr<TetraWorldInterface> currentChecker) :
    FourStateGroupBase(std::move(currentChecker)) {}
std::vector<TetraBlock> El::getAsNormal() const {
    return {
        {mCurrentCenterX, mCurrentCenterY},
        {mCurrentCenterX - 1, mCurrentCenterY},
        {mCurrentCenterX - 1, mCurrentCenterY + 1},
        {mCurrentCenterX + 1, mCurrentCenterY}};
}
std::vector<TetraBlock> El::getAsLeft() const {
    return {
        {mCurrentCenterX, mCurrentCenterY},
        {mCurrentCenterX, mCurrentCenterY - 1},
        {mCurrentCenterX, mCurrentCenterY + 1},
        {mCurrentCenterX + 1, mCurrentCenterY + 1}};
}
std::vector<TetraBlock> El::getAsRight() const {
    return {
        {mCurrentCenterX, mCurrentCenterY},
        {mCurrentCenterX, mCurrentCenterY - 1},
        {mCurrentCenterX - 1, mCurrentCenterY - 1},
        {mCurrentCenterX, mCurrentCenterY + 1}};
}
std::vector<TetraBlock> El::getAsInvert() const {
    return {
        {mCurrentCenterX, mCurrentCenterY},
        {mCurrentCenterX - 1, mCurrentCenterY},
        {mCurrentCenterX + 1, mCurrentCenterY},
        {mCurrentCenterX + 1, mCurrentCenterY - 1}};
}

#pragma mark ReverseEl ------------------------------------------------------------------------------------------------------------------------
ReverseEl::ReverseEl(std::shared_ptr<TetraWorldInterface> currentChecker) :
    FourStateGroupBase(std::move(currentChecker)) {}
std::vector<TetraBlock> ReverseEl::getAsNormal() const {
    return {
        {mCurrentCenterX, mCurrentCenterY},
        {mCurrentCenterX - 1, mCurrentCenterY},
        {mCurrentCenterX + 1, mCurrentCenterY},
        {mCurrentCenterX + 1, mCurrentCenterY + 1}};
}
std::vector<TetraBlock> ReverseEl::getAsLeft() const {
    return {
        {mCurrentCenterX, mCurrentCenterY},
        {mCurrentCenterX, mCurrentCenterY - 1},
        {mCurrentCenterX + 1, mCurrentCenterY - 1},
        {mCurrentCenterX, mCurrentCenterY + 1}};
}
std::vector<TetraBlock> ReverseEl::getAsRight() const {
    return {
        {mCurrentCenterX, mCurrentCenterY},
        {mCurrentCenterX, mCurrentCenterY - 1},
        {mCurrentCenterX, mCurrentCenterY + 1},
        {mCurrentCenterX - 1, mCurrentCenterY + 1}};
}
std::vector<TetraBlock> ReverseEl::getAsInvert() const {
    return {
        {mCurrentCenterX, mCurrentCenterY},
        {mCurrentCenterX - 1, mCurrentCenterY},
        {mCurrentCenterX - 1, mCurrentCenterY - 1},
        {mCurrentCenterX + 1, mCurrentCenterY}};
}