/*
 * Licensed to cpp-elasticsearch under one or more contributor
 * license agreements. See the NOTICE file distributed with
 * this work for additional information regarding copyright
 * ownership. Elasticsearch licenses this file to you under
 * the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "Json.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <cstring>
#include <stdexcept>

#define BACKSLASH 0x5c

/* ROOT's ClassImp macro for HTML doc generation */
ClassImp(JsonValue)
ClassImp(JsonArray)
ClassImp(JsonObject)


/*------------------- Json Value ------------------*/


JsonValue::JsonValue(): _type(nullType), _data(""), _object(0), _array(0) {

}

JsonValue::JsonValue(const JsonValue& val): _type(val._type), _data(val._data){

    if(val._object){
        assert(val._type == ValueType::objectType);
        _object = new JsonObject(*val._object);
    } else {
        _object = 0;
    }

    if(val._array){
        assert(val._type == ValueType::arrayType);
        _array = new JsonArray(*val._array);
    } else {
        _array = 0;
    }

}

JsonValue::~JsonValue() {
    if(_object)
        delete _object;

    if(_array)
        delete _array;
}

/// Returns the data in Json Format. Convert the values into string with escaped characters.
std::string JsonValue::escapeJsonString(const std::string& input) {

    std::stringstream output;
    //std::cout << "-To Format: " << input << std::endl;

    for (auto iter = input.cbegin(); iter != input.cend(); iter++) {
        switch (*iter) {
            case '\\': output << "\\\\"; break;
            case '"': output << "\\\""; break;
            //case '/': output << "\\/"; break;
            case '\b': output << "\\b"; break;
            case '\f': output << "\\f"; break;
            case '\n': output << "\\n"; break;
            case '\r': output << "\\r"; break;
            case '\t': output << "\\t"; break;
            default: output << *iter; break;
        }
    }

    //std::cout << "+To Format: " << output.str() << std::endl;
    return output.str();
}

const char* JsonValue::showType() const{

    switch (_type) {
        case JsonValue::objectType :
            return "object";
        case JsonValue::arrayType :
            return "array";
        case JsonValue::stringType :
            return "string";
        case JsonValue::booleanType :
            return "boolean";
        case JsonValue::numberType :
            return "number";
        case JsonValue::nullType :
            return "null";
        default:
            return "unknown";
    }
    return "--------------------";
}


const std::string& JsonValue::getString() const {

    if(_type == stringType)
        return _data;

    throw std::logic_error("not a string");

    static const std::string error("error: is not of string type.");
    return error;
}

JsonValue::operator const std::string&() const {
    return getString();
}


void JsonValue::show() const {
  std::cout << *this;
}

bool JsonValue::empty() const {

    switch(_type){
        case nullType:
            return true;

        case objectType:
            if(_object != 0)
                return _object->empty();
            break;

        case arrayType:
            if(_array != 0)
                return _array->empty();
            break;

        default:;
    }

    return false;
}

JsonValue::operator bool() const {

    return getBoolean();
}

bool JsonValue::getBoolean() const {
    switch(_type){
        case booleanType:
            assert(!_data.empty());
            if(_data[0] == 't')
                return true;
            else
                return false;

        case numberType:
            if(getInt() != 0)
                return true;

        case stringType:
            if(_data == "true")
                return true;
            if(_data == "false")
                return false;

        case nullType:
            return false;

        default:;
    }

    return false;
}

unsigned int JsonValue::getUnsignedInt() const {
    if(_type == nullType)
        return 0;

    if(_type != numberType && _type != stringType)
        throw std::logic_error("not an unsigned int");

    unsigned int number = 0;
    std::istringstream ( _data ) >> number;
    return number;
}

int JsonValue::getInt() const {

    if(_type == nullType)
        return 0.;

    if(_type != numberType && _type != stringType)
        throw std::logic_error("not an int");

    int number = 0;
    std::istringstream ( _data ) >> number;
    return number;
}

JsonValue::operator int() const {
    return getInt();
}

long int JsonValue::getLong() const {

    if(_type == nullType)
        return 0;

    if(_type != numberType && _type != stringType)
        throw std::logic_error("not a long int");

    long int number = 0;
    std::istringstream ( _data ) >> number;
    return number;
}

double JsonValue::getDouble() const{

    if(_type == nullType)
        return 0.;

    if(_type != numberType && _type != stringType)
        throw std::logic_error("not a double");

    double number = 0;
    std::istringstream ( _data ) >> number;
    return number;
}

// Automatic cast in string.
JsonValue::operator double() const{
    return getDouble();
}

