#include "TetraCurses.h"

#include "ncurses.h"

CursesTetraBoardRenderer::CursesTetraBoardRenderer(int maxX, int maxY) :
    mMaxX(maxX),
    mMaxY(maxY) {

    mProcessingThread = std::thread{&CursesTetraBoardRenderer::startThread, this};
}
CursesTetraBoardRenderer::~CursesTetraBoardRenderer() {
    mProcessingThread.join();
}
void CursesTetraBoardRenderer::startThread() {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    do {
        if (const auto input = getch();
            input != ERR) {
            if (auto translatedOpt = translateInput(input)) {
                mInputSync->emplace_back(*translatedOpt);
            }
        }

        std::vector<TetraBlock> toRender{};
        mBoardSync.swap(toRender);
        if (!toRender.empty()) {
            privateRender(toRender);
        }

    } while (true); // todo process "quit"
    // todo make sure we have enough screen room to print our board.
}
void CursesTetraBoardRenderer::render(std::vector<TetraBlock> currentBoard) {
    mBoardSync.swap(currentBoard);
}
std::vector<TetraInput> CursesTetraBoardRenderer::getInputs() {
    std::vector<TetraInput> retVal{};
    mInputSync.swap(retVal);

    return retVal;
}
std::optional<TetraInput> CursesTetraBoardRenderer::translateInput(int ch) const {
    if (ch == KEY_UP) {
        return hardPush;
    }
    if (ch == KEY_LEFT) {
        return moveLeft;
    }
    if (ch == KEY_RIGHT) {
        return moveRight;
    }
    if (ch == KEY_DOWN) {
        return moveDown;
    }
    if (ch == int{'q'} || ch == int{'Q'}) {
        return spinLeft;
    }
    if (ch == int{'e'} || ch == int{'E'}) {
        return spinRight;
    }

    return std::nullopt;
}
void CursesTetraBoardRenderer::privateRender(const std::vector<TetraBlock> &currentBoard) const {
    for (int x = 0; x < mMaxX; x++) {
        for (int y = 0; y < mMaxY; y++) {
            mvprintw(y, x, "_");
        }
    }

    for (const auto &block : currentBoard) {
        const auto &x = block.getX();
        const auto &y = block.getY();
        if (x >= 0 && x < mMaxX && y >= 0 && y < mMaxY) {
            mvprintw(y, x, "X");
        }
    }
    mvprintw(mMaxY + 1, mMaxX + 1, "a");

    refresh();
}