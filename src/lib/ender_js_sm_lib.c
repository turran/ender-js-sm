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
#include "ender_js_sm_private.h"
#include "ender_js_sm_item_private.h"
#include "ender_js_sm_string_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Class definition                               *
 *----------------------------------------------------------------------------*/
/* TODO
 * Store the lib on the private data
 */
static JSBool _ender_js_sm_lib_class_resolve(JSContext *cx, JSObject *obj, jsid id,
		uintN flags, JSObject **objp)
{
	JSBool ret = JS_FALSE;
	const Ender_Lib *lib;
	Ender_Item *item;
	char *name;

	/* initialize */
	*objp = NULL;

	lib = JS_GetPrivate(cx, obj);
	if (!lib) return JS_FALSE;

	if (!ender_js_sm_string_id_get(cx, id, &name))
		return JS_FALSE;

	/* find the item, if not found, try to prepend the lib name */
	item = ender_lib_item_find(lib, name);
	if (!item)
	{
		const char *lib_name;
		char *item_name;

		lib_name = ender_lib_name_get(lib);
		if (asprintf(&item_name, "%s.%s", lib_name, name) > 0)
		{
			item = ender_lib_item_find(lib, item_name);
			free(item_name);
		}
	}

	if (item)
	{
		JSObject *oi;

		DBG("Item found '%s' when looking for '%s'", ender_item_name_get(item), name);
		oi = ender_js_sm_item_create(cx, item);

		JS_DefinePropertyById(cx, obj, id, OBJECT_TO_JSVAL(oi), NULL,
				NULL, 0);
		*objp = obj;
		ret = JS_TRUE;
	}
	free(name);
	return ret;
}

static JSClass _ender_js_sm_class_lib = {
	/* .name  		= */ "ender_js_sm_lib",
	/* .flags 		= */ JSCLASS_HAS_PRIVATE | JSCLASS_NEW_RESOLVE | JSCLASS_NEW_RESOLVE_GETS_START,
	/* .addProperty 	= */ JS_PropertyStub,
	/* .delProperty 	= */ JS_PropertyStub,
	/* .getProperty 	= */ JS_PropertyStub,
	/* .setProperty 	= */ JS_StrictPropertyStub,
	/* .enumarate 		= */ JS_EnumerateStub,
	/* .resolve 		= */ (JSResolveOp) _ender_js_sm_lib_class_resolve,
	/* .convert 		= */ JS_ConvertStub,
	/* .finalize 		= */ NULL,
	/* .reserved 		= */ NULL,
	/* .checkAccess		= */ NULL,
	/* .call 		= */ NULL,
	/* .construct 		= */ NULL,
	/* .xdrObject		= */ NULL,
	/* .hasInstance 	= */ NULL,
	/* .trace 		= */ NULL,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
JSObject * ender_js_sm_lib_new(JSContext *cx, const Ender_Lib *lib)
{
	JSObject *ret;

	ret = JS_NewObject(cx, &_ender_js_sm_class_lib, NULL, NULL);
	JS_SetPrivate(cx, ret, (void *)lib);
	return ret;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