float JsonValue::getFloat() const{

    if(_type == nullType)
        return 0.;

    if(_type != numberType && _type != stringType)
        throw std::logic_error("not a float");

    float number = 0;
    std::istringstream ( _data ) >> number;
    return number;
}

// Automatic cast in string.
JsonValue::operator float() const{
    return getFloat();
}

const JsonObject& JsonValue::getObject() const {
    if(_type != objectType){
        std::cout << "My Type: " << showType() << std::endl;
        std::cout << *this << std::endl;
        throw std::logic_error("not a JsonObject");
    }

    if(_object == 0)
        throw std::runtime_error("no instance for JsonObject");

    return *_object;
}

const JsonArray& JsonValue::getArray() const{
    if(_type != arrayType)
        throw std::logic_error("not a JsonArray");

    if(_array == 0)
        throw std::runtime_error("no instance for JsonArray");

    return *_array;
}

std::ostream& operator<<(std::ostream& os, const JsonValue& value){

    if(value._type == JsonValue::objectType){
        assert(value._object != 0);
        os << *(value._object);
        return os;
    }

    if(value._type == JsonValue::arrayType){
        assert(value._array != 0);
        os << *(value._array);
        return os;
    }

    if(value._type == JsonValue::nullType){
        os << "null";
        return os;
    }

    if(value._type == JsonValue::booleanType){
        if(value)
            os << "true";
        else
            os << "false";
        return os;
    }

    if(value._type == JsonValue::stringType){
        os << "\"" << value._data << "\"";
        return os;
    }

    assert(value._type == JsonValue::numberType);
    os << value._data;
    return os;
}

// Set this value as a boolean.
void JsonValue::setBoolean(bool b){
    _type = booleanType;
    if(b)
        _data = "true";
    else
        _data = "false";

    assert(_object == 0);
    assert(_array == 0);
}

// Set this value as a double.
void JsonValue::setDouble(double v){

    _type = numberType;
    _data = std::to_string(v);

    assert(_object == 0);
    assert(_array == 0);
}

// Set this value as a int.
void JsonValue::setInt(unsigned int u){
    _type = numberType;
    _data = std::to_string(u);

    assert(_object == 0);
    assert(_array == 0);
}

// Set this value as a int.
void JsonValue::setInt(int i){
    _type = numberType;
    _data = std::to_string(i);

    assert(_object == 0);
    assert(_array == 0);
}

// Set this value as a long.
void JsonValue::setLong(long l){
    _type = numberType;
    _data = std::to_string(l);

    assert(_object == 0);
    assert(_array == 0);
}

// Set this value as a string.
void JsonValue::setString(const std::string& value){
    _type = stringType;
    _data = value;
    assert(_object == 0);
    assert(_array == 0);
}

/// Set this value as Object.
void JsonValue::setObject(const JsonObject& obj){
    assert(_type == nullType && _array == 0 && _object == 0);
    _type = objectType;
    _object = new JsonObject(obj);
    _data = obj.str();
}

/// Set this value as Array.
void JsonValue::setArray(const JsonArray& array){
    assert(_type == nullType && _array == 0 && _object == 0);
    _type = arrayType;
    _array = new JsonArray(array);
    _data = array.str();
}

