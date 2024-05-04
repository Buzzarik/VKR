#include "messagePack.h"

//========================== class MessagePack ============================
//							  Constructor
MessagePack::MessagePack(MessageType type, const std::vector<char>& buf) 
:Message(type, buf) {}

MessagePack::MessagePack()
:Message(){}
//								Getters
uint32_t MessagePack::size_fragment() const {
	return static_cast<uint32_t>(_frags.size());
}

std::vector<char> MessagePack::get_pack_fragment() const {
	if (_buf.empty() || is_read())
		return {};
	std::vector<char> pack = get_header();
	auto it1 = _buf.begin();
	if (_index > 0) {
		std::advance(it1, _index * MAXLEN);
	}
	auto it2 = it1;
	uint32_t size = _frags[_index].size;
	std::advance(it2, size);
	pack.insert(pack.cend(), it1, it2);
	if (size < MAXLEN){
		std::vector<char> tmp(MAXLEN - size, '\0');
		pack.insert(pack.cend(), tmp.begin(), tmp.end());
	}
	return pack;
}

bool MessagePack::is_read() const {
	return _index >= _frags.size();
}

std::vector<char> MessagePack::get_header() const {
	std::vector<char> header(HEADERLEN);
	size_t i = 0;
	converter conv;

	conv.value = static_cast<uint32_t>(_type);
	for (size_t j = 0; j < 4; j++, i++) {
		header[i] = conv.bytes[j];
	}

	conv.value = static_cast<uint32_t>(_frags.size());
	for (size_t j = 0; j < 4; j++, i++) {
		header[i] = conv.bytes[j];
	}

	conv.value = static_cast<uint32_t>(_frags[_index].id);
	for (size_t j = 0; j < 4; j++, i++) {
		header[i] = conv.bytes[j];
	}

	conv.value = static_cast<uint32_t>(_frags[_index].size);
	for (size_t j = 0; j < 4; j++, i++) {
		header[i] = conv.bytes[j];
	}

	return header;
}
//								Setters
void MessagePack::next_fragment() {
	_index++;
	if (_index >= _frags.size())
		_index = static_cast<uint32_t>(_frags.size());
}

void MessagePack::reload() {
	_index = 0;
}

void MessagePack::set_type(MessageType type) {
	_type = type;
}

void MessagePack::set_message(const std::vector<char>& buf) {
	_index = 0;
	_buf = buf;
	partition_fragments();
}

void MessagePack::set_current_fragment(uint32_t index) {
	if (index > _frags.size())
		_index = static_cast<uint32_t>(_frags.size());
	else {
		_index = index;
	}
}