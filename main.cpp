#include "message.h"
#include <OrderBook.cpp>
#include <unordered_map>

int main()
{
    auto obh = OrderBookHandler();
	auto opt = MessageReader::ReadNext(std::cin);
	for (; opt.has_value(); opt = MessageReader::ReadNext(std::cin))
	{
		auto& mh = opt.value();
		std::string str;
		switch(mh.header.msg_type)
		{
		case EventType::ADD:
			auto order = std::any_cast<OrderAdd>(mh.msg);
			str = order.String();
			processOrderAction(obh, order);
			break;
		case EventType::UPDATE:
			auto order = std::any_cast<OrderUpdate>(mh.msg);
			str = order.String();
			processOrderAction(obh, order);
			break;
		case EventType::DELETE:
			auto order = std::any_cast<OrderDelete>(mh.msg);
			str = order.String();
			processOrderAction(obh, order);
			break;
		case EventType::TRADED:
			auto order = std::any_cast<OrderTrade>(mh.msg);
			str = order.String();
			processOrderAction(obh, order);
			break;
		default:
			str = "UNKNOWN MESSAGE";
		}
		std::cout << mh.header.seq_num << "/" << static_cast<char>(mh.header.msg_type) << "/" << str << std::endl;
	}
	return 0;
}
