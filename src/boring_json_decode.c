#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "boring_json.h"
#include "boring_json_debug.h"

#define BO_MIN(a, b) ((a < b) ? a : b)

enum bo_json_token_type {
	BO_JSON_TOKEN_NONE = '_',
	BO_JSON_TOKEN_NULL = 'n',
	BO_JSON_TOKEN_TRUE = 't',
	BO_JSON_TOKEN_FALSE = 'f',
	BO_JSON_TOKEN_STRING = 's',
	BO_JSON_TOKEN_NUMBER = '0',
	BO_JSON_TOKEN_OBJECT_START = '{',
	BO_JSON_TOKEN_OBJECT_END = '}',
	BO_JSON_TOKEN_ARRAY_START = '[',
	BO_JSON_TOKEN_ARRAY_END = ']',
	BO_JSON_TOKEN_COLON = ':',
	BO_JSON_TOKEN_COMMA = ',',
};

struct bo_json_token {
	enum bo_json_token_type type;
	const char *start;
	const char *end;
};

static size_t token_len(const struct bo_json_token *token)
{
	return token->end - token->start;
}

static void bo_debug_token(const struct bo_json_token *token)
{
#if CONFIG_DEBUG
	char buf[token_len(token) + 1];
	strncpy(buf, token->start, token_len(token));
	buf[token_len(token)] = 0;

	BO_DEBUG("token: type='%c' len=%lu str='%s'", token->type, token_len(token), buf);
#else
	(void)token;
#endif // CONFIG_DEBUG
}

struct bo_json_lexer {
	const char *start;
	const char *end;
	const char *pos;
};

/**
 * @brief Skip all the whitespaces
 *
 * @param[in] lexer Pointer of lexer
 * @return BO_JSON_ERROR_NONE if success
 * @return BO_JSON_ERROR_PARTIAL if end of input
 */
static struct bo_json_error lexer_skip_spaces(struct bo_json_lexer *lexer)
{
	for (; lexer->pos < lexer->end; lexer->pos++) {
		switch (*lexer->pos) {
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			continue;
		default:
			return BO_JSON_OK();
		}
	}

	return BO_JSON_ERROR(BO_JSON_ERROR_PARTIAL, lexer->pos, NULL);
}

/**
 * @brief Get next symbol from the lexer
 *
 * @param[in]  lexer Pointer of lexer
 * @param[out] token Pointer of token
 * @return BO_JSON_ERROR_NONE if success
 */
static struct bo_json_error lexer_next_symbol(struct bo_json_lexer *lexer,
					      struct bo_json_token *token)
{
	token->type = (enum bo_json_token_type) * lexer->pos;
	lexer->pos = token->end;

	bo_debug_token(token);
	return BO_JSON_OK();
}

/**
 * @brief Get next string from the lexer
 *
 * @param[in]  lexer Pointer of lexer
 * @param[out] token Pointer of token
 * @return BO_JSON_ERROR_NONE if success
 * @return BO_JSON_ERROR_TYPE_NOT_MATCH if not a string
 * @return BO_JSON_ERROR_PARTIAL if not a complete string
 */
static struct bo_json_error lexer_next_string(struct bo_json_lexer *lexer,
					      struct bo_json_token *token)
{
	token->type = BO_JSON_TOKEN_STRING;
	token->start += 1;
	for (; token->end < lexer->end; token->end++) {
		switch (*token->end) {
		case '"':
			lexer->pos = token->end + 1;
			bo_debug_token(token);
			return BO_JSON_OK();
		case '\\':
			if (token->end + 1 < lexer->end) {
				token->end++;
				switch (*(token->end + 1)) {
				case '"':
				case '\\':
				case '/':
				case 'b':
				case 'f':
				case 'n':
				case 'r':
				case 't':
					break;
				case 'u':
					// unicode
					for (int i = 0; i < 4; i++) {
						if (token->end + 1 >= lexer->end) {
							return BO_JSON_ERROR(BO_JSON_ERROR_PARTIAL,
									     token->start, NULL);
						}

						token->end++;
						if (!isxdigit((int)*token->end)) {
							return BO_JSON_ERROR(
								BO_JSON_ERROR_TYPE_NOT_MATCH,
								token->start, NULL);
						}
					}
					bo_debug_token(token);
					return BO_JSON_OK();
				default:
					lexer->pos = token->end;
					return BO_JSON_ERROR(BO_JSON_ERROR_TYPE_NOT_MATCH,
							     token->start, NULL);
				}
			}
			break;
		default:
			break;
		}
	}

