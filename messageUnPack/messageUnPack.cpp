#include "messageUnPack.h"

//========================== class MessageUnpack ==========================
//							  Constructor
MessageUnpack::MessageUnpack(const std::vector<char>& pack) {
	set_message(pack);
}

MessageUnpack::MessageUnpack()
	:Message(){}
//								Getters
bool MessageUnpack::is_write() const {
	for (auto [id, size] : _frags) {
		if (size == 0)
			return false;
	}
	return true;
}

std::vector<uint32_t> MessageUnpack::get_missing_fragment() const {
	std::vector<uint32_t> missing;
	for (auto [id, size] : _frags) {
		if (size == 0)
			missing.push_back(id);
	}
	return missing;
}
//								Setters
void MessageUnpack::clear() {
	_frags.clear();
	_buf.clear();
	_type = MessageType::MAX_MESSAGE_TYPES;
}

void MessageUnpack::add_fragment(const std::vector<char>& frag) {
	MessageUnpack fragment(frag);
	if (is_empty() || _type != fragment._type || _frags.size() != fragment._frags.size()) {
		*this = fragment;
		return;
	}
	_buf.insert(_buf.cend(), fragment._buf.begin(), fragment._buf.end());


	for (auto [id, size] : fragment._frags) {
		if (size != 0)
			_frags[id - 1] = { id, size };
	}
}

void MessageUnpack::set_message(const std::vector<char>& pack) {
	clear();
	if (pack.size() < HEADERLEN)
		return;
	auto it = pack.begin();
	std::advance(it, HEADERLEN);
	set_header({ pack.begin(), it });
	auto it2 = it;
	std::advance(it2, _index);
	_buf = { it, it2 };
}

void MessageUnpack::set_header(const std::vector<char>& header) {
	converter conv;
	size_t i = 0;

	for (size_t j = 0; j < 4; j++, i++) {
		conv.bytes[j] = header[i];
	}
	_type = static_cast<MessageType>(conv.value);

	for (size_t j = 0; j < 4; j++, i++) {
		conv.bytes[j] = header[i];
	}
	uint32_t count = conv.value;

	for (size_t j = 0; j < 4; j++, i++) {
		conv.bytes[j] = header[i];
	}
	uint32_t id = conv.value;

	for (size_t j = 0; j < 4; j++, i++) {
		conv.bytes[j] = header[i];
	}
	uint32_t size = conv.value;

	_index = size;

	_frags.resize(count);
	
	for (auto &fr : _frags){
		fr = {0, 0};
	}

	_frags[id - 1] = { id, size };

}