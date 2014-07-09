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
#include "ender_js_sm_string_private.h"
#include "ender_js_sm_function_private.h"
#include "ender_js_sm_item_private.h"
#include "ender_js_sm_instance_private.h"
#include "ender_js_sm_value_private.h"
/*
 * TODO
 * When getting a property first the resolve method is called. In case the
 * script wants to set/get a value, the get/setProperty is called and then
 * we need to search again for the property. This is not efficient, we need
 * to know a way to cache the first resolve.
 */
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

static JSBool _ender_js_sm_instance_string_to(JSContext *cx, uintN argc, jsval *vp)
{
	Ender_Js_Sm_Instance *thiz;
	JSObject *jsthis;
	Eina_Bool ok;
	Eina_Error err;
	char *str = NULL;

	jsthis = JSVAL_TO_OBJECT(JS_THIS(cx, vp));
	thiz = JS_GetPrivate(cx, jsthis);

	if (!thiz->o)
	{
		ERR("No object in instance, nothing to do");
		return JS_FALSE;
	}

	ok = ender_item_object_string_to(thiz->i, thiz->o, &str, NULL, &err);
	if (ok)
	{
		jsval retval;

		DBG("String representation is %s", str);
		retval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, str));
		/* TODO in case the transfer is full, free it */
		JS_SET_RVAL(cx, vp, retval);
		return JS_TRUE;
	}
	else
	{
		/* TODO define the class like [foo Foo] or something like that */
		ERR("Converting to string '%s' failed", ender_item_name_get(thiz->i));
		return JS_FALSE;
	}
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
	char *name;
	char *conv_name;

	thiz = JS_GetPrivate(cx, obj);
	if (!thiz)
		return JS_FALSE;

	if (!ender_js_sm_string_id_get(cx, id, &name))
		return JS_FALSE;

	if (!strcmp(name, "valueOf") || !strcmp(name, "toString"))
	{
		ret = JS_TRUE;
		goto no_prop;
	}

	conv_name = ender_utils_name_convert(name, ENDER_CASE_CAMEL, ENDER_NOTATION_ENGLISH,
			ENDER_CASE_UNDERSCORE, ENDER_NOTATION_LATIN); 
	DBG("Looking for '%s' ('%s') in '%s'", name, conv_name, ender_item_name_get(thiz->i));
	
	/* for properties we do get the value */
	i = _ender_js_object_prop_get(thiz->i, conv_name);
	if (i)
	{
		Eina_Bool ok;
		Eina_Error err;
		Ender_Value v = { 0 };

		/* FIXME use the correct direction/transfer */
		ok = ender_item_attr_value_get(i, thiz->o, NULL, &v, &err);
		if (ok)
		{
			Ender_Item *type;
			jsval vret = JSVAL_VOID;

			type = ender_item_attr_type_get(i);
			ok = ender_js_sm_value_to_jsval(cx, type, ENDER_ITEM_ARG_DIRECTION_IN, ENDER_ITEM_TRANSFER_FULL, &v, &vret);
			if (ok)
			{
				JS_SET_RVAL(cx, vp, vret);
				ret = JS_TRUE;
			}
			ender_item_unref(type);
			ender_item_unref(i);
		}
		else
		{
			/* TODO Send the error */
			ender_item_unref(i);
		}
	}
	else
	{
		ret = JS_TRUE;
	}
	free(conv_name);
no_prop:
	free(name);
	return ret;
}

