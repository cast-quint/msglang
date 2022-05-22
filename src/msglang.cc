/*
 * Dimitrios Koropoulis 3967
 * csd3967@csd.uoc.gr
 * CS352 - Fall 2021
 * msglang.cc
 */

#include "util.h"
#include "msglang.h"

#include <iostream>
#include <cassert>
#include <memory>
#include <iostream>

static void print_warning(const std::string& str) {
    std::cout <<  "\033[1;35m\033[1;1mwarning: \033[1;0m" << str << std::endl;
}

/** CALL OBJECT **/

/* Constructor */
CallObject::CallObject(std::string _id) : id(_id) {}

/* Overload for values init */
std::vector<value_variant> CallObject::operator,(std::vector<value_variant> vec) {

    assert(vec.empty());

    CallObject* c = new CallObject(*this);
    vec.push_back(*c);

    return vec;
}

/* overload for key */
KeyFetcher CallObject::operator,(KeyFetcher& f) {
    f.set_func_id(id);
    return f;
}

/* getter */
std::string CallObject::get_id(void) {
    return id;
}

/** VALUE OBJECT **/
void Value::init(void) {

    ival = 0;
    dval = 0;
    sval = "";
    bval = false;
    mval = NULL;
    fval = NULL;
    owner = NULL;
    empty = false;
}

Value::Value(int val) {

    init();
    ival = val;
    curr_index = INT_INDEX;
}

Value::Value(double val) {

    init();
    dval = val;
    curr_index = DBL_INDEX;
}

Value::Value(std::string val) {

    init();
    sval = val;
    curr_index = STR_INDEX;
}

Value::Value(const char* val) {

    init();
    sval = val;
    curr_index = STR_INDEX;
}

Value::Value(bool val) {

    init();
    bval = val;
    curr_index = BOO_INDEX;
}

Value::Value(MessageObject* val) {

    init();
    mval = val;
    curr_index = OBJ_INDEX;
}

/* used for implicit convertion */
Value::Value(const MessageObject& val) {
    init();
    mval = new MessageObject(val);
    curr_index = OBJ_INDEX;
}

Value::Value(func_sig val) {

    init();
    fval = val;
    curr_index = FUN_INDEX;
}

Value::Value(None_t _none) {
    init();
    curr_index = NON_INDEX;
    empty = true;
}

Value::~Value(void) = default;

Value::Value(const Value& val) = default;

/* lambda call */
Value Value::operator()(void) {

    if (curr_index != FUN_INDEX) {
        print_warning("value is not callable!");
        return 0;
    }

    return fval(owner->get_rcvptr(), owner->get_msgptr());
}

/* nested object access */
Value& Value::operator[](const std::string& strkey) {

    if (curr_index != OBJ_INDEX) {
        print_warning("invalid use of [] to a non-object");
        return *this;
    }

    return (*mval)[strkey];
}

/* nested object access */
Value& Value::operator[](const char* strkey) {

    if (curr_index != OBJ_INDEX) {
        print_warning("invalid use of [] with a non-object value");
        return *this;
    }

    return (*mval)[strkey];
}

Value& Value::operator=(const Value& val) {

    if (curr_index == OBJ_INDEX) {
        delete mval;
        mval = NULL;
    }

    curr_index = val.index();
    empty = val.is_empty();

    switch (curr_index) {
    case INT_INDEX:
        ival = val.get_int();
        break;
    case DBL_INDEX:
        dval = val.get_double();
        break;
    case STR_INDEX:
        sval = val.get_str();
        break;
    case BOO_INDEX:
        bval = val.get_bool();
        break;
    case OBJ_INDEX:
        mval = new MessageObject(*val.get_msgptr());
        break;
    }

    return *this;
}

Value Value::operator+(const Value& val) {

    switch (val.index()) {
    case INT_INDEX:
        return *this + val.get_int();
    case DBL_INDEX:
        return *this + val.get_double();
    case STR_INDEX:
        return *this + val.get_str();
    default:
        print_warning("invalid operands");
        return *this;
    }
}

