#pragma once

namespace Game {
    namespace DB {
        struct Data {
            char playerName[8];

        };

        extern Data dbData;

        void init();
        void save();
        void load();
    }
}
