#pragma once

#include "TypeAlias.hpp"

class Order {
    private:
        OrderId order_id;
        Side side;
        Symbol symbol;
        Quantity quantity;
};