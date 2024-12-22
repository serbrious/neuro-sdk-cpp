#include "include/nlohmann/json.hpp"
#include "neuro-sdk.hpp" 

using json = nlohmann::json;

namespace neuro{
    // Some basic con/de-structors
    NeuroSDK::NeuroSDK(const std::string &gameName) : isConnected(false), gameName(gameName), ws() {}

    // Be a good citizen and clean up after ourselves.
    NeuroSDK::~NeuroSDK() {
        if(isConnected) {
            disconnect();
        }
    }   

    // Connect to the server. Return false if we can't connect.
    bool NeuroSDK::connect(const std::string& url) {
        if (!ws.connect(url)) {
            return false;
        }
        isConnected = true;
        return true;
    }

    // ***********************************************************************************
    // Wrappers, functions that wrap API commands
    // ***********************************************************************************

    // Send a game initialization message to the server
    bool NeuroSDK::gameinit() {
        json initMessage = {
            {"command", "startup"},
            {"game", gameName}
        };
        return sendCommand(initMessage);
    }

    bool NeuroSDK::sendContext(std::string contextMessage, bool silent){
        json contextMessageJson = {
            {"command", "context"},
            {"game", gameName},
            {"data",{
                        {"message", contextMessage},
                        {"silent", silent}
                } }
        };
        return sendCommand(contextMessageJson);   
    }

    bool NeuroSDK::registeredAction(std::string actionName, std::string description, std::vector<std::string> parameters) {
        Action action(actionName, description, ""); 

        json contextMessageJson = {
            {"name", actionName},
            {"description", description},
            {"schema",{
                } }
        };
        return sendCommand(contextMessageJson);
    }

    void NeuroSDK::unregisterAction(std::string actionName) {
        json messageJson = {
            { "command", "actions/unregister" },
            { "game", gameName },
            {"description", {actionName} },
        };
        sendCommand(messageJson);
    }

    void NeuroSDK::unregisterAllActions() {
        std::vector< std::string > actionArray;
        actionArray.push_back("action1");

        // Implementation for unregistering all actions
        json messageJson = {
            { "command", "actions/unregister" },
            { "game", gameName },
            { "description", actionArray },
        };
        sendCommand(messageJson);
    }

    // ***********************************************************************************
    // Internal functions
    // ***********************************************************************************

    // Basic RAW send function, will be replaced with task specific ones
    bool NeuroSDK::send(const std::string& message) {
        if(!isConnected) { 
            std::cerr << "Not connected to the server." << std::endl; 
            return false;
        }
        return ws.send(message);
    }

    bool NeuroSDK::sendCommand(const json &command) {
        try {
            std::string cmdStr = command.dump();
            ws.send(cmdStr);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error sending command: " << e.what() << std::endl;
            return false;
        }
    }

    bool NeuroSDK::receive(std::string* output) {
        if(!isConnected) { 
            std::cerr << "Not connected to the server." << std::endl; 
            return false;
        }   
        ws.receive(output);
        return output->length() > 0; // Return true if received data is not empty
    }

    void NeuroSDK::disconnect() {
        if(isConnected) { 
            ws.close(); 
            isConnected = false; 
        }
        std::cout << "Disconnected from the server." << std::endl;
    }   
}
