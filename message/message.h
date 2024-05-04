#ifndef MESSAGE_H
#define MESSAGE_H
#include <vector>

#define MAXLEN 1008  //����� ��������� 1024 - 16 ( -���������)
#define HEADERLEN 4 * 4 //4 * 4 = 16 ����

enum class MessageType {
//передача файлов
	TYPE_START_TRANSFER_FILES,		//начала передачи файлов (передаем кол-во файлов)
	TYPE_FINISH_TRANSFER_FILES,		//конец передачи файлов
	TYPE_START_TRANSFER_DIRECTORY,	//передача директории (передача названия + надо ее создать)
	TYPE_FINISH_TRANSFER_DIRECTORY,	//конец записи в директорию (выход на уровень выше в файл системе)
	TYPE_FILENAME,					//передача имени файла
	TYPE_TRANSFER_FILE,				//передача самого файла
	TYPE_LOAD_FILES,
	TYPE_INCORRECT_FILENAME,

	TYPE_LIST_FILES,
	TYPE_ERROR_ENTER,
	TYPE_EXIT_CONNECT,
//неизветсное сообщение
	MAX_MESSAGE_TYPES
};

union converter {
	unsigned char bytes[4];
	uint32_t value;
};

class Message {
public:
	Message(MessageType type, const std::vector<char>& buf);
	Message();
	uint32_t size() const;
	std::vector<char> get_buffer() const;
	MessageType get_type() const;
	bool is_empty() const;
protected:
	struct Fragments {
		uint32_t id;
		uint32_t size;
	};
	void partition_fragments();
	MessageType _type;
	uint32_t _index;
	std::vector<Fragments> _frags;
	std::vector<char> _buf;
};

#endif