Value Value::operator-(const Value& val) {

    switch (val.index()) {
    case INT_INDEX:
        return *this - val.get_int();
    case DBL_INDEX:
        return *this - val.get_double();
    default:
        print_warning("invalid operands");
        return *this;
    }
}

Value Value::operator*(const Value& val) {

    switch (val.index()) {
    case INT_INDEX:
        return *this * val.get_int();
    case DBL_INDEX:
        return *this * val.get_double();
    default:
        print_warning("invalid operands");
        return *this;
    }
}

Value Value::operator/(const Value& val) {

    switch (val.index()) {
    case INT_INDEX:
        return *this / val.get_int();
    case DBL_INDEX:
        return *this / val.get_double();
    default:
        print_warning("invalid operands");
        return *this;
    }
}

bool Value::operator==(const Value& val) {

    switch (curr_index) {
    case INT_INDEX:
        switch (val.index()) {
        case INT_INDEX:
            return (ival == val.get_int());
        case DBL_INDEX:
            return (ival == val.get_double());
        default:
            return false;
        }
        break;

    case DBL_INDEX:
        switch (val.index()) {
        case INT_INDEX:
            return (dval == val.get_int());
        case DBL_INDEX:
            return (dval == val.get_double());
        default:
            return false;
        }
        break;

    case STR_INDEX:
        switch (val.index()) {
        case STR_INDEX:
            return (sval == val.get_str());
        default:
            return false;
        }
        break;

    case BOO_INDEX:
        switch (val.index()) {
        case BOO_INDEX:
            return (bval == val.get_bool());
        default:
            return false;
        }
        break;

    case OBJ_INDEX:
        switch (val.index()) {
        case OBJ_INDEX:
            return (mval == val.get_msgptr());
        default:
            return false;
        }
        break;

    case FUN_INDEX:
        switch (val.index()) {
        default:
            return false;
        }
        break;

    case NON_INDEX:
        return val.is_empty();
    default:
        assert(false);
    }
}

bool Value::operator!=(const Value& val) {

    switch (curr_index) {
    case INT_INDEX:
        switch (val.index()) {
        case INT_INDEX:
            return (ival != get_int());
        case DBL_INDEX:
            return (ival != get_double());
        default:
            return true;
        break;
        }

    case DBL_INDEX:
        switch (val.index()) {
        case INT_INDEX:
            return (dval != val.get_int());
        case DBL_INDEX:
            return (dval != val.get_double());
        default:
            return true;
        break;
        }

    case STR_INDEX:
        switch (val.index()) {
        case STR_INDEX:
            return (sval != val.get_str());
        default:
            return true;
        }
        break;

    case BOO_INDEX:
        switch (val.index()) {
        case BOO_INDEX:
            return (bval != val.get_bool());
        default:
            return true;
        }
        break;

    case OBJ_INDEX:
        switch (val.index()) {
        case OBJ_INDEX:
            return (mval != val.get_msgptr());
        default:
            return true;
        }
        break;

    case FUN_INDEX:
        switch (val.index()) {
        default:
            return true;
        }
        break;

    case NON_INDEX:
        return val.is_empty();
    default:
        assert(false);
    }
}

bool Value::operator==(const int& val) {

    if (curr_index != INT_INDEX && curr_index != DBL_INDEX) {
        return false;
    }

    switch (curr_index) {
    case INT_INDEX:
        return ival == val;
    case DBL_INDEX:
        return dval == val;
    default:
        assert(false);
    }
}

bool Value::operator!=(const int& val) {

    if (curr_index != INT_INDEX && curr_index != DBL_INDEX) {
        return true;
    }

    switch (curr_index) {
    case INT_INDEX:
        return ival != val;
    case DBL_INDEX:
        return dval != val;
    default:
        assert(false);
    }
}

