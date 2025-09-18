ASSIGNMENT: HTTP web client

The purpose of this assignment is to implement a 
command-line HTTP client in order to acomodate with concepts
such as parsing json bodies and cookies, validatind HTTP 
requests over TCP sockets, managing cookies and JWT tokens, 
and so on.


FILES USED

I. Helper file (helper.h/.cpp)
This file is a small utility file that handles basic 
validation:

* promptCredentials: collects user responses into a string 
vector
* validateNumber: checks if a string is non-empty and can be
 characterized as a SIMPLE number (just digits)
* validateDouble: same deal as the previous function, except
 it checks if the number can be qualified as a double (max a '.')

II. Request file (requests.h/.cpp)
This module is used to get rid of all the ugly functions 
that make our commands work and leaves the cliens relatively 
clean-looking

* connectServer: opens a TCP socket toward the host and port
* sendRequest: sends a HTTP request string and returns the 
response text
* storesetCookie: extracts the Set-Cookie and saves it
* buildCookieHeader: as it says on the tin, builds the cookie
header (this function is made to acomodate multiple 
 cookies, however the server operates with only one 
 cookie, so i ve left it be)
* buildAuthHeader: builds the token header for when its 
requested

* the functions parseStatusCode and parseBody are used to 
return certain parts, such as reading the HTTP status and 
giving out the payload

* API calls functions:
- these are the functions used for the commands themselves,
 and most of them work almost identically:
    * builds the HTTP request;
    * gives gookies and/or JWT header if/when needed
    * serializes JSON;
    * interprets [200, 300] range as successes and responds
     accordingly;
    * all these functions are of type bool so I can execute
     them inside a check if, and if they are true, the 
     correct message is then printed.
- I've chosen nlohmann's json parser because it integrates well with c++, and made serializing responses and requests safe and reliable.

III. Client file (client.cpp)
This module just trims the whitespaces of and starts an if/else chain that checks for required commands, and prints either SUCCESS or ERROR if the bool functions for commands return true or false respectivelly.
