//
// Created by CinitDev on 2025/10/5.
//

/**
 * acw Cookie Generator (Final Readable Version V11 - Correct Algorithm)
 *
 * This definitive Lua C module contains a completely rewritten calculation logic
 * that is a direct, 1-to-1 translation of the user-provided, proven-to-work
 * Lua script. All previous bugs in the C calculation have been eliminated.
 * The 'debug' function now uses Lua's print for a clear debugging experience.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lua.h"
#include "lauxlib.h"

static void debug_print(lua_State *L, const char *message) {
    lua_getglobal(L, "print");
    if (lua_isfunction(L, -1)) {
        lua_pushstring(L, message);
        lua_pcall(L, 1, 0, 0);
    } else {
        lua_pop(L, 1);
    }
}

static char* b64_decode_to_string(const char *data, size_t input_length, size_t *output_length) {
    static const int b64_lookup[] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1 };
    if (input_length % 4 != 0) return NULL;
    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;
    char *decoded_data = (char*)malloc(*output_length + 1);
    if (!decoded_data) return NULL;
    for (size_t i = 0, j = 0; i < input_length; ) {
        uint32_t a = data[i] == '=' ? 0 & i++ : b64_lookup[(unsigned char)data[i++]];
        uint32_t b = data[i] == '=' ? 0 & i++ : b64_lookup[(unsigned char)data[i++]];
        uint32_t c = data[i] == '=' ? 0 & i++ : b64_lookup[(unsigned char)data[i++]];
        uint32_t d = data[i] == '=' ? 0 & i++ : b64_lookup[(unsigned char)data[i++]];
        uint32_t triple = (a << 18) + (b << 12) + (c << 6) + d;
        if (j < *output_length) decoded_data[j++] = (triple >> 16) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = triple & 0xFF;
    }
    decoded_data[*output_length] = '\0';
    return decoded_data;
}

static const char* find_hex_seed(const char* js_code) {
    const char* p = js_code;
    while ((p = strchr(p, '\'')) != NULL) {
        p++;
        const char* end = strchr(p, '\'');
        if (!end || (end - p) != 40) { if (end) p = end; else break; continue; }
        int is_hex = 1;
        for (size_t i = 0; i < 40; i++) {
            if (!isxdigit((unsigned char)p[i])) { is_hex = 0; break; }
        }
        if (is_hex) return p;
        p = end;
    }
    return NULL;
}

static const char* find_numeric_array(const char* js_code, size_t* len) {
    const char* p = js_code;
    while ((p = strstr(p, "=[")) != NULL) {
        p += 2;
        const char* end = strstr(p, "];");
        if (!end) { p++; continue; }
        if ((end - p) < 100) { p = end; continue; }
        int is_valid_array = 1;
        for (size_t i = 0; i < (end - p); i++) {
            char c = p[i];
            if (!isxdigit((unsigned char)c) && c != ',' && c != ' ' && c != 'x') {
                is_valid_array = 0;
                break;
            }
        }
        if (is_valid_array) { *len = end - p; return p; }
        p = end;
    }
    return NULL;
}

static const char* find_b64_mask(const char* js_code, const char* hex_seed, size_t* len) {
    const char* p = js_code;
    while ((p = strchr(p, '\'')) != NULL) {
        p++;
        const char* end = strchr(p, '\'');
        if (!end || (end - p) < 50) { if (end) p = end; else break; continue; }
        if (hex_seed && strncmp(p, hex_seed, 40) == 0) { p = end; continue; }
        int is_b64 = 1;
        for (size_t i = 0; i < (end - p); i++) {
            char c = p[i];
            if (!isalnum((unsigned char)c) && c != '+' && c != '/' && c != '=') { is_b64 = 0; break; }
        }
        if (is_b64) { *len = end - p; return p; }
        p = end;
    }
    return NULL;
}

// --- 内部计算函数 ---
static int calculate_acw(lua_State *L, int debug) {
    const char* js_code = luaL_checkstring(L, 1);
    char debug_buffer[1024];

    if (debug) debug_print(L, "--- [C DEBUG] 开始执行 ---");

    if (debug) debug_print(L, "步骤 1/4: 提取参数...");
    const char* hex_seed_str = find_hex_seed(js_code);
    size_t numeric_array_len;
    const char* numeric_array_str = find_numeric_array(js_code, &numeric_array_len);
    size_t b64_mask_len;
    const char* b64_mask_str = find_b64_mask(js_code, hex_seed_str, &b64_mask_len);

    if (!hex_seed_str || !numeric_array_str || !b64_mask_str) {
        if (debug) debug_print(L, "  -> 失败: 未能找到所有必需的参数。");
        return luaL_error(L, "错误: 未能通过特征找到所有必需的参数。");
    }
    if (debug) {
        char temp_hex[41]; strncpy(temp_hex, hex_seed_str, 40); temp_hex[40] = '\0';
        snprintf(debug_buffer, sizeof(debug_buffer), "  -> 成功: 找到 arg: %s", temp_hex);
        debug_print(L, debug_buffer);

        char temp_b64[b64_mask_len + 1]; strncpy(temp_b64, b64_mask_str, b64_mask_len); temp_b64[b64_mask_len] = '\0';
        snprintf(debug_buffer, sizeof(debug_buffer), "  -> 成功: 找到 mask: %s", temp_b64);
        debug_print(L, debug_buffer);

        char temp_array[numeric_array_len + 1]; strncpy(temp_array, numeric_array_str, numeric_array_len); temp_array[numeric_array_len] = '\0';
        snprintf(debug_buffer, sizeof(debug_buffer), "  -> 成功: 找到 posList: %s...", temp_array);
        debug_print(L, debug_buffer);
    }

    if (debug) debug_print(L, "步骤 2/4: 解码和解析...");
    size_t mask_len;
    char* mask_str = b64_decode_to_string(b64_mask_str, b64_mask_len, &mask_len);
    if (!mask_str) { if (debug) debug_print(L, "  -> 失败: Base64 解码失败。"); return luaL_error(L, "错误: Base64 解码失败。"); }
    if (debug) { snprintf(debug_buffer, sizeof(debug_buffer), "  -> 成功: Base64 解码完成, mask: %s", mask_str); debug_print(L, debug_buffer); }

    int posList[40];
    int count = 0;
    char* p = (char*)numeric_array_str;
    char* end = p + numeric_array_len;
    while (p < end && count < 40) {
        while (p < end && (*p == ',' || isspace((unsigned char)*p))) p++;
        if (p >= end) break;
        posList[count++] = (int)strtol(p, &p, 0);
    }
    if (count != 40) { free(mask_str); if (debug) { snprintf(debug_buffer, sizeof(debug_buffer), "  -> 失败: 解析 posList 失败 (%d/40)。", count); debug_print(L, debug_buffer); } return luaL_error(L, "错误: 解析 posList 失败 (%d/40)。", count); }
    if (debug) debug_print(L, "  -> 成功: posList 解析完成。");

    // --- 【终极核心修正】: 严格按照用户提供的 Lua 脚本逻辑重写计算部分 ---

    if (debug) debug_print(L, "步骤 3/4: 执行核心算法...");

    char outPutList[40];
    memset(outPutList, 0, 40);
    for (int i = 1; i <= 40; i++) {
        char c = hex_seed_str[i - 1];
        for (int j = 0; j < 40; j++) {
            if (posList[j] == i) {
                outPutList[j] = c;
            }
        }
    }

    char reordered_arg[41];
    memcpy(reordered_arg, outPutList, 40);
    reordered_arg[40] = '\0';
    if (debug) { snprintf(debug_buffer, sizeof(debug_buffer), "  -> 成功: arg 重排完成。"); debug_print(L, debug_buffer); }

    char result_value[41] = {0};
    for (int i = 0; i < 40; i += 2) {
        char str_hex[3] = { reordered_arg[i], reordered_arg[i+1], 0 };
        char mask_hex[3] = { mask_str[i], mask_str[i+1], 0 };
        int str_val = (int)strtol(str_hex, NULL, 16);
        int mask_val = (int)strtol(mask_hex, NULL, 16);
        snprintf(&result_value[i], 3, "%02x", str_val ^ mask_val);
    }
    free(mask_str);
    if (debug) { snprintf(debug_buffer, sizeof(debug_buffer), "  -> 成功: 异或计算完成, 结果: %s", result_value); debug_print(L, debug_buffer); }

    if (debug) debug_print(L, "步骤 4/4: 格式化并返回...");
    char final_cookie_string[64];
    snprintf(final_cookie_string, sizeof(final_cookie_string), "acw_sc__v2=%s", result_value);

    lua_pushstring(L, final_cookie_string);
    if (debug) debug_print(L, "--- [C DEBUG] 执行完毕 ---");
    return 1;
}

static int l_acw_get(lua_State *L) {
    return calculate_acw(L, 0);
}

static int l_acw_get_debug(lua_State *L) {
    return calculate_acw(L, 1);
}

static const struct luaL_Reg acw_lib[] = {
        {"get", l_acw_get},
        {"debug", l_acw_get_debug},
        {NULL, NULL}
};

int luaopen_acw(lua_State *L) {
    luaL_newlib(L, acw_lib);
    return 1;
}

