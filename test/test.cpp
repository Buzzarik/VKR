#include <gtest/gtest.h>
#include "../directory/directory.h"
#include "../messagePack/messagePack.h"
#include "../messageUnPack/messageUnPack.h"
#include "../socketAddress/socketAddress.h"

#define MAIN_DIR "/Users/evgeniy/Document/Programming/C++/P2P.2.0/serverFiles"
#define MAIN_FILE "/Users/evgeniy/Document/Programming/C++/P2P.2.0/serverFiles/bear.png"

class Test_SocketAddress : public ::testing::Test {};

class Test_Message : public ::testing::Test {};

class Test_Directory : public ::testing::Test {};



TEST(Test_SocketAddress, output_data){
    SocketAddress addr("127.197.23.2", 12345);
    EXPECT_EQ("127.197.23.2", addr.ip());
    EXPECT_EQ(12345, addr.port());
}

TEST(Test_Directory, incorrect_path){
    Directory dir;
    EXPECT_EQ(false, dir.is_directory());
    EXPECT_EQ(false, dir.is_file());
    dir.set_path("jqkva.v");
    EXPECT_EQ(false, dir.is_directory());
    EXPECT_EQ(false, dir.is_file());
}

TEST(Test_Directory, check_is_file){
    Directory dir(MAIN_FILE);
    EXPECT_EQ(false, dir.is_directory());
    EXPECT_EQ(true, dir.is_file());
}

TEST(Test_Directory, check_is_directory){
    Directory dir(MAIN_DIR);
    EXPECT_EQ(true, dir.is_directory());
    EXPECT_EQ(false, dir.is_file());
}

TEST(Test_Directory, get_count_files_incorrect_path){
    Directory dir;
    EXPECT_EQ(0, dir.get_count_files());
}

TEST(Test_Directory, get_count_files_file_path){
    Directory dir(MAIN_FILE);
    EXPECT_EQ(1, dir.get_count_files());
}

TEST(Test_Directory, get_count_files_directory_path){
    Directory dir(MAIN_DIR);
    EXPECT_EQ(13, dir.get_count_files());
}

TEST(Test_Directory, get_list_files_incorrect_path){
    Directory dir;
    EXPECT_EQ("", dir.get_list_files());
}

TEST(Test_Directory, get_list_files_file_path){
    Directory dir(MAIN_FILE);
    EXPECT_EQ("bear.png", dir.get_list_files());
}

TEST(Test_Directory, get_list_files_directory_path){
    Directory dir(MAIN_DIR);
    EXPECT_EQ(false, dir.get_list_files().empty());
}

TEST(Test_Directory, find_files_incorrect_path){
    Directory dir;
    EXPECT_EQ(std::vector<std::string>(), dir.find_files("bear.png"));
}

TEST(Test_Directory, find_files_file_path){
    Directory dir(MAIN_FILE);
    std::vector<std::string> ans {"bear.png"};
    EXPECT_EQ(ans, dir.find_files("bear.png"));
}

TEST(Test_Directory, find_files_directory_path){
    Directory dir(MAIN_DIR);
    std::vector<std::string> ans {MAIN_FILE};
    EXPECT_EQ(ans, dir.find_files("bear.png"));

    ans.clear();
    EXPECT_EQ(ans, dir.find_files("be.png"));  
}

TEST(Test_Message, test_methods_message){
    std::string text = "Hello, world!";
    for (size_t i = 0; i < static_cast<size_t>(MessageType::MAX_MESSAGE_TYPES); i++){
        Message msg(static_cast<MessageType>(i), {text.begin(), text.end()});
        EXPECT_EQ(static_cast<MessageType>(i), msg.get_type());
        EXPECT_EQ(false, msg.is_empty());
        EXPECT_EQ(text.size(), msg.size());
        EXPECT_EQ(std::vector<char>(text.begin(), text.end()), msg.get_buffer());
    }

    Message msg;
    EXPECT_EQ(MessageType::MAX_MESSAGE_TYPES, msg.get_type());
    EXPECT_EQ(true, msg.is_empty());
    EXPECT_EQ(std::vector<char>(), msg.get_buffer());
}

TEST(Test_Message, check_methods_fragments_pack){
    std::vector<char> text(10000, '1');
    MessagePack msg(MessageType::MAX_MESSAGE_TYPES, text);
    EXPECT_EQ(10, msg.size_fragment());
    EXPECT_EQ(MAXLEN + HEADERLEN, msg.get_pack_fragment().size());
    msg.set_current_fragment(10000);
    EXPECT_EQ(true, msg.is_read());
    msg.reload();
    uint32_t count = 0;
    while (!msg.is_read()){
        msg.get_pack_fragment();
        msg.next_fragment();
        count++;
    }
    EXPECT_EQ(10, count);
}

TEST(Test_Message, check_methods_unpack){
    std::vector<char> data;
    for (size_t i = 0; i < 10000; i++){
        data.push_back(rand() % 100);
    }
    MessagePack pack(MessageType::TYPE_FILENAME, data);
    MessageUnpack unpack;
    EXPECT_EQ(std::vector<uint32_t>(), unpack.get_missing_fragment());
    EXPECT_EQ(true, unpack.is_write());
    EXPECT_EQ(true, unpack.is_empty());
    while (!pack.is_read()){
        unpack.add_fragment(pack.get_pack_fragment());
        pack.next_fragment();
    }
    EXPECT_EQ(std::vector<uint32_t>(), unpack.get_missing_fragment());
    EXPECT_EQ(true, unpack.is_write());
    EXPECT_EQ(false, unpack.is_empty());
    EXPECT_EQ(pack.get_buffer(), unpack.get_buffer());
    EXPECT_EQ(pack.get_type(), unpack.get_type());

    unpack.clear();
    EXPECT_EQ(std::vector<uint32_t>(), unpack.get_missing_fragment());
    EXPECT_EQ(true, unpack.is_write());
    EXPECT_EQ(true, unpack.is_empty());
}

TEST(Test_Message, find_missing_fragments){
    std::vector<char> data;
    for (size_t i = 0; i < 10000; i++){
        data.push_back(rand() % 100);
    }
    MessagePack pack(MessageType::TYPE_FILENAME, data);
    MessageUnpack unpack;
    for (size_t i = 0; i < 10; i += 2){
        pack.set_current_fragment(i);
        unpack.add_fragment(pack.get_pack_fragment());
    }
    std::vector<uint32_t> missing{2, 4, 6, 8, 10};
    EXPECT_EQ(missing, unpack.get_missing_fragment());
    EXPECT_EQ(false, unpack.is_write());

    for (size_t i = 1; i < 10; i += 2){
        pack.set_current_fragment(i);
        unpack.add_fragment(pack.get_pack_fragment());
    }

    EXPECT_EQ(std::vector<uint32_t>(), unpack.get_missing_fragment());
    EXPECT_EQ(true, unpack.is_write());
    EXPECT_EQ(false, unpack.is_empty());
    EXPECT_EQ(pack.get_buffer(), unpack.get_buffer());
    EXPECT_EQ(pack.get_type(), unpack.get_type());

}

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}