bool Value::operator==(const double& val) {

    if (curr_index != INT_INDEX && curr_index != DBL_INDEX) {
       return false;
    }

    switch (curr_index) {
    case INT_INDEX:
        return ival == val;
    case DBL_INDEX:
        return dval == val;
    default:
        assert(false);
    }
}

bool Value::operator!=(const double& val) {

    if (curr_index != INT_INDEX && curr_index != DBL_INDEX) {
       return true;
    }

    switch (curr_index) {
    case INT_INDEX:
        return ival != val;
    case DBL_INDEX:
        return dval != val;
    default:
        assert(false);
    }
}

bool Value::operator==(const std::string& val) {

    if (curr_index != STR_INDEX) {
       return false;
    }

    return sval == val;
}

bool Value::operator!=(const std::string& val) {

    if (curr_index != STR_INDEX) {
       return true;
    }

    return sval != val;
}

bool Value::operator==(const char* val) {

    if (curr_index != STR_INDEX) {
       return false;
    }

    return sval == val;
}

bool Value::operator!=(const char* val) {

    if (curr_index != STR_INDEX) {
       return true;
    }

    return sval != val;
}

bool Value::operator==(const bool& val) {

    if (curr_index != BOO_INDEX) {
       if (is_empty()) {
            return !val;
       }

       return false;
    }

    return bval == val;
}

bool Value::operator!=(const bool& val) {

    if (curr_index != BOO_INDEX) {
       if (is_empty()) {
            return val;
       }

       return true;
    }

    return bval != val;
}

bool Value::operator==(const MessageObject& other) {

    if (curr_index != OBJ_INDEX) {
       return false;
    }

    return mval == &other;
}

bool Value::operator!=(const MessageObject& other) {

    if (curr_index != OBJ_INDEX) {
       return true;
    }

    return mval != &other;
}

bool Value::operator==(None_t _none) {
    return is_empty();
}

bool Value::operator!=(None_t _none) {
    return !is_empty();
}

Value& Value::operator=(const int& val) {

    if (curr_index == OBJ_INDEX) {
        delete mval;
        mval = NULL;
    }

    ival = val;
    curr_index = INT_INDEX;

    return *this;
}

Value& Value::operator=(const double& val) {

    if (curr_index == OBJ_INDEX) {
        delete mval;
        mval = NULL;
    }

    dval = val;
    curr_index = DBL_INDEX;

    return *this;
}

Value& Value::operator=(const std::string& val) {

    if (curr_index == OBJ_INDEX) {
        delete mval;
        mval = NULL;
    }

    sval = val;
    curr_index = STR_INDEX;

    return *this;
}

Value& Value::operator=(const char* val) {

    if (curr_index == OBJ_INDEX) {
        delete mval;
        mval = NULL;
    }

    sval = val;
    curr_index = STR_INDEX;

    return *this;
}

Value& Value::operator=(const bool& val) {

    if (curr_index == OBJ_INDEX) {
        delete mval;
        mval = NULL;
    }

    bval = val;
    curr_index = BOO_INDEX;

    return *this;
}

Value& Value::operator=(const MessageObject& val) {

    if (curr_index == OBJ_INDEX) {
        delete mval;
        mval = NULL;
    }

    mval = new MessageObject((MessageObject)val);
    curr_index = OBJ_INDEX;

    return *this;
}

Value& Value::operator=(None_t _none) {

    if (curr_index == OBJ_INDEX) {
        delete mval;
        mval = NULL;
    }

    curr_index = NON_INDEX;
    empty = true;

    return *this;
}

Value Value::operator+(const int& val) {

    if (curr_index != INT_INDEX && curr_index != DBL_INDEX) {
        print_warning("uncompatable operator arguments");
        return *this;
    }

    switch (curr_index) {
    case INT_INDEX:
        return Value(ival + val);
    case DBL_INDEX:
        return Value(dval + val);
    default:
        assert(false);
    }
}

