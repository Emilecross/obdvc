#include "message.h"
#include <OrderBook.cpp>
#include <unordered_map>

int main()
{
    std::unordered_map<Symbol, OrderBook> orderBooks;
	auto opt = MessageReader::ReadNext(std::cin);
	for (; opt.has_value(); opt = MessageReader::ReadNext(std::cin))
	{
		//implement me
		auto& mh = opt.value();
		std::string str;
		switch(mh.header.msg_type)
		{
		case EventType::DELETE:
			str = std::any_cast<OrderDelete>(mh.msg).String();
			break;
		case EventType::TRADED:
			str = std::any_cast<OrderTrade>(mh.msg).String();
			break;
		default:
			str = "UNKNOWN MESSAGE";
		}
		std::cout << mh.header.seq_num << "/" << static_cast<char>(mh.header.msg_type) << "/" << str << std::endl;
	}

	return 0;
}
