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
#include "ender_js_sm_function_private.h"
#include "ender_js_sm_item_private.h"
#include "ender_js_sm_instance_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Ender_Js_Sm_Instance
{
	Ender_Item *i;
	void *o;
} Ender_Js_Sm_Instance;

static Ender_Item * _ender_js_object_prop_get(Ender_Item *i, const char *name)
{
	Eina_List *items;
	Ender_Item *ret = NULL;
	Ender_Item *item;

	items = ender_item_object_props_get(i);
	EINA_LIST_FREE(items, item)
	{
		const char *iname;

		iname = ender_item_name_get(item);
		if (!ret && !strcmp(name, iname))
			ret = ender_item_ref(item);
		ender_item_unref(item);
	}
	if (!ret)
	{
		Ender_Item *inherit;

		inherit = ender_item_object_inherit_get(i);
		if (inherit)
		{
			ret = _ender_js_object_prop_get(inherit, name);
			ender_item_unref(inherit);
		}
	}

	return ret;
}

static Ender_Item * _ender_js_object_method_get(Ender_Item *i, const char *name)
{
	Eina_List *items;
	Ender_Item *ret = NULL;
	Ender_Item *item;

	items = ender_item_object_functions_get(i);
	EINA_LIST_FREE(items, item)
	{
		int flags;
		const char *iname;

		flags = ender_item_function_flags_get(item);
		if (!(flags & ENDER_ITEM_FUNCTION_FLAG_IS_METHOD))
			goto next;
		iname = ender_item_name_get(item);
		if (!ret && !strcmp(name, iname))
			ret = ender_item_ref(item);
next:
		ender_item_unref(item);
	}
	if (!ret)
	{
		Ender_Item *inherit;

		inherit = ender_item_object_inherit_get(i);
		if (inherit)
		{
			ret = _ender_js_object_method_get(inherit, name);
			ender_item_unref(inherit);
		}
	}

	return ret;

}
/*----------------------------------------------------------------------------*
 *                             Class definition                               *
 *----------------------------------------------------------------------------*/
static void _ender_js_sm_instance_class_finalize(JSContext *cx, JSObject *obj)
{
	Ender_Js_Sm_Instance *thiz;

	thiz = JS_GetPrivate(cx, obj);
	ender_item_object_unref(thiz->i, thiz->o);
	ender_item_unref(thiz->i);
	
	free(thiz);
}

static JSBool _ender_js_sm_instance_class_get_property(JSContext *cx, JSObject *obj, jsid id, jsval *vp)
{
	Ender_Js_Sm_Instance *thiz;
	Ender_Item *i;
	JSBool ret = JS_FALSE;
	jsid own_id;
	Eina_List *items;
	char *name;
	char *conv_name;
	char *item_name;

	thiz = JS_GetPrivate(cx, obj);
	if (!thiz)
		return JS_FALSE;

	if (!ender_js_sm_string_id_get(cx, id, &name))
		return JS_FALSE;

	conv_name = ender_utils_name_convert(name, ENDER_CASE_CAMEL, ENDER_NOTATION_ENGLISH,
			ENDER_CASE_UNDERSCORE, ENDER_NOTATION_LATIN); 
	ERR("Looking for '%s' ('%s') in '%s'", name, conv_name, ender_item_name_get(thiz->i));
	
	/* for properties we do get the value */
	i = _ender_js_object_prop_get(thiz->i, conv_name);
	if (i)
	{
		ender_item_unref(i);
		ret = JS_TRUE;
	}
	else
	{
		ret = JS_TRUE;
	}
done:
	free(conv_name);
	free(name);
	return ret;
}

