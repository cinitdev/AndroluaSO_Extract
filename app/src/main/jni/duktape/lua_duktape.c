/**
 * Duktape Lua Module for AndroLua (Grand Finale Version)
 *
 * This definitive version creates a more convincing browser-like environment
 * by adding 'window' and 'navigator' objects. This is designed to satisfy
 * the anti-debugging checks within the obfuscated JS, finally allowing it
 * to execute correctly without freezing.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lua.h"
#include "lauxlib.h"
#include "duktape.h"

// A structure to hold captured data, associated with a Duktape heap.
typedef struct {
    char *cookie_value;
} CapturedData;

// Native C function to handle the 'document.cookie' setter.
static duk_ret_t native_set_cookie(duk_context *ctx) {
    if (duk_get_top(ctx) >= 1 && duk_is_string(ctx, 0)) {
        const char *cookie_str = duk_get_string(ctx, 0);
        duk_memory_functions funcs;
        void *udata;
        duk_get_memory_functions(ctx, &funcs);
        udata = funcs.udata;
        if (udata) {
            CapturedData *data = (CapturedData *)udata;
            if (data->cookie_value) {
                free(data->cookie_value);
            }
            data->cookie_value = strdup(cookie_str);
        }
    }
    return 0;
}

// Dummy function for other browser APIs like 'reload()'.
static duk_ret_t native_dummy_func(duk_context *ctx) {
    return 0;
}

// The main function exposed to Lua: duktape.eval(js_code, host)
static int l_duktape_eval(lua_State *L) {
    size_t js_len;
    const char *js_code = luaL_checklstring(L, 1, &js_len);
    const char *host = luaL_optstring(L, 2, "localhost");

    CapturedData captured_data = { .cookie_value = NULL };
    duk_context *ctx = NULL;

    ctx = duk_create_heap(NULL, NULL, NULL, &captured_data, NULL);
    if (!ctx) {
        lua_pushnil(L);
        lua_pushstring(L, "Fatal: Could not create Duktape heap.");
        return 2;
    }

    // --- 【终极核心修正】: 创建一个更逼真的浏览器模拟环境 ---
    duk_push_global_object(ctx);

    // 1. 模拟 'window' 对象 (在浏览器中, window 就是 global athis)
    duk_dup_top(ctx); // 复制全局对象
    duk_put_prop_string(ctx, -2, "window");

    // 2. 模拟 'navigator' 对象
    duk_push_object(ctx); // 创建 navigator = {}
    // 添加一个常见的移动端 User Agent，这是最常被检测的属性
    duk_push_string(ctx, "Mozilla/5.0 (Linux; Android 10; K) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Mobile Safari/537.36");
    duk_put_prop_string(ctx, -2, "userAgent");
    duk_put_prop_string(ctx, -2, "navigator"); // global.navigator = { userAgent: "..." }

    // 3. 模拟 'location' 对象
    duk_push_object(ctx);
    duk_push_string(ctx, host);
    duk_put_prop_string(ctx, -2, "host");
    duk_put_prop_string(ctx, -2, "location");

    // 4. 模拟 'document' 对象，并通过 Proxy 拦截 'cookie' 写入
    duk_push_object(ctx); // 这是我们的目标 'document' 对象
    duk_push_object(ctx); // document.location
    duk_push_c_function(ctx, native_dummy_func, 0);
    duk_put_prop_string(ctx, -2, "reload");
    duk_put_prop_string(ctx, -2, "location");
    // 使用 JS Proxy 来拦截 cookie 设置
    duk_eval_string(ctx, "(function(t){return new Proxy(t,{set:function(o,p,v){if(p==='cookie'){_native_set_cookie(v)}o[p]=v;return true}})})");
    duk_dup(ctx, -2); // 复制目标 'document' 对象作为参数
    duk_call(ctx, 1); // 调用函数创建 Proxy, Proxy 现在在栈顶
    duk_put_global_string(ctx, "document"); // global.document = <Proxy>
    duk_pop(ctx); // 弹出原始的 'document' 对象

    // 5. 将我们的 C 函数注册为一个全局 JS 函数，供 Proxy 调用
    duk_push_c_function(ctx, native_set_cookie, 1);
    duk_put_global_string(ctx, "_native_set_cookie");

    // --- 执行用户 JS 代码 ---
    if (duk_peval_lstring(ctx, js_code, js_len) != 0) {
        const char* error_msg = duk_safe_to_string(ctx, -1);
        lua_pushnil(L);
        lua_pushstring(L, error_msg);
        duk_destroy_heap(ctx);
        if (captured_data.cookie_value) free(captured_data.cookie_value);
        return 2;
    }
    duk_pop(ctx);

    // --- 返回结果 ---
    if (captured_data.cookie_value) {
        char *start = strstr(captured_data.cookie_value, "acw_sc__v2=");
        if (start) {
            start += strlen("acw_sc__v2=");
            char *end = strchr(start, ';');
            if (end) {
                *end = '\0';
                lua_pushstring(L, start);
                *end = ';';
            } else {
                lua_pushstring(L, start);
            }
        } else {
            lua_pushstring(L, captured_data.cookie_value);
        }
        free(captured_data.cookie_value);
    } else {
        lua_pushnil(L);
    }

    duk_destroy_heap(ctx);
    return 1;
}

static const struct luaL_Reg duktape_lib[] = {
        {"eval", l_duktape_eval},
        {NULL, NULL}
};

int luaopen_duktape(lua_State *L) {
    luaL_newlib(L, duktape_lib);
    return 1;
}


