/* Ender_Js_Sm - JavaScript bindings for Ender
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

#ifndef _ENDER_JS_SM_H_
#define _ENDER_JS_SM_H_

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef ENDER_JS_SM_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif


#include <jsapi.h>
#include <Ender.h>

EAPI void ender_js_sm_init(void);
EAPI void ender_js_sm_shutdown(void);
EAPI JSClass * ender_js_sm_class_get(void);

EAPI JSObject * ender_js_sm_instance_new(JSContext *cx, Ender_Item *i, void *o);

#endif
