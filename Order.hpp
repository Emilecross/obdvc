#pragma once

#include "TypeAlias.hpp"
#include "message.h"

using LevelIterator = std::map<>;
class Order {
    private:
        OrderId orderId;
        Price price;
        Volume volume;
    
    public:
        Order(OrderId orderId_, Price price_, Volume volume_)
        :   orderId { orderId_ }
        ,   price { price_ }
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

        const Price getPrice () { return price; }

        const Volume getQuantity() { return volume; }
};