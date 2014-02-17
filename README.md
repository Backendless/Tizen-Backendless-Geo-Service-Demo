Tizen and Backendless Integration Example
=========================================
This example demonstrates the usage of the Backendless REST API to persist device's GPS coordinates from a Tizen app in the Backendless Geo location storage.

A video of the demo is available at: http://www.youtube.com/watch?v=WfX_ALW7Vfs

Before you run the app, make sure to login to your Backendless backend with the Developer console, select the app and click the Manage icon. Copy the Application ID and REST Secret Key from the console to src\Backendless.cpp.  The following two lines in the source code must be updated:

`const static wchar_t* BACKENDLESS_APPLICATION_ID = L"****** REPLACE THIS WITH YOUR APP ID ***********";`
`const static wchar_t* BACKENDLESS_SECRET_KEY = L"****** REPLACE THIS WITH YOUR REST SECREY KEY ***********";`
