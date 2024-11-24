#include <Moderation.h>
#include <Log.h>
#include <cJSON.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <io/File.h>
#include <io/Dir.h>

cJSON*	g_banned_ips = NULL;
cJSON*	g_banned_udids = NULL;
cJSON*	g_banned_nicknames = NULL;
cJSON*	g_timeouts = NULL;
cJSON*	g_ops = NULL;
Mutex	g_banned_ipMut;
Mutex	g_banned_udidMut;
Mutex	g_banned_nicknameMut;
Mutex	g_timeoutMut;
Mutex	g_opMut;

bool init_balls(void)
{
    (void)mkdir("Player_Data", 0777);

    MutexCreate(g_timeoutMut);
    MutexCreate(g_banned_ipMut);
    MutexCreate(g_banned_udidMut);
    MutexCreate(g_banned_nicknameMut);
    MutexCreate(g_opMut);

    RAssert(collection_init(&g_timeouts,	TIMEOUTS_FILE,	"{}"));
    RAssert(collection_init(&g_banned_ips,		BANNED_IPS_FILE,		"{}"));
    RAssert(collection_init(&g_banned_udids,		BANNED_UDIDS_FILE,		"{}"));
    RAssert(collection_init(&g_banned_nicknames,		BANNED_NICKNAMES_FILE,		"{}"));
    RAssert(collection_init(&g_ops,		OPERATORS_FILE, "{ \"127.0.0.1\": [3, \"Host (127.0.0.1)\", \"(pc-linux)fox\"] }"));

    return true;
}

bool ban_add(const char* nickname, const char* udid, const char* ip)
{
    bool res = true;
    bool changed = false;

    MutexLock(g_banned_ipMut);
    {
        bool changed = false;
        if (!cJSON_HasObjectItem(g_banned_ips, ip))
        {
            cJSON* js = cJSON_CreateString(nickname);
            cJSON_AddItemToObject(g_banned_ips, ip, js);
            changed = true;
        }
    }
    MutexUnlock(g_banned_ipMut);

    MutexLock(g_banned_udidMut);
    {
        if (!cJSON_HasObjectItem(g_banned_udids, udid))
        {
            cJSON* js = cJSON_CreateString(nickname);
            cJSON_AddItemToObject(g_banned_udids, udid, js);
            changed = true;
        }
    }
    MutexUnlock(g_banned_udidMut);

    MutexLock(g_banned_nicknameMut);
    {
        if (!cJSON_HasObjectItem(g_banned_nicknames, nickname))
        {
            cJSON* js = cJSON_CreateString(nickname);
            cJSON_AddItemToObject(g_banned_nicknames, nickname, js);
            changed = true;
        }
    }
    MutexUnlock(g_banned_nicknameMut);

    if (changed)
        res = collection_save(BANNED_IPS_FILE, g_banned_ips) &&
            collection_save(BANNED_UDIDS_FILE, g_banned_udids) &&
            collection_save(BANNED_NICKNAMES_FILE, g_banned_nicknames);

    return res;
}

bool ban_revoke(const char* argument)
{
    bool res = false;
    bool changed = false;

    MutexLock(g_banned_ipMut);
    {

        if (cJSON_HasObjectItem(g_banned_ips, argument))
        {
            cJSON_DeleteItemFromObject(g_banned_ips, argument);
            changed = true;
        }
    }
    MutexUnlock(g_banned_ipMut);

    MutexLock(g_banned_udidMut);
    {
        if (cJSON_HasObjectItem(g_banned_udids, argument))
        {
            cJSON_DeleteItemFromObject(g_banned_udids, argument);
            changed = true;
        }
    }
    MutexUnlock(g_banned_udidMut);

    MutexLock(g_banned_nicknameMut);
    {
        if (cJSON_HasObjectItem(g_banned_nicknames, argument))
        {
            cJSON_DeleteItemFromObject(g_banned_nicknames, argument);
            changed = true;
        }
    }
    MutexUnlock(g_banned_nicknameMut);

    if(changed)
        res = collection_save(BANNED_IPS_FILE, g_banned_ips) &&
            collection_save(BANNED_UDIDS_FILE, g_banned_udids) &&
            collection_save(BANNED_NICKNAMES_FILE, g_banned_nicknames);

    return res;
}

bool ban_check(const char* nickname, const char* udid, const char* ip, bool* result)
{
    *result = false;

    if (g_config.ban_ip) {
        MutexLock(g_banned_ipMut);
        {
            if (cJSON_HasObjectItem(g_banned_ips, ip))
                *result = true;
        }
        MutexUnlock(g_banned_ipMut);
    }

    if (g_config.ban_udid) {
        MutexLock(g_banned_udidMut);
        {
            if (cJSON_HasObjectItem(g_banned_udids, udid))
                *result = true;
        }
        MutexUnlock(g_banned_udidMut);
    }

    if (g_config.ban_nickname) {
        MutexLock(g_banned_nicknameMut);
        {
            if (cJSON_HasObjectItem(g_banned_nicknames, nickname))
                *result = true;
        }
        MutexUnlock(g_banned_nicknameMut);
    }

    return true;
}



