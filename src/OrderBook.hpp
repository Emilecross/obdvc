#include "Order.hpp"
#include "message.h"
#include <map>
#include <unordered_map>
#include <algorithm>


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
        void printOrderBook(const int seqNum) {
            std::cout << seqNum << "\n";
        };

        // add order
        template <typename UMap_, typename Map_>
        bool process(UMap_ &orderMap, Map_ &levelMap, const OrderAdd &orderAdd) {
            auto orderId = orderAdd.order_id;
            auto price = orderAdd.price;
            auto volume = orderAdd.volume;

            auto levelIterator = levelMap.find(price);
            (*levelIterator).second += volume;

            orderMap.emplace(std::piecewise_construct, std::forward_as_tuple(orderId), std::forward_as_tuple(price, volume));

            // avoid structured binding
            auto it = levelMap.begin();
            for (uint32_t i = 0; i < depth; i++, it++) {
                if (it == levelIterator) return true;
            }

            return false;
        };

        // delete order
        template <typename UMap_, typename Map_>
        bool process(UMap_ &orderMap, Map_ &levelMap, const OrderDelete orderDelete) {
            auto orderId = orderDelete.order_id;
            auto orderIt = orderMap.find(orderId);
            auto [oid, orderObj] = *orderIt;
            auto orderPrice = orderObj.getPrice();
            auto orderVolume = orderObj.getVolume();

            auto levelIterator = levelMap.find(orderPrice);
            (*levelIterator).second -= orderVolume;

            if ((*levelIterator).second == 0) levelIterator = levelMap.erase(levelIterator);
            if (levelIterator == levelMap.end()) return true;

            auto it = levelMap.begin();
            for (uint32_t i = 0; i < depth; i++, it++) {
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
            auto oldPrice = (*orderIt).second.getPrice();
            auto oldVolume = (*orderIt).second.getPrice();
            (*orderIt).second.UpdateOrder(newPrice, newVolume);

            auto oldLevelIterator = levelMap.find(oldPrice);
            (*oldLevelIterator).second -= oldVolume;
            auto newLevelIterator = levelMap.find(newPrice);
            (*newLevelIterator).second += newVolume;

            if ((*oldLevelIterator).second == 0) oldLevelIterator = levelMap.erase(oldLevelIterator);
            if ((*newLevelIterator).second == 0) newLevelIterator = levelMap.erase(newLevelIterator);

            auto it = levelMap.begin();
            for (uint32_t i = 0; i < depth; i++, it++) {
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
            (*orderIt).second.ExecuteOrder(tradeVolume);

            auto levelIterator = levelMap.find((*orderIt).second.getPrice());
            (*levelIterator).second -= tradeVolume;

            if ((*levelIterator).second == 0) levelIterator = levelMap.erase(levelIterator);

            auto it = levelMap.begin();
            for (uint32_t i = 0; i < depth; i++, it++) {
                if (it == levelIterator) return true;
            }

            return false;
        };

        BuyPriceTree& getBuyLevels() {
            return b_levels;
        }

        SellPriceTree& getSellLevels() {
            return s_levels;
        }

        OrdersHashMap& getBuyOrders() {
            return b_orders;
        }

        OrdersHashMap& getSellOrders() {
            return s_orders;
        }
};

class OrderBookHandler {
    private:
        std::unordered_map<Symbol, OrderBook> orderbooks;

    public:
        template <typename OrderAction>
        void processOrderAction(const int seqNum, const OrderAction &orderAction) {
            OrderBook &ob = orderbooks[orderAction.symbol];
            bool print = false;
            if (orderAction.side == Side::BUY) print = ob.process(ob.getBuyOrders(), ob.getBuyLevels(), orderAction);
            else if (orderAction.side == Side::SELL) print = ob.process(ob.getSellOrders(), ob.getSellLevels(), orderAction);
            if (print) ob.printOrderBook(seqNum);
        };
};