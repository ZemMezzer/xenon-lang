#include "xstring.h"
#include "xerror.h"
#include "stack_helper.h"
#include <string.h>
#include <ctype.h>
#include <stdint.h>

extern "C" {
#include "lauxlib.h"
}

static const char* lib_name = "string";
static const char* string_too_large_message = "Resulting string too large";

static lua_Integer find_index(XString* xs, XString* sub) {
	size_t n = xs->length;
	size_t m = sub->length;

	if (m == 0) return 1;
	if (m > n) return 0;

	for (size_t i = 0; i <= n - m; ++i) {
		if (memcmp(xs->string + i, sub->string, m) == 0)
			return (lua_Integer)i + 1;
	}

	return 0;
}

static int xl_xstr(lua_State* L) {
	
	size_t length = 0;
	const char* s = luaL_checklstring(L, 1, &length);

	XString* xs = (XString*)lua_newuserdata(L, sizeof(XString) + length + 1);
	xs->length = length;
	xs->string = (char*)(xs + 1);
	memcpy(xs->string, s, length);
	xs->string[length] = '\0';

	luaL_getmetatable(L, lib_name);
	lua_setmetatable(L, -2);
	return 1;
}

static int xl_to_string(lua_State* L) {
	size_t len;
	const char* s = luaL_tolstring(L, 1, &len);

	XString* xs = (XString*)lua_newuserdata(L, sizeof(XString) + len + 1);
	xs->length = (int)len;
	xs->string = (char*)(xs + 1);
	memcpy(xs->string, s, len);
	xs->string[len] = '\0';

	luaL_getmetatable(L, lib_name);
	lua_setmetatable(L, -2);

	lua_remove(L, -2);
	return 1;
}

static int l_join(lua_State* L) {
	XString* delim = (XString*)luaL_checkudata(L, 1, lib_name);
	luaL_checktype(L, 2, LUA_TTABLE);

	size_t delimLen = delim->length;

	lua_Integer n = luaL_len(L, 2);
	size_t totalLen = 0;

	for (lua_Integer i = 1; i <= n; ++i) {
		lua_rawgeti(L, 2, i);
		XString* part = (XString*)luaL_checkudata(L, -1, lib_name);
		totalLen += part->length;
		lua_pop(L, 1);
	}

	if (n > 1 && delimLen > 0) {
		if ((size_t)(n - 1) > (SIZE_MAX - totalLen) / delimLen)
			return luaL_error(L, string_too_large_message);
		totalLen += (size_t)(n - 1) * delimLen;
	}

	XString* out = (XString*)lua_newuserdata(L, sizeof(XString) + totalLen + 1);
	out->length = totalLen;
	out->string = (char*)(out + 1);

	char* dst = out->string;

	for (lua_Integer i = 1; i <= n; ++i) {
		lua_rawgeti(L, 2, i);
		XString* part = (XString*)luaL_checkudata(L, -1, lib_name);

		memcpy(dst, part->string, part->length);
		dst += part->length;

		lua_pop(L, 1);

		if (i < n && delimLen > 0) {
			memcpy(dst, delim->string, delimLen);
			dst += delimLen;
		}
	}

	out->string[totalLen] = '\0';

	luaL_getmetatable(L, lib_name);
	lua_setmetatable(L, -2);

	return 1;
}

static int to_string(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);
	lua_pushlstring(L, xs->string, xs->length);
	return 1;
}

static int length(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);
	lua_pushinteger(L, xs->length);
	return 1;
}

static int equals(lua_State* L) {
	XString* xs1 = (XString*)luaL_checkudata(L, 1, lib_name);
	XString* xs2 = (XString*)luaL_checkudata(L, 2, lib_name);
	if (xs1->length != xs2->length) {
		lua_pushboolean(L, 0);
		return 1;
	}
	int cmp = memcmp(xs1->string, xs2->string, xs1->length);
	lua_pushboolean(L, cmp == 0);
	return 1;
}

