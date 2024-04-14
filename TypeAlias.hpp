#pragma once

#include <string>

// if changes to types are needed in future
using Price = std::int32_t;
using Quantity = std::uint32_t;
using OrderId = std::string;
enum class Side { Buy, Sell };