	return BO_JSON_ERROR(BO_JSON_ERROR_PARTIAL, token->start, NULL);
}

/**
 * @brief Get next keyword from the lexer
 *
 * @param[in]  lexer   Pointer of lexer
 * @param[out] token   Pointer of token
 * @param[in]  keyword Pointer of string exclude first character
 * @return BO_JSON_ERROR_NONE if success
 * @return BO_JSON_ERROR_PARTIAL if end of input
 * @return BO_JSON_ERROR_INVALID_JSON if keyword not match
 */
static struct bo_json_error lexer_next_keyword(struct bo_json_lexer *lexer,
					       struct bo_json_token *token, const char *keyword)
{
	token->type = (enum bo_json_token_type)(*token->start);
	while (*keyword != '\0') {
		if (token->end >= lexer->end) {
			return BO_JSON_ERROR(BO_JSON_ERROR_PARTIAL, token->end, NULL);
		}
		if (*token->end != *keyword) {
			return BO_JSON_ERROR(BO_JSON_ERROR_INVALID_JSON, token->end, NULL);
		}
		token->end++;
		keyword++;
	}

	lexer->pos = token->end;

	bo_debug_token(token);
	return BO_JSON_OK();
}

/**
 * @brief Get next number from the lexer
 *
 * @param[in]  lexer Pointer of lexer
 * @param[out] token Pointer of token
 * @return BO_JSON_ERROR_NONE if success
 */
static struct bo_json_error lexer_next_number(struct bo_json_lexer *lexer,
					      struct bo_json_token *token)
{
	token->type = BO_JSON_TOKEN_NUMBER;
	// FIXME:
	// -01.1 should not pass
	// -0.0.0.0.1 should not pass
	for (; token->end < lexer->end; token->end++) {
		if ((*token->end < '0' || *token->end > '9') && (*token->end != '.')) {
			lexer->pos = token->end;
			break;
		}
	}

	bo_debug_token(token);
	return BO_JSON_OK();
}

/**
 * @brief Get the next token from the JSON lexer
 *
 * @param[in]  lexer Pointer of lexer
 * @param[out] token Pointer of token
 *
 * @return BO_JSON_ERROR_NONE if success
 * @return BO_JSON_ERROR_PARTIAL if end of input
 */
static struct bo_json_error lexer_next(struct bo_json_lexer *lexer, struct bo_json_token *token)
{
	struct bo_json_error err = lexer_skip_spaces(lexer);
	if (err.err != 0) {
		return err;
	}

	token->start = lexer->pos;
	token->end = lexer->pos + 1;

	switch (*token->start) {
	case '{':
	case '}':
	case '[':
	case ']':
	case ':':
	case ',':
		return lexer_next_symbol(lexer, token);
	case '"':
		return lexer_next_string(lexer, token);
	case 't':
		return lexer_next_keyword(lexer, token, "rue");
	case 'f':
		return lexer_next_keyword(lexer, token, "alse");
	case 'n':
		return lexer_next_keyword(lexer, token, "ull");
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
		return lexer_next_number(lexer, token);
	default:
		return BO_JSON_ERROR(BO_JSON_ERROR_PARTIAL, token->start, NULL);
	}
}

/**
 * @brief Expect next token from the JSON lexer
 *
 * @param[in]  lexer Pointer of lexer
 * @param[out] token Pointer of token
 * @param[in]  expect Expect token type
 *
 * @return BO_JSON_ERROR_NONE if success
 * @return BO_JSON_ERROR_PARTIAL if end of input
 * @return BO_JSON_ERROR_INVALID_JSON if type not match
 * #return BO_JSON_ERROR_TYPE_NOT_MATCH if string parsing failure
 */
static struct bo_json_error lexer_expect_next(struct bo_json_lexer *lexer,
					      struct bo_json_token *token,
					      enum bo_json_token_type type)
{
	struct bo_json_error err = lexer_next(lexer, token);
	if (err.err != BO_JSON_ERROR_NONE) {
		return err;
	}
	if (token->type != type) {
		return BO_JSON_ERROR(BO_JSON_ERROR_INVALID_JSON, token->start, NULL);
	}
	return BO_JSON_OK();
}

