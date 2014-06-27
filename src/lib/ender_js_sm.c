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
#include "Ender_Js_Sm.h"
#include "ender_js_sm_string_private.h"
#include "ender_js_sm_lib_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Class definition                               *
 *----------------------------------------------------------------------------*/
static JSBool _ender_js_sm_resolve(JSContext *cx, JSObject *obj, jsid id,
		uintN flags, JSObject **objp)
{
	JSBool ret = JS_FALSE;
	const Ender_Lib *lib;
	char *name;

	/* initialize */
	*objp = NULL;

	if (!ender_js_sm_string_id_get(cx, id, &name))
		return JS_FALSE;
	if (!strcmp(name, "valueOf") ||
		!strcmp(name, "toString") ||
		!strcmp(name, "__iterator__"))
		goto done;

	JS_BeginRequest(cx);
	/* TODO check what is the request on the flags */
	lib = ender_lib_find(name);
	if (lib)
	{
		JSObject *olib;
		jsval val;

		/* create an object for a lib */
		olib = ender_js_sm_lib_new(cx, obj, lib);
		JS_DefinePropertyById(cx, obj, id, OBJECT_TO_JSVAL(olib), NULL,
				NULL, JSPROP_READONLY | JSPROP_PERMANENT);
		*objp = obj;
		ret = JS_TRUE;
	}
	JS_EndRequest(cx);

done:
	free(name);
	return ret;
}

static JSClass _ender_js_sm_class = {
	/* .name  		= */ "ender_js_sm",
	/* .flags 		= */ JSCLASS_NEW_RESOLVE | JSCLASS_NEW_RESOLVE_GETS_START,
	/* .addProperty 	= */ JS_PropertyStub,
	/* .delProperty 	= */ JS_PropertyStub,
	/* .getProperty 	= */ JS_PropertyStub,
	/* .setProperty 	= */ JS_StrictPropertyStub,
	/* .enumarate 		= */ JS_EnumerateStub,
	/* .resolve 		= */ (JSResolveOp) _ender_js_sm_resolve,
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
Eina_Bool ender_js_sm_item_create(JSContext *cx, JSObject *parent, Ender_Item *i)
{
	Ender_Item_Type type;
	Eina_Bool ret = EINA_TRUE;

	type = ender_item_type_get(i);
	switch (type)
	{
		case ENDER_ITEM_TYPE_FUNCTION:
		break;

		default:
		ret = EINA_FALSE;
		break;
	}
	return ret;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void ender_js_sm_init(void)
{
	ender_init();
}

EAPI void ender_js_sm_shutdown(void)
{
	ender_shutdown();
}

EAPI JSClass * ender_js_sm_class_get(void)
{
	return &_ender_js_sm_class;
}