static int xl_to_upper(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);
	size_t len = xs->length;
	XString* upper_xs = (XString*)lua_newuserdata(L,sizeof(XString) + len + 1);

	upper_xs->length = (int)len;
	upper_xs->string = (char*)(upper_xs + 1);

	for (size_t i = 0; i < len; ++i) {
		upper_xs->string[i] =
			(char)toupper((unsigned char)xs->string[i]);
	}

	upper_xs->string[len] = '\0';

	luaL_getmetatable(L, lib_name);
	lua_setmetatable(L, -2);

	return 1;
}

static int xl_to_lower(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);
	size_t len = xs->length;
	XString* lower_xs = (XString*)lua_newuserdata(L, sizeof(XString) + len + 1);

	lower_xs->length = (int)len;
	lower_xs->string = (char*)(lower_xs + 1);

	for (size_t i = 0; i < len; ++i) {
		lower_xs->string[i] =
			(char)tolower((unsigned char)xs->string[i]);
	}

	lower_xs->string[len] = '\0';

	luaL_getmetatable(L, lib_name);
	lua_setmetatable(L, -2);

	return 1;
}

static int xl_index_of(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);
	XString* sub = (XString*)luaL_checkudata(L, 2, lib_name);

	lua_Integer pos = find_index(xs, sub);

	if (pos == 0) {
		lua_pushnil(L);
	}
	else {
		lua_pushinteger(L, pos);
	}

	return 1;
}

static int xl_substring(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);

	lua_Integer start = luaL_checkinteger(L, 2);
	if (start < 1 || start >(lua_Integer)xs->length) {
		return luaL_error(L, value_out_of_range_message);
	}

	lua_Integer len;
	if (lua_isnoneornil(L, 3)) {
		len = (lua_Integer)xs->length - start + 1;
	}
	else {
		len = luaL_checkinteger(L, 3);
		if (len < 0) {
			return luaL_error(L, value_out_of_range_message);
		}
	}

	size_t s0 = (size_t)(start - 1);
	size_t maxAvail = (size_t)xs->length - s0;

	size_t outLen = (size_t)len;
	if (outLen > maxAvail) outLen = maxAvail;

	XString* out = (XString*)lua_newuserdata(L, sizeof(XString) + outLen + 1);
	out->length = (int)outLen;
	out->string = (char*)(out + 1);

	memcpy(out->string, xs->string + s0, outLen);
	out->string[outLen] = '\0';

	luaL_getmetatable(L, lib_name);
	lua_setmetatable(L, -2);
	return 1;
}

static int xl_contains(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);
	XString* sub = (XString*)luaL_checkudata(L, 2, lib_name);

	lua_pushboolean(L, find_index(xs, sub) != 0);
	return 1;
}

static int xl_starts_with(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);
	XString* prefix = (XString*)luaL_checkudata(L, 2, lib_name);
	if (prefix->length > xs->length) {
		lua_pushboolean(L, 0);
		return 1;
	}
	int cmp = memcmp(xs->string, prefix->string, prefix->length);
	lua_pushboolean(L, cmp == 0);
	return 1;
}

static int xl_ends_with(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);
	XString* suffix = (XString*)luaL_checkudata(L, 2, lib_name);
	if (suffix->length > xs->length) {
		lua_pushboolean(L, 0);
		return 1;
	}
	size_t offset = xs->length - suffix->length;
	int cmp = memcmp(xs->string + offset, suffix->string, suffix->length);
	lua_pushboolean(L, cmp == 0);
	return 1;
}

