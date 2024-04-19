#include "Order.hpp"
#include "message.h"
#include <map>
#include <unordered_map>
#include <algorithm>

using OrdersHashMap = std::unordered_map<OrderId, Order>;
using BuyPriceTree = std::map<Price, Volume, std::greater<Price>>;
using SellPriceTree = std::map<Price, Volume, std::less<Price>>;
using Iterator = typename std::map<Price, Volume>::iterator;
class OrderBook {
    private:
        BuyPriceTree b_levels;
        SellPriceTree s_levels;
        Iterator b_iterator = b_levels.begin(), s_iterator = s_levels.begin();
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
        void printOrderBook(const std::string& symbol, const int seqNum) {
            std::cout << seqNum << ", " << symbol << ", ";
            printMap(b_levels);
            std::cout << ", ";
            printMap(s_levels);
            std::cout << "\n";
        };

        // add order
        template <typename UMap_, typename Map_>
        bool process(UMap_ &orderMap, Map_ &levelMap, Iterator &itr, const OrderAdd &orderAdd) {
            auto orderId = orderAdd.order_id;
            auto price = orderAdd.price;
            auto volume = orderAdd.volume;

            auto levelIterator = levelMap.emplace(price, 0).first;
            levelIterator->second += volume;
            orderMap.emplace(orderId, Order{price, volume, levelIterator});

            // checking if need to return
            bool rtn = false;
            if (levelIterator->first <= itr->first) rtn = true;
            else if (levelMap.size() >= depth && levelIterator->first > itr->first) rtn = false;
            else rtn = true;
            // moving iterator
            if (levelMap.size() >= depth && levelIterator->first < itr->first) itr--;
            else if (levelMap.size() < depth && levelIterator->first > itr->first) itr++;

            return rtn;
        };

        // delete order
        template <typename UMap_, typename Map_>
        bool process(UMap_ &orderMap, Map_ &levelMap, Iterator &itr, const OrderDelete &orderDelete) {
            auto orderId = orderDelete.order_id;
            auto orderIt = orderMap.find(orderId);
            auto order = orderIt->second;

            auto levelIterator = order.getIterator();
            levelIterator->second -= order.volume;

            bool rtn = false;
            if (levelIterator->first <= itr->first) {
                rtn = true;
            }

            if (levelIterator->second == 0) {
                // itr will be deleted so decrement as Nth deleted, we want point to N-1
                if (levelMap.size() <= depth && levelIterator->first == itr->first) itr--;

                // itr will move from N to N-1th, so increment itr
                else if (levelMap.size() > depth && levelIterator->first <= itr->first) itr++;
                levelMap.erase(levelIterator);
            }

            return rtn;
        };

        // update order
        template <typename UMap_, typename Map_>
        bool process(UMap_ &orderMap, Map_ &levelMap, Iterator &itr, const OrderUpdate &orderUpdate) {
            // iterator to the order <OrderId, Order(price, volume)>
            auto orderIt = orderMap.find(orderUpdate.order_id);

            // Order(price, volume)
            auto &order = orderIt->second;

            auto oldPrice = order.price;
            auto oldVolume = order.volume;
            auto newPrice = orderUpdate.price;
            auto newVolume = orderUpdate.volume;

            if (oldPrice == newPrice 
            && oldVolume == newVolume) return false;

            // iterator to the price level <Price, Volume>
            auto oldLevelIterator = order.getIterator();
            oldLevelIterator->second -= oldVolume;

            order.updateOrder(newPrice, newVolume);

            bool insertMade = (levelMap.find(newPrice) != levelMap.end());
            bool deleteMade = (oldPrice != newPrice && oldLevelIterator->second == 0);
            auto newLevelIterator = oldLevelIterator;

            bool rtn = false;
            auto nthPrice = itr->first;
            auto sz = levelMap.size();
            if ((oldPrice <= nthPrice) || (newPrice <= nthPrice)) rtn = true;
            if (oldPrice == newPrice || !rtn) {
                newLevelIterator = levelMap.emplace(order.price, 0).first;
            } else if (sz < depth) {
                if ((newPrice > nthPrice) && insertMade) {
                    newLevelIterator = levelMap.emplace(order.price, 0).first;
                    itr++;
                } else if ((oldPrice == nthPrice) && deleteMade) {
                    newLevelIterator = levelMap.emplace(order.price, 0).first;
                    itr--;
                }
            } else if (sz == depth) {
                if ((insertMade && !deleteMade && oldPrice != nthPrice)
                || ((insertMade || deleteMade) && oldPrice == nthPrice)) {
                    newLevelIterator = levelMap.emplace(order.price, 0).first;
                    itr--;
                } else if (oldPrice < nthPrice && nthPrice < newPrice && insertMade && deleteMade) {
                    newLevelIterator = levelMap.emplace(order.price, 0).first;
                    itr++;
                }
            } else {
                if (oldPrice <= nthPrice && deleteMade && !(newPrice < nthPrice && insertMade)) {
                    newLevelIterator = levelMap.emplace(order.price, 0).first;
                    itr++;
                } else if (newPrice < nthPrice && insertMade && !(oldPrice < nthPrice && deleteMade)) {
                    newLevelIterator = levelMap.emplace(order.price, 0).first;
                    itr--;
                }
            }

            // iterator to the new price level <Price, Volume>
            newLevelIterator->second += orderUpdate.volume;
            order.setIterator(newLevelIterator);
            if (oldLevelIterator->second == 0) oldLevelIterator = levelMap.erase(oldLevelIterator);
            return rtn;
        };

        // execute order
        template <typename UMap_, typename Map_>
        bool process(UMap_ &orderMap, Map_ &levelMap, Iterator &itr, const OrderTrade &orderTrade) {
            // iterator to the order <OrderId, Order(price, volume)>
            auto orderIt = orderMap.find(orderTrade.order_id);

            // Order(price, volume)
            auto &order = orderIt->second;

            // iterator to the price level <Price, Volume>
            auto levelIterator = order.getIterator();

            // execute order
            if (order.executeOrder(orderTrade.volume) == 0) orderMap.erase(orderIt);

            // update price level and delete if empty
            levelIterator->second -= orderTrade.volume;

            bool rtn = false;
            if (levelIterator->first <= itr->first) {
                rtn = true;
            }

            if (levelIterator->second == 0) {
                // itr will be deleted so decrement as Nth deleted, we want point to N-1
                if (levelMap.size() <= depth && levelIterator->first == itr->first) itr--;

                // itr will move from N to N-1th, so increment itr
                else if (levelMap.size() > depth && levelIterator->first <= itr->first) itr++;
                levelMap.erase(levelIterator);
            }

            return rtn;
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

        Iterator& getBuyItr() {
            return b_iterator;
        }

        Iterator& getSellItr() {
            return s_iterator;
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
            if (orderAction.side == Side::BUY) print = ob.process(ob.getBuyOrders(), ob.getBuyLevels(), ob.getBuyItr(), orderAction);
            else if (orderAction.side == Side::SELL) print = ob.process(ob.getSellOrders(), ob.getSellLevels(), ob.getSellItr() ,orderAction);
            if (print) ob.printOrderBook(s, seqNum);
        };
};