static JSBool _ender_js_sm_instance_class_set_property(JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp)
{
	Ender_Js_Sm_Instance *thiz;
	Ender_Item *i;
	JSBool ret = JS_FALSE;
	jsid own_id;
	char *name;
	char *conv_name;
	char *item_name;

	thiz = JS_GetPrivate(cx, obj);
	if (!thiz)
		return JS_FALSE;

	if (!ender_js_sm_string_id_get(cx, id, &name))
		return JS_FALSE;

	conv_name = ender_utils_name_convert(name, ENDER_CASE_CAMEL, ENDER_NOTATION_ENGLISH,
			ENDER_CASE_UNDERSCORE, ENDER_NOTATION_LATIN); 
	ERR("Looking for '%s' ('%s') in '%s'", name, conv_name, ender_item_name_get(thiz->i));
	
	/* for properties we do get the value */
	i = _ender_js_object_prop_get(thiz->i, conv_name);
	if (i)
	{
		ender_item_unref(i);
		ret = JS_TRUE;
	}
	else
	{
		ret = JS_TRUE;
	}
done:
	free(conv_name);
	free(name);
	return ret;
}

static JSBool _ender_js_sm_instance_class_resolve(JSContext *cx, JSObject *obj, jsid id,
		uintN flags, JSObject **objp)
{
	Ender_Js_Sm_Instance *thiz;
	Ender_Item *i;
	JSBool ret = JS_FALSE;
	jsid own_id;
	Eina_List *items;
	char *name;
	char *conv_name;
	char *item_name;

	/* initialize */
	*objp = NULL;

	thiz = JS_GetPrivate(cx, obj);
	if (!thiz)
		return JS_FALSE;

	if (!ender_js_sm_string_id_get(cx, id, &name))
		return JS_FALSE;

	conv_name = ender_utils_name_convert(name, ENDER_CASE_CAMEL, ENDER_NOTATION_ENGLISH,
			ENDER_CASE_UNDERSCORE, ENDER_NOTATION_LATIN); 
	ERR("Looking for '%s' ('%s') in '%s'", name, conv_name, ender_item_name_get(thiz->i));
	
	/* only resolve properties and methods */
	i = _ender_js_object_prop_get(thiz->i, conv_name);
	if (i)
	{
		*objp = obj;
		ret = JS_TRUE;
		ender_item_unref(i);
		goto done;
	}

	i = _ender_js_object_method_get(thiz->i, conv_name);
	if (i)
	{
		JSObject *oi;

		oi = ender_js_sm_item_create(cx, obj, i);
		JS_DefinePropertyById(cx, obj, id, OBJECT_TO_JSVAL(oi), NULL,
				NULL, 0);
		*objp = obj;
		ret = JS_TRUE;
	}

done:
	free(conv_name);
	free(name);
	return ret;
}

static JSClass _ender_js_sm_instance_class = {
	/* .name  		= */ "ender_js_sm_instance",
	/* .flags 		= */ JSCLASS_HAS_PRIVATE | JSCLASS_NEW_RESOLVE | JSCLASS_NEW_RESOLVE_GETS_START,
	/* .addProperty 	= */ JS_PropertyStub,
	/* .delProperty 	= */ JS_PropertyStub,
	/* .getProperty 	= */ _ender_js_sm_instance_class_get_property,
	/* .setProperty 	= */ _ender_js_sm_instance_class_set_property,
	/* .enumarate 		= */ JS_EnumerateStub,
	/* .resolve 		= */ (JSResolveOp) _ender_js_sm_instance_class_resolve,
	/* .convert 		= */ JS_ConvertStub,
	/* .finalize 		= */ _ender_js_sm_instance_class_finalize,
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
void ender_js_sm_instance_new(JSContext *cx, Ender_Item *i, void *o, jsval *val)
{
	Ender_Js_Sm_Instance *thiz;
	JSObject *obj;

	obj = JS_NewObject(cx, &_ender_js_sm_instance_class, NULL, NULL);

	thiz = calloc(1, sizeof(Ender_Js_Sm_Instance));
	thiz->i = ender_item_ref(i);
	thiz->o = o;
	JS_SetPrivate(cx, obj, thiz);

	*val = OBJECT_TO_JSVAL(obj);
}
