// This is a slightly modified version of TinyJSON, adapted for our needs.
// Original source: https://github.com/tiny-json/tinyjson
#pragma once

#include <istream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <codecvt>
#include <locale>

namespace tinyjson
{

using u32_istream = std::basic_istream<char32_t>;
using u32_sstream = std::basic_stringstream<char32_t>;

#ifndef CHECK_TYPE_MISMATCH
#define CHECK_TYPE_MISMATCH(t1, t2) \
    { \
        if (t1 != t2) \
        { \
            std::ostringstream oss; \
            oss << "expect type " << t1 << ", but found type " << t2; \
            throw std::runtime_error(oss.str() ); \
        } \
    }
#endif

inline std::string trim(const std::string&& str,
                 const std::string& whitespace = " \t\r\n")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return "";

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

inline std::string U32ToU8(std::u32string u32)
{
    try
    {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf32conv;
        return utf32conv.to_bytes(u32);
    }
    catch(std::range_error& ex)
    {
        // rethrow
        throw std::runtime_error("invalid utf32 string: " + std::string(ex.what()));
    }
}

inline std::u32string U8ToU32(std::string u8)
{
    try
    {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf32conv;
        return utf32conv.from_bytes(u8);
    }
    catch(const std::exception& ex)
    {
         // rethrow
        throw std::runtime_error("invalid utf32 string: " + std::string(ex.what()));
    }
}

enum json_t
{
    string = 0,
    number_integer,
    number_double,
    array,
    boolean,
    object,
    null,
    invalid
};

class json;

using json_object = std::map<std::string, json>;
using json_array = std::vector<json>;

class json
{
private:
    /// the payload
    void* _value;
    /// the value type of the json object
    json_t _type;

public:
    /// constructors
    json();
    json(std::string val);
    json(std::string& val);
    json(char val[]);
    json(double val);
    json(int val);
    json(long long val) ;
    json(bool val);
    json(json_array& array);
    json(json_array&& array);
    json(json_object& obj);
    json(json_object&& obj);

    /// destructor
    ~json();

    json_t type() const;
    const std::string type_name() const;

    // copy constructors
    json(const json& other);

    // copy assignment
    json& operator= (const json& other);
    bool operator== (const json& o) const;
    bool operator!= (const json& o) const;

    const std::string get_string() const;
    long long get_integer() const;
    double get_double() const;
    bool get_bool() const;
    const json_object get_object() const;
    const json_array get_array() const;
    const void* get_null() const;

    size_t size() const;
    bool has_member(std::string member_name);
    void add_member(std::string member_name, json member_value);
    void add_element(json elem);

    json *find(std::string member_name) const;

    // operator [] for object value
    json& operator [](const char * key);
    const json& operator [](const char * key) const;
    // operator [int] for array value
    json& operator [](int index);
    const json& operator [](int index) const;

