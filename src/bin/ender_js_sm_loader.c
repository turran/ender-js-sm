#include "Ender_Js_Sm.h"

/* The class of the global object. */
static JSClass global_class =
{
	"global", JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_StrictPropertyStub, JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub,
	JS_FinalizeStub, JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSBool js_print(JSContext *cx, uintN argc, jsval *vp)
{
	JSString* u16_txt;
	unsigned int length;
	char *txt;

	if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &u16_txt))
		return JS_FALSE;

	length = JS_GetStringEncodingLength(cx, u16_txt);
	txt = alloca(sizeof(char) * (length + 1));
	JS_EncodeStringToBuffer(u16_txt, txt, length);

	printf("%.*s\n", length, txt);
	return JS_TRUE;
}

static JSFunctionSpec global_functions[] =
{
    JS_FS("print", js_print, 1, 0),
    JS_FS_END
};

static void help(void)
{
	printf("ender-js-sm-loader FILE.js\n");
}

/* The error reporter callback. */
static void report_error(JSContext *cx, const char *message, JSErrorReport *report)
{
	fprintf(stderr, "%s:%u:%s\n",
			report->filename ? report->filename : "<no filename>",
			(unsigned int) report->lineno, message);
}

int main(int argc, char **argv)
{
	JSRuntime *rt;
	JSContext *cx;
	JSObject *global;
	JSObject *ender;
	Enesim_Stream *s;
	char *data;
	size_t data_len;
	jsval rval;

	if (argc < 2)
	{
		help();
		return -1;
	}

	ender_js_sm_init();

	s = enesim_stream_file_new(argv[1], "r");
	data = enesim_stream_mmap(s, &data_len);
	/* Create an instance of the engine */
	rt = JS_NewRuntime(1024*1024);
	if (!rt)
	{
		return -1;
	}

	/* Create an execution context */
	cx = JS_NewContext(rt, 8192);
	if (!cx)
	{
		return -2;
	}
	JS_SetErrorReporter(cx, report_error);
#if 0
	JS_SetOptions(cx, JSOPTION_VAROBJFIX | JSOPTION_JIT |
		  JSOPTION_METHODJIT);
	JS_SetVersion(cx, JSVERSION_LATEST);
#endif

	/* Create a global object and a set of standard objects */
	global = JS_NewCompartmentAndGlobalObject(cx, &global_class, NULL);
	if (!global)
	{
		return -3;
	}

	if (!JS_DefineFunctions(cx, global, global_functions))
	{
	        return -4;
	}

	/* Populate the global object with the standard globals,
	 * like Object and Array.
	 */
	if (!JS_InitStandardClasses(cx, global))
	{
		return -5;
	}

	JS_DefineObject(cx, global, "ender", ender_js_sm_class_get(), NULL, JSPROP_PERMANENT | JSPROP_READONLY);

	/* Load the file */
	JS_EvaluateScript(cx, global, data, data_len, argv[1], 0, &rval);
	// compile:
	//script = JS_CompileScript(cx, JS_GetGlobalObject(cx), TESTSCRIPT, strlen(TESTSCRIPT), "TEST", 0);
	//JS_ExecuteScript(cx, JS_GetGlobalObject(cx), script, &rval);
	/* Cleanup */

	JS_DestroyContext(cx);
	JS_DestroyRuntime(rt);
	JS_ShutDown();
	enesim_stream_unref(s);
	ender_js_sm_shutdown();

	return 0;
}
