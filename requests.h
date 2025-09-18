#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

struct HTTP {
    HTTP(const string& host, int port);

    // admin functions
    bool   loginAdmin(const string& user, const string& pass);
    bool   addUser(const string& user, const string& pass);
    bool   getUsers();
    bool   deleteUser(const string& username);
    bool   logoutAdmin();

    // user functions
    bool   loginUser(const string& adminUser,
                     const string& user,
                     const string& pass);
    bool   logoutUser();

    // access function
    bool   getAccess();

    // movies functions
    bool   getMovies();
    bool   getMovie(int movieId);
    bool   addMovie(const string& title, int year, const string& description, double rating);
    bool   updateMovie(int movieId, const string& title, int year, const string& description, double rating);
    bool   deleteMovie(int movieId);
   

public:
    //bool to check if there is a valid jwt token
    bool hasToken() const { return !jwt.empty(); }

private:
    string        host;
    int           port;
    int           sockfd = -1;
    vector<string> cookies;
    string        jwt;

    bool   connectServer();
    bool   sendRequest(const string& req, string& resp);

    // helper functions
    string buildCookieHeader() const;
    string buildAuthHeader()   const;
    int    parseStatusCode(const string& response) const;
    string extractBody(const string& response) const;
    void   storeSetCookie(const string& response);
    
};


