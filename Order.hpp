#pragma once

#include "TypeAlias.hpp"

class Order {
    private:
        OrderId orderId;
        Price price;
        Quantity quantity;
    
    public:
        Order(OrderId orderId_, Price price_, Quantity quantity_)
        :   orderId { orderId_ }
        ,   price { price_ }
        ,   quantity { quantity_ }
        {}
        /*
        Removal of order on quantity hitting 0 done in level
        */
        void ExecuteOrder(Quantity quantity_) {
            // assuming valid input for quantity
            quantity -= quantity_;
        }

        void UpdateOrder(Price price_, Quantity quantity_) {
            // assuming valid inputs
            quantity = quantity_;
            price = price_;
        }

        const Price getPrice () { return price; }

        const Quantity getQuantity() { return quantity; }
};