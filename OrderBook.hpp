#include "Order.hpp"
#include <map>
#include <unordered_map>

class OrderBook {
    private:
        Symbol symbol;
        std::map<Price, Quantity, std::greater<Price>> buys;
        std::map<Price, Quantity, std::greater<Price>> sells;
        std::unordered_map<OrderId, Order> b_orders, s_orders;
        uint32_t depth;

    public:
        bool processAddOrder(OrderId orderId, Side side, Quantity quantity, Price price);
        bool processDeleteOrder(OrderId orderId, Side side);
        bool processUpdateOrder(OrderId orderId, Side side, Quantity quantity, Price price);
        bool processExecuteOrder(OrderId orderId, Side side, Quantity quantity);
        void printOrderBook();
};

template <typename UMap_, typename Map_, typename OrderId_, typename Price_, typename Quantity_>
bool addOrder(UMap_ orderMap, Map_ levelMap, OrderId_ orderId, Quantity_ quantity, Price_ price) {
    levelMap[price] += quantity;

    orderMap.emplace(std::piecewise_construct, 
                std::forward_as_tuple(orderId), 
                std::forward_as_tuple(price, quantity));

    for (int i = 0, auto it = orderMap.begin(); it != orderMap.end() && i < depth; i++, it++) {
        if (it == modifiedIt) return true;
    }

    return false;
};

template <typename UMap_, typename Map_, typename OrderId_>
bool deleteOrder(UMap_ orderMap, Map_ levelMap, OrderId_ orderId) {
    orderMap[orderId] += quantity;
};