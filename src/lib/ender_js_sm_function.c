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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _ender_js_sm_function_value_to_jsval(Ender_Item *type,
		Ender_Item_Arg_Direction dir, Ender_Item_Arg_Transfer xfer,
		Ender_Value *v, jsval *jv)
{
	Eina_Bool ret = EINA_FALSE;

	/* handle the in/out direction */
	if (dir == ENDER_ITEM_ARG_DIRECTION_IN)
	{
		Ender_Item_Type it;

		it = ender_item_type_get(type);
		switch (it)
		{
			case ENDER_ITEM_TYPE_DEF:
			{
				Ender_Item *other;

				other = ender_item_def_type_get(type);
				ret = _ender_js_sm_function_value_to_jsval(other, dir, xfer, v, jv);
			}
			break;

			case ENDER_ITEM_TYPE_BASIC:
			case ENDER_ITEM_TYPE_ENUM:
			case ENDER_ITEM_TYPE_OBJECT:
			case ENDER_ITEM_TYPE_STRUCT:
			default:
			ERR("Type %s", ender_item_type_name_get(it));
			break;
		}
	}
	else
	{
		/* for in/inout directions we always pass a pointer */
		ERR("Unsupported direction %d", dir);
	}
	ender_item_unref(type);
	return ret;
}

static Eina_Bool _ender_js_sm_function_value_from_jsval(Ender_Item *type,
		Ender_Item_Arg_Direction dir, Ender_Item_Arg_Transfer xfer,
		Ender_Value *v, jsval *jv)
{
	Eina_Bool ret = EINA_FALSE;

	/* handle the in/out direction */
	if (dir == ENDER_ITEM_ARG_DIRECTION_IN)
	{
		Ender_Item_Type it;

		it = ender_item_type_get(type);
		switch (it)
		{
			case ENDER_ITEM_TYPE_DEF:
			{
				Ender_Item *other;

				other = ender_item_def_type_get(type);
				ret = _ender_js_sm_function_value_from_jsval(other, dir, xfer, v, jv);
			}
			break;

			case ENDER_ITEM_TYPE_BASIC:
			case ENDER_ITEM_TYPE_ENUM:
			case ENDER_ITEM_TYPE_OBJECT:
			case ENDER_ITEM_TYPE_STRUCT:
			default:
			ERR("Type %s", ender_item_type_name_get(it));
			break;
		}
	}
	else
	{
		/* for in/inout directions we always pass a pointer */
		ERR("Unsupported direction %d", dir);
	}
	ender_item_unref(type);
	return ret;
}

static Eina_Bool _ender_js_sm_function_arg_to_jsval(Ender_Item *arg,
		Ender_Value *v, jsval *jv)
{
	Ender_Item *type;
	Ender_Item_Arg_Direction dir;
	Ender_Item_Arg_Transfer xfer;
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
	ret = _ender_js_sm_function_value_to_jsval(type, dir, xfer, v, jv);

	return ret;
}

static Eina_Bool _ender_js_sm_function_arg_from_jsval(Ender_Item *arg,
		Ender_Value *v, jsval *jv)
{
	Ender_Item *type;
	Ender_Item_Arg_Direction dir;
	Ender_Item_Arg_Transfer xfer;
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
	ret = _ender_js_sm_function_value_from_jsval(type, dir, xfer, v, jv);

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
	jsval retval;
	Ender_Item *item;

	callee = JSVAL_TO_OBJECT(JS_CALLEE(cx, vp));
	item = JS_GetPrivate(cx, callee);

	if (ender_js_sm_function_call(cx, item, argc, vp, &retval))
	{
		return JS_TRUE;
	}
	else
	{
		return JS_FALSE;
	}
}

static JSClass _ender_js_sm_class_function = {
	/* .name  		= */ "ender_js_sm_lib",
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
Eina_Bool ender_js_sm_function_call(JSContext *cx, Ender_Item *i, int argc, jsval *argv, jsval *ret)
{
	Ender_Value *eargv;
	Ender_Value eret = { 0 };
	Ender_Item *arg;
	Eina_List *args;
	int nargs;
	int idx_ender = 0;
	int idx_js = 0;

	nargs = ender_item_function_args_count(i);
	if (argc != nargs)
	{
		JS_ReportError(cx, "Invalid number of arguments");
		JS_ReportPendingException(cx);

		/* Make sure the error is uncatchable. */
		JS_ClearPendingException(cx);
	}

	/* convert the args to ender values */
	eargv = calloc(nargs, sizeof(Ender_Value));
	EINA_LIST_FREE(args, arg)
	{
		_ender_js_sm_function_arg_from_jsval(arg, &eargv[idx_ender], &argv[idx_js]);
		ender_item_unref(arg);
		idx_ender++;
		idx_js++;
	}

	ender_item_function_call(i, eargv, &eret);
	free(args);

	/* convert the return type to a val */
	arg = ender_item_function_ret_get(i);
	if (arg)
	{
		_ender_js_sm_function_arg_to_jsval(arg, &eret, ret);
		ender_item_unref(arg);
	}

	return EINA_TRUE;
}

JSObject * ender_js_sm_function_new(JSContext *cx, Ender_Item *i)
{
	JSObject *ret;

	ret = JS_NewObject(cx, &_ender_js_sm_class_function, NULL, NULL);
	JS_SetPrivate(cx, ret, i);
	return ret;
}
