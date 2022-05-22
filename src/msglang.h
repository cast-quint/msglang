/*
 * Dimitrios Koropoulis 3967
 * csd3967@csd.uoc.gr
 * CS352 - Fall 2021
 * msglang.h
 */

#ifndef MSGLANG_H
#define MSGLANG_H

#include <map>
#include <variant>
#include <vector>
#include <string>
#include <functional>
#include <memory>

/** MSGLANG KEYWORDS **/
#define nl              '\n'
#define values          std::vector<value_variant>(),
#define object          MessageObject()
#define key(str)        KeyFetcher(str)
#define let             auto
#define input(msg)      input_impl(msg)
#define None_t          std::shared_ptr<NoneObject>
#define lambda          [&](MessageObject* rec, MessageObject* msg)
#define func(id)        key(id) = lambda
#define self(id)        (*rec)[#id]
#define arg(id)         (*msg)[#id]
#define args_list       (*msg).get_values()
#define call(id)        CallObject(id)
#define eval(id)        (*msg)[id]()
#define eval_cond(id)   eval_cond_impl(eval(#id))

#define INT_INDEX 0
#define DBL_INDEX 1
#define STR_INDEX 2
#define BOO_INDEX 3
#define OBJ_INDEX 4
#define FUN_INDEX 5
#define NON_INDEX 6
#define CAL_INDEX 7

/** FORWARD DECLARATIONS **/
class MessageObject;
class CallObject;
class KeyFetcher;
class Value;


/* Singleton Class */
class NoneObject {
private:
    static NoneObject *instance;
    NoneObject();
public:
    static NoneObject* get_instance(void);
};

/** NONE OBJECT **/
/* Single global 'None' object */
NoneObject* NoneObject::instance = 0;
const None_t none(none->get_instance());

/** ALIASES **/
using msg_ref = std::reference_wrapper<MessageObject>;
using func_sig = std::function<Value(MessageObject*, MessageObject*)>;
using value_variant = std::variant<int, double, std::string, bool, msg_ref, func_sig, None_t, std::reference_wrapper<CallObject>>;

/** CALL OBJECT **/
class CallObject {
private:
    std::string id;
public:
    CallObject(std::string _id);
    std::vector<value_variant> operator,(std::vector<value_variant> vec);
    KeyFetcher operator,(KeyFetcher& f);

    std::string get_id(void);
};

/** VALUE OBJECT **/
class Value {
private:
    int             ival;
    double          dval;
    std::string     sval;
    bool            bval;
    MessageObject*  mval;
    func_sig        fval;

    bool empty;
    size_t curr_index;

    void init(void);
public:
    MessageObject* owner;

    Value(int val);
    Value(double val);
    Value(std::string val);
    Value(const char* val);
    Value(bool val);
    Value(MessageObject* val);
    Value(const MessageObject& val);
    Value(func_sig val); // OK
    Value(None_t val);

    ~Value(void);
    Value(const Value& val);

    Value  operator()(void);
    Value& operator[](const std::string& other);
    Value& operator[](const char* other);

    Value& operator= (const Value& val);
    Value  operator+ (const Value& val);
    Value  operator- (const Value& val);
    Value  operator* (const Value& val);
    Value  operator/ (const Value& val);
    bool   operator==(const Value& val);
    bool   operator!=(const Value& val);

    Value& operator= (const int& other);
    Value  operator+ (const int& other);
    Value  operator- (const int& other);
    Value  operator* (const int& other);
    Value  operator/ (const int& other);
    bool   operator==(const int& other);
    bool   operator!=(const int& other);

    Value& operator= (const double& other);
    Value  operator+ (const double& other);
    Value  operator- (const double& other);
    Value  operator* (const double& other);
    Value  operator/ (const double& other);
    bool   operator==(const double& other);
    bool   operator!=(const double& other);

    Value& operator= (const std::string& other);
    Value  operator+ (const std::string& other);
    Value  operator- (const std::string& other);
    Value  operator* (const std::string& other);
    Value  operator/ (const std::string& other);
    bool   operator==(const std::string& other);
    bool   operator!=(const std::string& other);

    Value& operator= (const char* other);
    Value  operator+ (const char* other);
    Value  operator- (const char* other);
    Value  operator* (const char* other);
    Value  operator/ (const char* other);
    bool   operator==(const char* other);
    bool   operator!=(const char* other);