const char* JsonValue::read(const char* pCursor, const char* pEnd){

    // Call this function only once.
    assert(_data.empty());
    assert(_object == 0);
    assert(_array == 0);

    // Remove white spaces before the actual string.
    while(pCursor < pEnd && isspace(*pCursor))
        ++pCursor;

    const char* startPoint = pCursor;
    const char* endPoint = pCursor;

    // Interpret data.
    switch(*pCursor){

        case '}':
        case ']':
        case ',':
            _type = nullType;
            break;

        case 'n':
            _type = nullType;
            // Move until the end of the element, object or array.
            while(pCursor < pEnd && !isspace(*pCursor) && *pCursor != ',' && *pCursor != '}' && *pCursor != ']')
                ++pCursor;
            endPoint = pCursor;
            break;

        case '-':
        case 'e':
        case '.':
        case 'E':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            _type = numberType;
            // Move until the end of the element, object or array.
            while(pCursor < pEnd && !isspace(*pCursor) && *pCursor != ',' && *pCursor != '}' && *pCursor != ']')
                ++pCursor;
            endPoint = pCursor;
            break;

        case 'f':
        case 't':
            _type = booleanType;
            // Move until the end of the element, object or array.
            while(pCursor < pEnd && !isspace(*pCursor) && *pCursor != ',' && *pCursor != '}' && *pCursor != ']')
                ++pCursor;
            endPoint = pCursor;
            break;

        case '"':
            // Define type
            _type = stringType;

            // Don't store the quote
            ++startPoint;

            // Move to next character
            ++pCursor;

            // Move until the end of the string.
            while(pCursor < pEnd)
                if(*pCursor == '"' && *(pCursor - 1) != BACKSLASH)
                    break;
                else
                    ++pCursor;

            endPoint = pCursor;
            ++pCursor;

            #ifndef NDEBUG
            if(!(isspace(*pCursor) || *pCursor == ',' || *pCursor == '}' || *pCursor == ']'))
                printf("Debug Read 2: %s\n", pCursor);
            #endif

            assert( isspace(*pCursor) || *pCursor == ',' || *pCursor == '}' || *pCursor == ']');
            break;

        case '{':
            // Only one type is allowed.
            assert(_array == 0 && _object == 0);
            _type = objectType;
            _object = new JsonObject;
            pCursor = _object->addMember(pCursor, pEnd);
            endPoint = pCursor;
            break;

        case '[':
            // Only one type is allowed.
            assert(_array == 0 && _object == 0);
            _type = arrayType;
            _array = new JsonArray;
            pCursor = _array->addElement(pCursor, pEnd);
            endPoint = pCursor;
            break;

        default:
            throw std::logic_error("illformed JSON.");
    }

    assert(pCursor <= pEnd);
    assert(pCursor >= startPoint);
    assert(pCursor >= endPoint);

    if(endPoint == startPoint)
        _data = "";

    if(endPoint > startPoint)
        _data = std::string(startPoint, endPoint - startPoint);

    return pCursor;
}

bool JsonValue::operator==(const JsonValue& v) const {

    if( _type != v._type )
        return false;

    // Both null types.
    if( _type == nullType)
        return true;

    if( _object && !v._object )
        return false;

    if( !_object && v._object )
        return false;

    if( _array && !v._array )
        return false;

    if( !_array && v._array )
        return false;

    if( _object && v._object )
        return ( *_object == *v._object );

    if( _array && v._array )
        return ( *_array == *v._array );

    if( _type == booleanType)
        return ( _data[0] == v._data[0] );

    // Check only as double.
    if( _type == numberType){
        double number1, number2;
        std::istringstream (   _data ) >> number1;
        std::istringstream ( v._data ) >> number2;
        return ( number1 == number2 );
    }

    if( _data != v._data )
        return false;

    return true;
}

// Weak equality that can compare value of different types.
bool JsonValue::weakEquality(const JsonValue& a, const JsonValue& b){

    // Compare Null and Numbers
    if(a._type == nullType && b._type == numberType)
        return (b.getDouble() == 0.);

    if(b._type == nullType && a._type == numberType)
        return (a.getDouble() == 0.);

    // Compare Null and Boolean
    if(a._type == nullType && b._type == booleanType)
        return !(b.getBoolean());

    if(b._type == nullType && a._type == booleanType)
        return !(a.getBoolean());

    // Compare Null and Object
    if(a._type == nullType && b._type == objectType){
        if(b._object == 0 || b._object->empty())
            return true;
        else
            return false;
    }

    if(b._type == nullType && a._type == objectType){
        if(a._object == 0 || a._object->empty())
            return true;
        else
            return false;
    }

    // Compare Null and Array
    if(a._type == nullType && b._type == arrayType){
        if(b._array == 0 || b._array->empty())
            return true;
        else
            return false;
    }

    if(b._type == nullType && a._type == arrayType){
        if(a._array == 0 || a._array->empty())
            return true;
        else
            return false;
    }

    // Compare Null and String
    if(a._type == nullType && b._type == stringType){
        if(b._data.empty() || b._data == "" || b._data == "null")
            return true;
        else
            return false;
    }

    if(b._type == nullType && a._type == stringType){
        if(a._data.empty() || a._data == "" || a._data == "null")
            return true;
        else
            return false;
    }

    return (a == b);
}

/*------------------- Json Object ------------------*/
JsonObject::JsonObject(){

}

JsonObject::JsonObject(const JsonObject& obj): _memberMap(obj._memberMap){

}

