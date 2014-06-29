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

#ifndef _ENDER_JS_SM_PRIVATE_H
#define _ENDER_JS_SM_PRIVATE_H

#define ERR(...) EINA_LOG_DOM_ERR(ender_js_sm_log, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(ender_js_sm_log, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(ender_js_sm_log, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(ender_js_sm_log, __VA_ARGS__)
#define CRI(...) EINA_LOG_DOM_CRIT(ender_js_sm_log, __VA_ARGS__)

extern int ender_js_sm_log;

#endif
