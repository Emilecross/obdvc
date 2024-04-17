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

    public:
        uint32_t depth;
        template <typename Map_>
        void printMap(const Map_ &mp) {
            std::cout << "[";
            auto stop = mp.end();
            for (auto it = mp.begin(); it != stop; it++) {
                std::cout << "(" << it->first << ", " << it->second << ")";
                if (std::next(it) != stop) std::cout << ", ";
            }
            std::cout << "]";
        }

        // boolean value represents if depth needs to be reprinted
        void printOrderBook(const int seqNum) {
            std::cout << seqNum << ", " << symbol << ", ";
            printMap(b_levels);
            std::cout << ", ";
            printMap(s_levels);
            std::cout << "\n";
        };

        // add order
        template <typename UMap_, typename Map_>
        bool process(UMap_ &orderMap, Map_ &levelMap, const OrderAdd &orderAdd) {
            auto orderId = orderAdd.order_id;
            auto price = orderAdd.price;
            auto volume = orderAdd.volume;

            auto levelIterator = levelMap.emplace(price, 0).first;
            levelIterator->second += volume;
            orderMap.emplace(orderId, Order{price, volume});

            auto it = levelMap.begin();
            for (uint32_t i = 0; i < depth; i++, it++) {
                // should always hit the level iterator
                // even on a delete level iterator = map.end()
                if (it == levelIterator) return true;
            }

            return false;
        };

        // delete order
        template <typename UMap_, typename Map_>
        bool process(UMap_ &orderMap, Map_ &levelMap, const OrderDelete &orderDelete) {
            auto orderId = orderDelete.order_id;
            auto orderIt = orderMap.find(orderId);
            auto order = orderIt->second;

            auto levelIterator = levelMap.find(order.price);
            levelIterator->second -= order.volume;

            if (levelIterator->second == 0) levelIterator = levelMap.erase(levelIterator);
            auto it = levelMap.begin();
            for (uint32_t i = 0; i < depth; i++, it++) {
                if (it == levelIterator) return true;
            }

            return false;
        };

        // update order
        template <typename UMap_, typename Map_>
        bool process(UMap_ &orderMap, Map_ &levelMap, const OrderUpdate &orderUpdate) {
            // iterator to the order <OrderId, Order(price, volume)>
            auto orderIt = orderMap.find(orderUpdate.order_id);

            // Order(price, volume)
            auto &order = orderIt->second;

            auto oldPrice = order.price;
            auto oldVolume = order.volume;

            // iterator to the price level <Price, Volume>
            auto oldLevelIterator = levelMap.find(oldPrice);
            oldLevelIterator->second -= oldVolume;
            if (oldLevelIterator->second == 0) oldLevelIterator = levelMap.erase(oldLevelIterator);

            // early return if new volume is 0
            auto it = levelMap.begin();
            if (orderUpdate.volume == 0) {
                orderMap.erase(orderIt);
                for (uint32_t i = 0; i < depth; i++, it++) {
                    if (it == oldLevelIterator) return true;
                }
                return false;
            }

            order.updateOrder(orderUpdate.price, orderUpdate.volume);

            // iterator to the new price level <Price, Volume>
            auto newLevelIterator = levelMap.emplace(orderUpdate.price, 0).first;
            newLevelIterator->second += orderUpdate.volume;

            for (uint32_t i = 0; i < depth; i++, it++) {
                if (it == oldLevelIterator || it == newLevelIterator) return true;
            }
            return false;
        };

        // execute order
        template <typename UMap_, typename Map_>
        bool process(UMap_ &orderMap, Map_ &levelMap, const OrderTrade &orderTrade) {
            // iterator to the order <OrderId, Order(price, volume)>
            auto orderIt = orderMap.find(orderTrade.order_id);

            // Order(price, volume)
            auto &order = orderIt->second;

            // iterator to the price level <Price, Volume>
            auto levelIterator = levelMap.find(order.price);

            // execute order
            if (order.executeOrder(orderTrade.volume) == 0) orderMap.erase(orderIt);


            // update price level and delete if empty
            levelIterator->second -= orderTrade.volume;
            if (levelIterator->second == 0) levelIterator = levelMap.erase(levelIterator);

            auto it = levelMap.begin();
            for (uint32_t i = 0; i < depth; i++, it++) {
                if (it == levelIterator) return true;
            }

            return false;
        };

        BuyPriceTree &getBuyLevels() {
            return b_levels;
        }

        SellPriceTree &getSellLevels() {
            return s_levels;
        }

        OrdersHashMap &getBuyOrders() {
            return b_orders;
        }

        OrdersHashMap &getSellOrders() {
            return s_orders;
        }
};

struct OrderBookHandler {
    private:
        std::unordered_map<Symbol, OrderBook> orderbooks;

    public:
        uint32_t depth;
        template <typename OrderAction>
        void processOrderAction(const int seqNum, const OrderAction &orderAction) {
            std::string s{orderAction.symbol, 3};
            OrderBook &ob = orderbooks[s];
            ob.depth = depth;
            bool print = false;
            if (orderAction.side == Side::BUY) print = ob.process(ob.getBuyOrders(), ob.getBuyLevels(), orderAction);
            else if (orderAction.side == Side::SELL) print = ob.process(ob.getSellOrders(), ob.getSellLevels(), orderAction);
            if (print) ob.printOrderBook(seqNum);
        };
};