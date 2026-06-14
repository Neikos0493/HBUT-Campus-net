#ifndef AUTH_CLIENT_H
#define AUTH_CLIENT_H
#include <string>

class AuthClient {
public:
    bool login(const std::string& username, const std::string& password, const std::string& url);
    bool logout(const std::string& url);
};
#endif