#include "game_persistence.h"
#include "lib_storage.h"

namespace Game {
    namespace DB {
        Storage::Persistence db;

        Data dbData;

        void init() {
            Storage::init(db, "MicroVektoRoids/db.bin");
        }
        void save() {
        }
        void load() {
        }
    }

}
