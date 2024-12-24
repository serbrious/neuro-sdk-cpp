#pragma once
#include "include/simplews.hpp"
#include "include/nlohmann/json.hpp"
using json = nlohmann::json;
#include <thread>

namespace neuro{

// Forward def
class NeuroSDK;

class Action {
    public:
        Action(std::string name, std::string description, std::string schema): name(name), description(description){
            jSchema = json::parse(schema);
        };
        Action(std::string name, std::string description, json schema): name(name), description(description), jSchema(schema){}
        virtual ~Action() {};

        // Not normally a fan of getters and setters in c++ but we will need a modicum of thread safety
        std::string& GetName() { return name; };
        std::string& GetDescription() { return description; };
        json& GetSchema() { return jSchema; };  // Getter for JSON schema
        void SetName(std::string newName) { name = newName; };
        void SetDescription(std::string newDescription) { description = newDescription; };
        void SetSchema(std::string newSchema) { jSchema = json::parse(newSchema); };  // Setter for JSON schema
        void SetSchema(json newSchema) { jSchema = newSchema; };

        // Creates a schema(aka list of stuff to do) from provided list of options
        // This seems to be the generic case
        void SetSchemaFromArray( std::string enumName, std::vector<std::string> values);

        // Action state handlers
        // Called when an action is received from the Neuro
        // Return is success + a message to return
        virtual std::tuple<bool, std::string> onAction(json data);
        virtual void onRegister() {};  // Called when the action is registered with the server
        virtual void onUnregister() {};  // Called when the action is unregistered with the server
//        virtual void onError(const std::string &error) {};  // Called when an error occurs with the action

        // Convert this Action object into JSON format for sending over the network
        json toJSON();

        // operator json, return the JSON representation of this Action object
        operator json() { return toJSON(); };  // Allows for implicit conversion to json

    protected:
        friend class NeuroSDK;
        std::string name;
        std::string description;
        json jSchema;
};

class NeuroSDK {
public:
    NeuroSDK(const std::string &gameName);
    ~NeuroSDK();
    // Send the initial connection to the server (this also calls gameinit()) + start receive loop
    bool connect(const std::string &server);

    // Unregister all of our actions and close the connection + stop our receive loop
    void disconnect();

    // Send a new game to the server
    bool gameinit(); 

    // Register an action with Neuro
    bool registerAction(Action *action);

    // Unregister an action from Neuro 
    void unregisterAction(std::string actionName);

    // remove an array of action strings
    void unregisterActions( std::vector< std::string > actions );

    // Remove all actions from the server + unregister them locally
    void unregisterAllActions();

    // Send some context concerning whats happening
    // slient if set will allow Neuro to respond to the message otherwise it's slient
    bool sendContext(std::string contextMessage, bool slient=true);

    // Force a decsion from Neuro based on the list of registered actions
    // gamestate is what is currently happening, e.g. "the game is still under way"
    // whatToDo is what we want Neuro to do, e.g. "Its your turn, please make a move"
    bool forceAction( std::string gameState, std::string whatToDo, std::vector<std::string> listOfActions );

    // Disallow copy and asignment operators
    NeuroSDK(const NeuroSDK&) = delete;
    NeuroSDK& operator=(const NeuroSDK&) = delete;
   
private:
    // Our game name
    std::string gameName;

    // Are we connected?
    bool isConnected;

    // Array of actions that are currently registered
    std::vector<Action*> registeredActions;  // TODO: Replace with an unordered_set for faster lookups.

    // Send a RAW string to the server
    bool send(const std::string &message);

    // Get a RAW string from the server
    bool receive(std::string *output);
   
    // Send a JSON command to the server
    bool sendCommand(const json &command);

    // Action management
    // Removes an action, returns true if an action is removed.  Returns false otherwise.
    bool removeAction( const Action &action );

    void receiveLoop();

    std::thread *receiveThread;
    std::atomic_bool stop = false;

    // The websocket connection object we use to talk to the server.
    WebSocket ws;
};

}



