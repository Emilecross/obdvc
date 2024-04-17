#pragma once

#include "TypeAlias.hpp"
#include "message.h"

class Order {
    private:
        Price price;
        Volume volume;
    
    public:
        Order(Price price_, Volume volume_)
        :   price { price_ }
        ,   volume { volume_ }
        {}
        /*
        Removal of order on quantity hitting 0 done in level
        */
        void ExecuteOrder(Volume volume_) {
            // assuming valid input for quantity
            volume -= volume_;
        }

        void UpdateOrder(Price price_, Volume volume_) {
            // assuming valid inputs
            volume = volume_;
            price = price_;
        }

        Price getPrice () { return price; }

        Volume getVolume() { return volume; }
};