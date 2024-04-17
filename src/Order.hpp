#pragma once

#include "TypeAlias.hpp"
#include "message.h"

struct Order {
        Price price;
        Volume volume;
    public:
        // executes and returns new volume
        Volume executeOrder(Volume v) {
            // assuming valid input for quantity
            volume -= v;
            return volume;
        }

        void updateOrder(Price p, Volume v) {
            // assuming valid inputs
            volume = v;
            price = p;
        }
};