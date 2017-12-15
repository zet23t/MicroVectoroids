#include "game_persistence.h"
#include "lib_storage.h"
#include <memory.h>

namespace Game {
    namespace DB {
        Storage::Persistence db;

        void init() {
            printf("Gamestate size: %d\n", sizeof(Data));
            db.init("MicroVektoRoids/db.bin");
        }
        void save() {
            Data data;
            memset(&data,0,sizeof(Data));
            db.write(&data, 0, sizeof(Data));
        }
        void load() {
            Data data;
            db.read(&data, 0, sizeof(Data));
        }
    }

}