    Value& operator= (const bool& other);
    Value  operator+ (const bool& other);
    Value  operator- (const bool& other);
    Value  operator* (const bool& other);
    Value  operator/ (const bool& other);
    bool   operator==(const bool& other);
    bool   operator!=(const bool& other);

    Value& operator= (const MessageObject& other);
    Value  operator+ (const MessageObject& other);
    Value  operator- (const MessageObject& other);
    Value  operator* (const MessageObject& other);
    Value  operator/ (const MessageObject& other);
    bool   operator==(const MessageObject& other);
    bool   operator!=(const MessageObject& other);

    Value& operator= (None_t _none);
    Value  operator+ (None_t _none);
    Value  operator- (None_t _none);
    Value  operator/ (None_t _none);
    Value  operator* (None_t _none);
    bool   operator==(None_t _none);
    bool   operator!=(None_t _none);

    Value& operator= (const func_sig f);
    Value  operator+ (const func_sig f);
    Value  operator- (const func_sig f);
    Value  operator* (const func_sig f);
    Value  operator/ (const func_sig f);
    bool   operator==(const func_sig f);
    bool   operator!=(const func_sig f);

    int             get_int(void) const;
    double          get_double(void) const;
    std::string     get_str(void) const;
    bool            get_bool(void) const;
    MessageObject*  get_msgptr(void) const;
    func_sig        get_func(void) const;

    size_t index(void) const;
    bool is_empty(void) const;

    friend std::ostream& operator<<(std::ostream& os, const Value& val);
    friend std::ostream& operator<<(std::ostream& os, const MessageObject& msg);
};

/** KEY_FETCHER OBJECT **/
class KeyFetcher {
private:
    std::vector<std::pair<std::string, Value>> vec;
    std::string tkey;
    std::string func_id = "KEYFETCHER_DEFAULT";

    friend class MessageObject;

public:
    KeyFetcher(std::string _key);

    /* Used for fetching the other expressions inside the initilization array '[ ... ]' */
    KeyFetcher& operator,(const KeyFetcher& other);

    KeyFetcher& operator=(int val);
    KeyFetcher& operator=(double val);
    KeyFetcher& operator=(std::string val);
    KeyFetcher& operator=(const char* val);
    KeyFetcher& operator=(const MessageObject& val);
    KeyFetcher& operator=(func_sig val);

    void set_func_id(std::string id);
};

/** MESSAGE OBJECT **/
class MessageObject {
private:
    std::map<std::string, Value> map;
    MessageObject* rcvptr = NULL;
    MessageObject* msgptr = NULL;
    std::string func_id;
public:
    /* Constructor */
    MessageObject();

    /* Destructor */
    ~MessageObject();

    /* Copy constructor */
    MessageObject(const MessageObject& other);

    /* Copy Assignment operator */
    MessageObject& operator=(const MessageObject& other);

    /* Initialization: 'values' */
    MessageObject operator[](std::vector<value_variant>&& vec);

    /* Initialization: 'key' */
    MessageObject operator[](const KeyFetcher& f);

    /* Accessing */
    Value& operator[](std::string _key);

    /* Printing */
    Value operator<<(const MessageObject& sender);

    void set_callable_id(std::string id);
    std::string get_callable(void);

    MessageObject* get_msgptr(void);
    MessageObject* get_rcvptr(void);
    void set_msgptr(MessageObject* ptr);
    void set_rcvptr(MessageObject* ptr);
    std::vector<Value> get_values(void);
    const std::map<std::string, Value>& get_map(void) const;

    friend std::ostream& operator<<(std::ostream& os, const MessageObject& msg);
};

/** KEYWORD IMPLEMENTATIONS **/
Value input_impl(std::string str_msg);
bool eval_cond_impl(const Value& v);

/** VECTOR OVERLOADS */
template <typename T>
std::vector<value_variant> operator,(std::vector<value_variant> vec, T val);
std::vector<value_variant> operator,(std::vector<value_variant> vec, const MessageObject& val);

/** PRINTING **/
std::ostream& operator<<(std::ostream& os, const MessageObject& msg);
std::ostream& operator<<(std::ostream& os, const Value& val);

#endif /* MSGLANG_H */
