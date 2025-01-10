

---

# Checkpoint Manager for SA-MP OpenMP

## Overview

The **Checkpoint Manager** is a library for managing checkpoints in **San Andreas Multiplayer (SA-MP)** using the **OpenMP** framework. It allows server administrators to create, manage, and remove checkpoints, as well as manage their properties such as visibility, ownership, and interaction with players. This system is designed for both performance and flexibility, ensuring seamless integration into custom SA-MP server projects.

## Features

- **Create Checkpoints**: Easily create checkpoints with customizable positions, sizes, and ownership.
- **Dynamic Checkpoint Management**: Modify checkpoints' interior, virtual world, and activity status.
- **Automatic Checkpoint Visibility**: Checkpoints are dynamically shown to players based on their location.
- **Seeker System**: A periodic system that checks for players' proximity to checkpoints and updates their visibility.
- **Flexible Ownership**: Checkpoints can be linked to specific players or remain public.
- **Interior and Virtual World Support**: Allows for creating checkpoints inside interiors or specific virtual worlds.
- **Event Handling**: Includes hooks for when players enter or exit checkpoints.

## Installation

1. **Clone the Repository**:
   Clone the repository into your SA-MP server's `includes` directory:

   ```bash
   git clone https://github.com/Kingvornex/ACNR-OPENMP.git
   ```

2. **Include the Checkpoint Manager**:
   In your `gamemode` script, add the following line to include the checkpoint manager:

   ```pawn
   #include <CheckpointManager>
   ```

3. **Compile the Script**:
   Compile your gamemode as usual with the `CheckpointManager` included.

## API Functions

The **Checkpoint Manager** provides several functions for managing checkpoints. Below are the key functions available:

### `CreateCheckpoint(ownerid, chpid, Float:posX, Float:posY, Float:posZ, Float:size)`
Creates a new checkpoint at the specified position with a given size.

- **Parameters**:
  - `ownerid`: The ID of the player who owns the checkpoint (-1 for public checkpoints).
  - `chpid`: The unique ID for the checkpoint.
  - `posX`, `posY`, `posZ`: The 3D coordinates of the checkpoint.
  - `size`: The size of the checkpoint (radius).
  
- **Returns**: `1` if the checkpoint is created successfully, `0` if the maximum number of checkpoints is reached.

### `SetCheckpointInterior(chpid, interiorid)`
Changes the interior ID for a specific checkpoint.

- **Parameters**:
  - `chpid`: The checkpoint ID.
  - `interiorid`: The new interior ID.

- **Returns**: `1` if successful, `0` if the checkpoint is not found.

### `SetCheckpointVirtualWorld(chpid, virtualWorldID)`
Changes the virtual world ID for a specific checkpoint.

- **Parameters**:
  - `chpid`: The checkpoint ID.
  - `virtualWorldID`: The new virtual world ID.

- **Returns**: `1` if successful, `0` if the checkpoint is not found.

### `ToggleCheckpointActive(chpid, bool:active)`
Toggles the activity state of a checkpoint (active/inactive).

- **Parameters**:
  - `chpid`: The checkpoint ID.
  - `active`: `true` to activate, `false` to deactivate.

- **Returns**: `1` if successful, `0` if the checkpoint is not found.

### `ChangeCheckpointOwner(chpid, owner)`
Changes the owner of a checkpoint.

- **Parameters**:
  - `chpid`: The checkpoint ID.
  - `owner`: The new owner ID.

- **Returns**: `1` if successful, `0` if the checkpoint is not found.

### `RemoveCheckpoint(chpid)`
Removes a specific checkpoint from the system.

- **Parameters**:
  - `chpid`: The checkpoint ID.

- **Returns**: `1` if successful, `0` if the checkpoint is not found.

### `StartCheckpointSeeking()`
Starts the system that checks for players near checkpoints and updates their visibility.

- **Returns**: `1` if the system starts successfully.

### `StopCheckpointSeeking()`
Stops the system that checks for players near checkpoints.

- **Returns**: `1` if the system stops successfully.

### `VerifyCheckpoint(playerid)`
Verifies if a player is near a checkpoint and triggers the `OnCheckpointEnter` event.

- **Parameters**:
  - `playerid`: The player ID.

- **Returns**: `1` if the player enters a checkpoint, `0` otherwise.

## Events

### `OnCheckpointEnter(playerid, checkpointid)`
Triggered when a player enters a checkpoint.

- **Parameters**:
  - `playerid`: The player ID.
  - `checkpointid`: The ID of the checkpoint.

### `OnCheckpointExit(playerid, checkpointid)`
Triggered when a player exits a checkpoint.

- **Parameters**:
  - `playerid`: The player ID.
  - `checkpointid`: The ID of the checkpoint.

## Configuration

You can configure the maximum number of checkpoints and the delay between the periodic seeker checks by modifying the following constants:

```pawn
#define MAX_CHECKPOINTS 300            // Maximum number of checkpoints allowed
#define CHECKPOINT_SEEKER_DELAY 300    // Delay in milliseconds for the checkpoint seeker
#define GLOBAL_OWNER_ID -1             // Global owner ID for public checkpoints
```

## Dependencies

- **SA-MP 0.3.x+**: This library is designed for use with the SA-MP server.
- **OpenMP**: The library is built with OpenMP, so you need to have it installed on your server.

## Contributing

Feel free to fork this repository, make modifications, and submit pull requests. If you have any suggestions or improvements, please open an issue or submit a patch to the repository.

## License

This project is licensed under the terms of the **GPLv3 License**. See the [LICENSE](LICENSE) file for more information.

---

This README provides detailed information about the project, installation, usage, and contribution guidelines. You can adjust the sections based on your specific needs.
