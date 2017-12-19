#pragma once

#define INTERMISSION_TYPE_MODAL 0
#define INTERMISSION_TYPE_OVERLAY 1

namespace Game {
    namespace UI {
        namespace Intermission {
            void tick();
            void draw();
            void show(const char *text, uint8_t type);
            void show(const char *text);
            bool isActive();
            bool isBlocking();
        }
    }
}
