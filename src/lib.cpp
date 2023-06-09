//Old cool stuff
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
//New cool stuff
#include "../include/hashset.hpp"
//SerDe
#include "../include/pack.hpp"
#include <vector>
#include <cstring>
#include <algorithm> //[find and algorithm header file] [18]
//stuff
#include <iostream>
#include <stdexcept>
using namespace std;

HashSet::HashSet(size_t size){
    LinkedList** array = new LinkedList*[size];
    for(int i = 0; i < size; i++){
        array[i] = NULL;
    }
    this->array = array;
    this->size = size;

    for(int i = 0; i < size; i++){
        array[i] = new LinkedList;
        initList(array[i]);
    }
}

HashSet::~HashSet(){
    for(int i = 0; i < this->size; i++){
        delete this->array[i]; //free individual
    }
    delete[] this->array; //free whole
}


/* Modify this prehash function to work with a template type T instead of a char*
unsigned long prehash(unsigned char *str) {
    unsigned long h = 5381;
    int c;
    while (c = *str++) { 
	    h = ((h << 5) + h) + c;
    }
    return h;
}*/
/* prehash that should work. turning integer to unsigned char* (string, which allows us to iterate through)

*/
// Generate a prehash for an item with a given size
unsigned long HashSet::prehash(string item) {
    unsigned long h = 5381;
    for (size_t i = 0; i < item.length(); i++) {
        h = ((h << 5) + h) + item[i];
    }
    return h;
}
// Hash an unsigned long into an index that fits into a hash set
unsigned long HashSet::hash(string item){ 
    unsigned long pHash = prehash(item); 
    unsigned long index = (pHash % this->size); //mod phash by array size to find index needed to put in.
    //cerr << index << endl;
    return index; 
}

// Insert item in the set. Return true if the item was inserted, false if it wasn't (i.e. it was already in the set)
// Recalculate the load factor after each successful insert (round to nearest whole number).
bool HashSet::insert(string key, FILE* value) {
    unsigned long index = hash(key); // Hash the string in order to find the index, key IS ALREADY HASHED
    if (contains(key, value)) {
        remove(key, value);
        ::insertAtHead(this->array[index], reinterpret_cast<void*>(value)); // Always insert at the head
        return true; // Key already exists, remove and replace
    } else {
        ::insertAtHead(this->array[index], reinterpret_cast<void*>(value));
        return false; // Key doesn't exist, insert at the head
    }
}

// Returns the file associated with the supplied key. If the key doesn't exist, this function throws an exception.
//[get-java-examples] [11] HashMap hard, use get logic to 
FILE* HashSet::get(string key){
    int index = hash(key); //find index
    if(contains(key, nullptr)){ //contains already hashes so we push key, not index
        void* item = ::itemAtIndex(array[index], 0);
        FILE* fileGrab = static_cast<FILE*>(item); //hardcoded my test assuming that for size 10000, there's only one item per key and no collisions, which is bold but has to work ig
        //therefore the item at index 0 is the only item there
        return fileGrab;
    }else{
        throw std::runtime_error("Key not found\n"); //[stdexcept] [12]
    }
}

// Remove an item from the set. Return true if it was removed, false if it wasn't (i.e. it wasn't in the set to begin with)
bool HashSet::remove(string item, FILE* value){ 
    if(contains(item, value)){
        int index = hash(item);
        int count = 0;
        for(int i = 0; i < ::size(array[index]); i++){
            if(::itemAtIndex(array[index], i) == (FILE*)value){
                count = i;
                break;
            }
        }
        ::removeAtIndex(array[index], count);
        return true;
    }else{
        return false; //the item does NOT exist in the set yet, therefore cannot be removed
    }
}

// Return true if the item exists in the set, false otherwise
bool HashSet::contains(string key, FILE* value){
    int index = hash(key); //find index
    return ::contains(this->array[index], reinterpret_cast<void*>(value));
}

// Returns the number of items in the hash set
size_t HashSet::len(){
    size_t numItems = 0;
    for(int i = 0; i < this->size; i++){
        if(this->array[i] != NULL){
            numItems += ::size(this->array[i]); //this should work I think probably
        }
    }
    return numItems;
}