static int xl_replace(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);
	XString* old = (XString*)luaL_checkudata(L, 2, lib_name);
	XString* rep = (XString*)luaL_checkudata(L, 3, lib_name);

	const char* src = xs->string;
	size_t srcLen = xs->length;
	size_t oldLen = old->length;
	size_t repLen = rep->length;

	if (oldLen == 0) {
		return luaL_error(L, "Old value cannot be empty");
	}
	if (oldLen > srcLen) {
		lua_pushvalue(L, 1);
		return 1;
	}

	size_t count = 0;
	for (size_t i = 0; i + oldLen <= srcLen; ) {
		if (memcmp(src + i, old->string, oldLen) == 0) {
			count++;
			i += oldLen;
		}
		else {
			i++;
		}
	}

	if (count == 0) {
		lua_pushvalue(L, 1);
		return 1;
	}

	size_t newLen;
	if (repLen >= oldLen) {
		size_t delta = repLen - oldLen;
		if (delta != 0 && count > (SIZE_MAX - srcLen) / delta) {
			return luaL_error(L, string_too_large_message);
		}
		newLen = srcLen + count * delta;
	}
	else {
		size_t delta = oldLen - repLen;
		newLen = srcLen - count * delta;
	}

	XString* out = (XString*)lua_newuserdata(L, sizeof(XString) + newLen + 1);
	out->length = newLen;
	out->string = (char*)(out + 1);

	char* dst = out->string;

	for (size_t i = 0; i < srcLen; ) {
		if (i + oldLen <= srcLen && memcmp(src + i, old->string, oldLen) == 0) {
			memcpy(dst, rep->string, repLen);
			dst += repLen;
			i += oldLen;
		}
		else {
			*dst++ = src[i++];
		}
	}

	out->string[newLen] = '\0';

	luaL_getmetatable(L, lib_name);
	lua_setmetatable(L, -2);
	return 1;
}

static int xl_trim(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);
	size_t start = 0;
	size_t end = (size_t)xs->length;
	while (start < end && isspace((unsigned char)xs->string[start])) {
		start++;
	}
	while (end > start && isspace((unsigned char)xs->string[end - 1])) {
		end--;
	}
	size_t newLen = end - start;
	XString* out = (XString*)lua_newuserdata(L, sizeof(XString) + newLen + 1);
	out->length = (int)newLen;
	out->string = (char*)(out + 1);
	memcpy(out->string, xs->string + start, newLen);
	out->string[newLen] = '\0';
	luaL_getmetatable(L, lib_name);
	lua_setmetatable(L, -2);
	return 1;
}

static int xl_last_index_of(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);
	XString* sub = (XString*)luaL_checkudata(L, 2, lib_name);

	size_t n = xs->length;
	size_t m = sub->length;

	if (m == 0) {
		lua_pushinteger(L, (lua_Integer)n);
		return 1;
	}

	if (m > n) {
		lua_pushnil(L);
		return 1;
	}

	for (size_t i = n - m + 1; i-- > 0; ) {
		if (memcmp(xs->string + i, sub->string, m) == 0) {
			lua_pushinteger(L, (lua_Integer)i + 1);
			return 1;
		}
	}

	lua_pushnil(L);
	return 1;
}

static int xl_reverse(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);
	size_t len = xs->length;
	XString* rev_xs = (XString*)lua_newuserdata(L, sizeof(XString) + len + 1);
	rev_xs->length = (int)len;
	rev_xs->string = (char*)(rev_xs + 1);
	for (size_t i = 0; i < len; ++i) {
		rev_xs->string[i] = xs->string[len - 1 - i];
	}
	rev_xs->string[len] = '\0';
	luaL_getmetatable(L, lib_name);
	lua_setmetatable(L, -2);
	return 1;
}

static int xl_repeat(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);
	lua_Integer n = luaL_checkinteger(L, 2);

	if (n < 0)
		return luaL_error(L, "Repeat count cannot be negative");

	if (n == 0 || xs->length == 0) {
		XString* out = (XString*)lua_newuserdata(L, sizeof(XString) + 1);
		out->length = 0;
		out->string = (char*)(out + 1);
		out->string[0] = '\0';

		luaL_getmetatable(L, lib_name);
		lua_setmetatable(L, -2);
		return 1;
	}

	size_t len = xs->length;
	size_t newLen = len * (size_t)n;

	if (n > 0 && newLen / (size_t)n != len)
		return luaL_error(L, string_too_large_message);

	XString* out = (XString*)lua_newuserdata(L, sizeof(XString) + newLen + 1);
	out->length = newLen;
	out->string = (char*)(out + 1);

	char* dst = out->string;

	for (lua_Integer i = 0; i < n; ++i) {
		memcpy(dst, xs->string, len);
		dst += len;
	}

	out->string[newLen] = '\0';

	luaL_getmetatable(L, lib_name);
	lua_setmetatable(L, -2);

	return 1;
}