    const std::string to_string() const;

private:
    const std::string object_to_string() const;
    const std::string array_to_string() const;
};

//
// Implementation
//
inline json::json() : _value(nullptr), _type(json_t::null) {}

inline  json::json(std::string val)
    : _value(new std::string(val)), _type(json_t::string) {}

inline json::json(std::string& val)
    : _value(new std::string(val)), _type(json_t::string) {}

inline json::json(char val[])
    : _value(new std::string(val)), _type(json_t::string) {}

inline json::json(double val)
    : _value(new double(val)), _type(json_t::number_double) {}

inline json::json(long long val)
    : _value(new long long(val)), _type(json_t::number_integer) {}

inline json::json(int val)
    : _value(new long long(val)), _type(json_t::number_integer) {}

inline json::json(bool val)
    : _value(new bool(val)), _type(json_t::boolean) {}

inline json::json(json_array& array)
    : _value(new json_array(array)), _type(json_t::array) {}

inline json::json(json_array&& array)
    : _value(new json_array(array)), _type(json_t::array) {}

inline json::json(json_object& obj)
    : _value(new json_object(obj)), _type(json_t::object) {}

inline json::json(json_object&& obj)
    : _value(new json_object(obj)), _type(json_t::object) {}

inline json_t json::type() const { return _type; }

inline const std::string json::type_name() const
{
    switch(_type)
    {
        case(json_t::string):
            return "string";

        case(json_t::array):
            return "array";

        case(json_t::object):
            return "object";

        case(json_t::number_double):
        case(json_t::number_integer):
            return "number";

        case(json_t::boolean):
            return "boolean";

        case(json_t::null):
            return "null";

        case(json_t::invalid):
        default:
            return "invalid";
    }
}

//
// copy constructor
//
inline json::json(const json& other)
{
    _type = other._type;
    switch(_type)
    {
        case json_t::string:
            _value = new std::string(*static_cast<std::string*>(other._value));
            break;
        case json_t::object:
            _value = new json_object(*static_cast<json_object*>(other._value));
            break;
        case json_t::number_double:
            _value = new double(*static_cast<double*>(other._value));
            break;
        case json_t::number_integer:
            _value = new long long(*static_cast<long long*>(other._value));
            break;
        case json_t::array:
            _value = new json_array(*static_cast<json_array*>(other._value));
            break;
        case json_t::boolean:
            _value = new bool(*static_cast<bool*>(other._value));
            break;
        case json_t::null:
            _value = nullptr;
            break;
        default:
            throw std::runtime_error("unexpected json type: " + other.type_name());
    }
}

//
// copy assignment
//
inline json& json::operator= (const json& other)
{
    _type = other._type;
    switch(_type)
    {
        case json_t::string:
            _value = new std::string(*static_cast<std::string*>(other._value));
            break;
        case json_t::object:
            _value = new json_object(*static_cast<json_object*>(other._value));
            break;
        case json_t::number_double:
            _value = new double(*static_cast<double*>(other._value));
            break;
        case json_t::number_integer:
            _value = new long long(*static_cast<long long*>(other._value));
            break;
        case json_t::array:
            _value = new json_array(*static_cast<json_array*>(other._value));
            break;
        case json_t::boolean:
            _value = new bool(*static_cast<bool*>(other._value));
            break;
        case json_t::null:
            _value = nullptr;
            break;
        default:
            throw std::runtime_error("unexpected json type" + other.type_name());
    }
    return *this;
}

inline bool json::operator==(const json& o) const
{
    switch(_type)
    {
        case (json_t::array):
            if (o._type == array)
            {
                json_array* a = static_cast<json_array*>(_value);
                json_array* b = static_cast<json_array*>(o._value);
                return *a == *b;
            }

        case (json_t::object):
            if (o._type == object)
            {
                json_object* a = static_cast<json_object*>(_value);
                json_object* b = static_cast<json_object*>(o._value);
                return *a == *b;
            }

        case (json_t::null):
            if (o._type == null)
            {
                return true;
            }

        case (json_t::string):
            if (o._type == string)
            {
                const std::string* a = static_cast<std::string*>(_value);
                const std::string* b = static_cast<std::string*>(o._value);
                return *a == *b;
            }

        case (json_t::boolean):
            if (o._type == boolean)
            {
                const bool* a = static_cast<bool*>(_value);
                const bool* b = static_cast<bool*>(o._value);
                return *a == *b;
            }

        case (json_t::number_integer):
            if (o._type == number_integer)
            {
                const long long* a = static_cast<long long*>(_value);
                const long long* b = static_cast<long long*>(o._value);
                return *a == *b;
            }

        case (json_t::number_double):
            if (o._type == number_double)
            {
                const double* a = static_cast<double*>(_value);
                const double* b = static_cast<double*>(o._value);
                return *a == *b;
            }

        default:
            return false;
    }
}

inline bool json::operator!=(const json& o) const
{
    return ! (*this == o);
}

//
// belows method are for retrieving json values
//
inline const std::string json::get_string() const
{
    CHECK_TYPE_MISMATCH(this->_type, json_t::string);
    return *static_cast<std::string*>(_value);
}

inline long long json::get_integer() const
{
    CHECK_TYPE_MISMATCH(this->_type, json_t::number_integer);
    return *static_cast<long long*>(_value);
}

inline double json::get_double() const
{
    CHECK_TYPE_MISMATCH(this->_type, json_t::number_double);
    return *static_cast<double*>(_value);
}

inline bool json::get_bool() const
{
    CHECK_TYPE_MISMATCH(this->_type, json_t::boolean);
    return *static_cast<bool*>(_value);
}

inline const json_object json::get_object() const
{
    CHECK_TYPE_MISMATCH(this->_type, json_t::object);
    return *static_cast<json_object*>(_value);
}

inline const json_array json::get_array() const
{
    CHECK_TYPE_MISMATCH(this->_type, json_t::array);
    return *static_cast<json_array*>(_value);
}

inline const void* json::get_null() const
{
    CHECK_TYPE_MISMATCH(this->_type, json_t::null);
    return nullptr;
}

inline bool json::has_member(std::string member_name)
{
    CHECK_TYPE_MISMATCH(this->_type, json_t::object);
    auto members = static_cast<json_object*>(_value);
    return (members->find(member_name) != members->end());
}

inline json *json::find(std::string member_name) const
{
    CHECK_TYPE_MISMATCH(this->_type, json_t::object);
    auto members = static_cast<json_object*>(_value);
    auto it = members->find(member_name);
    return it != members->end() ? &it->second : nullptr;
}

inline void json::add_member(std::string member_name, json member_value)
{
    CHECK_TYPE_MISMATCH(this->_type, json_t::object);
    auto members = static_cast<json_object*>(_value);
    (*members)[member_name] = member_value;
}

inline void json::add_element(json elem)
{
    CHECK_TYPE_MISMATCH(this->_type, json_t::array);
    auto elems = static_cast<json_array*>(_value);
    elems->push_back(elem);
}

inline size_t json::size() const
{
    if (_type == json_t::array)
    {
        auto array = static_cast<json_array*>(_value);
        return array->size();
    }
    else if (_type == json_t::object)
    {
        auto members = static_cast<json_object*>(_value);
        return members->size();
    }

    throw std::runtime_error("Unexpeced json type " + type_name() + ", expected array or object");
}

// operator [] for object value
inline json& json::operator [](const char * key)
{
    CHECK_TYPE_MISMATCH(this->_type, json_t::object);

    json_object* members = static_cast<json_object*>(_value);
    if (members->find(key) == members->end())
    {
        throw std::runtime_error("key " + std::string(key) + " not found.");
    }

    return (*members)[key];
}

inline const json &json::operator[](const char *key) const {
    return const_cast<json *>(this)->operator[](key);
}

// operator [int] for array value
inline json& json::operator[](int index)
{
    CHECK_TYPE_MISMATCH(this->_type, json_t::array);

    json_array* array = static_cast<json_array*>(_value);
    if (index < 0 || (size_t)index >= array->size())
    {
        throw std::runtime_error("index " + std::to_string(index) + " out of range.");
    }

    return (*array)[index];
}

inline const json &json::operator[](int index) const {
    return const_cast<json *>(this)->operator[](index);
}

/// destructor
inline json::~json()
{
    switch (_type)
    {
        case (json_t::array):
            delete static_cast<json_array*>(_value);
            break;
        case (json_t::object):
            delete static_cast<json_object*>(_value);
            break;
        case (json_t::string):
            delete static_cast<std::string*>(_value);
            break;
        case (json_t::boolean):
            delete static_cast<bool*>(_value);
            break;
        case (json_t::number_double):
            delete static_cast<double*>(_value);
            break;
        case (json_t::number_integer):
            delete static_cast<long long*>(_value);
            break;
        default:
            break;
    }
}

inline const std::string json::to_string() const
{
    switch(type())
    {
        case json_t::object:
            return object_to_string();

        case json_t::array:
            return this->array_to_string();

        case json_t::string:
            return "\"" + get_string() + "\"";

        case json_t::number_integer:
            return std::to_string(get_integer());

        case json_t::number_double:
            return std::to_string(get_double());

        case json_t::boolean:
            return get_bool() ? "true" : "false";

        case json_t::null:
            return "null";

        default:
            throw std::runtime_error("invalid json type");
    }
}

inline const std::string json::object_to_string() const
{
    std::stringstream ss;
    ss << "{";

    json_object jobj = get_object();
    for(auto it = jobj.begin(); it != jobj.end(); it++ )
    {
        ss << "\"" << it->first.c_str() << "\"" ;

        ss << " : " ;

        ss << it->second.to_string().c_str();

        if (std::next(it) != jobj.end())
        {
            ss << ",";
        }
    }

    ss << "}";
    return ss.str();
}

inline const std::string json::array_to_string() const
{
    std::stringstream ss;
    ss << "[";

    json_array jarray = get_array();
    for(auto it = jarray.begin(); it != jarray.end(); it++ )
    {
        ss << it->to_string().c_str();

        if (std::next(it) != jarray.end())
        {
            ss << ",";
        }
    }

    ss << "]";

    return ss.str();
}

//
//  The Parser
//
class parser
{
public:

static json parse(const char* s)
{
    // take UTF8 input and convert to UTF32
    // so we can read char by char for parsing
    std::stringstream sstrm(s);

    std::wbuffer_convert<std::codecvt_utf8<char32_t>, char32_t> conv(sstrm.rdbuf());
    u32_istream u32strm(&conv);

    json ret_val;
    char32_t first_char = peek_next_non_space(u32strm);

    if (first_char == U'{')
    {
        ret_val = parse_object(u32strm);
    }
    else if(first_char == U'[')
    {
        ret_val = parse_array(u32strm);
    }
    else
    {
        throw std::runtime_error("invalid json format");
    }

    // Expecting EOF
    if (peek_next_non_space(u32strm) != EOF)
    {
        throw std::runtime_error("invalid json format");
    }

    return ret_val;
}

static json parse_value(u32_istream& strm)
{
    switch(peek_next_non_space(strm))
    {
        case U'\"':
            return parse_string(strm);

        case U'[':
            return parse_array(strm);

        case U'0':
        case U'1':
        case U'2':
        case U'3':
        case U'4':
        case U'5':
        case U'6':
        case U'7':
        case U'8':
        case U'9':
        case U'-':
        case U'.':
            return parse_number(strm);

        case U'{':
            return parse_object(strm);

        case U'T':
        case U't':
        case U'F':
        case U'f':
            return parse_bool(strm);

        case U'n':
        case U'N':
            return parse_null(strm);

        default:
            throw std::runtime_error("unexpected character");
    }
}

static json parse_object(u32_istream& strm)
{
    json return_val(json_object{});

    // skip the first '{' character
    skip_char(strm, U'{');

    int c = peek_next_non_space(strm);
    while (c != EOF)
    {
        if (c == U'\"')
        {
            std::string member = parse_member(strm);

            skip_char(strm, U':');

            return_val.add_member(member, parse_value(strm));
        }
        else if (c == U'}')
        {
            break;
        }
        else if (c == U',')
        {
            // go past the comma to parse the next part
            c = get_next_non_space(strm);
        }
        else
        {
            throw std::runtime_error("invalid object format");
        }

        c = peek_next_non_space(strm);
    }

    skip_char(strm, U'}');

    return return_val;
}

static std::string parse_member(u32_istream& strm)
{
    std::u32string returnVal;

    // go past the openning double quote
    skip_char(strm, U'\"');

    unsigned int c = strm.peek();
    while(c != (unsigned int)EOF && c != U'\"')
    {
        if (c == U'\\')
        {
            // escape character
            returnVal.push_back(escape_char(strm));
        }
        else
        {
            c = strm.get();
            returnVal.push_back(c);
        }

        c = strm.peek();
    }

    // go past the closing double quote
    skip_char(strm, U'\"');;

    return U32ToU8(returnVal);
}

static json parse_array(u32_istream& strm)
{
    json_array vector_val;

    // Go past the opening '['
    skip_char(strm, U'[');
    int c = peek_next_non_space(strm);

    // Empty array
    if (c == U']')
    {
        skip_char(strm, U']');
        return json(vector_val);
    }

    do
    {
        vector_val.push_back(parse_value(strm));
        c = peek_next_non_space(strm);

        if (c == U',')
        {
            skip_char(strm, U',');
        }
        else if (c == U']')
        {
            break;
        }
    }
    while(c != EOF);

    // skip the closing square bracket
    skip_char(strm, U']');

    json array_val(vector_val);
    return array_val;
}

static json parse_bool(u32_istream& strm)
{
    std::u32string val_str;

    int c = strm.peek();
    while(c != EOF && c != U',' && c != U'}' && c != U']')
    {
        c = strm.get();
        val_str.push_back(static_cast<char32_t>(std::tolower(c)));
        c = strm.peek();
    }

    std::string bool_str = trim(U32ToU8(val_str));

    bool val_bool = to_bool(bool_str);
    json return_val(val_bool);
    return return_val;
}

static json parse_null(u32_istream& strm)
{
    std::u32string val_str;

    int c = strm.peek();
    while(c != EOF && c != U',' && c != U'}' && c != U']')
    {
        c = strm.get();
        val_str.push_back(static_cast<char32_t>(std::tolower(c)));
        c = strm.peek();
    }

    std::string null_str = trim(U32ToU8(val_str));

    if (null_str == "null")
    {
        return json();
    }
    else
    {
        throw std::runtime_error("unexpected null string");
    }
}

static json parse_string(u32_istream& strm)
{
    std::u32string string_val;

    // skip the open double quote
    skip_char(strm, U'\"');

    char32_t c = strm.peek();

    while(c != static_cast<char32_t>(EOF) && c != U'\"')
    {
        if (c == U'\\')
        {
            // escape character
            string_val.push_back(escape_char(strm));
        }
        else
        {
            strm.get(c);
            string_val.push_back(c);
        }

        c = strm.peek();
    }

    // skip the closing double quote
    skip_char(strm, U'\"');

    json return_val(U32ToU8(string_val));
    return return_val;
}

static json parse_number(u32_istream& strm)
{
    std::u32string num_str;

    int c = strm.peek();
    while(c != EOF && c != U',' && c != U']' && c != U'}' )
    {
        c = strm.get();
        num_str.push_back(c);
        c = strm.peek();
    }

    std::string nums = trim(U32ToU8(num_str));

    // try integer first, if not then double
    if(nums.find_first_not_of("0123456789-") == std::string::npos)
    {
        return json(to_integer(nums));
    }
    else
    {
        return json(to_double(nums));
    }

}

static bool to_bool(std::string str)
{
    std::transform(
        str.begin(), str.end(), str.begin(),
        [](unsigned char c)
        {
            return static_cast<unsigned char>(std::tolower(c));
        });

    if (str == "true")
        return true;

    if (str == "false")
        return false;

    throw std::runtime_error("invalid boolean string");
}

static long long to_integer(std::string str)
{
    // TODO: exception handling
    size_t pos = 0;
    long long num = std::stoll(str, &pos);

    if (pos != str.size())
    {
        throw std::runtime_error("Unexpected number(integer) format.");
    }

    return num;
}

static double to_double(std::string str)
{
    size_t pos = 0;
    double num = std::stod(str, &pos);

    if (pos != str.size())
    {
        throw std::runtime_error("Unexpected number(double) format.");
    }

    return num;
}

static char32_t escape_char(u32_istream& strm)
{
    skip_char(strm, U'\\');

    char32_t c = strm.get();

    switch(c)
    {
        case U'\"':
            c = U'\"';
            break;

        case U'\\':
            c = U'\\';
            break;

        case U'/':
            c = U'/';
            break;

        case U'b':
            c = U'\b';
            break;

        case U'f':
            c = U'\f';
            break;

        case U'n':
            c = U'\n';
            break;

        case U'r':
            c = U'\r';
            break;

        case U't':
            c = U'\t';
            break;

        case U'u':
            c = parse_hex(strm);
            break;

        default:
            throw std::runtime_error("backslash is followed by invalid character");
    }

    return c;
}

static char32_t get_next_non_space(u32_istream& strm)
{
    skip_space(strm);
    return strm.get();
}

static int peek_next_non_space(u32_istream& strm)
{
    skip_space(strm);
    return strm.peek();
}

static void skip_char(u32_istream& strm, char32_t expected)
{
    // get the first non-space character and skip to next
    // throw if seeing different character

    skip_space(strm);
    char32_t next = (char32_t)strm.peek();

    if (next != expected || next == (char32_t)EOF)
    {
        std::basic_ostringstream<char32_t> oss;

        oss << U"expected char '" << expected << U"' not found";

        auto err_str = oss.str();
        throw std::runtime_error(U32ToU8(err_str));
    }

    // disgard the expected char by get()
    strm.get();
}

static void skip_space(u32_istream& strm)
{
    unsigned int next = strm.peek();
    while (next != (char32_t)EOF && std::isspace(next))
    {
        strm.get();
        next = strm.peek();
    }
}

static char32_t parse_hex(u32_istream& strm)
{
    std::u32string cs;

    // 4 hex numbers
    for(int i = 0; i < 4; i++)
    {
        char32_t c = strm.get();
        if (c != std::char_traits<char32_t>::eof() && isxdigit(c))
        {
            cs.push_back(c);
        }
        else
        {
            throw std::runtime_error("not hex number");
        }
    }

    std::basic_stringstream<char32_t> is(cs);

    unsigned int uc;
    is >> std::hex >> uc;

    return char32_t(uc);
}

};

}   // namespace tinyjson
