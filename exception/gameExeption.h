#ifndef SRC_GAMEEXEPTION_H
#define SRC_GAMEEXEPTION_H

#include <exception>
#include <string>

using namespace std;

class gameException: public exception {
    public:
        gameException(string const& i):info(i){}

        const char* what() const noexcept override
        {
            return info.c_str();
        }

    private:
        string info;
};

#endif //SRC_GAMEEXEPTION_H
