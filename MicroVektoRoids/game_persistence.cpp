#include "game_persistence.h"
#include "lib_storage.h"
#include <string.h>

namespace Game {
    namespace DB {
        Storage::Persistence db;
        Data dataCopy;

        void init() {
            printf("Gamestate size: %d\n", sizeof(Data));
            db.init("MicroVektoRoids/db.bin");
        }
        void save(int8_t state) {
            Data data;
            memcpy(&data, &dataCopy, sizeof(Data));
            data.formatVersion = 0x10;

            memcpy(data.playerName, PlayerStats::name,8);
            if (state >= 0) {

            }


            db.write(&data, 0, sizeof(Data));
        }
        void load(int8_t state) {
            Data data;
            db.read(&data, 0, sizeof(Data));
            if (data.formatVersion != 0x10) {
                return;
            }
            memcpy(&dataCopy, &data, sizeof(Data));
            if (state >= 0) {

            } else {
                memcpy(PlayerStats::name, data.playerName,8);
            }

        }
    }

}
