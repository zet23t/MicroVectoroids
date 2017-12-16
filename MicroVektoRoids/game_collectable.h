#pragma once

#include "lib_FixedMath.h"


namespace Game {
    namespace Collectable {
        void spawn(Fixed2D4 pos, Fixed2D4 vel);
        void tick();
        void draw();
        void init();
    }

}