Value Value::operator+(const double& val) {

    if (curr_index != INT_INDEX && curr_index != DBL_INDEX) {
        print_warning("uncompatable operator arguments");
        return *this;
    }

    switch (curr_index) {
    case INT_INDEX:
        return Value(ival + val);
    case DBL_INDEX:
        return Value(dval + val);
    default:
        assert(false);
    }
}

Value Value::operator+(const std::string& val) {

    if (curr_index != STR_INDEX) {
        print_warning("uncompatable operator arguments");
        return *this;
    }

    return Value(sval + val);
}

Value Value::operator+(const char* val) {

    if (curr_index != STR_INDEX) {
        print_warning("uncompatable operator arguments");
        return *this;
    }

    return Value(sval + val);
}

Value Value::operator+(const bool& other) {
    print_warning("invalid use of + operator on a non-number");
    return *this;
}

Value Value::operator+(const MessageObject& other) {
    print_warning("invalid use of + operator on a non-number");
    return *this;
}

Value Value::operator+(None_t _none) {
    print_warning("invalid use of + operator on a non-number");
    return *this;
}

Value Value::operator-(const int& val) {

    if (curr_index != INT_INDEX && curr_index != DBL_INDEX) {
        print_warning("uncompatable operator arguments");
        return *this;
    }

    switch (curr_index) {
    case INT_INDEX:
        return Value(ival - val);
    case DBL_INDEX:
        return Value(dval - val);
    default:
        assert(false);
    }
}

Value Value::operator-(const double& val) {

    if (curr_index != INT_INDEX && curr_index != DBL_INDEX) {
        print_warning("uncompatable operator arguments");
        return *this;
    }

    switch (curr_index) {
    case INT_INDEX:
        return Value(ival - val);
    case DBL_INDEX:
        return Value(dval - val);
    default:
        assert(false);
    }
}

Value Value::operator-(const std::string& other) {
    print_warning("invalid use of - operator on a non-number");
    return *this;
}

Value Value::operator-(const char* other) {
    print_warning("invalid use of - operator on a non-number");
    return *this;
}

Value Value::operator-(const bool& other) {
    print_warning("invalid use of - operator on a non-number");
    return *this;
}

Value Value::operator-(const MessageObject& other) {
    print_warning("invalid use of - operator on a non-number");
    return *this;
}

Value Value::operator-(None_t _none) {
    print_warning("invalid use of - operator on a non-number");
    return *this;
}

Value Value::operator/(const int& val) {

    if (curr_index != INT_INDEX && curr_index != DBL_INDEX) {
        print_warning("uncompatable operator arguments");
        return *this;
    }

    switch (curr_index) {
    case INT_INDEX:
        return Value(ival / val);
    case DBL_INDEX:
        return Value(dval / val);
    default:
        assert(false);
    }
}

Value Value::operator/(const double& val) {

    if (curr_index != INT_INDEX && curr_index != DBL_INDEX) {
        print_warning("uncompatable operator arguments");
        return *this;
    }

    switch (curr_index) {
    case INT_INDEX:
        return Value(ival / val);
    case DBL_INDEX:
        return Value(dval / val);
    default:
        assert(false);
    }
}

Value Value::operator/(const std::string& other) {
    print_warning("invalid use of / operator to a non-number");
    return *this;
}

Value Value::operator/(const char* other) {
    print_warning("invalid use of / operator to a non-number");
    return *this;
}

Value Value::operator/(const bool& other) {
    print_warning("invalid use of / operator to a non-number");
    return *this;
}

Value Value::operator/(const MessageObject& other) {
    print_warning("invalid use of / operator to a non-number");
    return *this;
}

Value Value::operator/(None_t _none) {
    print_warning("invalid use of / operator to a non-number");
    return *this;
}

