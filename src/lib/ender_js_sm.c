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
#include "ender_js_sm_string_private.h"
#include "ender_js_sm_function_private.h"
#include "ender_js_sm_lib_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static int _init = 0;
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

	DBG("Resolving lib '%s'", name);

	JS_BeginRequest(cx);
	/* TODO check what is the request on the flags */
	lib = ender_lib_find(name);
	if (lib)
	{
		JSObject *olib;

		/* create an object for a lib */
		olib = ender_js_sm_lib_new(cx, lib);
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
int ender_js_sm_log = -1;
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void ender_js_sm_init(void)
{
	if (!_init++)
	{
		ender_init();
		ender_js_sm_log = eina_log_domain_register("ender-js-sm", NULL);
	}
}

EAPI void ender_js_sm_shutdown(void)
{
	if (_init == 1)
	{
		eina_log_domain_unregister(ender_js_sm_log);
		ender_shutdown();
	}
	_init--;
}

EAPI JSClass * ender_js_sm_class_get(void)
{
	return &_ender_js_sm_class;
}
