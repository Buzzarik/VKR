#ifndef MESSAGE_UNPACK_H
#define MESSAGE_UNPACK_H
#include "../message/message.h"

class MessageUnpack : public Message {
public:
	MessageUnpack(const std::vector<char>& pack);
	MessageUnpack();
	bool is_write() const;
	std::vector<uint32_t> get_missing_fragment() const;

	void clear();
	void add_fragment(const std::vector<char>& frag);
	void set_message(const std::vector<char>& pack);
private:
	void set_header(const std::vector<char>& header);
};

#endif