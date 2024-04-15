#include <iostream>
#include "OrderBook.hpp"
        // returns whether to reprint orderbook price levels up to depth

        bool OrderBook::process(std::map<>, const OrderAdd &orderAdd) {
        }

        bool OrderBook::process(const OrderUpdate &orderUpdate) {
            if (orderUpdate.side == Side::BUY) {
                return addOrder(b_orders, b_levels, orderAdd.order_id, orderAdd.volume, orderAdd.price);
            } else if (orderAdd.side == Side::BUY) {
                return addOrder(s_orders, s_levels, orderAdd.order_id, orderAdd.volume, orderAdd.price);
            } else {
                // dont make any changes, return false
                // assume valid input so we never reach this state
                return false;
            }
        }

        bool OrderBook::process(const OrderDelete &orderDelete) {
            if (orderDelete.side == Side::BUY) {
                return addOrder(b_orders, b_levels, orderAdd.order_id);
            } else if (orderAdd.side == Side::BUY) {
                return addOrder(s_orders, s_levels, orderAdd.order_id);
            } else {
                // dont make any changes, return false
                // assume valid input so we never reach this state
                return false;
            }
        }

        bool OrderBook::process(const OrderTrade &orderTrade) {
            if (orderTrade.side == Side::BUY) {
                return addOrder(b_orders, b_levels, orderTrade.order_id, orderTrade.volume);
            } else if (orderTrade.side == Side::BUY) {
                return addOrder(s_orders, s_levels, orderTrade.order_id, orderTrade.volume);
            } else {
                // dont make any changes, return false
                // assume valid input so we never reach this state
                return false;
            }
        }