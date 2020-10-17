#include "RpcMsgParser.h"
#include "Logger/Logger.h"

bool RpcMsgParser::parse(MsgBuffer* buffer)
{
	gotAll_ = false;
	packets_.reset(new std::vector<RpcMsgPacket>());
	while (buffer->readableBytes() > 0)
	{
		static RpcLenType headSize = sizeof(RpcMsgHead);
		if (buffer->readableBytes() >= headSize)
		{
			RpcMsgHead* pHead = (RpcMsgHead*)buffer->peek();
			RpcLenType length = pHead->rpcLen_;
			if (length > RpcMsgPacket::maxRpcMsgSize_)
			{
				LOG_ERROR << "The size of the rpc message is too large!";
				buffer->retrieveAll();
				if (!packets_->empty()) return false;
			}
			if (buffer->readableBytes() >= length)
			{
				std::shared_ptr<std::string> body =
					std::make_shared<std::string>((*buffer)[headSize], length - headSize);
				packets_->emplace_back(*pHead, body);
				gotAll_ = true;
				buffer->retrieve(length);
			}
		}
	}
	return true;
}

bool RpcMsgParser::gotAll(MsgPtr& msg)
{
	LOG_CASSERT(msg->empty());
	if (!gotAll_)
		return false;
	msg = packets_;
	packets_ = nullptr;
	return true;
}

