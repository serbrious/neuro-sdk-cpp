#pragma once
#include "include/simplews.hpp"
#include "include/nlohmann/json.hpp"
using json = nlohmann::json;

namespace neuro{

class NeuroSDK {
public:
    NeuroSDK(const std::string &gameName);
    ~NeuroSDK();
    // Send the initial connection to the server (this also calls gameinit())
    bool connect(const std::string &server);

    // Unregister all of our actions and close the connection
    void disconnect();

    // Send a new game to the server
    bool gameinit(); 

    // Send some context concerning whats happening
    // slient if set will allow Neuro to respond to the message otherwise it's slient
    bool sendContext(std::string contextMessage, bool slient=true);

    // Disallow copy and asignment operators
    NeuroSDK(const NeuroSDK&) = delete;
    NeuroSDK& operator=(const NeuroSDK&) = delete;
   
private:
    // Our game name
    std::string gameName;

    // Are we connected?
    bool isConnected;

    // Array of actions that are currently registered
    std::vector<std::string> registeredActions;  // TODO: Replace with an unordered_set for faster lookups.

    // Send a RAW string to the server
    bool send(const std::string &message);

    // Get a RAW string from the server
    bool receive(std::string *output);
   
    // Send a JSON command to the server
    bool sendCommand(const json &command);

    // The websocket connection object we use to talk to the server.
    WebSocket ws;
};

}



