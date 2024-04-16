#include "Order.hpp"
#include "message.h"
#include <map>
#include <unordered_map>

class OrderBookHandler {
    private:
        std::unordered_map<Symbol, OrderBook> orderbooks;
    public:

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

        // add order
        template <typename UMap_, typename Map_>
        bool process(UMap_ &orderMap, Map_ &levelMap, const OrderAdd &orderAdd) {
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
            for (int i = 0; i < depth; i++, it++) {
                if (it == levelIterator) return true;
            }

            return false;
        };

        // delete order
        template <typename UMap_, typename Map_>
        bool process(UMap_ &orderMap, Map_ &levelMap, const OrderDelete orderDelete) {
            auto orderId = orderAction.order_id;
            auto orderIt = orderMap.find(orderId);
            auto orderPrice = *orderIt.getPrice();
            auto orderVolume = *orderIt.getVolume();

            auto levelIterator = levelMap.find(orderPrice);
            *levelIterator -= orderVolume;

            if (*levelIterator == 0) levelIterator = levelMap.erase(levelIterator);
            if (levelIterator == levelMap.end()) return true;

            auto it = levelMap.begin();
            for (int i = 0; i < depth; i++, it++) {
                if (it == levelIterator) return true;
            }

            return false;
        };

        // update order
        template <typename UMap_, typename Map_>
        bool process(UMap_ &orderMap, Map_ &levelMap, const OrderUpdate orderUpdate) {
            auto orderId = orderUpdate.order_id;
            auto newPrice = orderUpdate.price;
            auto newVolume = orderUpdate.volume;

            auto orderIt = orderMap.find(orderId);
            auto oldPrice = *orderIt.getPrice();
            auto oldVolume = *orderIt.getVolume();
            *orderIt.UpdateOrder(newPrice, newVolume);

            auto oldLevelIterator = levelMap.find(oldPrice);
            *oldLevelIterator -= oldVolume;
            auto newLevelIterator = levelMap.find(newPrice);
            *newLevelIterator += newVolume;

            if (*oldLevelIterator.getVolume() == 0) oldLevelIterator = levelMap.erase(oldLevelIterator);
            if (*newLevelIterator.getVolume() == 0) newLevelIterator = levelMap.erase(newLevelIterator);

            auto it = levelMap.begin();
            for (int i = 0; i < depth; i++, it++) {
                if (it == oldLevelIterator || it == newLevelIterator) return true;
            }

            return false;
        };

        // execute order
        template <typename UMap_, typename Map_>
        bool process(UMap_ &orderMap, Map_ &levelMap, const OrderTrade orderTrade) {
            auto orderId = orderTrade.order_id;
            auto orderIt = orderMap.find(orderId);
            auto tradeVolume = orderTrade.volume;
            *orderIt.ExecuteOrder(tradeVolume);

            auto levelIterator = levelMap.find(*orderIt.getPrice());
            *levelIterator -= tradeVolume;

            if (*levelIterator == 0) levelIterator = levelMap.erase(levelIterator);

            auto it = levelMap.begin();
            for (int i = 0; i < depth; i++, it++) {
                if (it == levelIterator) return true;
            }

            return false;
        };
};
