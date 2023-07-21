#include "json.hpp"

struct Cella{
    json info;
    Cella* next;
    Cella* prev;

    Cella() : info(), next(nullptr), prev(nullptr) {}
    Cella(const json& j) : info(j), next(nullptr), prev(nullptr) {}
    Cella(const Cella& other) : info(other.info), next(other.next), prev(other.prev) {}
    ~Cella() {}
};
typedef Cella* Pcella;
struct Dizionario{
    std::string chiave;
    json info;
    Dizionario* next;
    Dizionario* prev;

    Dizionario() : chiave(), info(), next(nullptr), prev(nullptr) {}
    Dizionario(const Dizionario& other) : chiave(other.chiave), info(other.info), next(other.next), prev(other.prev) {}
    Dizionario(const std::string& chiave, const json& info) : chiave(chiave), info(info), next(nullptr), prev(nullptr) {}
    ~Dizionario() {}
    std::string get_chiave(){
        return chiave;
    }
};
typedef Dizionario* Pdiz;

struct json::impl{
    enum class tipo_dato{ null, number, boolean, string, list, dictionary };
    tipo_dato tipo;

    union{
        double m_number;
        std::string m_string;
        bool m_boolean;
        bool m_null;
        struct{
            Pcella head;
            Pcella tail;
        } list;
        struct{
            Pdiz head;
            Pdiz tail;
        } dict;
    };

    impl() : tipo(tipo_dato::null) , m_null(true) {}
    impl(const impl& other){
        this->tipo = other.tipo;
        if(this->tipo == tipo_dato::number) this->m_number = other.m_number;
        if(this->tipo == tipo_dato::boolean) this->m_boolean = other.m_boolean;
        if(this->tipo == tipo_dato::string) new(&this->m_string) std::string(other.m_string);
        if(this->tipo == tipo_dato::list){
                this->list.head = nullptr;
                this->list.tail = nullptr;
                auto it = other.list.head;
                while(it!=nullptr){
                    Pcella nuova_cell = new Cella{ it->info };
                    if (list.head == nullptr) { //lista vuota
                        list.head = list.tail = nuova_cell;
                    }else{ //lista piena
                        list.tail->next = nuova_cell;
                        nuova_cell->prev = list.tail;
                        list.tail = nuova_cell;
                    }
                    it=it->next;
                }
        }
         if(this->tipo == tipo_dato::dictionary){
                this->dict.head = nullptr;
                this->dict.tail = nullptr;
                for (auto it = other.dict.head; it != nullptr; it = it->next) {
                    Pdiz nuovo = new Dizionario{ it->chiave, it->info };

                    if (dict.head == nullptr) { //dizionario vuoto
                        dict.head = nuovo;
                        dict.tail = nuovo;
                    } else { //dizionario pieno
                        nuovo->prev = dict.tail;
                        dict.tail->next = nuovo;
                        dict.tail = nuovo;
                    }
                }
         }
    }

    impl(tipo_dato t) : tipo(t) {
        switch(t) {
            case tipo_dato::number:
                m_number = 0.0;
                break;
            case tipo_dato::string:
                new(&m_string) std::string();
                break;
            case tipo_dato::boolean:
                m_boolean = false;
                break;
            case tipo_dato::null:
                m_null = true;
                break;
            case tipo_dato::list:
                list.head = nullptr;
                list.tail = nullptr;
                break;
            case tipo_dato::dictionary:
                dict.head = nullptr;
                dict.tail = nullptr;
                break;
        }
    }
    /*~impl() {
        if (tipo == tipo_dato::string) {
            m_string.~basic_string();
        } else if (tipo == tipo_dato::list) {
            auto current = list.head;
            while (current) {
                auto next = current->next;
                delete current;
                current = next;
            }
        } else if (tipo == tipo_dato::dictionary) {
            auto current = dict.head;
            while (current) {
                auto next = current->next;
                delete current;
                current = next;
            }
        }
    }*/
    ~impl() {
        if (tipo == tipo_dato::string) {
            m_string.~basic_string();
        } else if (tipo == tipo_dato::list) {
            auto current = list.head;
            while (current) {
                auto next = current->next;
                delete current;
                current = next;
            }
        } else if (tipo == tipo_dato::dictionary) {
            auto current = dict.head;
            while (current) {
                auto next = current->next;
                delete current;
                current = next;
            }
        }
    }
};