static int xl_split(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);
	XString* delim = (XString*)luaL_checkudata(L, 2, lib_name);

	size_t srcLen = xs->length;
	size_t delimLen = delim->length;

	if (delimLen == 0)
		return luaL_error(L, "Delimiter cannot be empty");

	lua_newtable(L);

	size_t start = 0;
	int outIndex = 1;

	for (size_t i = 0; i + delimLen <= srcLen; ) {

		if (memcmp(xs->string + i, delim->string, delimLen) == 0) {
			size_t partLen = i - start;
			XString* part = (XString*)lua_newuserdata(L, sizeof(XString) + partLen + 1);
			part->length = partLen;
			part->string = (char*)(part + 1);

			memcpy(part->string, xs->string + start, partLen);
			part->string[partLen] = '\0';

			luaL_getmetatable(L, lib_name);
			lua_setmetatable(L, -2);

			lua_rawseti(L, -2, outIndex++);

			i += delimLen;
			start = i;
		}
		else {
			i++;
		}
	}

	size_t partLen = srcLen - start;

	XString* part = (XString*)lua_newuserdata(L, sizeof(XString) + partLen + 1);
	part->length = partLen;
	part->string = (char*)(part + 1);

	memcpy(part->string, xs->string + start, partLen);
	part->string[partLen] = '\0';

	luaL_getmetatable(L, lib_name);
	lua_setmetatable(L, -2);

	lua_rawseti(L, -2, outIndex++);

	return 1;
}

static int xl_is_empty(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);
	lua_pushboolean(L, xs->length == 0);
	return 1;
}

static std::string get_string_by_index(lua_State* L, int idx) {
	if(!lua_isuserdata(L, idx)) {
		return stack_value_to_string(L, idx);
	}

	XString* xs = (XString*)luaL_checkudata(L, idx, lib_name);
	return xs->to_std_string();
}

static int add(lua_State* L) {

	std::string a = get_string_by_index(L, 1);
	std::string b = get_string_by_index(L, 2);

	size_t len = (size_t)a.length() + (size_t)b.length();

	XString* out = (XString*)lua_newuserdata(L, sizeof(XString) + len + 1);
	out->length = (int)len;
	out->string = (char*)(out + 1);

	memcpy(out->string, a.c_str(), a.length());
	memcpy(out->string + a.length(), b.c_str(), b.length());
	out->string[len] = '\0';

	luaL_getmetatable(L, lib_name);
	lua_setmetatable(L, -2);
	return 1;
}

static int index(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);

	if (lua_type(L, 2) == LUA_TNUMBER) {

		lua_Integer idx = lua_tointeger(L, 2);

		if (idx < 1 || idx > xs->length) {
			return luaL_error(L, value_out_of_range_message);
		}

		char c = xs->string[idx - 1];

		XString* out = (XString*)lua_newuserdata(L, sizeof(XString) + 2);
		out->length = 1;
		out->string = (char*)(out + 1);
		out->string[0] = c;
		out->string[1] = '\0';

		luaL_getmetatable(L, lib_name);
		lua_setmetatable(L, -2);

		return 1;
	}

	luaL_getmetatable(L, lib_name);
	lua_getfield(L, -1, "__methods");

	lua_pushvalue(L, 2);
	lua_rawget(L, -2);

	lua_remove(L, -2);
	lua_remove(L, -2);

	return 1;
}

