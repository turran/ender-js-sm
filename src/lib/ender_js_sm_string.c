/* Ender Js SM - JavaScript bindings for Ender (SpiderMonkey based)
 * Copyright (C) 2014 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#include "Ender_Js_Sm.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
Eina_Bool ender_js_sm_string_get(JSContext *cx, const jsval value,
		char **out)
{
	JSString *str;
	size_t len;
	char *bytes;

	JS_BeginRequest(cx);

	if (!JSVAL_IS_STRING(value))
	{
		JS_EndRequest(cx);
		return EINA_FALSE;
	}

	str = JSVAL_TO_STRING(value);

	len = JS_GetStringEncodingLength(cx, str);
	if (len < 0)
	{
		JS_EndRequest(cx);
		return EINA_FALSE;
	}

	if (out)
	{
		bytes = JS_EncodeString(cx, str);
		*out = bytes;
	}

	JS_EndRequest(cx);
	return EINA_TRUE;
}

 
Eina_Bool ender_js_sm_string_id_get(JSContext *cx, jsid id, char **out)
{
	jsval id_val;

	if (!JS_IdToValue(cx, id, &id_val))
		return EINA_FALSE;

	if (JSVAL_IS_STRING(id_val))
	{
		return ender_js_sm_string_get(cx, id_val, out);
	}
	else 
	{
		*out = NULL;
		return EINA_FALSE;
	}
}
