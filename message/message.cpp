#include "message.h"
#include <iterator>

//========================== class Message ================================
//							  Constructor
Message::Message()
	:_type(MessageType::MAX_MESSAGE_TYPES), _index(0), _buf({})
{}

Message::Message(MessageType type, const std::vector<char>& buf)
:_type(type),  _index(0), _buf(buf) {
	partition_fragments();
}

//								Getters
uint32_t Message::size() const {
	return static_cast<uint32_t>(_buf.size());
}

std::vector<char> Message::get_buffer() const {
	return _buf;
}

MessageType Message::get_type() const {
	return _type;
}

bool Message::is_empty() const {
	return _buf.empty();
}

//								Setters
void Message::partition_fragments() {
	if (_buf.empty()) {
		_frags.clear();
		return;
	}
	uint32_t count = static_cast<uint32_t>(ceil(static_cast<double>(_buf.size()) / MAXLEN));
	_frags.resize(count);
	for (uint32_t i = 0; i < count - 1; i++) {
		_frags[i] = { i + 1, MAXLEN };
	}
	_frags[count - 1] = { count, static_cast<uint32_t>(_buf.size()) - (count - 1) * MAXLEN };
}