#pragma once

namespace Game {
    namespace UI {
        namespace Intermission {
            void tick();
            void draw();
            void show(const char *text);
            bool isActive();
        }
    }
}