static void create_methods(lua_State* L) {
	lua_newtable(L);

	lua_pushcfunction(L, xl_to_upper);
	lua_setfield(L, -2, "to_upper");

	lua_pushcfunction(L, xl_to_lower);
	lua_setfield(L, -2, "to_lower");

	lua_pushcfunction(L, xl_index_of);
	lua_setfield(L, -2, "index_of");

	lua_pushcfunction(L, xl_substring);
	lua_setfield(L, -2, "substring");

	lua_pushcfunction(L, xl_contains);
	lua_setfield(L, -2, "contains");

	lua_pushcfunction(L, xl_starts_with);
	lua_setfield(L, -2, "starts_with");

	lua_pushcfunction(L, xl_ends_with);
	lua_setfield(L, -2, "ends_with");

	lua_pushcfunction(L, xl_replace);
	lua_setfield(L, -2, "replace");

	lua_pushcfunction(L, xl_trim);
	lua_setfield(L, -2, "trim");

	lua_pushcfunction(L, xl_last_index_of);
	lua_setfield(L, -2, "last_index_of");

	lua_pushcfunction(L, xl_reverse);
	lua_setfield(L, -2, "reverse");

	lua_pushcfunction(L, xl_repeat);
	lua_setfield(L, -2, "rep");

	lua_pushcfunction(L, xl_split);
	lua_setfield(L, -2, "split");

	lua_pushcfunction(L, xl_is_empty);
	lua_setfield(L, -2, "is_empty");

	lua_setfield(L, -2, "__methods");
}

static void create_lib(lua_State* L) {
	lua_newtable(L);

	lua_pushcfunction(L, l_join);
	lua_setfield(L, -2, "join");

	lua_setglobal(L, lib_name);
}

XString* xstring_check(lua_State* L, int index) {

	if(lua_isstring(L, index)) {
		size_t len;
		const char* s = lua_tolstring(L, index, &len);
		XString* xs = (XString*)lua_newuserdata(L, sizeof(XString) + len + 1);
		xs->length = (int)len;
		xs->string = (char*)(xs + 1);
		memcpy(xs->string, s, len);
		xs->string[len] = '\0';
		luaL_getmetatable(L, lib_name);
		lua_setmetatable(L, -2);
		return xs;
	}

	return (XString*)luaL_checkudata(L, index, lib_name);
}

bool is_xstring(lua_State* L, int index) {
	return luaL_testudata(L, index, lib_name) != nullptr || lua_isstring(L, index);
}

std::string xstring_to_std_string(lua_State* L, int idx) {
	XString* xs = xstring_check(L, idx);
	return std::string(xs->string, xs->length);
}

void xstring_push(lua_State* L, const char* str, size_t length) {
	XString* xs = (XString*)lua_newuserdata(L, sizeof(XString) + length + 1);
	xs->length = (int)length;
	xs->string = (char*)(xs + 1);
	memcpy(xs->string, str, length);
	xs->string[length] = '\0';
	luaL_getmetatable(L, lib_name);
	lua_setmetatable(L, -2);
}

extern "C" int xenon_openlib_xstring(lua_State* L) {

	if (luaL_newmetatable(L, lib_name)) {
		lua_pushcfunction(L, to_string);
		lua_setfield(L, -2, "__tostring");

		lua_pushcfunction(L, length);
		lua_setfield(L, -2, "__len");

		lua_pushcfunction(L, equals);
		lua_setfield(L, -2, "__eq");

		lua_pushcfunction(L, add);
		lua_setfield(L, -2, "__add");

		lua_pushstring(L, lib_name);
		lua_setfield(L, -2, "__name");

		create_methods(L);

		lua_pushcfunction(L, index);
		lua_setfield(L, -2, "__index");
	}
	lua_pop(L, 1);

	create_lib(L);

	lua_pushcfunction(L, xl_xstr);
	lua_setglobal(L, "_xstr");

	lua_pushcfunction(L, xl_to_string);
	lua_setglobal(L, "to_string");

	return 0;
}