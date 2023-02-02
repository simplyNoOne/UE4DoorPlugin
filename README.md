# UE4 Doors Plugin
A content plugin for interacting with and managing doors behaviour in Unreal Engine projects (this is not a custom editor tool, merely a group of assets and interfaces helping to create and manage doors ready to be used by both AI and players).

Main branch holds only the plugin - without the unreal testing project attached. In order to use the plugin, download the main branch as zip, unpack, and then paste DoorsPlugin folder into Plugins folder in your target project.

There is also testing-project branch, that apart from the plugin, contains also a simple UE4 project used to test plugin functionality.
The Plugin branch consists only of the plugin directory, but may contain not fully developed features.

In order for actors to be able to use the doors, they must implement either playerActionsInterface or AIActionsInterface. In order to have a player interact with doors, DoorsInteracted funcion must be bound to some kind of interaction logic implemented on player character.
There are two example classes included, but custom classes derived from BaseDoors can be created and modified as well.

Example door frame and door mesh are also available. (the assets are not made by me)

Work in progress