struct json::list_iterator{
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = json;
        using pointer = json*;
        using reference = json&;

        list_iterator(Pcella p) : curr{p} {}
        ~list_iterator() = default;
        
        reference operator*() const{
            return curr->info;
        }
        pointer operator->() const{
            return &(curr->info);
        }
        list_iterator& operator++(){
            curr = curr->next;
            return *this;
        }
        list_iterator operator++(int){
            list_iterator temp{*this};
            curr = curr->next;
            return temp;
        }
        operator bool() const{
            return curr != nullptr;
        }
        bool operator==(list_iterator const& rhs) const{
            return curr == rhs.curr;
        }
        bool operator!=(list_iterator const& rhs) const{
            return curr != rhs.curr;
        }

    private:
        Pcella curr;
};
struct json::const_list_iterator{
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = json const;
        using pointer = json const*;
        using reference = json const&;

        const_list_iterator(Pcella p) : curr{p} {}
        ~const_list_iterator() = default;

        reference operator*() const{
            return curr->info;
        }
        pointer operator->() const{
            return &(curr->info);
        }
        const_list_iterator& operator++(){
            curr = curr->next;
            return *this;
        }
        const_list_iterator operator++(int){
            const_list_iterator temp{*this};
            curr = curr->next;
            return temp;
        }
        operator bool() const{
            return curr != nullptr;
        }
        bool operator==(const_list_iterator const& rhs) const{
            return curr == rhs.curr;
        }
        bool operator!=(const_list_iterator const& rhs) const{
            return curr != rhs.curr;
        }
    private:
        Pcella curr;
};
struct json::dictionary_iterator{
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Dizionario;
        using pointer = Dizionario*;
        using reference = Dizionario&;

        dictionary_iterator() : curr(nullptr) {}
        dictionary_iterator(Pdiz p) : curr{p}{}
        dictionary_iterator(const dictionary_iterator&) = default;
        ~dictionary_iterator() = default;

        std::pair<std::string, json>& operator*() const{
            return *new std::pair<std::string, json> {curr->chiave, curr->info};
        }
        std::pair<std::string, json>* operator->() const{
            return new std::pair<std::string, json> {curr->chiave, curr->info};
        }
        dictionary_iterator& operator++(){
            curr = curr->next;
            return *this;
        }
        dictionary_iterator operator++(int){
            dictionary_iterator temp {*this};
            operator++();
            return temp;
        }
        operator bool() const{
            return curr != nullptr;
        }
        bool operator==(dictionary_iterator const& rhs) const{
            return curr == rhs.curr;
        }
        bool operator!=(dictionary_iterator const& rhs) const{
            return curr != rhs.curr;
        }

    private:
        Pdiz curr;
};
struct json::const_dictionary_iterator{
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = json const;
        using pointer = json const*;
        using reference = json const&;

        const_dictionary_iterator() : curr(nullptr) {}
        const_dictionary_iterator(Pdiz p) : curr{p}{}
        const_dictionary_iterator(const const_dictionary_iterator&) = default;
        ~const_dictionary_iterator() = default;

        std::pair<std::string, json>& operator*() const{
            return *new std::pair<std::string, json> {curr->chiave, curr->info};
        }
        std::pair<std::string, json>* operator->() const{
            return new std::pair<std::string, json> {curr->chiave, curr->info};
        }
        const_dictionary_iterator& operator++(){
            curr = curr->next;
            return *this;
        }
        const_dictionary_iterator operator++(int){
            const_dictionary_iterator temp {*this};
            curr = curr->next;
            return temp;
        }
        operator bool() const{
            return curr != nullptr;
        }
        bool operator==(const_dictionary_iterator const& rhs) const{
            return curr == rhs.curr;
        }
        bool operator!=(const_dictionary_iterator const& rhs) const{
            return curr != rhs.curr;
        }

    private:
        Pdiz curr;
};

//default constructor, copy constructor, move constructor, descructor
json::json() : pimpl(new impl{ impl::tipo_dato::null }) {}
json::json(json const& other) {
    pimpl = new impl(*other.pimpl);
}
json::json(json&& other) : pimpl(nullptr) {
    std::swap(pimpl, other.pimpl);
}
json::~json() {
    delete pimpl;
}

