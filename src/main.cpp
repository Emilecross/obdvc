#include "message.h"
#include "OrderBook.hpp"

int main(int argc, char* argv[])
{
	if (argc != 2) {
        std::cerr << "Usage: cat data/<data>.stream | ./main" " <number>" << std::endl;
        return -1;
    }
    auto obh = OrderBookHandler();
	obh.depth = std::stoi(argv[1]);
	auto opt = MessageReader::ReadNext(std::cin);
	for (; opt.has_value(); opt = MessageReader::ReadNext(std::cin))
	{
		auto& mh = opt.value();
		std::string str;
		switch(mh.header.msg_type)
		{
		case EventType::ADD: 
		{
			auto order = std::any_cast<OrderAdd>(mh.msg);
			str = order.String();
			obh.processOrderAction(mh.header.seq_num, order);
			break;
		}
		case EventType::UPDATE: 
		{
			auto order = std::any_cast<OrderUpdate>(mh.msg);
			str = order.String();
			obh.processOrderAction(mh.header.seq_num, order);
			break;
		}
		case EventType::DELETE:
		{
			auto order = std::any_cast<OrderDelete>(mh.msg);
			str = order.String();
			obh.processOrderAction(mh.header.seq_num, order);
			break;
		}
		case EventType::TRADED:
		{
			auto order = std::any_cast<OrderTrade>(mh.msg);
			str = order.String();
			obh.processOrderAction(mh.header.seq_num, order);
			break;
		}
		default:
			str = "UNKNOWN MESSAGE";
		}
		std::cout << mh.header.seq_num << "/" << static_cast<char>(mh.header.msg_type) << "/" << str << "\n\n";
	}
	return 0;
}
