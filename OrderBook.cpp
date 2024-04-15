#include <iostream>
#include "OrderBook.hpp"


bool OrderBook::processAddOrder(OrderId orderId, Side side, Quantity quantity, Price price) {
    if (side == buy) {
        addOrder(b_orders, buys, orderId, quantity, price);
    } else {
        addOrder(s_orders, sells, orderId, quantity, price);
    }
}

bool OrderBook::processDeleteOrder(OrderId orderId, Side side) {
    
}

bool OrderBook::processUpdateOrder(OrderId orderId, Side side, Quantity quantity, Price price) {

}

bool OrderBook::processExecuteOrder(OrderId orderId, Side side, Quantity quantity) {

}