Value Value::operator*(const int& val) {

    if (curr_index != INT_INDEX && curr_index != DBL_INDEX) {
        print_warning("uncompatable operator arguments");
        return *this;
    }

    switch (curr_index) {
    case INT_INDEX:
        return Value(ival * val);
    case DBL_INDEX:
        return Value(dval * val);
    default:
        assert(false);
    }
}

Value Value::operator*(const double& val) {

    if (curr_index != INT_INDEX && curr_index != DBL_INDEX) {
        print_warning("uncompatable operator arguments");
        return *this;
    }

    switch (curr_index) {
    case INT_INDEX:
        return Value(ival * val);
    case DBL_INDEX:
        return Value(dval * val);
    default:
        assert(false);
    }
}

Value Value::operator*(const std::string& other) {
    print_warning("invalid use of * operator to a non-number");
    return *this;
}

Value Value::operator*(const char* other) {
    print_warning("invalid use of * operator to a non-number");
    return *this;
}

Value Value::operator*(const bool& other) {
    print_warning("invalid use of * operator to a non-number");
    return *this;
}

Value Value::operator*(const MessageObject& other) {
    print_warning("invalid use of * operator to a non-number");
    return *this;
}

Value Value::operator*(None_t _none) {
    print_warning("invalid use of * operator to a non-number");
    return *this;
}

size_t Value::index(void) const {
    return curr_index;
}

bool Value::is_empty(void) const {
    return empty;
}

int Value::get_int(void) const {
    if (curr_index != INT_INDEX) {
        printf("error: bad access!\n");
        exit(EXIT_FAILURE);
    }

    return ival;
}

double Value::get_double(void) const {
    if (curr_index != DBL_INDEX) {
        printf("error: bad access!\n");
        exit(EXIT_FAILURE);
    }

    return dval;
}

std::string Value::get_str(void) const {
    if (curr_index != STR_INDEX) {
        printf("error: bad access!\n");
        exit(EXIT_FAILURE);
    }

    return sval;
}

bool Value::get_bool(void) const {
    if (curr_index != BOO_INDEX) {
        printf("error: bad access!\n");
        exit(EXIT_FAILURE);
    }

    return bval;
}

MessageObject* Value::get_msgptr(void) const {
    if (curr_index != OBJ_INDEX) {
        printf("error: bad access!\n");
        exit(EXIT_FAILURE);
    }

    return mval; /* NOTE: check it out */
}

/** MESSAGE OBJECT **/
MessageObject* MessageObject::get_msgptr(void) {
    return msgptr;
}

void MessageObject::set_msgptr(MessageObject* ptr) {
    msgptr = ptr;
}

MessageObject* MessageObject::get_rcvptr(void) {
    return rcvptr;
}

void MessageObject::set_rcvptr(MessageObject* ptr) {
    rcvptr = ptr;
}

void MessageObject::set_callable_id(std::string id) {
    func_id = id;
}

/* Constructor */
MessageObject::MessageObject() = default;

/* Destructor */
MessageObject::~MessageObject() = default;

/* Copy constructor */
MessageObject::MessageObject(const MessageObject& other) {

    msgptr = other.msgptr;
    func_id = other.func_id;

    map.clear();
    for (auto& p: other.get_map()) {

        /* NOTE: possible bug, no deep copy? */
        if (p.second.index() != OBJ_INDEX) {
            Value temp = p.second;
            temp.owner = this;
            map.insert({p.first, temp});
            continue;
        }

        Value temp = Value(new MessageObject(*p.second.get_msgptr()));
        temp.owner = this;

        map.insert({p.first, temp});
    }
}

/* Copy Assignment operator */
MessageObject& MessageObject::operator=(const MessageObject& other) = default;

Value MessageObject::operator<<(const MessageObject& sender) {

    msgptr = const_cast<MessageObject *>(&sender);
    rcvptr = this;

    const_cast<MessageObject&>(sender).set_msgptr(msgptr);
    const_cast<MessageObject&>(sender).set_rcvptr(this);

    return (*this)[msgptr->get_callable()]();
}