static struct bo_json_error decode_object(struct bo_json_lexer *lexer,
					  const struct bo_json_value_desc *desc, void *out);
static struct bo_json_error decode_array(struct bo_json_lexer *lexer,
					 const struct bo_json_value_desc *desc, void *out);

/**
 * @brief Decodes a null value from a JSON token and stores it in the specified output location.
 *
 * @param[in] token The JSON token containing the null value to decode.
 * @param[in] desc The description of the JSON value type
 * @param[out] out A base pointer to the output structure where the decoded null value will be
 * stored.
 *
 * @return BO_JSON_ERROR_NONE if success
 * #return BO_JSON_ERROR_TYPE_NOT_MATCH if desc is not BO_JSON_VALUE_TYPE_NULL
 */
static struct bo_json_error decode_null(const struct bo_json_token *token,
					const struct bo_json_value_desc *desc, void *out)
{
	(void)out;

	if (desc == NULL) {
		return BO_JSON_OK();
	}
	if (desc->type != BO_JSON_VALUE_TYPE_NULL) {
		if (desc->flags & BO_JSON_FLAGS_NULLABLE) {
			*((unsigned char *)out + desc->flags_offset) |= BO_JSON_NULL_BIT;
			return BO_JSON_OK();
		}
		return BO_JSON_ERROR(BO_JSON_ERROR_TYPE_NOT_MATCH, token->start, desc);
	}

	return BO_JSON_ERROR(BO_JSON_ERROR_NOT_SUPPORT, token->start, desc);
}

/**
 * @brief Decodes a true value from a JSON token and stores it in the specified output location.
 *
 * @param[in] token The JSON token containing the true value to decode.
 * @param[in] desc The description of the JSON value type
 * @param[out] out A base pointer to the output structure where the decoded true value will be
 * stored.
 *
 * @return BO_JSON_ERROR_NONE if success
 * #return BO_JSON_ERROR_TYPE_NOT_MATCH if desc is not BO_JSON_VALUE_TYPE_BOOL
 */
static struct bo_json_error decode_true(const struct bo_json_token *token,
					const struct bo_json_value_desc *desc, void *out)
{
	if (desc == NULL) {
		return BO_JSON_OK();
	}
	if (desc->type != BO_JSON_VALUE_TYPE_BOOL) {
		return BO_JSON_ERROR(BO_JSON_ERROR_TYPE_NOT_MATCH, token->start, desc);
	}

	*((bool *)((char *)out + desc->value_offset)) = true;

	return BO_JSON_OK();
}

/**
 * Decodes a false value from a JSON token and stores it in the specified output location.
 *
 * @param[in] token The JSON token containing the false value to decode.
 * @param[in] desc The description of the JSON value type
 * @param[out] out A base pointer to the output structure where the decoded false value will be
 * stored.
 *
 * @return BO_JSON_ERROR_NONE if success
 * #return BO_JSON_ERROR_TYPE_NOT_MATCH if desc is not BO_JSON_VALUE_TYPE_BOOL
 */
static struct bo_json_error decode_false(const struct bo_json_token *token,
					 const struct bo_json_value_desc *desc, void *out)
{
	if (desc == NULL) {
		return BO_JSON_OK();
	}
	if (desc->type != BO_JSON_VALUE_TYPE_BOOL) {
		return BO_JSON_ERROR(BO_JSON_ERROR_TYPE_NOT_MATCH, token->start, desc);
	}

	*((bool *)((char *)out + desc->value_offset)) = false;

	return BO_JSON_OK();
}

/**
 * Decodes a string value from a JSON token and stores it in the specified output location.
 *
 * @param[in] token The JSON token containing the string value to decode.
 * @param[in] desc The description of the JSON value type
 * @param[out] out A base pointer to the output structure where the decoded string value will be
 * stored.
 *
 * @return BO_JSON_ERROR_NONE if success
 * #return BO_JSON_ERROR_TYPE_NOT_MATCH if desc is not BO_JSON_VALUE_TYPE_CSTR
 * #return BO_JSON_ERROR_SUFFICIENT_SPACE if token length greater or equal to capacity of string
 */
