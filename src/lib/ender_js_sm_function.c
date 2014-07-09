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
#include "Ender_Js_Sm.h"
#include "ender_js_sm_item_private.h"
#include "ender_js_sm_instance_private.h"
#include "ender_js_sm_function_private.h"
#include "ender_js_sm_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _ender_js_sm_function_arg_to_jsval(JSContext *cx, Ender_Item *arg,
		Ender_Value *v, jsval *jv)
{
	Ender_Item *type;
	Ender_Item_Arg_Direction dir;
	Ender_Item_Transfer xfer;
	Eina_Bool ret = EINA_FALSE;

	type = ender_item_arg_type_get(arg);

	if (!type)
	{
		ERR("No type found");
		v->ptr = NULL;
		return EINA_FALSE;
	}

	dir = ender_item_arg_direction_get(arg);
	xfer = ender_item_arg_transfer_get(arg);
	ret = ender_js_sm_value_to_jsval(cx, type, dir, xfer, v, jv);
	ender_item_unref(type);

	return ret;
}

static Eina_Bool _ender_js_sm_function_arg_from_jsval(JSContext *cx, Ender_Item *arg,
		Ender_Value *v, jsval jv)
{
	Ender_Item *type;
	Ender_Item_Arg_Direction dir;
	Ender_Item_Transfer xfer;
	Eina_Bool ret = EINA_FALSE;

	type = ender_item_arg_type_get(arg);

	if (!type)
	{
		ERR("No type found");
		v->ptr = NULL;
		return EINA_FALSE;
	}

	dir = ender_item_arg_direction_get(arg);
	xfer = ender_item_arg_transfer_get(arg);
	ret = ender_js_sm_value_from_jsval(cx, type, dir, xfer, v, jv);
	ender_item_unref(type);

	return ret;
}
/*----------------------------------------------------------------------------*
 *                             Class definition                               *
 *----------------------------------------------------------------------------*/
static void _ender_js_sm_function_class_finalize(JSContext *cx, JSObject *obj)
{
	ender_js_sm_item_destroy(cx, obj);
}

static JSBool _ender_js_sm_function_class_call(JSContext *cx, uintN argc, jsval *vp)
{
	JSObject *callee;
	jsval retval = JSVAL_NULL;
	Ender_Item *item;

	callee = JSVAL_TO_OBJECT(JS_CALLEE(cx, vp));
	item = JS_GetPrivate(cx, callee);

	if (ender_js_sm_function_call(cx, callee, item, argc, JS_ARGV(cx, vp), &retval))
	{
		JS_SET_RVAL(cx, vp, retval);
		return JS_TRUE;
	}
	else
	{
		return JS_FALSE;
	}
}

static JSClass _ender_js_sm_function_class = {
	/* .name  		= */ "ender_js_sm_function",
	/* .flags 		= */ JSCLASS_HAS_PRIVATE,
	/* .addProperty 	= */ JS_PropertyStub,
	/* .delProperty 	= */ JS_PropertyStub,
	/* .getProperty 	= */ JS_PropertyStub,
	/* .setProperty 	= */ JS_StrictPropertyStub,
	/* .enumarate 		= */ JS_EnumerateStub,
	/* .resolve 		= */ JS_ResolveStub,
	/* .convert 		= */ JS_ConvertStub,
	/* .finalize 		= */ _ender_js_sm_function_class_finalize,
	/* .reserved 		= */ NULL,
	/* .checkAccess		= */ NULL,
	/* .call 		= */ _ender_js_sm_function_class_call,
	/* .construct 		= */ NULL,
	/* .xdrObject		= */ NULL,
	/* .hasInstance 	= */ NULL,
	/* .trace 		= */ NULL,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool ender_js_sm_function_call(JSContext *cx, JSObject *callee, Ender_Item *i, int argc, jsval *argv, jsval *vret)
{
	JSObject *parent = NULL;
	Ender_Value *eargv;
	Ender_Value eret = { 0 };
	Ender_Item *arg;
	Eina_List *args;
	Eina_Bool ret;
	int flags;
	int nargs;
	int idx_ender = 0;
	int idx_js = 0;
	int idx_throw = -1;

	DBG("Calling function '%s'", ender_item_name_get(i));
	/* Check if it is a method, and if so, we will pass one more
	 * argument
	 */
	flags = ender_item_function_flags_get(i);
	if (flags & ENDER_ITEM_FUNCTION_FLAG_IS_METHOD)
	{
		if (!callee)
			return EINA_FALSE;
		parent = JS_GetParent(cx, callee);
		if (!parent)
			return EINA_FALSE;

		if (!ender_js_sm_is_instance(cx, parent))
			return EINA_FALSE;
		argc++;
	}
	/* in case we throw, increase the number of passed in args */
	if (flags & ENDER_ITEM_FUNCTION_FLAG_THROWS)
	{
		idx_throw = ender_item_function_throw_position_get(i);
		argc++;
	}
	nargs = ender_item_function_args_count(i);
	if (argc != nargs)
	{
		JS_ReportError(cx, "Invalid number of arguments");
		JS_ReportPendingException(cx);

		/* Make sure the error is uncatchable. */
		JS_ClearPendingException(cx);
		return EINA_FALSE;
	}

	/* convert the passed in args to ender values */
	eargv = calloc(nargs, sizeof(Ender_Value));
	args = ender_item_function_args_get(i);
	/* set self */
	if (flags & ENDER_ITEM_FUNCTION_FLAG_IS_METHOD)
	{
		eargv[idx_ender].ptr = ender_js_sm_instance_ptr_get(cx, parent);
		idx_ender++;

		ender_item_unref(args->data);
		args = eina_list_remove_list(args, args);
	}

	EINA_LIST_FREE(args, arg)
	{
		/* skip the error in case we need to */
		if (idx_ender == idx_throw)
		{
			DBG("Skipping the error arg at %d", idx_throw);
			goto next;
		}
		_ender_js_sm_function_arg_from_jsval(cx, arg, &eargv[idx_ender], argv[idx_js]);
		idx_js++;
next:
		ender_item_unref(arg);
		idx_ender++;
	}

	ret = ender_item_function_call(i, eargv, &eret);
	free(args);

	if (!ret)
	{
		return EINA_FALSE;
	}
	/* convert the return type to a val */
	arg = ender_item_function_ret_get(i);
	if (arg)
	{
		_ender_js_sm_function_arg_to_jsval(cx, arg, &eret, vret);
		ender_item_unref(arg);
	}

	return EINA_TRUE;
}

JSObject * ender_js_sm_function_new(JSContext *cx, JSObject *parent, Ender_Item *i)
{
	JSObject *ret;

	ret = JS_NewObject(cx, &_ender_js_sm_function_class, NULL, parent);
	JS_SetPrivate(cx, ret, i);
	return ret;
}
