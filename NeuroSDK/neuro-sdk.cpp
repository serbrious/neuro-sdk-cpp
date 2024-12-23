#include "include/nlohmann/json.hpp"
#include "neuro-sdk.hpp" 

using json = nlohmann::json;

namespace neuro{

    // Actions class
    json Action::toJSON(){
        json j;
        j["name"] = name;
        j["description"] = description;
        j["schema"] = jSchema;
        return j;
    }

    void Action::SetSchemaFromArray( std::string enumName, std::vector<std::string> values){
       jSchema = {{ enumName, {{
			        "enum", values,
		         }}}
	    };
    }

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

    bool NeuroSDK::registerAction(Action action) {
        registeredActions.push_back(action);

        // We need these as an array for the server to process.
        std::vector< json > actionArray;
        actionArray.push_back(action.toJSON());

        json contextMessageJson = {
                { "command", "actions/register" },
                {"game", gameName},
                {"data", {{
                    "actions", actionArray,
                }}}
            };
        return sendCommand(contextMessageJson);
    }

    void NeuroSDK::unregisterAction(std::string actionName) {
        std::vector< std::string > actionArray;
        actionArray.push_back(actionName);
        json messageJson = {
            { "command", "actions/unregister" },
            { "game", gameName },
            { "data", {{"action_names", actionArray } }},
        };
        sendCommand(messageJson);

        // Remove the action from the local list of registered actions
        removeAction(Action(actionName,"",""));
    }

    void NeuroSDK::unregisterAllActions() {
        std::vector< std::string > actionArray;

        for(const Action& action : registeredActions) 
            actionArray.push_back(action.name);

        // Implementation for unregistering all actions
        json messageJson = {
            { "command", "actions/unregister" },
            { "game", gameName },
            { "description", actionArray },
        };
        sendCommand(messageJson);

        registeredActions.clear(); // Clear the local list of registered actions
    }

    bool NeuroSDK::forceAction( std::string gameState, std::string whatToDo, std::vector<std::string> listOfActions ) {
        json messageJson = {
            { "command", "actions/force" },
            { "game", gameName },
            { "data",  {
                {"state", gameState},
                {"query", whatToDo},
                {"ephemeral_context", false},
                {"action_names", listOfActions},
            }
            }
        };

        return sendCommand(messageJson);  // Send the command to force an action and return the result of the operation
    }


    // ***********************************************************************************
    // Internal functions
    // ***********************************************************************************

    // Action list management

    // Remove an action 
    bool NeuroSDK::removeAction( const Action &action )
    {
        // Walk the registeredActions list and remove the specified action (if exists)
        for(auto it = registeredActions.begin(); it != registeredActions.end(); ++it) {
            if(it->name == action.name) {
                registeredActions.erase(it);
                return true; // Action removed successfully
            }
        }

        return false;
    }

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
            std::cout << cmdStr << std::endl;
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
