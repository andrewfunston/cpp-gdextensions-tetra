#pragma once

#include "../tetris/Tetra.h"

#include <thread>
#include <vector>
#include <optional>

#include <boost/thread/synchronized_value.hpp>

class CursesTetraBoardRenderer {
    const int mMaxX;
    const int mMaxY;

    boost::synchronized_value<std::vector<TetraInput>> mInputSync{};
    boost::synchronized_value<std::vector<TetraBlock>> mBoardSync{};

    std::thread mProcessingThread;

    [[nodiscard]] std::optional<TetraInput> translateInput(int c) const;
    void privateRender(const std::vector<TetraBlock> &currentBoard) const;

public:
    CursesTetraBoardRenderer(int maxX, int maxY);
    ~CursesTetraBoardRenderer();

    void render(std::vector<TetraBlock> currentBoard);
    std::vector<TetraInput> getInputs();

    void startThread();
};
