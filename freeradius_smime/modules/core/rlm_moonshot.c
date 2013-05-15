#include <freeradius-devel/ident.h>
RCSID("$Id$")

#include <freeradius-devel/radiusd.h>
#include <freeradius-devel/radius.h>
#include <freeradius-devel/modules.h>

#include "proxymodule.h"
#include "idpmodule.h"

typedef struct rlm_moonshot_t {
	char		*string;
} rlm_moonshot_t;

static const CONF_PARSER module_config[] = {
  { "string",  PW_TYPE_STRING_PTR, offsetof(rlm_moonshot_t,string), NULL,  NULL},
  { NULL, -1, 0, NULL, NULL }		/* end the list */
};

/* CONF_SECTION *conf seems to be the raw config data, rlm_moonshot_t *data is our defined struct that will hold our data, CONF_PARSER module_config[] are our config parser rules */
static int moonshot_init(CONF_SECTION *conf, void **instance)
{
	//Array that will store our parsed config data
	rlm_moonshot_t *data;
	
	data = rad_malloc(sizeof(*data));
	if (!data) {
		return -1;
	}
	memset(data, 0, sizeof(*data));

	//Parse the config file using conf, data and our parse rules in module_config
	if (cf_section_parse(conf, data, module_config) < 0) {
		free(data);
		return -1;
	}

	*instance = data;

	return 0;
}

static int moonshot_preproxy(void *instance, REQUEST *request)
{
	/* quiet the compiler */
	instance = instance;
	request = request;

	handle_certinject(request);
	
	return RLM_MODULE_OK;
}

static int moonshot_postauth(void *instance, REQUEST *request)
{
	/* quiet the compiler */
	instance = instance;
	request = request;

	if (request->reply->code == PW_AUTHENTICATION_ACK)
	{
		handle_sentrequests(request);
	}

	return RLM_MODULE_OK;
}

static int moonshot_detach(void *instance)
{
	free(instance);
	return 0;
}

//Register our functions in the correct places
module_t rlm_moonshot = {
	RLM_MODULE_INIT,
	"moonshot",			/* module name */
	RLM_TYPE_THREAD_SAFE,		/* type */
	moonshot_init,			/* instantiation */
	moonshot_detach,			/* detach */
	{
		NULL,			/* authentication */
		NULL,			/* authorization */
		NULL,			/* preaccounting */
		NULL,			/* accounting */
		NULL,			/* checksimul */
		moonshot_preproxy,	/* pre-proxy */
		NULL,			/* post-proxy */
		moonshot_postauth	/* post-auth */
	},
};