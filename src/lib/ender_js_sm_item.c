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

#include "ender_js_sm_private.h"
#include "ender_js_sm_item_private.h"
#include "ender_js_sm_object_private.h"
#include "ender_js_sm_enum_private.h"
#include "ender_js_sm_function_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void ender_js_sm_item_destroy(JSContext *cx, JSObject *obj)
{
	Ender_Item *i;

	i = JS_GetPrivate(cx, obj);
	ender_item_unref(i);
}

JSObject * ender_js_sm_item_create(JSContext *cx, JSObject *parent, Ender_Item *i)
{
	JSObject *ret = NULL;
	Ender_Item_Type type;

	type = ender_item_type_get(i);
	switch (type)
	{
		case ENDER_ITEM_TYPE_FUNCTION:
		ret = ender_js_sm_function_new(cx, parent, i);
		break;

		case ENDER_ITEM_TYPE_OBJECT:
		ret = ender_js_sm_object_new(cx, i);
		break;

		case ENDER_ITEM_TYPE_ENUM:
		ret = ender_js_sm_enum_new(cx, i);
		break;

		default:
		ERR("Unsupported type '%s'", ender_item_type_name_get(type));
		ender_item_unref(i);
		break;
	}
	return ret;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