// Move assignment, copy asigment
json& json::operator=(json&& other) {
    if (this != &other) {
        delete pimpl;
        std::swap(pimpl, other.pimpl);
    }
    return *this;
}
json& json::operator=(json const& other) {
    if (this != &other) {
        delete pimpl;
        pimpl = new impl(*other.pimpl);
    }
    return *this;
}

bool json::is_list() const{
    if (!pimpl)return false;
    return pimpl->tipo==impl::tipo_dato::list;
}
bool json::is_dictionary() const {
    if (!pimpl) return false;
    return pimpl->tipo == impl::tipo_dato::dictionary;
}

bool json::is_string() const{
    if (!pimpl)return false;
    return pimpl->tipo==impl::tipo_dato::string;
}
bool json::is_number() const{
    if (!pimpl)return false;
    return pimpl->tipo==impl::tipo_dato::number;
}
bool json::is_bool() const{
    if (!pimpl)return false;
    return pimpl->tipo==impl::tipo_dato::boolean;
}
bool json::is_null() const{
    if (!pimpl)return false;
    return pimpl->tipo==impl::tipo_dato::null;
}


json const& json::operator[](std::string const& key) const {
    if (!is_dictionary()) throw json_exception{"Invalid operator[]: object is not a dictionary"};
    bool find = false;
    Pdiz pc = pimpl->dict.head;
    while (pc!=nullptr && !find){
        if(pc->chiave==key) find = true;
        else pc=pc->next;
    }
    if(find) return pc->info;
    else throw json_exception{"Key not found in dictionary"};;
}
json& json::operator[](std::string const& key) {
    if (!is_dictionary()) throw json_exception{"Invalid operator[]: object is not a dictionary"};
    bool find = false;
    Pdiz pc = pimpl->dict.head;
    while (pc!=nullptr && !find){
        if(pc->chiave==key) find = true;
        else pc=pc->next;
    }
    
    if(find) return pc->info;
    else{ 
        insert({key, json()});
        pc = pimpl->dict.tail;
        return pc->info;
    }
}

json::list_iterator json::begin_list() {
    if (!is_list()) throw json_exception{"Invalid begin_list(): object is not a list"};
    return list_iterator{pimpl->list.head};
}
json::const_list_iterator json::begin_list() const {
    if (!is_list()) throw json_exception{"Invalid begin_list(): object is not a list"};
    return const_list_iterator{pimpl->list.head};
}
json::list_iterator json::end_list() {
    if (!is_list()) throw json_exception{"Invalid end_list(): object is not a list"};
    return list_iterator{nullptr};
}
json::const_list_iterator json::end_list() const {
    if (!is_list()) throw json_exception{"Invalid end_list(): object is not a list"};
    return const_list_iterator{nullptr};
}

json::dictionary_iterator json::begin_dictionary() {
    if (!is_dictionary()) throw json_exception{"Object is not a dictionary."};
    return dictionary_iterator(pimpl->dict.head);
}
json::const_dictionary_iterator json::begin_dictionary() const {
    if (!is_dictionary()) throw json_exception{"Object is not a dictionary."};
    return const_dictionary_iterator(pimpl->dict.head);
}
json::dictionary_iterator json::end_dictionary() {
    if (!is_dictionary()) throw json_exception{"Object is not a dictionary."};
    return dictionary_iterator(nullptr);
}
json::const_dictionary_iterator json::end_dictionary() const {
    if (!is_dictionary()) throw json_exception{"Object is not a dictionary."};
    return const_dictionary_iterator(nullptr);
}

double& json::get_number() {
    if (!is_number()) throw json_exception{"json object is not a number"};
    return pimpl->m_number;
}
double const& json::get_number() const {
    if (!is_number()) throw json_exception{"json object is not a number"};
    return pimpl->m_number;
}
bool& json::get_bool(){
    if (!is_bool()) throw json_exception{"json object is not a bool"};
    return pimpl->m_boolean;
}
bool const& json::get_bool() const{
    if (!is_bool()) throw json_exception{"json object is not a bool"};
    return pimpl->m_boolean;
}
std::string& json::get_string(){
    if (!is_string()) throw json_exception{"json object is not a string"};
    return pimpl->m_string;
}
std::string const& json::get_string() const{
    if (!is_string()) throw json_exception{"json object is not a string"};
    return pimpl->m_string;
}

