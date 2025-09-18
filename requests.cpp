#include "requests.h"
#include "helper.h"
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// client initialization based on port and host
HTTP::HTTP(const string& h, int p): host(h), port(p) { }

// starts TCP connection
bool HTTP::connectServer()
{
    // if sockfd is valid, close previous connection
    if (sockfd != -1)
    {
        close(sockfd);
        sockfd = -1;
    }

    //preparing to start TCP connection
    addrinfo hints{}, *res;
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // if getaddrinfo not valid, aborts
    if (getaddrinfo(host.c_str(), to_string(port).c_str(), &hints, &res) != 0)
        return false;

    //resolves host and port to an address
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    // if socket creation fails
    if (sockfd < 0)
    {
        freeaddrinfo(res);
        return false;
    }

    //connect to the resolved address otherwise
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0){
        close(sockfd);
        sockfd = -1;
        freeaddrinfo(res);
        return false;
    }

    //freeing address info
    freeaddrinfo(res);
    return true;
}

// sends HTTP request and collects the full response
bool HTTP::sendRequest(const string& req, string& resp)
{
    // if unable to connect
    if (!connectServer())
        return false;

    // prepares to have receive the request payload
    size_t sent = 0;
    size_t total = req.size();
    while (sent < total) {
        int n = send(sockfd, req.data() + sent, total - sent, 0);
        if (n <= 0)
            return false;
        sent += n;
    }

    char buffer[4096];
    resp.clear();
    int n;
    // read until server closes connection
    while ((n = recv(sockfd, buffer, 4096, 0)) > 0)
        resp.append(buffer, n);

    //closes socket after response fully read
    close(sockfd);
    sockfd = -1;
    return true;
}

// builds cookie header out of the stored cookie (yum)
string HTTP::buildCookieHeader() const
{
    // if no cookies. during these tests there s only one but oh well
    if (cookies.empty())
        return "";

    ostringstream hdr;
    hdr << "Cookie: ";
    /* for to join all cookies; after i reread the task in hand, 
    i realized this for is redundant, but the code works so i didnt
    bother changing it*/
    for (size_t i = 0; i < cookies.size(); i++)
        hdr << cookies[i];
    //ends header line and returns header
    hdr << "\r\n";
    return hdr.str();
}

// builds JWT access header
string HTTP::buildAuthHeader() const
{
    if (jwt.empty()) // if jwt empty
        return "";
    return "Authorization: Bearer " + jwt + "\r\n";
}

// parses status code
int HTTP::parseStatusCode(const string& response) const
{
    // finds first space after http version
    size_t pos = response.find(' ');
    if (pos == string::npos)//if response wrong
        return -1;
    //extracts status code and converts to int
    return stoi(response.substr(pos + 1, 3));
}

// extrats body from http headers
string HTTP::extractBody(const string& response) const
{
    //separates header from body
    size_t pos = response.find("\r\n\r\n");
    if (pos == string::npos)
        return "";
    return response.substr(pos + 4);
}

// collects and stores cookies
void HTTP::storeSetCookie(const string& response)
{
    const string prefix = "Set-Cookie: ";
    // clear old cookie as we only use one(the current) for this session
    cookies.clear();

    // find header
    size_t pos = response.find(prefix);
    if (pos == string::npos)
        return;  // no cookie to store

    // locate end of header line
    size_t eol = response.find("\r\n", pos);
    size_t start = pos + prefix.size();

    // extract "name=value" portion
    string cookieLine = response.substr(start, eol - start);
    auto semi = cookieLine.find(';');
    if (semi != string::npos)
        cookieLine.resize(semi);

    // store the cookie
    cookies.push_back(cookieLine);
}

// functin for login_admin
bool HTTP::loginAdmin(const string& user, const string& pass)
{
    //builds json body with credentials
    json body = {{"username", user}, {"password", pass}};
    string payload = body.dump();

    //builds the http post request for admin login
    ostringstream req;
    req << "POST /api/v1/tema/admin/login HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << "Content-Type: application/json\r\n"
        << "Content-Length: " << payload.size() << "\r\n"
        << "Connection: close\r\n\r\n" << payload;

    string resp;
    //sends request and collects response; of course, if req invalid, returns false
    if (!sendRequest(req.str(), resp))
        return false;

    //if status code not 200
    if (parseStatusCode(resp) != 200)
        return false;

    //extract and store cookie header
    storeSetCookie(resp);
    return true;
}

