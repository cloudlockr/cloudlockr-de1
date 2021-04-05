#include "jsmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/**
 * str: the character string representing a JSON object
 *
 * Returns array of jsmntok_t objects if parsing succeeds
 * If parsing fails, an error message is printed to stderr and NULL is returned
 */
extern jsmntok_t *str_to_json(const char *str) {
  jsmn_parser p;
  int num_tokens;
  jsmntok_t *tokens;
  int error;
  size_t len = strlen(str);

  jsmn_init(&p);
  num_tokens = jsmn_parse(&p, str, len, NULL, SIZE_MAX);
  if (num_tokens < 0) {
    switch (num_tokens) {
      case JSMN_ERROR_NOMEM:
        fprintf(stderr, "Not enough memory");
        break;
      case JSMN_ERROR_INVAL:
        fprintf(stderr, "JSON string contains invalid character");
        break;
      case JSMN_ERROR_PART:
        fprintf(stderr, "JSON string incomplete");
        break;
      default:
        fprintf(stderr, "Something really bad happened");
        break;
    }
    return NULL;
  }

  tokens = (jsmntok_t *)malloc(num_tokens * sizeof(jsmntok_t));
  if (!tokens) return NULL;

  error = jsmn_parse(&p, str, len, tokens, num_tokens);
  if (error < 0) {
    switch (error) {
      case JSMN_ERROR_NOMEM:
        fprintf(stderr, "Couldn't allocate %i tokens", num_tokens);
        break;
      case JSMN_ERROR_INVAL:
        fprintf(stderr, "JSON string contains invalid character");
        break;
      case JSMN_ERROR_PART:
        fprintf(stderr, "JSON string incomplete");
        break;
      default:
        fprintf(stderr, "Something really bad happened");
        break;
    }
    free(tokens);
    tokens = NULL;
    return NULL;
  }

  return tokens;
}

/**
 * From the example code
 *
 * Compare a given token with the string s
 * 
 * If they are equal, 1 is returned, otherwise 0 is returned
 */
extern int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 1;
  }
  return 0;
}
