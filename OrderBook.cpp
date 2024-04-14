#include <iostream>
#include "OrderBook.hpp"

// fine with polluting the namespace
enum Side { buy, sell };

void OrderBook::processAddOrder(OrderId orderId, Side side, Quantity quantity, Price price) {
    if (side == buy) {
        addOrder(b_orders, buys, orderId, quantity, price);
    } else {
        addOrder(s_orders, sells, orderId, quantity, price);
    }
}

void OrderBook::processDeleteOrder(OrderId orderId, Side side) {
    
}

void OrderBook::processUpdateOrder(OrderId orderId, Side side, Quantity quantity, Price price) {

}

void OrderBook::processExecuteOrder(OrderId orderId, Side side, Quantity quantity) {

}