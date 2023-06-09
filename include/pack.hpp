#ifndef PACK_HPP
#define PACK_HPP

#include <vector>
#include <string>

//[program-4-solution] [16]
typedef unsigned char u8; //unsigned byte
typedef std::vector<u8> vec; //array
typedef std::string string; //string

#define PACK109_M8    0xae
#define PACK109_S8    0xaa
#define PACK109_A8    0xac
#define PACK109_U8    0xa2

struct File{ //map tag --> string "File"
    string name; //string name --> string filename
    vec bytes; //string bytes --> vec bytes
};

struct Request{
    string name;
};
/* for reference logic
struct Person { //string "person"
  u8 age;
  f32 height;
  string name;
};
  struct Person ann = {10, 3.4, "Ann"}; //u8 age, f32 height, string name
  vec v17{0xAE, 0x01, 
          0xAA, 0x06, 0x50, 0x65, 0x72, 0x73, 0x6F, 0x6E, //"Person"
          0xae, 0x03, //3 pairs
          0xAA, 0x03, 0x61, 0x67, 0x65, //"age"
          0xA2, 0x0A, //10
          0xAA, 0x06, 0x68, 0x65, 0x69, 0x67, 0x68, 0x74, //"height"
          0xA8, 0x40, 0x59, 0x99, 0x9A, //3.4 in hex is 0x4059999a
          0xAA, 0x04, 0x6E, 0x61, 0x6D, 0x65, //"name"
          0xAA, 0x03, 0x41, 0x6E, 0x6E};  //"Ann"
*/

namespace pack{
    //unsigned char
    vec serialize(u8 item);
    u8 deserialize_u8(vec bytes);
    //strings
    vec serialize(string item);
    string deserialize_string(vec bytes);
    //vector of u8
    vec serialize(std::vector<u8> item);
    std::vector<u8> deserialize_vec_u8(vec bytes);
    //file
    vec serialize(struct File item);
    struct File deserialize_file(vec bytes);
    //request
    vec serialize(struct Request item);
    struct Request deserialize_request(vec bytes);

}

#endif