static struct bo_json_error decode_string(const struct bo_json_token *token,
					  const struct bo_json_value_desc *desc, void *out)
{
	if (desc == NULL) {
		return BO_JSON_OK();
	}
	if (desc->type != BO_JSON_VALUE_TYPE_CSTR) {
		return BO_JSON_ERROR(BO_JSON_ERROR_TYPE_NOT_MATCH, token->start, desc);
	}

	if (token_len(token) >= desc->string.capacity) {
		return BO_JSON_ERROR(BO_JSON_ERROR_INSUFFICIENT_SPACE, token->start, desc);
	}

	strncpy((char *)out + desc->value_offset, token->start, token_len(token));
	*((char *)((char *)out + desc->value_offset + token_len(token))) = '\0';

	return BO_JSON_OK();
}

/**
 * Decodes a number value from a JSON token and stores it in the specified output location.
 *
 * @param[in] token The JSON token containing the number value to decode.
 * @param[in] desc The description of the JSON value type
 * @param[out] out A base pointer to the output structure where the decoded number value will be
 * stored.
 *
 * @return BO_JSON_ERROR_NONE if success
 * #return BO_JSON_ERROR_TYPE_NOT_MATCH if desc is not BO_JSON_VALUE_TYPE_INT and
 * BO_JSON_VALUE_TYPE_DOUBLE
 */
static struct bo_json_error decode_number(const struct bo_json_token *token,
					  const struct bo_json_value_desc *desc, void *out)
{
	if (desc == NULL) {
		return BO_JSON_OK();
	}
	if (desc->type != BO_JSON_VALUE_TYPE_INT && desc->type != BO_JSON_VALUE_TYPE_INT64 &&
	    desc->type != BO_JSON_VALUE_TYPE_DOUBLE) {
		return BO_JSON_ERROR(BO_JSON_ERROR_TYPE_NOT_MATCH, token->start, desc);
	}

	char buf[24] = {0};

	assert(sizeof(buf) > token_len(token));

	strncpy(buf, token->start, token_len(token));

	if (desc->type == BO_JSON_VALUE_TYPE_INT) {
		*((int *)((char *)out + desc->value_offset)) = (int)strtol(buf, NULL, 10);
	} else if (desc->type == BO_JSON_VALUE_TYPE_INT64) {
		*((int64_t *)((char *)out + desc->value_offset)) = (int64_t)strtoll(buf, NULL, 10);
	} else {
		*((double *)((char *)out + desc->value_offset)) = strtod(buf, NULL);
	}

	return BO_JSON_OK();
}

static struct bo_json_error decode_value(struct bo_json_lexer *lexer,
					 const struct bo_json_value_desc *desc, void *out)
{
	struct bo_json_token token;
	struct bo_json_error err;

	err = lexer_next(lexer, &token);
	if (err.err != BO_JSON_ERROR_NONE) {
		return err;
	}

	switch (token.type) {
	case BO_JSON_TOKEN_NULL:
		return decode_null(&token, desc, out);
	case BO_JSON_TOKEN_TRUE:
		return decode_true(&token, desc, out);
	case BO_JSON_TOKEN_FALSE:
		return decode_false(&token, desc, out);
	case BO_JSON_TOKEN_STRING:
		return decode_string(&token, desc, out);
	case BO_JSON_TOKEN_NUMBER:
		return decode_number(&token, desc, out);
	case BO_JSON_TOKEN_OBJECT_START:
		return decode_object(lexer, desc, out);
	case BO_JSON_TOKEN_ARRAY_START:
		return decode_array(lexer, desc, out);
	default:
		return BO_JSON_ERROR(BO_JSON_ERROR_NOT_SUPPORT, token.start, desc);
	}

	if (err.err != BO_JSON_ERROR_NONE) {
		return err;
	}

	return BO_JSON_OK();
}

/**
 * Decode an object key-value pair from a JSON lexer.
 *
 * This function decodes a key-value pair from a JSON lexer, matching the key with the provided
 * description.
 *
 * @param[in] lexer Pointer of lexer
 * @param[in] key Pointer of token which contain the key
 * @param[in] desc The description of the object value.
 * @param[out] out Pointer to the output value.
 * @return BO_JSON_ERROR_NONE if success
 */
