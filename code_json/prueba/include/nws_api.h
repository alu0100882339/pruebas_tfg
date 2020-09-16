/* $Id: nws_api.h,v 1.106 2005/04/28 21:52:32 graziano Exp $ */

#ifndef NWS_API_H
#define NWS_API_H

#ifdef	__cplusplus
extern "C" {
#endif


/*
 * This file defines the API for the Network Weather Service.  The functions
 * defined here allow programs to ask for forecasts, store measurements in NWS
 * memories, retrieve previously-stored measurements, register objects with the
 * NWS name server, and retrieve previously-registered objects.
 *
 * NOTE: In order to avoid name collisions, all names defined here begin with
 * NWSAPI_.  To avoid having to use this prefix, #define NWSAPI_SHORTNAMES
 * before including this file.
 */


/* 
 * First definition used for the Forecasting capabilities (measurements,
 * forecasts, and how to interact with them).
 */


/*
 * A description of a measurement.  The time the measurement was taken
 * (represented as a number of seconds since midnight, 1/1/1970 GMT) and the
 * observed value.
 */
typedef struct {
	double timeStamp;
	double measurement;
} NWSAPI_Measurement;


/*
 * A description of a forecast.  The forecast value itself, an estimate of the
 * precision of the forecast, and an index for the method used to generate it.
 * #error# is the MSE and #MAE_error# is the MAE error of the forecaster.
 */
typedef struct {
	double forecast;
	double error;
	double MAE_error;
	unsigned int methodUsed;
} NWSAPI_Forecast;


/**
 * A description of a collection of forecasts, each one based on minimizing a
 * different measurement of the error (MAE == MEAN_ABSOLUTE_ERROR, MSE ==
 * MEAN_SQUARE_ERROR).  The actual measurement taken at the same time is
 * included for convenience when available.
 */
#define NWSAPI_FORECAST_TYPE_COUNT 2
#define NWSAPI_MAE_FORECAST 0
#define NWSAPI_MSE_FORECAST 1
typedef struct {
	NWSAPI_Measurement measurement;
	NWSAPI_Forecast forecasts[NWSAPI_FORECAST_TYPE_COUNT];
} NWSAPI_ForecastCollection;


/**
 * A description of a forecasting state.  Client code should obtain these
 * structures via NewForecastState(), update them with new values via
 * UpdateForecastState(), then delete them via FreeForecastState().
 */
typedef struct {
	char *Forecaster_state;
} NWSAPI_ForecastState;


/**
 * Frees the memory occupied by the forecasting state pointed to by #state#
 * and sets #state# to NULL.
 */
void
NWSAPI_FreeForecastState(NWSAPI_ForecastState **state);


/**
 * Returns the name of the #methodIndex#th forecasting method used by the
 * forecasting library, or NULL if #methodIndex# is out of range.
 */
const char *
NWSAPI_MethodName(unsigned int methodIndex);


/**
 * Allocates and returns a new forecasting state, or NULL if memory is
 * exhausted.  The caller should eventually call FreeForecastingState() to
 * free the memory. 
 */
NWSAPI_ForecastState *
NWSAPI_NewForecastState();


/**
 * Incorporates the #howManyMeasurements#-long series #measurements# into the
 * forecasting state #state#.  If #forecasts# is non-NULL, it points to a
 * #howManyForecasts#-long array into which the function should store
 * forecasts generated during the course of the update  If #howManyForecasts#
 * is less than #howManyMeasurements#, the forecasts derived from the later
 * measurements (which appear at the beginning of #measurements#) are stored.
 */
void
NWSAPI_UpdateForecastState(	NWSAPI_ForecastState *state,
                                const NWSAPI_Measurement *measurements,
                                unsigned int howManyMeasurements,
                                NWSAPI_ForecastCollection *forecasts,
                                unsigned int howManyForecasts);
#define NWSAPI_UpdateForecast(state, measurements, howMany) \
	NWSAPI_UpdateForecastState(state, measurements, howMany, NULL, 0)

/**
 * Return a set of forecast base on the current state of the forecaster.
 * #forecast# need to have space for at least one ForecastCollection. The
 * measurement associate is the last known measurement associated with
 * the forecaster.
 *
 * Return 0 in case of error, 1 on success.
 */
int
NWSAPI_ComputeForecast(	NWSAPI_ForecastState *state,
				NWSAPI_ForecastCollection *forecast);


/* General NWS API declarations. */


/* A description of an NWS host -- machine name and port number. The
 * field #sd# is used internally and should be ignored. You should use
 * NWSAPI_MakeHostSpec to fill one of these.
 */
#define NWSAPI_MACHINE_NAME_LENGTH (63 + 1)
typedef unsigned short NWSAPI_Port;
typedef struct {
	char machineName[NWSAPI_MACHINE_NAME_LENGTH];
	NWSAPI_Port machinePort;
} NWSAPI_HostSpec;


/*
** Predefined names for the resources that the NWS generally monitors.  Other
** resources may be monitored, depending the configuration of the NWS system.
** Exactly which information is available can be determined by examining series
** registered with the NWS name server (see GetObjects()).
*/
#define NWSAPI_RESOURCE_NAME_LENGTH (31 + 1)
#define NWSAPI_DEFAULT_AVAILABLE_CPU_RESOURCE "availableCpu"
#define NWSAPI_DEFAULT_BANDWIDTH_RESOURCE     "bandwidthTcp"
#define NWSAPI_DEFAULT_CURRENT_CPU_RESOURCE   "currentCpu"
#define NWSAPI_DEFAULT_DISK_RESOURCE          "freeDisk"
#define NWSAPI_DEFAULT_LATENCY_RESOURCE       "latencyTcp"
#define NWSAPI_DEFAULT_MEMORY_RESOURCE        "freeMemory"


/**
 * A description of an NWS experiment series.  A source machine, a destination
 * machine (ignored for single-machine resources), and a resource name.
 */
typedef struct {
	char sourceMachine[NWSAPI_MACHINE_NAME_LENGTH];
	char destinationMachine[NWSAPI_MACHINE_NAME_LENGTH];
	char resourceName[NWSAPI_RESOURCE_NAME_LENGTH];
} NWSAPI_SeriesSpec;


/* Used with GetForecasts() and GetMeasurements() to retrieve all numbers. */
#define NWSAPI_BEGINNING_OF_TIME 0.0

/**
 * Checks the user's environment variables and initialization file for a
 * setting for the variable #name#.  Returns its value if one is found, else
 * returns #defaultValue#.  The value returned need to be freed.
 */
char *
NWSAPI_EnvironmentValue_r(	const char *name,
				const char *defaultValue);

/**
 * Returns the default port that #hostType# is listening to (ie the
 * nameserver usually listen to 8090). #daemon# can be
 * NWSAPI_NAME_SERVER ... or 0 if invalid.
 */
#define NWSAPI_MEMORY_HOST 1
#define NWSAPI_NAME_SERVER_HOST 2
#define NWSAPI_SENSOR_HOST 3
#define NWSAPI_PROXY_HOST 4
unsigned short
NWSAPI_GetDefaultPort(	unsigned int hostType);

/**
 * Returns an internal index if #resource# is a known NWS resource, -1
 * otherwise.
 */
int
NWSAPI_IsResourceValid(const char *resource);

/**
 * Returns 1 or 0 depending on whether or not the resource named #resourceName#
 * involves multiple machines (like bandwidth) or a single machine (like CPU).
 * #resourceName# has to be a valid NWS resource (check IsResourceValid).
 */
int
NWSAPI_IntermachineResource(const char *resourceName);


/**
 * A convenience function for constructing series specifications.  The returned
 * value is volatile and will be overwritten by subsequent calls.
 */
const NWSAPI_SeriesSpec *
NWSAPI_MakeSeriesSpec(const char *sourceMachine,
                      const char *destinationMachine,
                      const char *resourceName);


/**
 * A convenience function for constructing host specifications.  #host# may be
 * either a DNS name or an IP address image.  It may also include a trailing
 * colon followed by a port number, in which case this port number is used in
 * preference to #machinePort#.  The return value is volatile and will be
 * overwritten by subsequent calls.
 */
const NWSAPI_HostSpec *
NWSAPI_MakeHostSpec(const char *host,
                    NWSAPI_Port machinePort);


/**
 * Constructs and returns a name for #whichSeries# which can be passed to
 * the memory and forecaster retrieval and storage functions. 
 * The returned value needs to be freed.
 * User-generated series (i.e., those stored via calls to PutMeasurements())
 * may be given any name, but using this function to generate a name may make
 * it easier to reconstruct the name for later retrieval.
 */
char *
NWSAPI_SeriesName_r(const NWSAPI_SeriesSpec *whichSeries);

/*
 * NWS name server API declarations **
 */

/*
 * Definitions of name server attributes, objects, and sets of objects.
 */
typedef char *NWSAPI_NwsAttribute;
typedef char *NWSAPI_Object;
typedef char *NWSAPI_ObjectSet;


/*
 * NwsAttributes associated with NWS objects.  
 */
#define NWSAPI_ACTIVITIES  "(objectclass=nwsActivity)"
#define NWSAPI_CLIQUES     "(&(objectclass=nwsActivity)(controlName=clique))"
#define NWSAPI_CONTROLS    "(objectclass=nwsControl)"
#define NWSAPI_FORECASTERS "(hostType=forecaster)"
#define NWSAPI_HOSTS       "(objectclass=nwsHost)"
#define NWSAPI_MEMORIES    "(hostType=memory)"
#define NWSAPI_SENSORS     "(hostType=sensor)"
#define NWSAPI_SERIES      "(objectclass=nwsSeries)"
#define NWSAPI_SKILLS      "(objectclass=nwsSkill)"
#define NWSAPI_CLASS_ATTR      "objectclass"
#define NWSAPI_CONTROL_ATTR    "controlName"
#define NWSAPI_HOST_ATTR       "host"
#define NWSAPI_IP_ATTR         "ipAddress"
#define NWSAPI_MEMBER_ATTR     "member"
#define NWSAPI_MEMORY_ATTR     "memory"
#define NWSAPI_NAME_ATTR       "name"
#define NWSAPI_OPTION_ATTR     "option"
#define NWSAPI_PERIOD_ATTR     "period"
#define NWSAPI_PORT_ATTR       "port"
#define NWSAPI_RESOURCE_ATTR   "resource"
#define NWSAPI_SKILL_ATTR      "skillName"
#define NWSAPI_TARGET_ATTR     "target"
#define NWSAPI_UNIT_LABEL_ATTR "unitLabel"


/**
 * These two routines retrieve and return, respectively, the name and value
 * associated with their parameter attribute. The returned value needs to
 * be freed. Returns NULL on error.
 */
char *
NWSAPI_NwsAttributeName_r(NWSAPI_NwsAttribute attribute);
char *
NWSAPI_NwsAttributeValue_r(NWSAPI_NwsAttribute attribute);


/**
 * These two functions and four macros are used to iterate through the
 * object set returned by a successful call to GetObjects().  The
 * NextObject function iterates through each object in the set; the
 * NextNwsAttribute function iterates through each attribute in an
 * object.  Each function returns NULL when no objects/attributes remain.
 */
NWSAPI_NwsAttribute
NWSAPI_NextNwsAttribute(NWSAPI_Object object,
                     NWSAPI_NwsAttribute current);
#define NWSAPI_FirstNwsAttribute(object) \
        NWSAPI_NextNwsAttribute(object, NULL)
#define NWSAPI_ForEachNwsAttribute(object,attribute) \
  for(attribute = NWSAPI_FirstNwsAttribute(object); \
      attribute != NULL; \
      attribute = NWSAPI_NextNwsAttribute(object, attribute))
NWSAPI_Object
NWSAPI_NextObject(NWSAPI_ObjectSet objects,
                  NWSAPI_Object current);
#define NWSAPI_FirstObject(objectSet) \
        NWSAPI_NextObject(objectSet, NULL)
#define NWSAPI_ForEachObject(objectSet,object) \
  for(object = NWSAPI_FirstObject(objectSet); \
      object != NULL; \
      object = NWSAPI_NextObject(objectSet, object))
/**
 * The following functions operates on object and objectSet. They usually
 * returns 1 on success. 
 * AddObject adds an object to an objectSet.
 * DeleteObject deletes an object from an objectSet
 * NewObjectSet creates an empty objectSet.
 * NWSAPI_FreeObjectSet free the memory used by an objectSet.
 */
int
NWSAPI_AddObject(	NWSAPI_ObjectSet *toSet,
			const NWSAPI_Object obj);
int
NWSAPI_DeleteObject(	NWSAPI_ObjectSet *fromSet,
			const NWSAPI_Object obj);
NWSAPI_ObjectSet
NWSAPI_NewObjectSet();

void
NWSAPI_FreeObjectSet(NWSAPI_ObjectSet *toBeFreed);


/**
 * Returns the NwsAttribute (or NULL if not found) in #object# whose name
 * is #name#.
 */
NWSAPI_NwsAttribute
NWSAPI_FindNwsAttribute(	const NWSAPI_Object object,
				const char *name);


/**
 * A convenience function that calls GetObjects() in order to retrieve info
 * about NWS hosts.  Retrieves up to #atMost# NWS host objects that match
 * #filter#.  Copies the retrieved hosts into #whereTo# and stores the count of
 * retrieved hosts in #numberReturned#.  Use #timeout# seconds as
 * communication timeout (-1 enable automatic timeout, 0 disable
 * timeout). If succesfull #whereTo# needs to be freed by the caller.
 * Returns 1 if successful, else 0.
 */
int
NWSAPI_GetHostsTimeout(	const char *filter,
			NWSAPI_HostSpec *whereTo,
			unsigned int atMost,
			unsigned int *numberReturned,
			int timeout);
#define NWSAPI_GetHosts(a, b, c, d) NWSAPI_GetHostsTimeout(a, b, c, d, -1)


/**
 * Contacts the NWS host #whoToAsk# and returns in #nameServer# the NWS name
 * server with which it is registered.  #whoToAsk# is usually a
 * nws_sensor.  Use #timeout# seconds as communication timeout (-1 enable
 * automatic timeout, 0 disable
 * Returns 1 if successful, else 0.
 */
int
NWSAPI_GetNameServerTimeout(	const NWSAPI_HostSpec *whoToAsk,
				NWSAPI_HostSpec *nameServer,
				int timeout);
#define NWSAPI_GetNameServer(a, b) NWSAPI_GetNameServerTimeout(a, b, -1)

/**
 * Contacts the NWS host #whoToAsk# and returns in #memory# the NWS
 * memory with which it register its experiments. #whoToAsk# is usually a
 * nws_sensor.  Use #timeout# seconds as communication timeout (-1 enable
 * automatic timeout, 0 disable timeout). 
 * Returns 1 if successful, else 0.
 *
 * NOTE: only from version 2.7 sensors know how to answer this
 * call.
 */
int
NWSAPI_GetMemoryTimeout(	const NWSAPI_HostSpec *whoToAsk,
				NWSAPI_HostSpec *memory,
				int timeout);
#define NWSAPI_GetMemory(a, b) NWSAPI_GetMemoryTimeout(a, b, -1)


/**
 * Contacts the selected NWS name server (see UseNameServer()) in order
 * to retrieve information about registered objects that match the
 * characteristics specified in #filter#.  Returns a representation of
 * the matching objects in #whereTo#; after a successful call, this value
 * may be passed to {First,Next}Object() in order to extract individual
 * objects.  After the caller is done with the object set it pass it to
 * NWSAPI_FreeObjectSet() so that the space can be reclaimed. Use
 * #timeout# seconds as communication timeout (-1 enable automatic
 * timeout, 0 disable timeout). Returns 1 if successful, else 0.
 */
int
NWSAPI_GetObjectsTimeout(	const char *filter,
				NWSAPI_ObjectSet *whereTo,
				int timeout);
#define NWSAPI_GetObjects(a, b) NWSAPI_GetObjectsTimeout(a, b, -1)

/**
 * Small utility functions: returns in <hosts> the comma-separated list
 * of hostnames member of clique <clique_name> Returns the number of
 * hostnames in <hosts> or 0 in case of error. <hosts> needs to be freed.
*/
int
NWSAPI_GetCliqueHosts(	const char *clique_name,
			char **hosts);

/**
 * Add #whichNS# to the list of known nameserver. If #whichNS# is NULL.
 * it will reset the list. This list contains the nameserver that will be
 * query when a registration is needed. Returns 1 if successful, else 0.
 */
int
NWSAPI_UseNameServer(const NWSAPI_HostSpec *whichNS);

/**
 * Set the Proxy to use. It works as UseNameServer but for the proxy.
 */
int
NWSAPI_UseProxy(const NWSAPI_HostSpec *whichProxy);


/*
 * NWS memory API declarations **
 */


/**
 * Contacts the memory associated with the series #seriesName# and
 * instructs it to begin forwarding new measurements stored for the
 * series. #seriesName# can be a tab separated list of series.  Such
 * "auto-fetched" measurements are made available via calls to the
 * AutoFetchCheck() function.  Returns 1 if successful (all series
 * started to autofetch), else 0 (and you can check AutoFetchError()).
 */
int
NWSAPI_AutoFetchBegin(const char *seriesName);

/** 
 * Same as AutoFetchBegin, but uses ObjectSet instead of tab delimited
 * series name. The object (likely retrieved from the nameserver) will
 * have to contain the "memory" attribute to be usefull. This function is
 * much faster then AutoFetchBegin because doesn't need to look up which
 * memory serves the series.  Returns 1 if successful (all series
 * started to autofetch), else 0 (and you can check AutoFetchError()).
 * */
int
NWSAPI_AutoFetchBeginObject(NWSAPI_ObjectSet seriesSet);

/**
 * Returns 1 if #seriesName# is been already autofetched, 0 otherwise. 
 */
int
NWSAPI_AutoFetchAlready(const char *seriesName);

/**
 * If an auto-fetch measurement (see AutoFetchBegin()) has arrived since the
 * prior call or arrives within #timeOut# seconds, returns 1 and copies the
 * series name into the #nameLen#-long string #seriesName# and the measurement
 * value into #seriesMeasurement#; else returns 0. If #timeOut# is -1 it
 * is polling. 
 *
 * After an AutoFetchCheck call that returned 0, it's always good to call
 * AutoFetchError to inspect a (possible) list of series that needs to be
 * restarted.
 *
 * AutoFetchMessage is for now just used internally by NWS.
 */
int
NWSAPI_AutoFetchCheck(	char *seriesName,
			unsigned int nameLen,
			NWSAPI_Measurement *seriesMeasurement,
			long timeOut);
int
NWSAPI_AutoFetchMessage(int sock,
			char *seriesName,
			unsigned int nameLen,
			NWSAPI_Measurement *seriesMeasurement);


/**
 * Returns a list of series (tab separated) that have failed and need to
 * be restarted (with AutoFetchBegin). The memory returned needs to be
 * freed. The failed list is reset every time this function is called.
 */
char *
NWSAPI_AutoFetchError();

/**
 * Contacts the memory associated with the series #seriesName# and instructs it
 * to stop forwarding new measurements stored for the series.  If #seriesName#
 * is an empty string, terminates forwarding of all auto-fetching.
 * #seriesName# can be a tab-separated list of series names.
 */
void
NWSAPI_AutoFetchEnd(const char *seriesName);


/**
 * Retrieves up to #atMost# measurements for the series named #seriesName# that
 * have been taken since #sinceWhen#.  If more than #atMost# measurements have
 * been taken since #sinceWhen#, the most recent ones are returned.  Returns
 * the measurements in the #atMost#-long array #whereTo# with the newest one in
 * the first element and the oldest measurement in the last.  Stores the count
 * of retrieved measurements in #numberReturned# if it is non-NULL.  Returns 1
 * if successful, else 0.
 */
int
NWSAPI_GetMeasurements(const char *seriesName,
                       double sinceWhen,
                       NWSAPI_Measurement *whereTo,
                       unsigned int atMost,
                       unsigned int *numberReturned);
#define NWSAPI_GetMeasurement(which,where) \
        NWSAPI_GetMeasurements(which, NWSAPI_BEGINNING_OF_TIME, where, 1, NULL)


/**
 * when query the proxy, you can tell the library either to do a perfect
 * match on the hostname, or to just match the name. For example, there
 * are a lot of machines that reports their name without domain name: in
 * this case foo.bar.edu and foo are considered different host. If you
 * #relax# is 1, then foo.bar.edu and foo will be considered equivalent.
 */
void
NWSAPI_RelaxHostNameCheck(int relax);

/**
 * Similar to GetMeasurements, but retrieve Forecastes insteades. The
 * parameters are the same as GetMeasuremets expect for #whereTo# that
 * now is a ForecastCollection. Uses GetMeasurements to retrieves the
 * measurements first.
 */
int
NWSAPI_GetForecasts(	const char *seriesName,
			double sinceWhen,
			NWSAPI_ForecastCollection *whereTo,
			unsigned int atMost,
			unsigned int *numberReturned);
#define NWSAPI_GetForecast(which,where) \
        NWSAPI_GetForecasts(which, NWSAPI_BEGINNING_OF_TIME, where, 1, NULL)


/**
 * Asks a NWS proxy (see UseProxy()) for (only) the latest  forecast
 * for #hosts# (been #hosts# a comma separated list of host:port) .
 * #resource# and #opts# specifies which resource (and relative options)
 * to search for. If succesfull returns the # of forecasts in #col#
 * (otherwise 0): the memory for #col# need to be freed. The number of
 * forecasts returned depends on the resource: for any single host
 * resource (ie availableCPU) are the number of #hosts#, otherwise for 2
 * hosts resources (ie bandwitdhTcp) square that number.
 *
 * When #valid# is greater then 0, all data with a timestamp older then
 * now - #valid# will be discarded as stale.
 *
 * #mode# and #level# are positive when virtual data are request: in this
 * mode, the proxy tries to find super-cliques and substitutes
 * measurements of these hosts for missing data.
 *
 * #mode# can be MODE_MAX (returns the maximum value found in the path),
 * MODE_MIN (minimum in the path). Any other values will disable the
 * virtual call, and returns only the real measurements.
 *
 * Example: in hosts you had "A,B,C" now you have the following:
 * 	col[i + j*3] will contain the forecast from host j to host i. So
 * 	the bandwitdh from B to C will be in col[5] (2 + 1*3). 
 *
 * If the proxy doesn't have data for a specific series, the timestamp of
 * that series will be 0. In the previous example, if there is no data
 * for B to C, then
 * 	col[5].measurement.timeStamp 
 * will be 0.
 *
 */
#define MODE_NO_VIRTUAL (0)
#define MODE_MAX (1)
#define MODE_MIN (2)
int
NWSAPI_GetVirtualForecasts(	const char *resource,
				const char *opts,
				const char *hosts,
				int mode,
				int level,
				unsigned long valid,
				NWSAPI_ForecastCollection **col,
				const int timeOut);
#define NWSAPI_GetAllForecasts(r, o, h, v, c, t) \
	NWSAPI_GetVirtualForecasts(r, o, h, MODE_NO_VIRTUAL, 0, v, c, t)

/**
 * This is a small utility functions used by GetVirtualForecasts and
 * others to compare 2 hosts. With this we try to work around broken DNS
 * or wrong DNS configurations so that hosts that doesn't report their
 * domain name can be found anyway (for example node111.whatever.edu
 * should match node111:8060). #src# and #dst# are the 2 host to compare.
 * Returns 1 id the hosts are 'similar', 0 otherwise.
 */
int
NWSAPI_HostCompare(	const char *src,
			const char *dst);


/**
 * Similar to GetVirtualForecasts, returns the series of a set of
 * cliques. The clique names (#cliqueName#) is a comma separated lists of
 * cliques names, needs to be specified and has to be present in the
 * Nameserver (that you can set using UseNameServer()). Returns the lists
 * of hosts in <hosts> as a comma-separated list of names:port (needs to
 * be freed) and the forecasts in #col#: depending on the resource there
 * are either the returned number of hosts or the square root.
 *
 * When #valid# is greater than 0, data with timestamp older then now -
 * #valid# will be considered stale and discarded.
 *
 * NOTE: despite the name, cliqueName can be any activity, but  only with
 * clique (large number of hosts) this calls make sense.
 *
 * GetCliqueForecasts is just to retrieve the series of only one clique
 * (no virtualization done at all)
 */
int
NWSAPI_GetCliquesForecasts(	const char *resource,
				const char *cliqueName,
				char **hosts,
				int mode,
				int level,
				unsigned long valid,
				NWSAPI_ForecastCollection **col,
				int timeOut);
#define NWSAPI_GetCliqueForecasts(r, c, h, v, col, t) \
	NWSAPI_GetCliquesForecasts(r, c, h, 0, 0, v, col, t)

/**
 * Asks a NWS #proxy# to start fetching series for #hosts# and
 * #resource#, possibly matching the options in #opts#. hosts is a
 * comma-separated list of hosts.
 * Returns 0 on failure, 1 on success.
 */
int
NWSAPI_StartFetching(	const NWSAPI_HostSpec *proxy,
			const char *resource,
			const char *opts,
			const char *hosts,
			int timeOut);

/**
 * Similar to NWSAPI_StartFetching, but asks the proxy to start fetching
 * series for activity <clique_name>. Returns 0 on failure, 1 on success.
 */
int
NWSAPI_StartCliqueFetching(	const NWSAPI_HostSpec *proxy,
				const char *clique_name,
				int timeOut);


/**
 * EXPERIMENTAL: finds a path (of hosts and cliques) from host #src# to
 * host #dst#. #cliques# is a list of cliques used to find the path,
 * #path# will contains two attribute: hosts and cliques (the hosts hops
 * and the cliques they belong to) and #level# is how deep to look for
 * a path. Returns 0 if no path have been found otherwise 1 (and #path#
 * will contain the way and its memory will need to be freed).
 */
int
NWSAPI_FindHostsPath(	const char *src,
			const char *dst,
			NWSAPI_ObjectSet cliques,
			NWSAPI_ObjectSet *path,
			int level);
int
NWSAPI_FindCliquesPath(	const char *src,
			const char *dst,
			NWSAPI_ObjectSet cliques,
			NWSAPI_ObjectSet *path,
			int level);
/* EXPERIMENTAL: don't use it */
int
NWSAPI_FindCliquesTree(	char ***cliques,
			int **tree);


/**
 * Contacts the chosen memory (see UseMemory) to store the #howMany#-long
 * series of measurements #measurements# under the name #seriesName#.  Also
 * registers the series with the NWS name server using #whichSeries#.  Returns
 * 1 if successful, else 0.
 */
int
NWSAPI_PutMeasurements(const char *seriesName,
                       const NWSAPI_SeriesSpec *whichSeries,
                       const NWSAPI_Measurement *measurements,
                       unsigned int howMany);


/**
 * Directs the interface to contact #whichMemory# to store measurements.  By
 * default the interface attempts to choose the best available memory for each
 * set of measurements.  Returns 1 if successful, else 0.
 */
int
NWSAPI_UseMemory(const NWSAPI_HostSpec *whichMemory);


/*
** NWS sensor API declarations **
*/


/*
** Asks #whichSensor# to terminate the activity registered as #activityName#.
** Returns 1 if successful, else 0.
*/
int
NWSAPI_HaltActivity(const NWSAPI_HostSpec *whichSensor,
                    const char *activityName);


/*
** Contacts #whichSensor# to use #controlName# to start a #skillName# activity,
** configured using the elements of the #howManyOptions#-long array #options#.
** Each element of this array is a name:value pair.  Uses #activityName# to
** register the activity with the name server.  Returns 1 if successful, else 0.
*/
int
NWSAPI_StartActivity(const NWSAPI_HostSpec *whichSensor,
                     const char *activityName,
                     const char *controlName,
                     const char *skillName,
                     const char **options,
                     unsigned int howManyOptions);

/*
** Contacts #whichSensor# to ask him to restart all its activities
*/
int
NWSAPI_RestartActivities(const NWSAPI_HostSpec *whichSensor);

/* DEPRECATED: the following functions will be removed */
const char *
NWSAPI_NwsAttributeName(NWSAPI_NwsAttribute attribute);
const char *
NWSAPI_NwsAttributeValue(NWSAPI_NwsAttribute attribute);
const char *
NWSAPI_SeriesName(const NWSAPI_SeriesSpec *whichSeries);
const char *
NWSAPI_EnvironmentValue(const char *name, const char *defaultValue);
/* END DEPRECATED */


#ifdef NWSAPI_SHORTNAMES
#	define Measurement NWSAPI_Measurement
#	define Forecast NWSAPI_Forecast
#	define FORECAST_TYPE_COUNT NWSAPI_FORECAST_TYPE_COUNT
#	define MAE_FORECAST NWSAPI_MAE_FORECAST
#	define MSE_FORECAST NWSAPI_MSE_FORECAST
#	define ForecastCollection NWSAPI_ForecastCollection
#	define ForecastStateStruct NWSAPI_ForecastStateStruct
#	define ForecastState NWSAPI_ForecastState
#	define FreeForecastState NWSAPI_FreeForecastState
#	define MethodName NWSAPI_MethodName
#	define NewForecastState NWSAPI_NewForecastState
#	define UpdateForecastState NWSAPI_UpdateForecastState
#	define UpdateForecast NWSAPI_UpdateForecast
#	define ComputeForecast NWSAPI_ComputeForecast
#	define MACHINE_NAME_LENGTH NWSAPI_MACHINE_NAME_LENGTH
#	define Port NWSAPI_Port
#	define HostSpec NWSAPI_HostSpec
#	define RESOURCE_NAME_LENGTH NWSAPI_RESOURCE_NAME_LENGTH
#	define DEFAULT_AVAILABLE_CPU_RESOURCE NWSAPI_DEFAULT_AVAILABLE_CPU_RESOURCE
#	define DEFAULT_BANDWIDTH_RESOURCE NWSAPI_DEFAULT_BANDWIDTH_RESOURCE
#	define DEFAULT_CURRENT_CPU_RESOURCE NWSAPI_DEFAULT_CURRENT_CPU_RESOURCE
#	define DEFAULT_LATENCY_RESOURCE NWSAPI_DEFAULT_LATENCY_RESOURCE
#	define DEFAULT_MEMORY_RESOURCE NWSAPI_DEFAULT_MEMORY_RESOURCE
#	define SeriesSpec NWSAPI_SeriesSpec
#	define BEGINNING_OF_TIME NWSAPI_BEGINNING_OF_TIME
#	define EnvironmentValue NWSAPI_EnvironmentValue
#	define EnvironmentValue_r NWSAPI_EnvironmentValue_r
#	define GetDefaultPort NWSAPI_GetDefaultPort
#	define IntermachineResource NWSAPI_IntermachineResource
#	define IsResourceValid NWSAPI_IsResourceValid
#	define MakeSeriesSpec NWSAPI_MakeSeriesSpec
#	define MakeHostSpec NWSAPI_MakeHostSpec
#	define METHOD_NAME_LENGTH NWSAPI_METHOD_NAME_LENGTH
#	define ForecasterMethodName NWSAPI_ForecasterMethodName
#	define GenerateForecasts NWSAPI_GenerateForecasts
#	define GetForecasts NWSAPI_GetForecasts
#	define GetForecast NWSAPI_GetForecast
#	define AutoFetchBegin NWSAPI_AutoFetchBegin
#	define AutoFetchCheck NWSAPI_AutoFetchCheck
#	define AutoFetchEnd NWSAPI_AutoFetchEnd
#	define AutoFetchAlready NWSAPI_AutoFetchAlready
#	define AutoFetchError NWSAPI_AutoFetchError
#	define AutoFetchMessage NWSAPI_AutoFetchMessage
#	define GetMeasurements NWSAPI_GetMeasurements
#	define GetMeasurement NWSAPI_GetMeasurement
#	define PutMeasurements NWSAPI_PutMeasurements
#	define RelaxHostNameCheck NWSAPI_RelaxHostNameCheck
#	define GetVirtualForecasts NWSAPI_GetVirtualForecasts
#	define HostCompare NWSAPI_HostCompare
#	define GetAllForecasts NWSAPI_GetAllForecasts
#	define GetCliquesForecasts NWSAPI_GetCliquesForecasts
#	define GetCliqueForecasts NWSAPI_GetCliqueForecasts
#	define StartFetching NWSAPI_StartFetching
#	define StartCliqueFetching NWSAPI_StartCliqueFetching
#	define FindHostsPath NWSAPI_FindHostsPath
#	define FindCliquesPath NWSAPI_FindCliquesPath
#	define SeriesName NWSAPI_SeriesName
#	define SeriesName_r NWSAPI_SeriesName_r
#	define UseMemory NWSAPI_UseMemory
#	define NwsAttribute NWSAPI_NwsAttribute
#	define NwsAttributeName_r NWSAPI_NwsAttributeName_r
#	define NwsAttributeValue_r NWSAPI_NwsAttributeValue_r
#	define Object NWSAPI_Object
#	define ObjectSet NWSAPI_ObjectSet
#	define ACTIVITIES NWSAPI_ACTIVITIES
#	define CLIQUES NWSAPI_CLIQUES
#	define CONTROLS NWSAPI_CONTROLS
#	define FORECASTERS NWSAPI_FORECASTERS
#	define HOSTS NWSAPI_HOSTS
#	define MEMORIES NWSAPI_MEMORIES
#	define SENSORS NWSAPI_SENSORS
#	define SERIES NWSAPI_SERIES
#	define SKILLS NWSAPI_SKILLS
#	define CLASS_ATTR NWSAPI_CLASS_ATTR
#	define CONTROL_ATTR NWSAPI_CONTROL_ATTR
#	define HOST_ATTR NWSAPI_HOST_ATTR
#	define IP_ATTR NWSAPI_IP_ATTR
#	define MEMBER_ATTR NWSAPI_MEMBER_ATTR
#	define MEMORY_ATTR NWSAPI_MEMORY_ATTR
#	define NAME_ATTR NWSAPI_NAME_ATTR
#	define OPTION_ATTR NWSAPI_OPTION_ATTR
#	define PERIOD_ATTR NWSAPI_PERIOD_ATTR
#	define PORT_ATTR NWSAPI_PORT_ATTR
#	define RESOURCE_ATTR NWSAPI_RESOURCE_ATTR
#	define SENSOR_ATTR NWSAPI_SENSOR_ATTR
#	define SKILL_ATTR NWSAPI_SKILL_ATTR
#	define SOURCE_ATTR NWSAPI_SOURCE_ATTR
#	define TARGET_ATTR NWSAPI_TARGET_ATTR
#	define UNIT_LABEL_ATTR NWSAPI_UNIT_LABEL_ATTR
#	define NwsAttributeName NWSAPI_NwsAttributeName
#	define NwsAttributeValue NWSAPI_NwsAttributeValue
#	define NextNwsAttribute NWSAPI_NextNwsAttribute
#	define FindNWsAttribute NWSAPI_FindNWsAttribute
#	define FirstNwsAttribute NWSAPI_FirstNwsAttribute
#	define ForEachNwsAttribute NWSAPI_ForEachNwsAttribute
#	define NextObject NWSAPI_NextObject
#	define FirstObject NWSAPI_FirstObject
#	define ForEachObject NWSAPI_ForEachObject
#	define AddObject NWSAPI_AddObject
#	define DeleteObject NWSAPI_DeleteObject
#	define NewObjectSet NWSAPI_NewObjectSet
#	define FreeObjectSet NWSAPI_FreeObjectSet
#	define GetHostsTimeout NWSAPI_GetHostsTimeout
#	define GetHosts NWSAPI_GetHosts
#	define GetNameServerTimeout NWSAPI_GetNameServerTimeout
#	define GetNameServer NWSAPI_GetNameServer
#	define GetMemory NWSAPI_GetMemory
#	define GetMemoryTimeout NWSAPI_GetMemoryTimeout
#	define GetObjects NWSAPI_GetObjects
#	define GetObjectsTimeout NWSAPI_GetObjectsTimeout
#	define UseNameServer NWSAPI_UseNameServer
#	define UseProxy NWSAPI_UseProxy
#	define HaltActivity NWSAPI_HaltActivity
#	define StartActivity NWSAPI_StartActivity
#	define RestartActivities NWSAPI_RestartActivities
#endif	/* NWSAPI_SHORTNAMES */

#ifdef NWS_API_COMPAT
#	define NWSAPI_Attribute NWSAPI_NwsAttribute
#	define NWSAPI_AttributeName NWSAPI_NwsAttributeName
#	define NWSAPI_AttributeValue NWSAPI_NwsAttributeValue
#	define NWSAPI_NextAttribute NWSAPI_NextNwsAttribute
#	define NWSAPI_FirstAttribute NWSAPI_FirstNwsAttribute
#	define NWSAPI_ForEachAttribute NWSAPI_ForEachNwsAttribute
#endif  /* NWS_API_COMPAT */



#ifdef	__cplusplus
}
#endif

#endif
