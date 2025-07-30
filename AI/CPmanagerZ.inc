#include <a_samp>
#include <zcmd>
#include <sscanf2>
#include <foreach>

#define MAX_CHECKPOINTS 100
#define STREAM_DISTANCE 100.0

enum E_CHECKPOINT
{
    cpId,
    Float:cpX,
    Float:cpY,
    Float:cpZ,
    Float:cpSize,
    cpVirtualWorld,
    cpInterior,
    cpCreated,
    cpCallback[32],
    cpStreamedIn[MAX_PLAYERS char],
    cpForPlayerId,
    cpActivePlayerCheckpoint[MAX_PLAYERS char]
}

new Checkpoints[MAX_CHECKPOINTS][E_CHECKPOINT];
new Iterator:CheckpointsIterator<MAX_CHECKPOINTS>;

forward OnPlayerEnterDynamicCP(playerid, cpId);
forward OnPlayerLeaveDynamicCP(playerid, cpId);

public OnFilterScriptInit()
{
    for (new i = 0; i < MAX_CHECKPOINTS; i++)
    {
        Checkpoints[i][cpCreated] = false;
        Checkpoints[i][cpForPlayerId] = INVALID_PLAYER_ID;
        for (new j = 0; j < MAX_PLAYERS; j++)
        {
            Checkpoints[i][cpStreamedIn{j}] = false;
            Checkpoints[i][cpActivePlayerCheckpoint{j}] = false;
        }
    }
    
    SetTimer("UpdateCheckpointStreaming", 500, true);
    print("Checkpoint Manager loaded.");
    return 1;
}

public OnFilterScriptExit()
{
    for (new i = 0; i < MAX_CHECKPOINTS; i++)
    {
        if (Checkpoints[i][cpCreated])
        {
            if (Checkpoints[i][cpForPlayerId] != INVALID_PLAYER_ID)
            {
                if (IsPlayerConnected(Checkpoints[i][cpForPlayerId]))
                {
                    DisablePlayerCheckpoint(Checkpoints[i][cpForPlayerId]);
                }
            }
            else
            {
                for (new j = 0; j < MAX_PLAYERS; j++)
                {
                    if (IsPlayerConnected(j) && Checkpoints[i][cpStreamedIn{j}])
                    {
                        DisablePlayerCheckpoint(j);
                    }
                }
            }
        }
    }
    
    print("Checkpoint Manager unloaded.");
    return 1;
}

stock CreateDynamicCheckpoint(Float:x, Float:y, Float:z, Float:size, worldid = -1, interiorid = -1, const callback[] = "")
{
    new id = Iter_Free(CheckpointsIterator);
    if (id == -1) return -1;
    
    if (size <= 0.0) return -1;
    
    Checkpoints[id][cpX] = x;
    Checkpoints[id][cpY] = y;
    Checkpoints[id][cpZ] = z;
    Checkpoints[id][cpSize] = size;
    Checkpoints[id][cpVirtualWorld] = worldid;
    Checkpoints[id][cpInterior] = interiorid;
    Checkpoints[id][cpCreated] = true;
    Checkpoints[id][cpForPlayerId] = INVALID_PLAYER_ID;
    
    if (strlen(callback) > 0)
    {
        strmid(Checkpoints[id][cpCallback], callback, 0, strlen(callback), 32);
    }
    else
    {
        Checkpoints[id][cpCallback][0] = '\0';
    }
    
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        Checkpoints[id][cpStreamedIn{i}] = false;
        Checkpoints[id][cpActivePlayerCheckpoint{i}] = false;
    }
    
    Iter_Add(CheckpointsIterator, id);
    UpdateCheckpointStreaming();
    
    return id;
}