void json::set_string(std::string const& x){
    if(pimpl!=nullptr) delete pimpl;

    pimpl = new impl;
    pimpl->tipo = impl::tipo_dato::string;
    //pimpl->m_string = x;
    new (&pimpl->m_string) std::string(x);
}
void json::set_bool(bool x) {
    if(pimpl!=nullptr) delete pimpl;

    pimpl = new impl;
    pimpl->tipo = json::impl::tipo_dato::boolean;
    pimpl->m_boolean = x;
}
void json::set_number(double x) {
    if(pimpl!=nullptr) delete pimpl;

    pimpl = new impl;
    pimpl->tipo = impl::tipo_dato::number;
    pimpl->m_number = x;
}
void json::set_null() {
    if (pimpl != nullptr) delete pimpl;

    pimpl = new impl{ impl::tipo_dato::null };
}
void json::set_list() {
    if (pimpl != nullptr) {
        if (is_list()) { //cancello lista
            Pcella it = pimpl->list.head;
            while (it != nullptr) {
                Pcella tmp = it->next;
                delete it;
                it = tmp;
            }
            pimpl->list.head = nullptr;
            pimpl->list.tail = nullptr;
        } else if (is_dictionary()) { //cancello dizionario
            Pdiz it = pimpl->dict.head;
            while (it != nullptr) {
                Pdiz tmp = it->next;
                delete it;
                it = tmp;
            }
            pimpl->dict.head = nullptr;
            pimpl->dict.tail = nullptr;
        }
        pimpl->tipo = impl::tipo_dato::list;
        pimpl->list.head = nullptr;
        pimpl->list.tail = nullptr;
    } else {
        pimpl = new impl();
        pimpl->tipo = impl::tipo_dato::list;
        pimpl->list.head = nullptr;
        pimpl->list.tail = nullptr;
    }
}
void json::set_dictionary() {
    if (pimpl != nullptr) delete pimpl;

    pimpl = new impl();
    pimpl->tipo = impl::tipo_dato::dictionary;
    pimpl->dict.head = nullptr;
    pimpl->dict.tail = nullptr;
}

void json::push_front(json const& j) {
    if (!is_list()) throw json_exception{"Cannot push front to non-list"};

    auto& list = this->pimpl->list;
    Pcella nuova_cell = new Cella{j};
    if (list.head == nullptr) { //lista vuota
        list.head = list.tail = nuova_cell;
    }else{ //lista piena
        nuova_cell->next = list.head;
        nuova_cell->prev = nullptr;
        list.head = nuova_cell;
    }
}
void json::push_back(json const& j) {
    if (!is_list()) throw json_exception{"Cannot push back to non-list"};

    auto& list = this->pimpl->list;
    Pcella nuova_cell = new Cella{j};
    if (list.head == nullptr) { //lista vuota
        list.head = list.tail = nuova_cell;
    }else{ //lista piena
        list.tail->next = nuova_cell;
        nuova_cell->prev = list.tail;
        list.tail = nuova_cell;
    }
}
void json::insert(std::pair<std::string,json> const& p) {
    std::string key = p.first;
    json value = p.second;

    if (!is_dictionary()) throw json_exception{"Cannot pinsert to non-dictionary"};

    Pdiz nuovo = new Dizionario{key, value};

    if (pimpl->dict.head == nullptr) { //dizionario vuoto
        pimpl->dict.head = nuovo;
        pimpl->dict.tail = nuovo;
    } else { //dizionario pieno
        nuovo->prev = pimpl->dict.tail;
        pimpl->dict.tail->next = nuovo;
        pimpl->dict.tail = nuovo;
    }
}


std::ostream& operator<<(std::ostream& lhs, json const& rhs){
    if(rhs.is_null()) {
        lhs << "null";
    } else if(rhs.is_bool()) {
        if(rhs.get_bool()) lhs << "true";
        else lhs << "false";
    } else if(rhs.is_number()) {
        lhs << rhs.get_number();
    } else if(rhs.is_string()) {
        lhs << '"' << rhs.get_string() << '"';
    } else if(rhs.is_list()){
        lhs << '[';
        if(!(rhs.begin_list() == rhs.end_list())){
            auto it = rhs.begin_list();
            lhs << *it;
            for (++it; it != rhs.end_list(); ++it) {
                lhs << ", " << *it;
            }
        }
        lhs << ']';
    } else if(rhs.is_dictionary()){
        lhs << '{';
        if(!(rhs.begin_dictionary() == rhs.end_dictionary())){
            auto it = rhs.begin_dictionary();
            lhs << '"' << it->first << '"' << ": " << it->second;
            for (++it; it != rhs.end_dictionary(); ++it) {
                lhs << ", \"" << it->first << '"' << ": " << it->second;
            }
        }
        lhs << '}';
    }
    return lhs;
}


