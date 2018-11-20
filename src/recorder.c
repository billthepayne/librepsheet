#include <stdlib.h>
#include <string.h>

#include "repsheet.h"
#include "recorder.h"

/**
 * @file recorder.c
 * @author Aaron Bedra
 * @date 12/09/2014
 */

/**
 * Records details about the request and stores them in Redis
 *
 * @param context the redis connection
 * @param timestamp the request timestamp
 * @param user_agent the requets user agent string
 * @param http_method the http method
 * @param uri the endpoint for the request
 * @param arguments the query string
 * @param redis_max_length the maximum number of requests to keep in Redis
 * @param redis_expiry the time to live for the entry
 * @param actor the ip address of the actor
 *
 * @returns an integer response
 */
int record(redisContext *context, char *timestamp, const char *user_agent,
           const char *http_method, char *uri, char *arguments, int redis_max_length,
           int redis_expiry, const char *actor, char *transaction_id)
{
  char *t, *ua, *method, *u, *args, *thehour, *rec, *rechour, *recua, *recuid, *hour;

  //TODO: should probably use asprintf here to save a bunch of
  //nonsense calls. Make sure there is a sane way to do this across
  //platforms.

  if (timestamp == NULL) {
    t = malloc(2);
    hour = malloc(2);
    sprintf(t, "%s", "-");
    sprintf(hour, "%s", "0"); // No timestamp = midnight... Maybe better to use "-" as well?
  } else {
    char *hoursep;

    t = malloc(strlen(timestamp) + 1);
    sprintf(t, "%s", timestamp);

    hour = malloc(strlen(timestamp) + 1);
    sprintf(hour, "%s", timestamp);
    hoursep = strchr(hour, ':');
    if (hoursep != 0)
      *hoursep = 0;
  }

  if (user_agent == NULL) {
    ua = malloc(2);
    sprintf(ua, "%s", "-");
  } else {
    ua = malloc(strlen(user_agent) + 1);
    sprintf(ua, "%s", user_agent);
  }

  if (http_method == NULL) {
    method = malloc(2);
    sprintf(method, "%s", "-");
  } else {
    method = malloc(strlen(http_method) + 1);
    sprintf(method, "%s", http_method);
  }

  if (uri == NULL) {
    u = malloc(2);
    sprintf(u, "%s", "-");
  } else {
    u = malloc(strlen(uri) + 1);
    sprintf(u, "%s", uri);
  }

  if (arguments == NULL) {
    args = malloc(2);
    sprintf(args, "%s", "-");
  } else {
    args = malloc(strlen(arguments) + 1);
    sprintf(args, "%s", arguments);
  }

  rec = (char*)malloc(strlen(t) + strlen(ua) + strlen(method) + strlen(u) + strlen(args) + 9);
  sprintf(rec, "%s, %s, %s, %s, %s", t, ua, method, u, args);

  rechour = (char *)malloc(strlen(t) + strlen(transaction_id) + strlen(method) + strlen(u) + 4);
  sprintf(rechour, "%s,%s,%s,%s", t, transaction_id, method, u);

  recua = (char *)malloc(strlen(ua) + 1);
  sprintf(recua, "%s", ua);

  recuid = (char *)malloc(strlen(actor) + 1);
  sprintf(recuid, "%s", actor);

  free(t);
  free(ua);
  free(method);
  free(u);
  free(args);

  redisReply * reply;

  redisCommand(context, "MULTI");
  // rec
  redisCommand(context, "LPUSH %s:requests %s", actor, rec);
  redisCommand(context, "LTRIM %s:requests 0 %d", actor, (redis_max_length - 1));
  if (redis_expiry > 0) {
    redisCommand(context, "EXPIRE %s:requests %d", actor, redis_expiry);
  }
  
  //rechour
  redisCommand(context, "LPUSH %s:requests:%s %s", actor, hour, rechour);
  redisCommand(context, "LTRIM %s:requests:%s 0 %d", actor, hour, (redis_max_length - 1));
  if (redis_expiry > 0) {
    redisCommand(context, "EXPIRE %s:requests:%s %d", actor, hour, redis_expiry);
  }

  //recuid
  redisCommand(context, "SET %s %s", transaction_id, recuid);
  if (redis_expiry > 0) {
    redisCommand(context, "EXPIRE %s %d", transaction_id, redis_expiry);
  }

  //recua
  redisCommand(context, "SET %s:userAgent %s", transaction_id, recua);
  if (redis_expiry > 0) {
    redisCommand(context, "EXPIRE %s:userAgent %d", transaction_id, redis_expiry);
  }

  reply = redisCommand(context, "EXEC");


  if (reply) {
    free(rec);
    free(rechour);
    free(recuid);
    free(recua);
    free(hour);
    freeReplyObject(reply);
    return LIBREPSHEET_OK;
  } else {
    free(rec);
    free(rechour);
    free(recuid);
    free(recua);
    free(hour);
    return DISCONNECTED;
  }
}