/// Loops over the string and splits into members.
const char* JsonObject::addMember(const char* startStr, const char* endStr){

    // Means it starts with {
    if(startStr[0] != '{')
        throw std::logic_error("Object illformed, does not start with {");

    ++startStr;

    // Loop over members.
    while(startStr <= endStr) {

        // Key start and end point.
        const char* pKeyStart = 0;
        const char* pKeyEnd = 0;

        // Remove white spaces until we find the key.
        while(startStr <= endStr && isspace(*startStr))
            ++startStr;

        // The object is empty.
        if(*startStr == '}')
            break;

        pKeyStart = ++startStr;

        // Loop until the end of the key.
        while(startStr <= endStr){

            if(*startStr == '"'  && *(startStr - 1) != BACKSLASH){
                pKeyEnd = startStr - 1;
                ++startStr;
                break;
            }

            ++startStr;
        }

        // Loop to move to the beginning of the value.
        while(startStr <= endStr){

            if(*startStr == ':'){
                ++startStr;
                break;
            }

            ++startStr;
        }

        // We must never reach the end of the string.
        if(*startStr == '\0')
            throw std::logic_error("Object illformed, end of the string reached.");

        // Get the end of the value.
        startStr = _memberMap[ JsonKey(pKeyStart, pKeyEnd - pKeyStart + 1) ].read(startStr, endStr);

        // Remove white spaces at the end of the value if any.
        while(isspace(*startStr)){
            ++startStr;
        }

        // We reached the end of the child member.
        if(*startStr == '}')
          break;

        if(*startStr != ',')
            throw std::logic_error("Object illformed, missing coma object separator.");

        // Move to next character.
        ++startStr;
    }

    if(*startStr != '}')
        throw std::logic_error("Object illformed, does not end with }");

    ++startStr;

    assert(startStr <= endStr);

    // Returns the consummed size
    return startStr;
}

// Add a string member
void JsonObject::addMemberByKey(const std::string& key, const std::string& str){
    // Add a value as string
    _memberMap[key].setString(JsonValue::escapeJsonString(str));
}

// Add member by key value.
void JsonObject::addMemberByKey(const std::string& key, const JsonArray& array){
    // Add a value as array
    _memberMap[key].setArray(array);
}

// Add member by key value.
void JsonObject::addMemberByKey(const std::string& key, const JsonValue& value){
    _memberMap[key] = value;
}

// Add member by key value.
void JsonObject::addMemberByKey(const std::string& key, const JsonObject& obj){
    // Add a value as object
    _memberMap[key].setObject(obj);
}

// Add member by key value.
void JsonObject::addMemberByKey(const std::string& key, double v){
    // Add a value as object
    _memberMap[key].setDouble(v);
}

/// Add member by key value.
void JsonObject::addMemberByKey(const std::string& key, bool v){
    // Add a value as boolean
    _memberMap[key].setBoolean(v);
}

/// Add member by key value.
void JsonObject::addMemberByKey(const std::string& key, const char* s){
    // Add a value as string
    _memberMap[key].setString(JsonValue::escapeJsonString(s));
}

/// Add member by key value.
void JsonObject::addMemberByKey(const std::string& key, unsigned int u){
    // Add a value as int
    _memberMap[key].setInt(u);
}

/// Add member by key value.
void JsonObject::addMemberByKey(const std::string& key, int i){
    // Add a value as int
    _memberMap[key].setInt(i);
}

/// Add member by key value.
void JsonObject::addMemberByKey(const std::string& key, long i){
    // Add a value as int
    _memberMap[key].setLong(i);
}

/// Add member by key value.
void JsonObject::addMemberByKey(const std::string& key, unsigned long i){
    // Add a value as int
    _memberMap[key].setLong(i);
}


// Tells if member exists.
bool JsonObject::member(const std::string& key) const {
    return (_memberMap.find(key) != _memberMap.end());
}

// Append another object to this one.
void JsonObject::append(const JsonObject& obj){

    for(std::map< JsonKey, JsonValue >::const_iterator cit = obj._memberMap.cbegin(); cit != obj._memberMap.cend(); ++cit){

        if(_memberMap.find(cit->first) != _memberMap.end())
            throw std::logic_error("Cannot merge objects: one key appears in both.");

        _memberMap.insert(std::make_pair(cit->first, cit->second));
    }
}

// Return the value of the member[key], key must exist in the map.
const JsonValue& JsonObject::getValue(const std::string& key) const {

  std::map< JsonKey, JsonValue >::const_iterator it = _memberMap.find(key);

    if(it == _memberMap.end()){
        throw std::logic_error("failed finding key.");
    }

    return it->second;
}

// Return the value of the member[key], does not test if exists.
const JsonValue& JsonObject::operator[](const std::string& key) const {
    return _memberMap.find(key)->second;
}


/// Returns the data in Json Format.
std::string JsonObject::str() const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

bool JsonObject::contain(const JsonObject& o) const {

    for(const std::pair< JsonKey, JsonValue >& p : o._memberMap){
        std::map< JsonKey, JsonValue >::const_iterator cit = _memberMap.find(p.first);
        if( cit == _memberMap.end() ){
            return false;
        }
        if( cit->second != p.second){
            return false;
        }
    }

    return true;
}