// function for add_user
bool HTTP::addUser(const string& user, const string& pass)
{
    //same implementation as the previous when it comes to building json and http post
    json body = {{"username", user}, {"password", pass}};
    string payload = body.dump();

    ostringstream req;
    req << "POST /api/v1/tema/admin/users HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << buildCookieHeader()
        << "Content-Type: application/json\r\n"
        << "Content-Length: " << payload.size() << "\r\n"
        << "Connection: close\r\n\r\n" << payload;

    string resp;
    if (!sendRequest(req.str(), resp))
        return false;

    int code = parseStatusCode(resp);
    // if code is valid
    return (code >= 200 && code < 300);
}

// function for get_users
bool HTTP::getUsers()
{
    //builds http get request 
    ostringstream req;
    req << "GET /api/v1/tema/admin/users HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << buildCookieHeader() << "Connection: close\r\n\r\n";

    string resp;
    if (!sendRequest(req.str(), resp))
        return false;

    //if status not OK
    if (parseStatusCode(resp) != 200)
        return false;

    //extract json body
    auto body = extractBody(resp);
    json j = json::parse(body);

    //prints out user entries
    cout << "SUCCESS: Lista utilizatorilor\n";
    for (auto& u : j["users"]) {
        string uname = u["username"].get<string>();
        string pass  = u["password"].get<string>();
        cout << "#" << u["id"] << " " << uname << ":" << pass << "\n";
    }

    return true;
}

//function for delete_user
bool HTTP::deleteUser(const string& username)
{
    //builds http req to delete specified user
    ostringstream req;
    req << "DELETE /api/v1/tema/admin/users/" << username << " HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << buildCookieHeader() << "Connection: close\r\n\r\n";

    string resp;
    if (!sendRequest(req.str(), resp))
        return false;

    // returns true only if http 200 ok
    return parseStatusCode(resp) == 200;
}

//function for logout admins
bool HTTP::logoutAdmin()
{
    //builds http request for admin logout
    ostringstream req;
    req << "GET /api/v1/tema/admin/logout HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << buildCookieHeader() << "Connection: close\r\n\r\n";

    string resp;
    if (!sendRequest(req.str(), resp))
        return false;

    if (parseStatusCode(resp) != 200)
        return false;

    //clears stored session data
    cookies.clear();
    jwt.clear();
    return true;
}

// function for login_user
bool HTTP::loginUser(const string& adminUser,const string& user,const string& pass) {
    //same deal as the login admin
    json body = {{"admin_username", adminUser}, {"username", user}, {"password", pass}};
    string payload = body.dump();

    ostringstream req;
    req << "POST /api/v1/tema/user/login HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << "Content-Type: application/json\r\n"
        << "Content-Length: " << payload.size() << "\r\n"
        << "Connection: close\r\n\r\n"
        << payload;

    string resp;
    if (!sendRequest(req.str(), resp))
        return false;

    // if HTTP status not valid
    if (parseStatusCode(resp) < 200 || parseStatusCode(resp) >= 300)
        return false;

    storeSetCookie(resp);
    return true;
}

// function for logout_user
bool HTTP::logoutUser()
{
    //builds http request to logout the user
    ostringstream req;
    req << "GET /api/v1/tema/user/logout HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << buildCookieHeader()
        << "Connection: close\r\n\r\n";

    string resp;
    if (!sendRequest(req.str(), resp))
        return false;

    if (parseStatusCode(resp) != 200)
        return false;

    cookies.clear();
    jwt.clear();
    return true;
}

