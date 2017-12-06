#pragma once

namespace Game {
    namespace Menu {
        const char *getMenuText(bool selected, const char* str);
        void tick();
        void draw();
    }
}