/* Initialization:'values' */
MessageObject MessageObject::operator[](std::vector<value_variant>&& vec) {

    size_t i = 0;
    for (auto& v: vec) {
        switch (v.index()) {
            case CAL_INDEX:
            {
                func_id = std::get<CAL_INDEX>(v).get().get_id();
                //std::cout << "My callable method id is: " << func_id << nl;
                break;
            }
            case INT_INDEX:
            {
                Value temp = Value(std::get<INT_INDEX>(v));
                temp.owner = this;
                map.insert({std::to_string(i++), temp});
                break;
            }
            case DBL_INDEX:
            {
                Value temp = Value(std::get<DBL_INDEX>(v));
                temp.owner = this;
                map.insert({std::to_string(i++), temp});
                break;
            }
            case STR_INDEX:
            {
                Value temp = Value(std::get<STR_INDEX>(v));
                temp.owner = this;
                map.insert({std::to_string(i++), temp});
                break;
            }
            case BOO_INDEX:
            {
                Value temp = Value(std::get<BOO_INDEX>(v));
                temp.owner = this;
                map.insert({std::to_string(i++), temp});
                break;
            }
            case OBJ_INDEX:
            {
                Value temp = Value(new MessageObject(std::get<OBJ_INDEX>(v).get()));
                temp.owner = this;
                map.insert({std::to_string(i++), temp});
                break;
            }
            case FUN_INDEX:
            {
                Value temp = Value(std::get<FUN_INDEX>(v));
                temp.owner = this;
                map.insert({std::to_string(i++), temp});
                break;
            }
            case NON_INDEX:
            {
                Value temp = Value(none);
                temp.owner = this;
                map.insert({std::to_string(i++), temp});
                break;
            }
            default:
                assert(0);
                break;
        }
    }

    return *this;
}

/* Initialization: 'key' */
MessageObject MessageObject::operator[](const KeyFetcher& f) {

    func_id = f.func_id;

    for (auto& p: f.vec) {
        Value temp = Value(p.second);
        temp.owner = this;

        map.insert({p.first, temp});
    }

    return *this;
}

/* Accessing */
Value& MessageObject::operator[](std::string _key) {

    try {
        return map.at(_key);
    } catch (const std::out_of_range& e) {
        std::cerr << "error: no value with key: \"" << _key << "\" exists" << '\n';
        exit(EXIT_FAILURE);
    }
}

const std::map<std::string, Value>& MessageObject::get_map(void) const {
    return map;
}

std::vector<Value> MessageObject::get_values(void) {

    std::vector<Value> vec;
    for (auto& pair: map) {
        vec.push_back(pair.second);
    }

    return vec;
}

std::string MessageObject::get_callable(void) {
    return func_id;
}

/** KEY_FETCHER OBJECT **/
KeyFetcher::KeyFetcher(std::string _key) {
    tkey = _key;
}

/* Fetches the other expressions inside the initialization array '[ k1 = v, k2 = v, ... ]' */
KeyFetcher& KeyFetcher::operator,(const KeyFetcher& other) {

    for (auto& pair: other.vec) {
        vec.push_back(pair);
    }

    return *this;
}

KeyFetcher& KeyFetcher::operator=(int val) {
    vec.push_back({tkey, Value(val)});
    return *this;
}

KeyFetcher& KeyFetcher::operator=(double val) {
    vec.push_back({tkey, Value(val)});
    return *this;
}

KeyFetcher& KeyFetcher::operator=(std::string val) {
    vec.push_back({tkey, Value(val)});
    return *this;
}

KeyFetcher& KeyFetcher::operator=(const char* val) {
    vec.push_back({tkey, Value(val)});
    return *this;
}

KeyFetcher& KeyFetcher::operator=(const MessageObject& val) {
    vec.push_back({tkey, Value(new MessageObject(val))});
    return *this;
}