bool timeout_set(const char* nickname, const char* ip, const char* udid, uint64_t timestamp)
{
    bool res = true;

    MutexLock(g_timeoutMut);
    {
        bool changed = false;

        cJSON* obj = cJSON_GetObjectItem(g_timeouts, ip);
        if (!obj)
        {
            cJSON* root = cJSON_CreateArray();

            // store nickname
            cJSON* js = cJSON_CreateString(nickname);
            cJSON_AddItemToArray(root, js);

            // store timestamp
            js = cJSON_CreateNumber((double)timestamp);
            cJSON_AddItemToArray(root, js);

            cJSON_AddItemToObject(g_timeouts, ip, root);
            changed = true;
        }
        else
        {
            cJSON* item = cJSON_GetArrayItem(obj, 1);
            if (item)
            {
                cJSON_SetNumberValue(item, timestamp);
                changed = true;
            }
            else
                Warn("Missing timestamp in array");
        }

        obj = cJSON_GetObjectItem(g_timeouts, udid);
        if (!obj)
        {
            cJSON* root = cJSON_CreateArray();

            // store nickname
            cJSON* js = cJSON_CreateString(nickname);
            cJSON_AddItemToArray(root, js);

            // store timestamp
            js = cJSON_CreateNumber((double)timestamp);
            cJSON_AddItemToArray(root, js);

            cJSON_AddItemToObject(g_timeouts, udid, root);
            changed = true;
        }
        else
        {
            cJSON* item = cJSON_GetArrayItem(obj, 1);
            if (item)
            {
                cJSON_SetNumberValue(item, timestamp);
                changed = true;
            }
            else
                Warn("Missing timestamp in array");
        }

        if (changed)
            res = collection_save(TIMEOUTS_FILE, g_timeouts);
    }
    MutexUnlock(g_timeoutMut);

    return res;
}

bool timeout_revoke(const char* udid, const char* ip)
{
    bool res = false;

    MutexLock(g_timeoutMut);
    {
        bool changed = false;

        if (cJSON_HasObjectItem(g_timeouts, ip))
        {
            cJSON_DeleteItemFromObject(g_timeouts, ip);
            changed = true;
        }

        if (cJSON_HasObjectItem(g_timeouts, udid))
        {
            cJSON_DeleteItemFromObject(g_timeouts, udid);
            changed = true;
        }

        if (changed)
            res = collection_save(TIMEOUTS_FILE, g_timeouts);
    }
    MutexUnlock(g_timeoutMut);

    return res;
}

bool timeout_check(const char* udid, const char* ip, uint64_t* result)
{
    *result = 0;

    MutexLock(g_opMut);
    {
        cJSON* obj = cJSON_GetObjectItem(g_timeouts, ip);

        if(!obj)
            obj = cJSON_GetObjectItem(g_timeouts, udid);

        if (obj)
        {
            cJSON* timeout = cJSON_GetArrayItem(obj, 1);

            if (timeout)
                *result = (uint64_t)cJSON_GetNumberValue(timeout);
            else
                Warn("Missing timestamp in array");
        }
    }
    MutexUnlock(g_opMut);

    return true;
}

bool op_add(const char* nickname, const char* ip, uint8_t level, const char* note)
{
    bool res = true;

    MutexLock(g_opMut);
    {
        if (!cJSON_HasObjectItem(g_ops, ip))
        {
            // Create a JSON array for the new structure
            cJSON* op_array = cJSON_CreateArray();

            // Add the level as a number
            cJSON_AddItemToArray(op_array, cJSON_CreateNumber(level));

            // Add note1
            cJSON_AddItemToArray(op_array, cJSON_CreateString(note));

            // Add nickname/note2
            cJSON_AddItemToArray(op_array, cJSON_CreateString(nickname));

            // Add the array to the operators object
            cJSON_AddItemToObject(g_ops, ip, op_array);

            // Save the updated operators file
            res = collection_save(OPERATORS_FILE, g_ops);
        }
        else
        {
            Warn("IP %s is already an operator", ip);
        }
    }
    MutexUnlock(g_opMut);

    return res;
}

bool op_revoke(const char* ip)
{
    bool res = false;

    MutexLock(g_opMut);
    {
        if (cJSON_HasObjectItem(g_ops, ip))
        {
            cJSON_DeleteItemFromObject(g_ops, ip);

            res = collection_save(OPERATORS_FILE, g_ops);
        }
    }
    MutexUnlock(g_opMut);

    return res;
}

bool op_check(const char* ip, uint8_t* level)
{
    *level = 0;

    MutexLock(g_opMut);
    {
        cJSON* obj = cJSON_GetObjectItem(g_ops, ip);

        if (obj && cJSON_IsArray(obj))
        {
            // Extract level
            cJSON* level_item = cJSON_GetArrayItem(obj, 0);
            if (level_item && cJSON_IsNumber(level_item))
            {
                *level = (uint8_t)cJSON_GetNumberValue(level_item);
            }
        }
    }
    MutexUnlock(g_opMut);

    return true;
}

