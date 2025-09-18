#include <iostream>
#include <string>
#include <vector>
#include "helper.h"
#include "requests.h"

using namespace std;

int main() {
    // initialize HTTP client with given host and port
    HTTP client("63.32.125.183", 8081);
    string line;

    while (getline(cin, line)) {
        // while for removing whitespaces
        while (!line.empty() && (line.back() == '\r' || isspace(line.back())))
            line.pop_back();

        if (line == "login_admin") {
            // gives necessary credentials
            auto creds = promptCredentials({"username", "password"});
            // success bool that checks if the admin logged in correctly
			bool success = client.loginAdmin(creds[0], creds[1]);
            if (success) {
                cout << "SUCCESS: Admin autentificat cu succes\n";
            } else {
                cout << "ERROR: Autentificare eșuată\n";
            }
        } else if (line == "add_user") {
			auto creds = promptCredentials({"username", "password"});
            bool success = client.addUser(creds[0], creds[1]);
            if (success) {
                cout << "SUCCESS: Utilizator adăugat cu succes\n";
            } else {
                cout << "ERROR: Nu s-a putut adăuga utilizatorul\n";
            }
        } else if (line == "get_users") {
            // bool to get users. if it fails, a error is sent
            bool success = client.getUsers();
            if (!success) {
                cout << "ERROR: Nu s-au putut încărca utilizatorii\n";
            }
        } else if (line == "delete_user") {
            // gives necessary credentials, only the username here
            auto creds = promptCredentials({"username"});
            bool success = client.deleteUser(creds[0]);
            if (success) {
                cout << "SUCCESS: Utilizator șters\n";
            } else {
                cout << "ERROR: Nu s-a putut șterge utilizatorul\n";
            }
        } else if (line == "logout_admin") {
            // bool to see if admin logs in
            bool success = client.logoutAdmin();
            if (success) {
                cout << "SUCCESS: Admin delogat\n";
            } else {
                cout << "ERROR: Nu s-a putut deloga admin\n";
            }
        } else if (line == "login") {
            // gives necessary credentials
            auto creds = promptCredentials({"admin_username", "username", "password"});
            bool success = client.loginUser(creds[0], creds[1], creds[2]);
            if (success) {
                cout << "SUCCESS: Autentificare reușită\n";
            } else {
                cout << "ERROR: Autentificare eșuată\n";
            }
        } else if (line == "logout") {
            // log out the normal user
            bool success = client.logoutUser();
            if (success) {
                cout << "SUCCESS: Utilizator delogat\n";
            } else {
                cout << "ERROR: Nu s-a putut deloga utilizatorul\n";
            }
        } else if (line == "get_access") {
            // regquests access token
            bool success = client.getAccess();
            if (success) {
                cout << "SUCCESS: Token JWT primit\n";
            } else {
                cout << "ERROR: Obținere acces eșuată\n";
            }
        } else if (line == "get_movies") {
            // gives all movies
            bool success = client.getMovies();
            if (!success) {
                cout << "ERROR: Nu s-au putut încărca filmele\n";
            }
        } else if (line == "get_movie") {
            // gives necessary credentials, only the id
            auto creds = promptCredentials({"id"});
            if (!validateNumber(creds[0])) {
                cout << "ERROR: ID invalid\n";
            } else {
                bool success = client.getMovie(stoi(creds[0]));
                if (!success) {
                    cout << "ERROR: Nu s-au putut obține detaliile filmului\n";
                }
            }
        }else if (line == "add_movie") {
            // gives necessary credenmtials
            auto creds = promptCredentials({"title", "year", "description", "rating"});
            if (!validateNumber(creds[1]) || !validateDouble(creds[3])) {
                cout << "ERROR: Nu s-a putut adăuga filmul\n";
            } else {
                bool success = client.addMovie(creds[0], stoi(creds[1]), creds[2], stod(creds[3]));
                if (success) {
                    cout << "SUCCESS: Film adăugat\n";
                } else {
                    cout << "ERROR: Nu s-a putut adăuga filmul\n";
                }
            }
        } else if (line == "update_movie") {
            // gives necessary credentials
            auto creds = promptCredentials({"id", "title", "year", "description", "rating"});
            if (!validateNumber(creds[0]) || !validateNumber(creds[2]) || !validateDouble(creds[4])) {
                cout << "ERROR: Nu s-au putut actualiza datele filmului\n";
            } else {
                bool success = client.updateMovie( stoi(creds[0]), creds[1], stoi(creds[2]), creds[3], stod(creds[4]));
                if (success) {
                    cout << "SUCCESS: Film actualizat\n";
                } else {
                    cout << "ERROR: Nu s-au putut actualiza datele filmului\n";
                }
            }
        } else if (line == "delete_movie") {
            // gives necessary credenmtials
            auto creds = promptCredentials({"id"});
            if (!validateNumber(creds[0])) {
                cout << "ERROR: ID invalid\n";
            } else {
                bool success = client.deleteMovie(stoi(creds[0]));
                if (success) {
                    cout << "SUCCESS: Film șters cu succes\n";
                } else {
                    cout << "ERROR: Nu s-a putut șterge filmul\n";
                }
            }
        } else if (line == "exit") {
            break;
        } else { // if command s unknown
            cout << "ERROR: Comandă necunoscută\n";
        }
    }

    return 0;
}
