/* Esvg - SVG
 * Copyright (C) 2012 Jorge Luis Zapata
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
#include <stdlib.h>
#include "Ender_Js_Sm.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Class definition                               *
 *----------------------------------------------------------------------------*/
static JSBool _ender_js_sm_resolve(JSContext *context, JSObject *obj, jsid id,
		uintN flags, JSObject **objp)
{
	*objp = NULL;
	return JS_TRUE;
#if 0
	Importer *priv;
	char *name;
	JSBool ret = JS_TRUE;

	*objp = NULL;

	if (!gjs_get_string_id(context, id, &name))
	return JS_FALSE;

	/* let Object.prototype resolve these */
	if (strcmp(name, "valueOf") == 0 ||
	strcmp(name, "toString") == 0 ||
	strcmp(name, "__iterator__") == 0)
	goto out;

	priv = priv_from_js(context, obj);
	gjs_debug_jsprop(GJS_DEBUG_IMPORTER, "Resolve prop '%s' hook obj %p priv %p", name, obj, priv);

	if (priv == NULL) /* we are the prototype, or have the wrong class */
	goto out;

	JS_BeginRequest(context);
	if (do_import(context, obj, priv, name)) {
	*objp = obj;
	} else {
	ret = JS_FALSE;
	}
	JS_EndRequest(context);

out:
	g_free(name);
	return ret;
#endif
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
#if 0
JSBool ender_js_sm_get_string_id(JSContext *context, jsid id, char **name_p)
{
    jsval id_val;

    if (!JS_IdToValue(context, id, &id_val))
        return JS_FALSE;

    if (JSVAL_IS_STRING(id_val)) {
        gjs_string_get_binary_data(context, id_val, name_p, NULL);
        return JS_TRUE;
    } else {
        *name_p = NULL;
        return JS_FALSE;
    }
}
#endif
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
