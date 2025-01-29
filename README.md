# XRCore
XRCore is a multiplayer XR interaction system designed for Unreal Engine, featuring a player pawn with hands that support both hand tracking and controllers.
 
## Overview

[Demo Video](https://vimeo.com/1002763137)



## Pawn & Hands
The foundation of XRCore’s player system, including the XRCore Pawn and XRHand components, which enable movement, interaction, and hand-based controls.

### XRCore Pawn
Maintains and owns the players XRHands. Supports optional snap turn and teleportation locomotion.
### XRCore Hand
Handles the player's hands using OpenXR controller data. Supports both tracking hand and motion controllers.

#### Unreals Enhanced Input

#### OpenXR Handtracking


### XRCore GameMode
Manages the overall multiplayer experience, ensuring seamless player spawning.
#### PlayerSpawnPoint
A networked spawn system that places players in predefined locations using the XRCore Game Mode.


## Interaction System
A robust, networked system for handling XR interactions, including grabbing, triggering, and custom interaction mechanics. 
### XRInteractorComponent
A component responsible for detecting and interacting with objects in the XR world, owned by the Pawn to initiate serverside logic.
#### Replication
An overview of how interactions are correctly synchronized across all players in a multiplayer session including late-joining and high latency/package loss scenarios.


### XRInteractionComponent
Base class for interactive objects that can be used together with the XRInteractorComponent.
#### Grab Interaction
Allows players to pick up, hold, and manipulate objects using their hands or controllers. Can be physics or kinematics based (multi hand grab only available with physics system).
#### Trigger Interaction
Handles button presses, switches, and other trigger-based mechanics.
#### Creating custom Interactions
Provides a how-to guide for developers to create their own interactions.

### Connection System
A modular and fully replicated system for dynamically connecting and attaching actors at runtime.
#### XRConnectorComponent
A component that allows objects to establish and manage networked connections.
#### XRConnectorSocket
Defines attachment points for objects.
#### Holograms
A visual feedback system used for guiding interactions, providing object placement previews.


## Utilities
A collection of adjacent helper functions and tools to streamline development.
#### XRCoreUtilityFunctions
A set of utility functions to simplify common XR-related tasks and operations.
#### Spatial Annotation
Customizeable UI element that can be attached to actors and tracks the players head location.

#### MetaXR Integration
A separate plugin that provides additional functionality for Meta Quest devices by integrating the UE5 MetaXR plugin.
See https://github.com/V4C38/ue5-xrcore-meta-integration