void skip_space(std::istream& is);  //remove space
void J(std::istream&, json&);       //file json
void L(std::istream&, json&);       //liste
void D(std::istream&, json&);       //dizionari

std::istream& operator>>(std::istream& lhs, json& rhs){
    skip_space(lhs);
    if(!lhs.eof())
        J(lhs, rhs);

    return lhs;
}



/*
GRAMMATICA
    J() -> [L] || {D} || "string" || double || bool || null
    L() -> J || L, L
    D() -> "string" : J || D, D
*/


void skip_space(std::istream& is){
    while (std::isspace(is.peek()))
        is.ignore();
}

void J(std::istream& is, json& j){
    skip_space(is);
    if (is.eof()) throw json_exception{"unexpected end of input"};
    
    char c = is.peek();
    if(c=='['){                 //inizio lista
        is.ignore();
        j.set_list();
        L(is, j);
        skip_space(is);
        if (is.get() != ']') throw json_exception{"expected ']' after list"};
    }else if(c=='{'){           //inizio dizionario
        is.ignore();
        j.set_dictionary();
        D(is, j);
        skip_space(is);
        if (is.get() != '}') throw json_exception{"expected '}' after dictiornary"};
    }else if(c=='"'){           //string
        is.ignore(); // ignora eventuali spazi o altri caratteri prima delle virgolette
        std::string str;
        char c = 0;
        bool escaped = false;
        bool found = false;
        while (!found) {
            c = is.get();
            if (c == '\\') {
                escaped = true;
            }
            if (c == '"' && !escaped) {
                found = true;
            }else{
                if(c=='\\'){
                    str += '\\';
                    escaped = true;
                }else{
                    str += c;
                    escaped = false;
                }
            }
        }

        if (c != '"') {
            throw json_exception{"expected '\"' after string"};
        }

        j.set_string(str);
    }else if(c=='t'){           //(bool) => true
        std::string s;
        std::getline(is, s, 'e');
        if (s != "tru" || is.eof()) throw json_exception{"expected 'true': "+s};
        //is.ignore(); // consuma 'e' and ' '
        j.set_bool(true);
    }else if(c=='f'){           //(bool) => true
        std::string s;
        std::getline(is, s, 'e');
        if (s != "fals" || is.eof()) throw json_exception{"expected 'false': "+s};
        //is.ignore(); // consuma 'e', ' ' and ' '
        j.set_bool(false);
    }else if(c=='n'){           //null
        std::string s;
        std::getline(is, s, 'l');
        if (s != "nu" || is.peek() != 'l') throw json_exception{"expected 'null': "+s};
        is.ignore(); // consuma 'l' and ' '
        j.set_null();
    }else{                      //double
        double d;
        if (!(is >> d)) throw json_exception{"expected double number"};
        j.set_number(d);
    }
}

void L(std::istream& is, json& j){
    skip_space(is);
    if (is.peek() != ']') {
        bool found = true;
        while (found) {
            json item;
            J(is, item);
            j.push_back(item);
            skip_space(is);
            char c = is.peek();
            if (c == ']') {
                found = false;
            } else if (c != ',') throw json_exception{"expected ',' or ']'"};
            else{
                is.ignore();
                skip_space(is);
                c = is.peek();
                if(c==']') found = false;
            }
        }
    }
}

void D(std::istream& is, json& j){
    skip_space(is);
    if (is.peek() != '}'){
        bool found = true;
        while (found) {
            skip_space(is);
            if (is.peek() != '"') throw json_exception{"expected string"};
            is.ignore();
            std::string key;
            std::getline(is, key, '"');
            skip_space(is);
            if (is.get() != ':') throw json_exception{"expected ':' after key"};
            skip_space(is);
            json value;
            J(is, value);
            j[key] = value;
            skip_space(is);
            char c = is.peek();
            if (c == ',') {
                is.ignore();
                skip_space(is);
            } else if (c == '}') {
                found = false;
            }else throw json_exception{"expected ',' or '}'"};
        }
    }
}