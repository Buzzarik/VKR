#ifndef MESSAGE_PACK_H
#define MESSAGE_PACK_H
#include "../message/message.h"

class MessagePack : public Message {
public:
	MessagePack(MessageType type, const std::vector<char>& buf);
	MessagePack();
	uint32_t size_fragment() const;
	std::vector<char> get_pack_fragment() const;
	bool is_read() const;

	void next_fragment();
	void reload();
	void set_type(MessageType type);
	void set_message(const std::vector<char>& buf);
	void set_current_fragment(uint32_t index);
private:
	std::vector<char> get_header() const;
};

#endif