# Neuro Game SDK
This repository contains the API documentation for allowing Neuro-sama to play games.

This is a c++ verion of the offical SDK hosted at [here](https://github.com/VedalAI/neuro-game-sdk)

This is published of the offchance that anyone will find it useful - I'll probably revise it when I use it in anger.

The demo is a version of tic-tac-toe and hopefully there are enough comments to show the basic usage of the SDK.
You are required to implement any thread safety required - though games are usually not highly threaded BUT this still needs to be kept in mind.

> [NOTE]: This is currently only tested under Windows, but there is no reason why it shouldn't work on other platforms.

## Use
The main sdk code is contained in the `NeuroSDK` the example tic-tac-toe game is in the root folder.  You need to use a compiler thats capable and configured for at least c++17.

## API

The API is contained in the `neuro` namespace.

There are two classes of interest; the Action class and the NeuroSDK class.  The designed method of use is to create an instance of the NeuroSDK class, and create derived classes of the Action class to represent specific actions that Neuro-sama can take.

### Action Class

The Action class is an abstract(ish) base class.  On creation (unless you overide it) is to take the action name, this is the name that will be used both in sending to Neuro & as a key for all operations.
  
```cpp
namespace neuro{
    class Action {
        public:
            Action(std::string name, std::string description, json schema = {}): name(name), description(description), jSchema(schema){}
            std::string& GetName() { return name; };
            std::string& GetDescription() { return description; };
            json& GetSchema() { return jSchema; };  // Getter for JSON schema
            void SetName(std::string newName) { name = newName; };
            void SetDescription(std::string newDescription) { description = newDescription; };
            void SetSchema(std::string newSchema) { jSchema = json::parse(newSchema); };  // Setter for JSON schema
            void SetSchema(json newSchema) { jSchema = newSchema; };
            void SetSchemaFromArray( std::string enumName, std::vector<std::string> values);
            // Action state handlers
            virtual std::tuple<bool, std::string> onAction(json data) { return {false, "Action not implemented"};  };
            virtual void onRegister() {};
            virtual void onUnregister() {};
            json toJSON();
            operator json() { return toJSON(); };  // Allows for implicit conversion to json
    };
}
```  

Main functions:  

`Action(std::string name, std::string description, json schema = {}): name(name), description(description), jSchema(schema)`  
Params:
- `name`: The name of the action, this is used as a key in all operations.
- `description`: A description of what the action does.
- `schema`: A JSON schema that describes the parameters required for the action.  This is optional - and will be overriden by the SetSchema, SetSchemaFromArray.

onAction, this is called by the underlying SDK when the action is triggered.  The data passed in will be from the schema(if provided) in the action class.

`std::tuple<bool, std::string> onAction(json data)`  
Params:  
- `data`: The data passed in from the action trigger.  This will be validated against the schema if provided.  

Returns:  
- `std::tuple<bool, std::string>`: A tuple where the first element is a boolean indicating success or failure of the action, and the second element is a string containing any error message.

The Action class also has a few other methods that you can override to customize the behavior of your action.  These include:  
- `onRegister()` called when the action is registered with Neuro.
- `onUnregister()` called when the action is unregistered with Neuro.

## MainSDK class

```cpp
namespace neuro{
    class NeuroSDK {
    public:
        NeuroSDK(const std::string &gameName);
        ~NeuroSDK();
        bool connect(const std::string &server);
        void disconnect();
        bool gameinit(); 
        bool registerAction(Action *action);
        void unregisterAction(std::string actionName);
        void unregisterActions( std::vector< std::string > actions );
        void unregisterAllActions();
        bool sendContext(std::string contextMessage, bool slient=true);
        bool forceAction( std::string gameState, std::string whatToDo, std::vector<std::string> listOfActions );
    }
}
```

Functions of interest:  
`NeuroSDK(const std::string &gameName)`  
Constructor for the NeuroSDK class.
Params:  
- `gameName`: The name of the game that this SDK instance is associated with - this is passed directly to Neuro.

`bool registerAction(Action *action)`   
Registers an action with Neuro.
Params:  
- `action`: A pointer to an Action object that you want to register with Neuro.

Returns:  
- `bool`: True if the action was successfully registered, false otherwise.

`void unregisterAction(std::string actionName)`    
Unregisters an action from Neuro by its name.
Params:
- `actionName`: The name of the action you want to unregister.

Returns:  
- `void`: No return value.


`void unregisterAllActions()`   
Unregisters all actions from Neuro and removes them locally.
Params:
- None

Returns:
- None

`bool sendContext(std::string contextMessage, bool silent=true)`    
Sends a context message to Neuro.  If `silent` is set to true, Neuro will not respond to the message.
Params:
- 'contextMessage': A string containing the context message you want to send to Neuro. 
- 'silent': A boolean indicating whether or not Neuro should respond to the message.  Defaults to true.

Returns:
- `bool`: True if the context message was successfully sent, false otherwise.


`bool forceAction(std::string gameState, std::string whatToDo, std::vector<std::string> listOfActions)`    
Forces a decision from Neuro based on the list of registered actions.
Params:
- `gameState`: A string containing the current game state.
- `whatToDo`: A string describing what you want Neuro to do.
- `listOfActions`: A vector of strings containing the names of the actions that Neuro should consider when making a decision.

Returns:
- `bool`: True if the action was successfully forced, false otherwise.






    






