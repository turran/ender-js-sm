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

#ifndef _ENDER_JS_SM_VALUE_PRIVATE_H_
#define _ENDER_JS_SM_VALUE_PRIVATE_H_

Eina_Bool ender_js_sm_value_to_jsval(JSContext *cx,
		Ender_Item *type, Ender_Item_Arg_Direction dir,
		Ender_Item_Transfer xfer, Ender_Value *v, jsval *jv);
Eina_Bool ender_js_sm_value_from_jsval(JSContext *cx,
		Ender_Item *type, Ender_Item_Arg_Direction dir,
		Ender_Item_Transfer xfer, Ender_Value *v, jsval jv);
#endif
