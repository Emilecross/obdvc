#include "Order.hpp"
#include "message.h"
#include <map>
#include <unordered_map>

class OrderBookHandler {
    private:
        std::unordered_map<Symbol, OrderBook> orderbooks;
};

class OrderBook {
    private:
        Symbol symbol;
        std::map<Price, Quantity, std::greater<Price>> b_levels;
        std::map<Price, Quantity, std::less<Price>> s_levels;
        std::unordered_map<OrderId, Order> b_orders, s_orders;
        uint32_t depth;

    public:
        // boolean value represents if depth needs to be reprinted
        bool process(const OrderAdd &orderAdd);
        bool process(const OrderUpdate &orderUpdate);
        bool process(const OrderDelete &orderDelete);
        bool process(const OrderTrade &orderTrade);
        void printOrderBook();
};

template <class OrderBookHandler, typename OrderAction>
void processOrderAction(OrderBookHandler orderBookHandler, OrderAction orderAction) {
    OrderBook &ob = orderBookHandler[orderAction.symbol];
    bool print = false;
    if (orderAction.side == Side::BUY) print = ob.process(ob.b_levels, ob.b_orders, orderAction);
    else if (orderAction.side == Side::SELL) print = ob.process(ob.s_levels, ob.s_orders, orderAction);
    if (print) OrderBook::printOrderBook();
};

template <typename UMap_, typename Map_, typename OrderAction>
bool addOrder(UMap_ &orderMap, Map_ &levelMap, OrderAction orderAction) {
    auto orderId = orderAction.order_id;
    auto price = orderAction.price;
    auto volume = orderAction.volume;

    levelMap[price] += volume;
    orderMap.emplace(std::piecewise_construct, 
                std::forward_as_tuple(orderId), 
                std::forward_as_tuple(price, quantity));

    for (int i = 0, auto it = orderMap.begin(); it != orderMap.end() && i < depth; i++, it++) {
        if (it == modifiedIt) return true;
    }

    return false;
};