bool JsonObject::operator==(const JsonObject& o) const {

    if(_memberMap.size() != o._memberMap.size())
        return false;

    return contain(o);
}

// Output in Json format
std::ostream& operator<<(std::ostream& os, const JsonObject& obj){
    os << "{";
    for(std::map< JsonKey, JsonValue >::const_iterator it = obj._memberMap.begin(); it != obj._memberMap.end(); ){
        os << "\"" << it->first << "\":" << it->second;
        ++it;
        if(it != obj._memberMap.end())
            os << ",";
    }
    os << "}";

    return os;
}

/*------------------- Json Array ------------------*/
// Constructor.
JsonArray::JsonArray(){

}

// Loops over the string, splits into elements and returns the consummed size
const char* JsonArray::addElement(const char* pStart, const char* pEnd){

    // Means it starts with [
    assert(pStart[0] == '[');
    ++pStart;

    // Remove white spaces until we find the first value.
    while(pStart <= pEnd && isspace(*pStart))
        ++pStart;

    // The array is empty.
    if(*pStart == ']')
        return ++pStart;

    while(pStart <= pEnd){
        _elementList.push_back(JsonValue());
        pStart = _elementList.back().read(pStart, pEnd);

        // Remove white spaces until we find the end of the array or the next value.
        while(pStart <= pEnd && isspace(*pStart))
            ++pStart;

        if(*pStart == ']')
            break;

        assert(*pStart == ',' || pStart > pEnd);
        ++pStart;
    }

    ++pStart;

   assert(pStart <= pEnd);

    // Returns the consummed size
    return pStart;
}

// Copy and add this value to the list.
void JsonArray::addElement(const JsonValue& val){
    _elementList.push_back(val);
}

/// Copy the object to a value and add this value to the list.
void JsonArray::addElement(const JsonObject& obj){
    _elementList.push_back(JsonValue());
    _elementList.back().setObject(obj);
}

bool JsonArray::operator==(const JsonArray& a) const {

    for(const JsonValue& v0 : a._elementList){
        bool found = false;

        for(const JsonValue& v1 : _elementList){

            if(v0 == v1){
                found = true;
                break;
            }
        }

        if(!found){
            return false;
        }
    }
    return true;
}

// Output in Json pretty format
std::ostream& operator<<(std::ostream& os, const JsonArray& array){

    os << "[";

    std::list<JsonValue>::const_iterator it = array._elementList.begin();
    if(it != array._elementList.end())
        os << *it;

    for(++it; it != array._elementList.end(); ++it)
        os << "," << *it;

    os << ']';

    return os;
}

// Returns the data in Json Format.
std::string JsonArray::str() const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

/// Methods for pretty formatted output.

#define NORMAL (char)27 << "[0;24m"
#define BOLD (char)27 << "[1m"
#define GREEN (char)27 << "[32m"
#define YELLOW (char)27 << "[33m"

// Output Json in a pretty format.
std::string JsonValue::pretty(int tab) const {

    std::ostringstream oss;

    if(isObject() && _object != 0)
        return _object->pretty(tab);

    if(isArray() && _array != 0)
        return _array->pretty(tab);

    oss << YELLOW << " " << *this << NORMAL;
    return oss.str();
}

// Output Json in a pretty format.
std::string JsonArray::pretty(int tab) const {

    std::ostringstream tabStream;
    for(int i = 0; i < tab; ++i)
        tabStream << "\t";

    std::ostringstream oss;
    oss << " [\n";
    for(std::list< JsonValue >::const_iterator it = _elementList.begin(); it != _elementList.end(); ){
        oss << tabStream.str() << it->pretty(tab);
        ++it;
        if(it != _elementList.end())
            oss << ",\n";
    }
    oss << "\n" << tabStream.str() << "]";

    return oss.str();
}

// Output Json in a pretty format with same colors as Marvel/Sense.
std::string JsonObject::pretty(int tab) const{
    std::ostringstream tabStream;
    for(int i = 0; i < tab; ++i)
        tabStream << "\t";

    std::ostringstream oss;
    oss << " {\n";
    for(std::map< JsonKey, JsonValue >::const_iterator it = _memberMap.begin(); it != _memberMap.end(); ){

        oss << GREEN << BOLD << tabStream.str() << "\"" << it->first << "\"" << NORMAL << ":";
        oss << it->second.pretty(tab+1);
        ++it;
        if(it != _memberMap.end())
            oss << ",\n";
    }
    oss << "\n" << tabStream.str() << "}";

    return oss.str();
}
