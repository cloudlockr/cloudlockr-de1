#include "jsmn.h"
#include "test.h"
#include <stdio.h>
#include <stdlib.h>

static int tokens_equal(jsmntok_t *tok1, jsmntok_t *tok2) {
  return tok1->type == tok2->type && tok1->start == tok2->start &&
    tok1->end == tok1->end && tok1->size == tok2->size;
}

static void print_token(jsmntok_t *tok, const char *str) {
  if (!tok) return;
  fprintf(stderr, "--------TOKEN--------\n");
  switch (tok->type) {
    case JSMN_UNDEFINED:
      fprintf(stderr, "Type: UNDEFINED\n");
      break;
    case JSMN_OBJECT:
      fprintf(stderr, "Type: OBJECT\n");
      break;
    case JSMN_ARRAY:
      fprintf(stderr, "Type: ARRAY\n");
      break;
    case JSMN_STRING:
      fprintf(stderr, "Type: STRING\n");
      break;
    case JSMN_PRIMITIVE:
      fprintf(stderr, "Type: PRIMITIVE\n");
      break;
    default:
      fprintf(stderr, "Type: %i\n", tok->type);
      break;
  }

  fprintf(stderr, "Start position: %i, End position: %i\n", tok->start, tok->end);
  fprintf(stderr, "Contents of token: %.*s\n", tok->end - tok->start, str + tok->start);
  fprintf(stderr, "Token size: %i\n\n", tok->size);
}

static int compare_token_arrays(jsmntok_t *arr1, jsmntok_t *arr2, size_t num_tokens, const char *str) {
  jsmntok_t *iter1 = arr1, *iter2 = arr2;
  while (iter1 - arr1 < num_tokens && tokens_equal(iter1, iter2)) {
    iter1++;
    iter2++;
  }

  if (iter1 < arr1 + num_tokens) {
    print_token(iter1, str);
    print_token(iter2, str);
    return 0;
  }
  return 1;
}

jsmntok_t *str_to_json(const char *str);

int test_empty(void) {
  const char *str = "";
  jsmntok_t expected[1];
  jsmntok_t *result = str_to_json(str);
  expected[0].type = JSMN_UNDEFINED;
  expected[0].start = 0;
  expected[0].end = 0;
  expected[0].size = 0;
  check(compare_token_arrays(expected, result, 1, str));
  return 0;
}

int test_malformed(void) {
  const char *str = "{{{"; // Incomplete JSON object
  jsmntok_t *result = str_to_json(str);
  print_token(result, str);
  check(result == NULL);
  return 0;
}

int test_proper(void) {
  const char *str = "{}"; // Empty JSON object
  jsmntok_t expected[1];
  jsmntok_t expected2[6];
  const char *str2 = "{\"list\": [1, 2, 3]}";
  jsmntok_t *result = str_to_json(str);
  check(result != NULL);

  expected[0].type = JSMN_OBJECT;
  expected[0].start = 0;
  expected[0].end = 2;
  expected[0].size = 0;
  check(compare_token_arrays(expected, result, 1, str));
  free(result);

  result = str_to_json(str2);
  check(result != NULL);

  expected2[0].type = JSMN_OBJECT;
  expected2[0].start = 0;
  expected2[0].end = 19;
  expected2[0].size = 1;

  expected2[1].type = JSMN_STRING;
  expected2[1].start = 2;
  expected2[1].end = 6;
  expected2[1].size = 1;

  expected2[2].type = JSMN_ARRAY;
  expected2[2].start = 9;
  expected2[2].end = 18;
  expected2[2].size = 3;

  expected2[3].type = JSMN_PRIMITIVE;
  expected2[3].start = 10;
  expected2[3].end = 11;
  expected2[3].size = 0;

  expected2[4].type = JSMN_PRIMITIVE;
  expected2[4].start = 13;
  expected2[4].end = 14;
  expected2[4].size = 0;

  expected2[5].type = JSMN_PRIMITIVE;
  expected2[5].start = 16;
  expected2[5].end = 17;
  expected2[5].size = 0;
  check(compare_token_arrays(expected2, result, 6, str2));
  free(result);
  return 0;
}

int main(void) {
  test(test_empty, "test empty string");
  test(test_malformed, "test malformed JSON string");
  test(test_proper, "test various properly constructed JSON strings");
  printf("\nPASSED: %i\nFAILED: %i\n", test_passed, test_failed);
  return test_failed > 0;
}
