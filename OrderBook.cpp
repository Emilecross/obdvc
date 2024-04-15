#include <iostream>
#include "OrderBook.hpp"
        // returns whether to reprint orderbook price levels up to depth

    bool OrderBook::process(const OrderAdd &orderAdd) {
        auto orderId = orderAdd.order_id;
        auto price = orderAdd.price;
        auto volume = orderAdd.volume;

        auto levelIterator = b_levels.find(price);
        *it += volume;

        orderMap.emplace(std::piecewise_construct, 
                    std::forward_as_tuple(orderId), 
                    std::forward_as_tuple(price, volume, levelIterator));

        for (int i = 0, auto it = orderMap.begin(); it != orderMap.end() && i < depth; i++, it++) {
            if (it == levelIterator) return true;
        }

        return false;
    }