stock CreatePlayerDynamicCheckpoint(playerid, Float:x, Float:y, Float:z, Float:size, worldid = -1, interiorid = -1, const callback[] = "")
{
    if (!IsPlayerConnected(playerid)) return -1;
    
    new id = Iter_Free(CheckpointsIterator);
    if (id == -1) return -1;
    
    if (size <= 0.0) return -1;
    
    Checkpoints[id][cpX] = x;
    Checkpoints[id][cpY] = y;
    Checkpoints[id][cpZ] = z;
    Checkpoints[id][cpSize] = size;
    Checkpoints[id][cpVirtualWorld] = worldid;
    Checkpoints[id][cpInterior] = interiorid;
    Checkpoints[id][cpCreated] = true;
    Checkpoints[id][cpForPlayerId] = playerid;
    
    if (strlen(callback) > 0)
    {
        strmid(Checkpoints[id][cpCallback], callback, 0, strlen(callback), 32);
    }
    else
    {
        Checkpoints[id][cpCallback][0] = '\0';
    }
    
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        Checkpoints[id][cpStreamedIn{i}] = false;
        Checkpoints[id][cpActivePlayerCheckpoint{i}] = false;
    }
    
    Iter_Add(CheckpointsIterator, id);
    StreamCheckpointForPlayer(playerid, id);
    
    return id;
}

stock DestroyDynamicCheckpoint(cpId)
{
    if (cpId < 0 || cpId >= MAX_CHECKPOINTS) return 0;
    if (!Checkpoints[cpId][cpCreated]) return 0;
    
    if (Checkpoints[cpId][cpForPlayerId] != INVALID_PLAYER_ID)
    {
        if (IsPlayerConnected(Checkpoints[cpId][cpForPlayerId]) && Checkpoints[cpId][cpStreamedIn{Checkpoints[cpId][cpForPlayerId]}])
        {
            DisablePlayerCheckpoint(Checkpoints[cpId][cpForPlayerId]);
            Checkpoints[cpId][cpActivePlayerCheckpoint{Checkpoints[cpId][cpForPlayerId]}] = false;
        }
    }
    else
    {
        for (new i = 0; i < MAX_PLAYERS; i++)
        {
            if (IsPlayerConnected(i) && Checkpoints[cpId][cpStreamedIn{i}])
            {
                DisablePlayerCheckpoint(i);
                Checkpoints[cpId][cpActivePlayerCheckpoint{i}] = false;
            }
        }
    }
    
    Checkpoints[cpId][cpCreated] = false;
    Iter_Remove(CheckpointsIterator, cpId);
    
    return 1;
}

stock StreamCheckpointForPlayer(playerid, cpId)
{
    if (cpId < 0 || cpId >= MAX_CHECKPOINTS) return 0;
    if (!Checkpoints[cpId][cpCreated]) return 0;
    if (!IsPlayerConnected(playerid)) return 0;
    
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    
    new Float:distance = GetDistanceBetweenPoints(x, y, z, Checkpoints[cpId][cpX], Checkpoints[cpId][cpY], Checkpoints[cpId][cpZ]);
    
    new playerWorld = GetPlayerVirtualWorld(playerid);
    new playerInterior = GetPlayerInterior(playerid);
    
    if ((Checkpoints[cpId][cpVirtualWorld] == -1 || playerWorld == Checkpoints[cpId][cpVirtualWorld]) &&
        (Checkpoints[cpId][cpInterior] == -1 || playerInterior == Checkpoints[cpId][cpInterior]))
    {
        if (distance < STREAM_DISTANCE)
        {
            if (!Checkpoints[cpId][cpStreamedIn{playerid}])
            {
                new bool:hasActiveCheckpoint = false;
                for (new i = 0; i < MAX_CHECKPOINTS; i++)
                {
                    if (Checkpoints[i][cpCreated] && Checkpoints[i][cpActivePlayerCheckpoint{playerid}])
                    {
                        hasActiveCheckpoint = true;
                        break;
                    }
                }
                
                if (hasActiveCheckpoint) return 0;
                
                SetPlayerCheckpoint(playerid, Checkpoints[cpId][cpX], Checkpoints[cpId][cpY], Checkpoints[cpId][cpZ], Checkpoints[cpId][cpSize]);
                Checkpoints[cpId][cpStreamedIn{playerid}] = true;
                Checkpoints[cpId][cpActivePlayerCheckpoint{playerid}] = true;
                return 1;
            }
        }
        else
        {
            if (Checkpoints[cpId][cpStreamedIn{playerid}])
            {
                DisablePlayerCheckpoint(playerid);
                Checkpoints[cpId][cpStreamedIn{playerid}] = false;
                Checkpoints[cpId][cpActivePlayerCheckpoint{playerid}] = false;
                return 1;
            }
        }
    }
    else
    {
        if (Checkpoints[cpId][cpStreamedIn{playerid}])
        {
            DisablePlayerCheckpoint(playerid);
            Checkpoints[cpId][cpStreamedIn{playerid}] = false;
            Checkpoints[cpId][cpActivePlayerCheckpoint{playerid}] = false;
            return 1;
        }
    }
    
    return 0;
}

