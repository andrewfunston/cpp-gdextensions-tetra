#include "TetraWorldImpl.h"

#pragma mark TetraGenerator --------------------------------------------------------------------------------------------------------------------
TetraGenerator::TetraGenerator() :
    gen{std::random_device{}()},
    dis{0, 6} {}
TetraType TetraGenerator::getRnd() const {
    return static_cast<TetraType>(dis(gen));
}

#pragma mark TetraBoard --------------------------------------------------------------------------------------------------------------------
TetraBoard::TetraBoard(uint x, uint y) :
    mMaxX{x},
    mMaxY{y},
    mBoard(y, std::vector<std::optional<TetraBlock>>(x)) {
}
std::shared_ptr<TetraGroup> TetraBoard::generate() {
    switch (mGenerator.getRnd()) {
        case TetraType::TypeBlock:
            return std::make_shared<Block>(shared_from_this());
        case TetraType::TypeZee:
            return std::make_shared<Zee>(shared_from_this());
        case TetraType::TypeReverseZee:
            return std::make_shared<ReverseZee>(shared_from_this());
        case TetraType::TypeLine:
            return std::make_shared<Line>(shared_from_this());
        case TetraType::TypeTee:
            return std::make_shared<Tee>(shared_from_this());
        case TetraType::TypeEl:
            return std::make_shared<El>(shared_from_this());
        case TetraType::TypeReverseEl:
        default:
            return std::make_shared<ReverseEl>(shared_from_this());
    }
}
void TetraBoard::initialize() {
    if (!mCurrentTetraGroup) {
        mCurrentTetraGroup = generate();
    }
}
bool TetraBoard::canExist(int x, int y) const {
    if (x < 0 || x >= mMaxX || y >= mMaxY) {
        return false;
    }
    if (y < 0) {
        // we allow rotation "above" the top of the board.
        return true;
    }

    return !mBoard[y][x].has_value();
}
void TetraBoard::worldStep(const WorldStepParams &params) {
    if (params.mHardPush) {
        while (mCurrentTetraGroup->tryInput(TetraInput::moveDown)) {
        }
    } else if (mCurrentTetraGroup->tryInput(TetraInput::moveDown)) {
        // The current group moved down, we're not settled.
        return;
    }

    // the current group can't move down, we've settled
    for (auto curBlock : mCurrentTetraGroup->getGroupCoordinates()) {
        mBoard[curBlock.getY()][curBlock.getX()] = curBlock;
    }

    // check to see if the user made any rows.
    for (auto it = mBoard.begin(); it != mBoard.end();) {
        bool complete{true};

        for (const auto &opt : *it) {
            if (!opt) {
                complete = false;
                break;
            }
        }
        if (complete) {
            it = mBoard.erase(it);
        } else {
            ++it;
        }
    }

    // If we made any rows, make a new row at the top, moving it all down
    if (auto removed = mMaxY - mBoard.size();
        removed > 0) {
        for (; removed > 0; removed--) {
            mBoard.insert(mBoard.begin(), std::vector<std::optional<TetraBlock>>(mMaxX));
        }
    }

    // Replace our old, check to see if it can be added, if not game-over.
    mCurrentTetraGroup = generate();
}
void TetraBoard::inputReceived(const std::vector<TetraInput> &inputs) {
    if (!mCurrentTetraGroup) {
        return;
    }

    for (const auto &input : inputs) {
        mCurrentTetraGroup->tryInput(input);
    }
}
std::vector<TetraBlock> TetraBoard::getCurrentWorld() {
    std::vector<TetraBlock> mPlaced{};

    for (int x = 0; x < mMaxX; ++x) {
        for (int y = 0; y < mMaxY; ++y) {
            if (mBoard[y][x]) {
                mPlaced.emplace_back(x, y);
            }
        }
    }

    if (!mCurrentTetraGroup) {
        return mPlaced;
    }
    auto update = mPlaced;
    auto toAdd = mCurrentTetraGroup->getGroupCoordinates();
    for (auto tetra_block : toAdd) {
        update.push_back(tetra_block);
    }
    return update;
}