static struct bo_json_error decode_object_key_value(struct bo_json_lexer *lexer,
						    const struct bo_json_token *key,
						    const struct bo_json_value_desc *desc,
						    void *out)
{
	struct bo_json_error err;

	const struct bo_json_obj_attr_desc *descs = (desc != NULL) ? desc->object.attr_descs : NULL;
	const size_t descs_len = (desc != NULL) ? desc->object.n_attr_descs : 0;

	for (size_t i = 0; i < descs_len; i++) {
		if (strncmp(descs[i].name, key->start, token_len(key)) != 0) {
			continue;
		}

		BO_DEBUG("decode obj: matched: name='%s' type=%d offset=%lu", descs[i].name,
			 descs[i].desc.type, descs[i].desc.value_offset);

		err = decode_value(lexer, &descs[i].desc, out);
		if (err.err != BO_JSON_ERROR_NONE) {
			return err;
		}

		if (desc != NULL) {
			*((bool *)((char *)out + descs[i].exist_offset)) = true;
		}

		return BO_JSON_OK();
	}

	return decode_value(lexer, NULL, 0);
}

static struct bo_json_error decode_object(struct bo_json_lexer *lexer,
					  const struct bo_json_value_desc *desc, void *out)
{
	struct bo_json_error err;

	if (desc != NULL) {
		out = (char *)out + desc->value_offset;
	}

	for (;;) {
		struct bo_json_token key, colon, next;

		// key
		err = lexer_expect_next(lexer, &key, BO_JSON_TOKEN_STRING);
		if (err.err != BO_JSON_ERROR_NONE) {
			return err;
		}

		// :
		err = lexer_expect_next(lexer, &colon, BO_JSON_TOKEN_COLON);
		if (err.err != BO_JSON_ERROR_NONE) {
			return err;
		}

		// value
		err = decode_object_key_value(lexer, &key, desc, out);
		if (err.err != BO_JSON_ERROR_NONE) {
			return err;
		}

		// , or }
		err = lexer_next(lexer, &next);
		if (err.err != BO_JSON_ERROR_NONE) {
			return err;
		}

		if (next.type == BO_JSON_TOKEN_COMMA) {
			continue;
		} else if (next.type == BO_JSON_TOKEN_OBJECT_END) {
			goto DONE;
		} else {
			return BO_JSON_ERROR(BO_JSON_ERROR_INVALID_JSON, next.start, NULL);
		}
	}

DONE:
	return BO_JSON_OK();
}

/**
 * @brief Decode JSON array and store values into out (only if desc is not NULL)
 *
 * @param[in]  lexer  pointer of lexer
 * @param[in]  desc   pointer of descriptor
 * @param[out] out    pointer of variable to store to
 * @return
 */
static struct bo_json_error decode_array(struct bo_json_lexer *lexer,
					 const struct bo_json_value_desc *desc, void *out)
{
	struct bo_json_error err;

	const struct bo_json_value_desc *elem_attr =
		(desc != NULL) ? desc->array.elem_attr_desc : NULL;
	const size_t elem_size = (desc != NULL) ? desc->array.elem_size : 0;
	void *array_base = (char *)out + ((desc != NULL) ? desc->value_offset : 0);

	size_t count = 0;
	while (desc == NULL || count < desc->array.capacity) {
		struct bo_json_token next;

		// value
		err = decode_value(lexer, elem_attr, (char *)array_base + (count * elem_size));
		if (err.err != BO_JSON_ERROR_NONE) {
			return err;
		}

		count += 1;

		// , or ]
		err = lexer_next(lexer, &next);
		if (err.err != BO_JSON_ERROR_NONE) {
			return err;
		}

		if (next.type == BO_JSON_TOKEN_COMMA) {
			continue;
		} else if (next.type == BO_JSON_TOKEN_ARRAY_END) {
			goto DONE;
		} else {
			return BO_JSON_ERROR(BO_JSON_ERROR_INVALID_JSON, next.start, NULL);
		}
	}

DONE:
	if (desc != NULL) {
		*((size_t *)((char *)out + desc->array.count_offset)) = count;
	}

	return BO_JSON_OK();
}

struct bo_json_error bo_json_decode(const char *in, const size_t in_len,
				    const struct bo_json_value_desc *obj_desc, void *out)
{
	struct bo_json_lexer lexer = {
		.start = in,
		.pos = in,
		.end = in + in_len,
	};

	struct bo_json_error err = decode_value(&lexer, obj_desc, out);
	if (err.err != BO_JSON_ERROR_NONE) {
		return err;
	}

	err.pos = lexer.pos;
	return err;
}