forward UpdateCheckpointStreaming();
public UpdateCheckpointStreaming()
{
    foreach (new playerid : Player)
    {
        for (new i = 0; i < MAX_CHECKPOINTS; i++)
        {
            if (Checkpoints[i][cpCreated])
            {
                Checkpoints[i][cpActivePlayerCheckpoint{playerid}] = false;
            }
        }
        
        new closestCpId = -1;
        new Float:closestDistance = STREAM_DISTANCE;
        
        foreach (new cpId : CheckpointsIterator)
        {
            if (Checkpoints[cpId][cpCreated])
            {
                if (Checkpoints[cpId][cpForPlayerId] == INVALID_PLAYER_ID || Checkpoints[cpId][cpForPlayerId] == playerid)
                {
                    new playerWorld = GetPlayerVirtualWorld(playerid);
                    new playerInterior = GetPlayerInterior(playerid);
                    
                    if ((Checkpoints[cpId][cpVirtualWorld] == -1 || playerWorld == Checkpoints[cpId][cpVirtualWorld]) &&
                        (Checkpoints[cpId][cpInterior] == -1 || playerInterior == Checkpoints[cpId][cpInterior]))
                    {
                        new Float:x, Float:y, Float:z;
                        GetPlayerPos(playerid, x, y, z);
                        
                        new Float:distance = GetDistanceBetweenPoints(x, y, z, Checkpoints[cpId][cpX], Checkpoints[cpId][cpY], Checkpoints[cpId][cpZ]);
                        
                        if (distance < STREAM_DISTANCE && distance < closestDistance)
                        {
                            closestDistance = distance;
                            closestCpId = cpId;
                        }
                    }
                }
            }
        }
        
        if (closestCpId != -1)
        {
            if (!Checkpoints[closestCpId][cpStreamedIn{playerid}])
            {
                SetPlayerCheckpoint(playerid, Checkpoints[closestCpId][cpX], Checkpoints[closestCpId][cpY], Checkpoints[closestCpId][cpZ], Checkpoints[closestCpId][cpSize]);
                Checkpoints[closestCpId][cpStreamedIn{playerid}] = true;
                Checkpoints[closestCpId][cpActivePlayerCheckpoint{playerid}] = true;
            }
        }
        
        foreach (new cpId : CheckpointsIterator)
        {
            if (Checkpoints[cpId][cpCreated] && Checkpoints[cpId][cpStreamedIn{playerid}])
            {
                if (Checkpoints[cpId][cpForPlayerId] == INVALID_PLAYER_ID || Checkpoints[cpId][cpForPlayerId] == playerid)
                {
                    if (cpId != closestCpId)
                    {
                        DisablePlayerCheckpoint(playerid);
                        Checkpoints[cpId][cpStreamedIn{playerid}] = false;
                        Checkpoints[cpId][cpActivePlayerCheckpoint{playerid}] = false;
                    }
                }
            }
        }
    }
    
    return 1;
}

