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
#include "ender_js_sm_string_private.h"
#include "ender_js_sm_instance_private.h"
#include "ender_js_sm_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _ender_js_sm_value_basic_to_jsval(JSContext *cx,
		Ender_Item *type, Ender_Value *v, jsval *jv)
{
	Ender_Value_Type vtype;
	Eina_Bool ret = EINA_TRUE;

	vtype = ender_item_basic_value_type_get(type);
	switch (vtype)
	{
		case ENDER_VALUE_TYPE_BOOL:
		*jv = BOOLEAN_TO_JSVAL(v->b);
		break;

		case ENDER_VALUE_TYPE_UINT8:
		*jv = INT_TO_JSVAL(v->u8);
		break;

		case ENDER_VALUE_TYPE_INT8:
		*jv = INT_TO_JSVAL(v->i8);
		break;

		case ENDER_VALUE_TYPE_UINT32:
		if (v->u32 > JSVAL_INT_MAX)
			*jv = DOUBLE_TO_JSVAL((double)v->u32);
		else
			*jv = INT_TO_JSVAL(v->u32);
		break;

		case ENDER_VALUE_TYPE_INT32:
		*jv = INT_TO_JSVAL(v->i32);
		break;

		case ENDER_VALUE_TYPE_DOUBLE:
		*jv = DOUBLE_TO_JSVAL(v->d);
		break;

		case ENDER_VALUE_TYPE_STRING:
		*jv = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, v->ptr));
		break;

		case ENDER_VALUE_TYPE_UINT64:
		case ENDER_VALUE_TYPE_INT64:
		case ENDER_VALUE_TYPE_POINTER:
		ret = EINA_FALSE;
		break;
	}
	return ret;
}

static Eina_Bool _ender_js_sm_value_basic_from_jsval(JSContext *cx,
		Ender_Item *type, Ender_Value *v, jsval jv)
{
	Ender_Value_Type vtype;
	Eina_Bool ret = EINA_TRUE;

	vtype = ender_item_basic_value_type_get(type);
	switch (vtype)
	{
		case ENDER_VALUE_TYPE_BOOL:
		v->b = JSVAL_TO_BOOLEAN(jv);
		break;

		case ENDER_VALUE_TYPE_UINT8:
		v->u8 = JSVAL_TO_INT(jv);
		break;

		case ENDER_VALUE_TYPE_INT8:
		v->i8 = JSVAL_TO_INT(jv);
		break;

		case ENDER_VALUE_TYPE_UINT32:
		if (JSVAL_IS_INT(jv))
			v->u32 = (uint32_t)JSVAL_TO_INT(jv);
		else if (JSVAL_IS_DOUBLE(jv))
			v->u32 = (uint32_t)JSVAL_TO_DOUBLE(jv);
		else
			ret = EINA_FALSE;
		break;

		case ENDER_VALUE_TYPE_INT32:
		v->i32 = JSVAL_TO_INT(jv);
		break;

		case ENDER_VALUE_TYPE_DOUBLE:
		v->d = JSVAL_TO_DOUBLE(jv);
		break;

		case ENDER_VALUE_TYPE_STRING:
		{
			JSString *s;

			s = JS_ValueToString(cx, jv);
			ret = ender_js_sm_string_get(cx, STRING_TO_JSVAL(s), (char **)&v->ptr);
		}
		break;

		case ENDER_VALUE_TYPE_UINT64:
		case ENDER_VALUE_TYPE_INT64:
		case ENDER_VALUE_TYPE_POINTER:
		ret = EINA_FALSE;
		break;
	}
	return ret;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool ender_js_sm_value_to_jsval(JSContext *cx,
		Ender_Item *type, Ender_Item_Arg_Direction dir,
		Ender_Item_Transfer xfer, Ender_Value *v, jsval *jv)
{
	Eina_Bool ret = EINA_FALSE;

	/* handle the in/out direction */
	*jv = JSVAL_VOID;
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
				ret = ender_js_sm_value_to_jsval(cx, other, dir, xfer, v, jv);
				ender_item_unref(other);
			}
			break;

			case ENDER_ITEM_TYPE_OBJECT:
			*jv = OBJECT_TO_JSVAL(ender_js_sm_instance_new(cx, type, v->ptr));
			ret = EINA_TRUE;
			break;

			case ENDER_ITEM_TYPE_BASIC:
			ret = _ender_js_sm_value_basic_to_jsval(cx, type, v, jv);
			/* TODO in case the transfer is full and the type is a string, free it */
			break;

			case ENDER_ITEM_TYPE_ENUM:
			case ENDER_ITEM_TYPE_STRUCT:
			default:
			ERR("Unsupported type %s", ender_item_type_name_get(it));
			break;
		}
	}
	else
	{
		/* for in/inout directions we always pass a pointer */
		ERR("Unsupported direction %d", dir);
	}
	return ret;
}

Eina_Bool ender_js_sm_value_from_jsval(JSContext *cx,
		Ender_Item *type, Ender_Item_Arg_Direction dir,
		Ender_Item_Transfer xfer, Ender_Value *v, jsval jv)
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
				ret = ender_js_sm_value_from_jsval(cx, other, dir, xfer, v, jv);
				ender_item_unref(other);
			}
			break;

			case ENDER_ITEM_TYPE_BASIC:
			ret = _ender_js_sm_value_basic_from_jsval(cx, type, v, jv);
			break;

			case ENDER_ITEM_TYPE_ENUM:
			v->i32 = JSVAL_TO_INT(jv);
			ret = EINA_TRUE;
			break;

			/* TODO handle the transfer */
			case ENDER_ITEM_TYPE_OBJECT:
			case ENDER_ITEM_TYPE_STRUCT:
			if (JSVAL_IS_NULL(jv))
			{
				DBG("Converting from js object to NULL");
				v->ptr = NULL;
			}
			else if (JSVAL_IS_OBJECT(jv))
			{
				JSObject *obj;

				DBG("Converting from js object to ender object");
				obj = JSVAL_TO_OBJECT(jv);
				if (ender_js_sm_is_instance(cx, obj))
				{
					v->ptr = ender_js_sm_instance_ptr_get(cx, obj);
					ret = EINA_TRUE;
				}
			}
			else if (it == ENDER_ITEM_TYPE_OBJECT)
			{
				Eina_List *ctors;
				Ender_Item *ctor;

				DBG("Constructing a new object");
				ctors = ender_item_object_ctor_get(type);
				EINA_LIST_FREE(ctors, ctor)
				{
					Ender_Value vr = { 0 };
					Ender_Item *arg;
					Ender_Item *arg_type;
					int nargs;

					nargs = ender_item_function_args_count(ctor);
					if (nargs != 1)
					{
						ender_item_unref(ctor);
						continue;
					}
					/* convert the arg from a jsval to the first arg type */
					arg = ender_item_function_args_at(ctor, 0);
					arg_type = ender_item_arg_type_get(arg);
					ender_js_sm_value_from_jsval(cx, arg_type,
							ender_item_arg_direction_get(arg),
							ender_item_arg_transfer_get(arg),
							v, jv);
					ret = ender_item_function_call(ctor, v, &vr);
					v->ptr = vr.ptr;
					ender_item_unref(arg_type);
					ender_item_unref(arg);
					/* TODO in case the newly created object is not passed full, we need to free it */
				}
			}
			else
			{
				ERR("Unsupported type");
			}
			break;

			default:
			ERR("Unsupported type %s", ender_item_type_name_get(it));
			break;
		}
	}
	else
	{
		/* for in/inout directions we always pass a pointer */
		ERR("Unsupported direction %d", dir);
	}
	return ret;
}
