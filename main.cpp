#include "json.cpp"

int main(){
    try{
        json j;
        std::cin>>j;
        std::cout<<j<<std::endl;
        j["string7"];
        std::cout<<j["string1"]<<std::endl;
        std::cout<<j["string8"]<<std::endl;
        std::cout<<j<<std::endl;
        auto it = j.begin_dictionary();
        it++;
        std::cout<<it->first<<std::endl;
        it++;
        std::cout<<it->first<<std::endl;
    }
    catch(const json_exception& e)
    {
        std::cerr << e.msg << '\n';
    }

    return 0;
}