public OnPlayerEnterCheckpoint(playerid)
{
    foreach (new cpId : CheckpointsIterator)
    {
        if (Checkpoints[cpId][cpCreated] && Checkpoints[cpId][cpStreamedIn{playerid}])
        {
            if (Checkpoints[cpId][cpForPlayerId] == INVALID_PLAYER_ID || Checkpoints[cpId][cpForPlayerId] == playerid)
            {
                new Float:x, Float:y, Float:z;
                GetPlayerPos(playerid, x, y, z);
                
                new Float:distance = GetDistanceBetweenPoints(x, y, z, Checkpoints[cpId][cpX], Checkpoints[cpId][cpY], Checkpoints[cpId][cpZ]);
                
                if (distance < Checkpoints[cpId][cpSize])
                {
                    CallLocalFunction("OnPlayerEnterDynamicCP", "ii", playerid, cpId);
                    
                    if (strlen(Checkpoints[cpId][cpCallback]) > 0)
                    {
                        new func[32];
                        format(func, sizeof(func), "%s", Checkpoints[cpId][cpCallback]);
                        CallLocalFunction(func, "ii", playerid, cpId);
                    }
                    
                    return 1;
                }
            }
        }
    }
    
    return 1;
}

public OnPlayerLeaveCheckpoint(playerid)
{
    foreach (new cpId : CheckpointsIterator)
    {
        if (Checkpoints[cpId][cpCreated] && Checkpoints[cpId][cpStreamedIn{playerid}])
        {
            if (Checkpoints[cpId][cpForPlayerId] == INVALID_PLAYER_ID || Checkpoints[cpId][cpForPlayerId] == playerid)
            {
                new Float:x, Float:y, Float:z;
                GetPlayerPos(playerid, x, y, z);
                
                new Float:distance = GetDistanceBetweenPoints(x, y, z, Checkpoints[cpId][cpX], Checkpoints[cpId][cpY], Checkpoints[cpId][cpZ]);
                
                if (distance > Checkpoints[cpId][cpSize])
                {
                    CallLocalFunction("OnPlayerLeaveDynamicCP", "ii", playerid, cpId);
                    return 1;
                }
            }
        }
    }
    
    return 1;
}

stock Float:GetDistanceBetweenPoints(Float:x1, Float:y1, Float:z1, Float:x2, Float:y2, Float:z2)
{
    return floatsqroot(floatpower(floatsub(x1, x2), 2) + floatpower(floatsub(y1, y2), 2) + floatpower(floatsub(z1, z2), 2));
}

CMD:createcp(playerid, params[])
{
    new Float:size;
    if (sscanf(params, "f", size))
    {
        SendClientMessage(playerid, -1, "Usage: /createcp [size]");
        return 1;
    }
    
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    
    new cpId = CreateDynamicCheckpoint(x, y, z, size, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid));
    
    if (cpId != -1)
    {
        new msg[128];
        format(msg, sizeof(msg), "Checkpoint created with ID %d", cpId);
        SendClientMessage(playerid, -1, msg);
    }
    else
    {
        SendClientMessage(playerid, -1, "Failed to create checkpoint. Maximum limit reached.");
    }
    
    return 1;
}

CMD:destroycp(playerid, params[])
{
    new cpId;
    if (sscanf(params, "d", cpId))
    {
        SendClientMessage(playerid, -1, "Usage: /destroycp [checkpointid]");
        return 1;
    }
    
    if (DestroyDynamicCheckpoint(cpId))
    {
        new msg[128];
        format(msg, sizeof(msg), "Checkpoint %d destroyed", cpId);
        SendClientMessage(playerid, -1, msg);
    }
    else
    {
        SendClientMessage(playerid, -1, "Invalid checkpoint ID.");
    }
    
    return 1;
}