// function for get_access
bool HTTP::getAccess()
{
    //builds http request for access endpoint
    ostringstream req;
    req << "GET /api/v1/tema/library/access HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << buildCookieHeader() << "Connection: close\r\n\r\n";

    string resp;
    if (!sendRequest(req.str(), resp))
        return false;

    // on code 200 extracts json body and token
    if (parseStatusCode(resp) == 200)
    {
        auto body = extractBody(resp);
        json j = json::parse(body);
        jwt = j["token"].get<string>(); //stores the received jwt and returns true
        return true;
    }

    return false;
}

// function that gives out mofie list(get_movies)
bool HTTP::getMovies()
{
    ostringstream req;
    req << "GET /api/v1/tema/library/movies HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << buildAuthHeader() << "Connection: close\r\n\r\n";

    string resp;
    if (!sendRequest(req.str(), resp))
        return false;

    if (parseStatusCode(resp) != 200)
        return false;

    //extracts json body and parses it
    auto body = extractBody(resp);
    auto root = json::parse(body);

    json arr;
    //handles both if there are movies added, or if there are none
    if (root.is_object() && root.contains("movies") && root["movies"].is_array()) {
        arr = root["movies"];
    } else if (root.is_array()) {
        arr = root;
    } else {
        return false;
    }

    cout << "SUCCESS: Lista filmelor\n";
    for (auto& m : arr)
        cout << "#" << m["id"] << " " << m["title"] << "\n";
    return true;
}

// gives out details about movie using ID
bool HTTP::getMovie(int movieId)
{
    ostringstream req;
    req << "GET /api/v1/tema/library/movies/" << movieId << " HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << buildAuthHeader()
        << "Connection: close\r\n\r\n";

    string resp;
    if (!sendRequest(req.str(), resp))
        return false;

    if (parseStatusCode(resp) != 200)
        return false;

    auto body = extractBody(resp);
    json m = json::parse(body);

    cout << "SUCCESS: Detalii film\n";
    cout << "title: " << m["title"].get<string>() << "\n" << "year: " << m["year"] << "\n" << "description: " << m["description"].get<string>()  << "\n";
    string rating_str;
         if (m["rating"].is_string()) {
             rating_str = m["rating"].get<string>();
         } else {
             rating_str = m["rating"].dump();
         }
         cout << "rating: " << rating_str << "\n";
    return true;
}

// adds new movie
bool HTTP::addMovie(const string& title, int year, const string& description, double rating)
{
    //checks if we have access to jwt token first
    if (!getAccess())
        return false;

    // builds json payload for movie details
    json body = {{"title",title}, {"year",year}, {"description", description}, {"rating",rating}};

    auto payload = body.dump();

    ostringstream req;
    req << "POST /api/v1/tema/library/movies HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << buildAuthHeader()
        << "Content-Type: application/json\r\n"
        << "Content-Length: " << payload.size() << "\r\n"
        << "Connection: close\r\n\r\n" << payload;

    string resp;
    if (!sendRequest(req.str(), resp))
        return false;
    int code = parseStatusCode(resp);
    return (code >= 200 && code < 300);
}

bool HTTP::updateMovie(int movieId, const string& title, int year, const string& description, double rating)
{
    //same deal as addMovie, except it asks for the movie id first to fint it 
    if (!hasToken()) {
        return false;
    }

    json body = {{"title", title}, {"year", year}, {"description", description}, {"rating",rating}};
    string payload = body.dump();

    ostringstream req;
    req << "PUT /api/v1/tema/library/movies/" << movieId << " HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << buildAuthHeader()
        << "Content-Type: application/json\r\n"
        << "Content-Length: " << payload.size() << "\r\n"
        << "Connection: close\r\n\r\n"
        << payload;

    string resp;
    if (!sendRequest(req.str(), resp))
        return false;

    int code = parseStatusCode(resp);
    return (code >= 200 && code < 300);
}

bool HTTP::deleteMovie(int movieId)
{
    //builds http request to delete movie from specified id
    ostringstream req;
    req << "DELETE /api/v1/tema/library/movies/" << movieId << " HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << buildAuthHeader()
        << "Connection: close\r\n\r\n";

    string resp;
    if (!sendRequest(req.str(), resp))
        return false;

    return parseStatusCode(resp) == 200;
}

