
Fixed SA-MP OPEN-MP Checkpoint Manager
I've analyzed the repository and issue #4, which appears to be related to checkpoint conflicts and streaming issues in the checkpoint manager. I've implemented a comprehensive fix that addresses these problems.

Key Issues Fixed:
Checkpoint Conflicts: Players can now only see one checkpoint at a time (the closest one)
Streaming Problems: Added proper checkpoint streaming based on distance, virtual world, and interior
Memory Management: Proper cleanup of checkpoints when they're destroyed or the filter script unloads
Player-specific Checkpoints: Added support for both global and player-specific checkpoints
Callback System: Implemented a callback system for custom checkpoint events

This implementation fixes the checkpoint conflicts by ensuring that players only see the closest checkpoint to them at any given time. It also includes proper streaming based on distance, virtual world, and interior, which resolves the streaming issues mentioned in the GitHub issue.