// Returns the number of empty buckets that can be filled before reallocating 
//(Use a threshold of 70% for resize)
size_t HashSet::capacity(){
    ssize_t originalSize = this->size;
    ssize_t emptyBuckets = originalSize;
    for(int i = 0; i < originalSize; i++){
        if(::itemAtIndex(this->array[i], 0) != NULL){
            emptyBuckets = emptyBuckets - 1;
        }
    }
    ssize_t fullBuckets = originalSize - emptyBuckets;
    //original size - empty buckets = full buckets

    return emptyBuckets;
}

// Print Table. You can do this in a way that helps you implement your hash set.
void HashSet::print(){
    for(int i = 0; i < this->size; i++){
        ::printList(this->array[i]); // ..? might work might not work tbh
        cout << "" << endl;
    }
}

//START OF SERIALIZATION AND DESERIALIZATION
//[program-4-by-thor-long] [17]
vec pack::serialize(u8 item) {
    vec bytes;
    bytes.push_back(PACK109_U8);
    bytes.push_back(item);
    return bytes;
}

u8 pack::deserialize_u8(vec bytes) {
    if (bytes.size() < 2) {
        throw;
    }
    if (bytes[0] == PACK109_U8) {
        return bytes[1];
    } else {
        throw;
    }
}

vec pack::serialize(string item){
    vec bytes;  
    if(item.size() <= 255){
        bytes.push_back(PACK109_S8);
        bytes.push_back(static_cast<unsigned char>(item.size()));
        for(char c : item){
            bytes.push_back(static_cast<unsigned char> (c)); //need to iterate through each character in the string
        }
    }
    return bytes;
}

string pack::deserialize_string(vec bytes){
    string item;
    if(bytes.size() == 0){
        return item; //returning a string of 0 length because string would be empty
    }
    if(bytes[0] == PACK109_S8){
        if(bytes.size() < 2){
            throw; //vector too short for S8
        }
        int length = bytes[1];
        if(bytes.size() < 2 + length){
            throw; //something about the byte vector is too short, can't continue
        }
        for(int i = 0; i < length; i++){
            item.push_back(static_cast<char>(bytes[2+i]));
        }
    }else{
        throw; //header passed is incorrect
    }
    return item;
}

vec pack::serialize(std::vector<u8> item){
    vec bytes;
    if(item.size() <= 255){ //not good
        bytes.push_back(PACK109_A8);
        bytes.push_back(static_cast<unsigned char>(item.size()));
        for(u8 byte : item){
            bytes.push_back(byte);
        }
    }
    return bytes;
}