KeyFetcher& KeyFetcher::operator=(func_sig val) {
    vec.push_back({tkey, Value(val)});
    return *this;
}

void KeyFetcher::set_func_id(std::string id) {
    func_id = id;
}

/** VECTOR OVERLOADS **/
template <typename T>
std::vector<value_variant> operator,(std::vector<value_variant> vec, T val) {
    vec.push_back(val);
    return vec;
}

/* Heaps allocates everything it touches, careful! */
std::vector<value_variant> operator,(std::vector<value_variant> vec, const MessageObject& val) {

    /* FIXME: possible memory leak */
    MessageObject* msgptr = new MessageObject(val);
    vec.push_back(*msgptr);

    return vec;
}

/** NONE OBJECT **/
NoneObject::NoneObject() = default;

NoneObject* NoneObject::get_instance(void) {

    if (!instance) {
        instance = new NoneObject;
    }

    return instance;
}

/** KEYWORD IMPLEMENTATIONS **/
Value input_impl(std::string msg) {

    std::string str_in;

    std::cout << msg;
    std::cin >> str_in;

    if (is_int(str_in)) {
        return Value(std::stoi(str_in));
    } else if (is_float(str_in)) {
        return Value(std::stod(str_in));
    } else if (is_bool(str_in)) {
        return Value(str_in == "true");
    } else {
        return Value(str_in);
    }
}

bool eval_cond_impl(const Value& v) {

    switch (v.index()) {
    case INT_INDEX:
        return v.get_int() != 0;
    case DBL_INDEX:
        return v.get_double() != 0;
    case STR_INDEX:
        return v.get_str().size() != 0;
    case OBJ_INDEX:
        return v.get_msgptr()->get_map().size() != 0;
    case BOO_INDEX:
        return v.get_bool();
    case NON_INDEX:
        return false;
    case CAL_INDEX:
        return true;
    default:
        assert(false);
    }
}

/** PRINTING **/
std::ostream& operator<<(std::ostream& os, const MessageObject& msg) {

    size_t size = msg.map.size();

    os << "object[";
    for (auto& p: msg.map) {
        os << "\"" << p.first << "\":" << p.second << (--size ? ", " : "");
    }
    os << "]";

    return os;
}

std::ostream& operator<<(std::ostream& os, const Value& val) {

    switch (val.index()) {
    case INT_INDEX:
        os << val.get_int();
        break;
    case DBL_INDEX:
        os << val.get_double();
        break;
    case STR_INDEX:
        os << "\"" + val.get_str() + "\"";
        break;
    case BOO_INDEX:
        os << (val.get_bool() ? "true" : "false");
        break;
    case OBJ_INDEX:
        os << *(val.get_msgptr());
        break;
    case FUN_INDEX:
        os << "method";
        break;
    case NON_INDEX:
        os << "None";
        break;
    default:
        assert(false);
        break;
    }

    return os;
}

int main(void) {

    let printf_impl = object [
        func("printf") {
            for (auto& v: args_list) {
                std::cout << v << nl;
            }

            return none;
        }
    ];

    let a = object [call("printf"), values 1, "2", true, -3.14];

    printf_impl << a;

    let connection = object [
        call("connect"),
        key("ip") = "1.1.1.1",
        key("port") = 5000,
        func("_cond_") { return arg(port) == self(port); },
        func("_success_") {
            std::cout << arg(ip) << " connnected to " << self(ip) << nl;
            return none;
        },

        func("_failure_") {
            std::cout << arg(ip) << " failed to connect to " << self(ip) << nl;
            return none;
        }
    ];

    let conn_impl = object [
        key("ip") = "127.0.0.1",
        key("port") = 30303,
        func("connect") {
            if (eval_cond(_cond_)) {
                eval("_success_");
            }
            else {
                eval("_failure_");
            }
            return none;
        }
    ];


    conn_impl << connection;

    return 0;
}

