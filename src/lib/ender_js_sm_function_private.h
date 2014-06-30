/* Ender Js SM - Ender bindings for JavaScript (SpiderMonkey based)
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

#ifndef _ENDER_JS_SM_FUNCTION_PRIVATE_H_
#define _ENDER_JS_SM_FUNCTION_PRIVATE_H_

JSObject * ender_js_sm_function_new(JSContext *cx, Ender_Item *i);
Eina_Bool ender_js_sm_function_call(JSContext *cx, Ender_Item *i, int argc, jsval *argv, jsval *ret);

#endif
