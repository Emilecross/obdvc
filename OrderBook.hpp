#include "Order.hpp"
#include "message.h"
#include <map>
#include <unordered_map>

class OrderBookHandler {
    private:
        std::unordered_map<Symbol, OrderBook> orderbooks;

    template <typename OrderAction>
    void processOrderAction(const OrderAction &orderAction) {
        OrderBook &ob = orderbooks[orderAction.symbol];
        bool print = false;
        if (orderAction.side == Side::BUY) print = ob.process(ob.b_levels, ob.b_orders, orderAction);
        else if (orderAction.side == Side::SELL) print = ob.process(ob.s_levels, ob.s_orders, orderAction);
        if (print) ob.printOrderBook();
    };
};

using OrdersHashMap = std::unordered_map<OrderId, Order>;
using BuyPriceTree = std::map<Price, Volume, std::greater<Price>>;
using SellPriceTree = std::map<Price, Volume, std::less<Price>>;

class OrderBook {
    private:
        Symbol symbol;
        BuyPriceTree b_levels;
        SellPriceTree s_levels;
        OrdersHashMap b_orders, s_orders;
        uint32_t depth;

    public:
        // boolean value represents if depth needs to be reprinted
        void printOrderBook();

    template <typename UMap_, typename Map_>
    bool process(UMap_ &orderMap, Map_ &levelMap, OrderAdd orderAdd) {
        auto orderId = orderAdd.order_id;
        auto price = orderAdd.price;
        auto volume = orderAdd.volume;

        auto levelIterator = levelMap.find(price);
        *levelIterator += volume;

        orderMap.emplace(std::piecewise_construct, 
                    std::forward_as_tuple(orderId), 
                    std::forward_as_tuple(price, volume, levelIterator));

        // avoid structured binding
        auto it = orderMap.begin();
        for (int i = 0; it != orderMap.end() && i < depth; i++, it++) {
            if (it == levelIterator) return true;
        }

        return false;
    };

    template <typename UMap_, typename Map_>
    bool process(UMap_ &orderMap, Map_ &levelMap, OrderAction orderAction) {
        auto orderId = orderAction.order_id;
        auto price = orderAction.price;
        auto volume = orderAction.volume;

        auto levelIterator = levelMap.find(price);
        *levelIterator += volume;

        orderMap.emplace(std::piecewise_construct, 
                    std::forward_as_tuple(orderId), 
                    std::forward_as_tuple(price, volume, levelIterator));

        auto it = orderMap.begin();
        for (int i = 0; it != orderMap.end() && i < depth; i++, it++) {
            if (it == levelIterator) return true;
        }

        return false;
    };
};
