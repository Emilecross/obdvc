#pragma once

#include "TypeAlias.hpp"
#include "message.h"
#include <map>
#include <unordered_map>
#include <algorithm>

using Iterator = typename std::map<Price, Volume>::iterator;
struct Order {
        Price price;
        Volume volume;
        Iterator priceLeveIt;

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

        Iterator getIterator() {
            return priceLeveIt;
        }

        void setIterator(Iterator iterator) {
            priceLeveIt = iterator;
        }
};