static JSBool _ender_js_sm_instance_class_set_property(JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp)
{
	Ender_Js_Sm_Instance *thiz;
	Ender_Item *i;
	JSBool ret = JS_FALSE;
	char *name;
	char *conv_name;

	thiz = JS_GetPrivate(cx, obj);
	if (!thiz)
		return JS_FALSE;

	if (!ender_js_sm_string_id_get(cx, id, &name))
		return JS_FALSE;

	conv_name = ender_utils_name_convert(name, ENDER_CASE_CAMEL, ENDER_NOTATION_ENGLISH,
			ENDER_CASE_UNDERSCORE, ENDER_NOTATION_LATIN); 
	DBG("Looking for '%s' ('%s') in '%s'", name, conv_name, ender_item_name_get(thiz->i));
	
	/* for properties we do get the value */
	i = _ender_js_object_prop_get(thiz->i, conv_name);
	if (i)
	{
		Eina_Bool ok;
		Eina_Error err;
		Ender_Item *type;
		Ender_Value v = { 0 };

		type = ender_item_attr_type_get(i);
		/* FIXME use the correct direction/transfer */
		ok = ender_js_sm_value_from_jsval(cx, type, ENDER_ITEM_ARG_DIRECTION_IN, ENDER_ITEM_TRANSFER_FULL, &v, vp[0]);
		ender_item_unref(type);
		if (ok)
		{
			ok = ender_item_attr_value_set(i, thiz->o, &v, &err);
			ender_item_unref(i);
			ret = JS_TRUE;
		}
		else
		{
			/* TODO Send the error */
			ender_item_unref(i);
		}
	}
	else
	{
		ret = JS_TRUE;
	}
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
	char *name;
	char *conv_name;

	/* initialize */
	*objp = NULL;

	thiz = JS_GetPrivate(cx, obj);
	if (!thiz)
		return JS_FALSE;

	if (!ender_js_sm_string_id_get(cx, id, &name))
		return JS_FALSE;

	if (!strcmp(name, "__iterator__"))
		goto no_conv;

	/* for valueOf or toString use the string_to ender function */
	if (!strcmp(name, "valueOf") || !strcmp(name, "toString"))
	{
		DBG("Resolving the value of an object (%s)", name);
		JS_DefineFunction(cx, obj, name, _ender_js_sm_instance_string_to, 0, 0);
		*objp = obj;
		ret = JS_TRUE;
		goto no_conv;
	}

	conv_name = ender_utils_name_convert(name, ENDER_CASE_CAMEL, ENDER_NOTATION_ENGLISH,
			ENDER_CASE_UNDERSCORE, ENDER_NOTATION_LATIN); 
	DBG("Looking for '%s' ('%s') in '%s'", name, conv_name, ender_item_name_get(thiz->i));

	/* only resolve properties and methods */
	i = _ender_js_object_prop_get(thiz->i, conv_name);
	if (i)
	{
		DBG("Property '%s' found", ender_item_name_get(i));
		*objp = obj;
		ret = JS_TRUE;
		ender_item_unref(i);
		goto done;
	}

	i = _ender_js_object_method_get(thiz->i, conv_name);
	if (i)
	{
		JSObject *oi;

		DBG("Method '%s' found", ender_item_name_get(i));
		oi = ender_js_sm_item_create(cx, obj, i);
		JS_DefinePropertyById(cx, obj, id, OBJECT_TO_JSVAL(oi), NULL,
				NULL, 0);
		*objp = obj;
		ret = JS_TRUE;
		goto done;
	}
	WRN("No item '%s' found in '%s'", conv_name, ender_item_name_get(thiz->i));
	JS_ReportError(cx, "Instance has no property '%s'", name);
done:
	free(conv_name);
no_conv:
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
Eina_Bool ender_js_sm_is_instance(JSContext *cx, JSObject *obj)
{
	JSClass *klass;

	klass = JS_GetClass(cx, obj);
	if (strcmp(klass->name, "ender_js_sm_instance"))
		return EINA_FALSE;
	else
		return EINA_TRUE;
}

void * ender_js_sm_instance_ptr_get(JSContext *cx, JSObject *obj)
{
	Ender_Js_Sm_Instance *thiz;

	thiz = JS_GetPrivate(cx, obj);
	return thiz->o;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI JSObject * ender_js_sm_instance_new(JSContext *cx, Ender_Item *i, void *o)
{
	Ender_Js_Sm_Instance *thiz;
	JSObject *obj;

	if (!i) return NULL;

	/* check if the item i supports downcast, if so, get the real
	 * item and unref i
	 */
	if (ender_item_type_get(i) == ENDER_ITEM_TYPE_OBJECT)
	{
		Ender_Item *downcast;

		downcast = ender_item_object_downcast(i, o);
		if (downcast)
		{
			DBG("Downcasting from %s to %s", ender_item_name_get(i),
					ender_item_name_get(downcast));
			ender_item_unref(i);
			i = downcast;
		}
	}

	obj = JS_NewObject(cx, &_ender_js_sm_instance_class, NULL, NULL);

	thiz = calloc(1, sizeof(Ender_Js_Sm_Instance));
	thiz->i = i;
	thiz->o = o;
	JS_SetPrivate(cx, obj, thiz);

	return obj;
}
