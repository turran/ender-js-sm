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
#include "ender_js_sm_enum_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Item * _ender_js_enum_value_get(Ender_Item *i, const char *name)
{
	Eina_List *items;
	Ender_Item *ret = NULL;
	Ender_Item *item;

	items = ender_item_enum_values_get(i);
	EINA_LIST_FREE(items, item)
	{
		const char *iname;

		iname = ender_item_name_get(item);
		if (!ret && !strcmp(name, iname))
			ret = ender_item_ref(item);
		ender_item_unref(item);
	}
	return ret;
}
/*----------------------------------------------------------------------------*
 *                             Class definition                               *
 *----------------------------------------------------------------------------*/
static void _ender_js_sm_enum_class_finalize(JSContext *cx, JSObject *obj)
{
	ender_js_sm_item_destroy(cx, obj);
}

static JSBool _ender_js_sm_enum_class_get_property(JSContext *cx, JSObject *obj, jsid id, jsval *vp)
{
	JSBool ret = JS_FALSE;
	Ender_Item *item;
	Ender_Item *i;
	char *name;
	char *conv_name;

	item = JS_GetPrivate(cx, obj);
	if (!item)
		return JS_FALSE;

	if (!ender_js_sm_string_id_get(cx, id, &name))
		return JS_FALSE;

	conv_name = ender_utils_to_lower(name);
	DBG("Looking for '%s' ('%s') in '%s'", name, conv_name, ender_item_name_get(item));
	
	/* for properties we do get the value */
	i = _ender_js_enum_value_get(item, conv_name);
	if (i)
	{
		Ender_Value v;

		ender_item_constant_value_get(i, &v);
		JS_SET_RVAL(cx, vp, INT_TO_JSVAL(v.i32));
		ender_item_unref(i);
	}
	ret = JS_TRUE;

	free(conv_name);
	free(name);
	return ret;
}


static JSBool _ender_js_sm_enum_class_resolve(JSContext *cx, JSObject *obj, jsid id,
		uintN flags, JSObject **objp)
{
	JSBool ret = JS_FALSE;
	Ender_Item *item;
	Ender_Item *i;
	char *name;
	char *conv_name;

	/* initialize */
	*objp = NULL;

	item = JS_GetPrivate(cx, obj);
	if (!item) return JS_FALSE;

	if (!ender_js_sm_string_id_get(cx, id, &name))
		return JS_FALSE;

	conv_name = ender_utils_to_lower(name);
	DBG("Looking for '%s' ('%s') in '%s'", name, conv_name, ender_item_name_get(item));

	/* only resolve values */
	i = _ender_js_enum_value_get(item, conv_name);
	if (i)
	{
		*objp = obj;
		ret = JS_TRUE;
		ender_item_unref(i);
	}

	free(conv_name);
	free(name);
	return ret;
}

static JSClass _ender_js_sm_enum_class = {
	/* .name  		= */ "ender_js_sm_enum",
	/* .flags 		= */ JSCLASS_HAS_PRIVATE | JSCLASS_NEW_RESOLVE | JSCLASS_NEW_RESOLVE_GETS_START,
	/* .addProperty 	= */ JS_PropertyStub,
	/* .delProperty 	= */ JS_PropertyStub,
	/* .getProperty 	= */ _ender_js_sm_enum_class_get_property,
	/* .setProperty 	= */ JS_StrictPropertyStub,
	/* .enumarate 		= */ JS_EnumerateStub,
	/* .resolve 		= */ (JSResolveOp) _ender_js_sm_enum_class_resolve,
	/* .convert 		= */ JS_ConvertStub,
	/* .finalize 		= */ _ender_js_sm_enum_class_finalize,
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
JSObject * ender_js_sm_enum_new(JSContext *cx, Ender_Item *i)
{
	JSObject *ret;

	ret = JS_NewObject(cx, &_ender_js_sm_enum_class, NULL, NULL);
	JS_SetPrivate(cx, ret, i);
	return ret;
}