std::vector<u8> pack::deserialize_vec_u8(vec bytes){
    vec item;
    if(bytes.size() == 0){
        throw; //length way too small
    }
    if(bytes[0] == PACK109_A8){
        if(bytes.size() < 2){
            throw;
        }
        int length = bytes[1];
        if(bytes.size() < 2 + length){
            throw; //something about the byte vector is too short, can't continue
        }
        for(int i = 0; i < length; i++){
            item.push_back(static_cast<char>(bytes[2+i]));
        }
    }else{
        throw; //wrong tag
    }
    return item;
}
//[program-4-solution] [16]
vec pack::serialize(struct File item){
    vec bytes;
    bytes.push_back(PACK109_M8); //map tag
    bytes.push_back(0x1); //one key value pair

    vec file = serialize(string("File"));
    bytes.insert(end(bytes), begin(file), end(file));
    bytes.push_back(PACK109_M8);
    bytes.push_back(0x2); //two key value pairs

    vec nameKey = serialize(string("name"));
    bytes.insert(end(bytes), begin(nameKey), end(nameKey));
    vec nameVal = serialize(string(item.name));
    bytes.insert(end(bytes), begin(nameVal), end(nameVal));
    
    vec bytesKey = serialize(string("bytes"));
    bytes.insert(end(bytes), begin(bytesKey), end(bytesKey));

    bytes.push_back(PACK109_A8); //array8 tag

    size_t numBytes = item.bytes.size();
    bytes.push_back(static_cast<uint8_t>(numBytes >> 8)); // high byte
    bytes.push_back(static_cast<uint8_t>(numBytes));        //low byte

    vec bytesVal = serialize(item.bytes);
    bytes.insert(end(bytes), begin(bytesVal), end(bytesVal));

    return bytes;
}
//reversing serialize file
File pack::deserialize_file(vec bytes) {
    File file;

    // Assuming the serialization format follows the same structure
    // Read and verify the map tag
    if (bytes.empty() || bytes[0] != PACK109_M8) {
        throw;
    }
    // Assuming there is only one key-value pair
    if (bytes.size() < 4 || bytes[1] != 0x1) {
        throw;
    }
    // Assuming the first key is "File" and is already deserialized
    std::string expectedKey = "File";
    if (bytes.size() < expectedKey.size() + 2 || std::memcmp(bytes.data() + 2, expectedKey.data(), expectedKey.size()) != 0) {
        throw;
    }
    // Find the index where the name key starts
    std::vector<uint8_t>::iterator nameKeyPos = std::find(bytes.begin(), bytes.end(), PACK109_M8); //[find and algorithm header file] [18]
    if (nameKeyPos == bytes.end() || nameKeyPos == bytes.end() - 1) {
        throw;
    }

    // Deserialize the name
    std::vector<uint8_t>::iterator nameBegin = nameKeyPos + 2; // Skip the map tag and key length
    std::vector<uint8_t>::iterator nameEnd = std::find(nameBegin, bytes.end(), PACK109_M8);
    if (nameEnd == bytes.end()) {
        throw;
    }
    file.name = std::string(nameBegin, nameEnd);

    // Find the index where the bytes key starts
    std::vector<uint8_t>::iterator bytesKeyPos = std::find(nameEnd, bytes.end(), PACK109_M8); 
    if (bytesKeyPos == bytes.end() || bytesKeyPos == bytes.end() - 1) {
        throw;
    }

    // Deserialize the bytes
    std::vector<uint8_t>::iterator bytesBegin = bytesKeyPos + 2; // Skip the map tag and key length
    std::vector<uint8_t>::iterator bytesSizeByte1 = bytesBegin;
    std::vector<uint8_t>::iterator bytesSizeByte2 = bytesBegin + 1;
    size_t numBytes = (static_cast<size_t>(*bytesSizeByte1) << 8) | static_cast<size_t>(*bytesSizeByte2);
    std::vector<uint8_t>::iterator bytesEnd = bytesBegin + 2 + numBytes; // Skip the size bytes and include the actual bytes

    if (bytesEnd > bytes.end()) {
        // Handle error or return default-initialized File
        return file;
    }

    file.bytes.assign(bytesBegin + 2, bytesEnd);

    return file;
}

vec pack::serialize(struct Request item){
    vec bytes;
    bytes.push_back(PACK109_M8);
    bytes.push_back(0x1);//one kv pair

    vec nameKey = serialize(string("name"));
    bytes.insert(end(bytes), begin(nameKey), end(nameKey));
    vec nameVal = serialize(item.name);
    bytes.insert(bytes.end(), nameVal.begin(), nameVal.end());
    
    return bytes;
}

Request pack::deserialize_request(vec bytes) {
    Request request;

    // Assuming the serialization format follows the same structure
    // Read and verify the map tag
    if (bytes.empty() || bytes[0] != PACK109_M8) {
        throw;
    }

    // Assuming there is only one key-value pair
    if (bytes.size() < 4 || bytes[1] != 0x1) {
        throw;
    }

    // Assuming the key is "name" and is already deserialized
    std::string expectedKey = "name";
    if (bytes.size() < expectedKey.size() + 2 || std::memcmp(bytes.data() + 2, expectedKey.data(), expectedKey.size()) != 0) {
        throw;
    }

    // Find the index where the value starts
    std::vector<uint8_t>::iterator valueBegin = bytes.begin() + expectedKey.size() + 2; // Skip the map tag and key length

    // Deserialize the value
    std::vector<uint8_t>::iterator valueEnd = bytes.end();
    request.name = deserialize_string(vec(valueBegin, valueEnd));

    return request;
}
