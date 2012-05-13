/* 
 * File:   json.h
 * Author: jami
 *
 * Created on April 20, 2012, 3:34 AM
 * Supersimple jsonparser
 */
#ifndef JSON_H
#define	JSON_H

#include "ressource.h"

#define JSON_NODE_TOKEN_ERROR 0
#define JSON_NODE_TOKEN_WS 1
#define JSON_NODE_TOKEN_OBJECT 2
#define JSON_NODE_TOKEN_ARRAY 4
#define JSON_NODE_TOKEN_STRING 8
#define JSON_NODE_TOKEN_SEPARATOR 16
#define JSON_NODE_TOKEN_NUMBER 32
#define JSON_NODE_TOKEN_CONST 64
#define JSON_NODE_TOKEN_EOS 128
#define JSON_NODE_TOKEN_ALL_TYPES (JSON_NODE_TOKEN_OBJECT \
| JSON_NODE_TOKEN_ARRAY \
| JSON_NODE_TOKEN_STRING \
| JSON_NODE_TOKEN_CONST \
| JSON_NODE_TOKEN_NUMBER)

#define JSON_NODE_TYPE_OBJECT 1
#define JSON_NODE_TYPE_ARRAY  2
#define JSON_NODE_TYPE_VALUE  3
#define JSON_NODE_TYPE_VALUE_STRING 4
#define JSON_NODE_TYPE_VALUE_BOOLEAN 5
#define JSON_NODE_TYPE_VALUE_NULL 6
#define JSON_NODE_TYPE_VALUE_OBJECT 7
#define JSON_NODE_TYPE_VALUE_ARRAY 8
#define JSON_NODE_TYPE_VALUE_NUMBER 9

#define JSON_NODE_TOKEN_TO_STRING(x) \
    (x == JSON_NODE_TOKEN_ERROR ? "JSON_NODE_TOKEN_ERROR" : \
    x == JSON_NODE_TOKEN_WS ? "JSON_NODE_TOKEN_WS" : \
    x == JSON_NODE_TOKEN_OBJECT ? "JSON_NODE_TOKEN_OBJECT" : \
    x == JSON_NODE_TOKEN_ARRAY ? "JSON_NODE_TOKEN_ARRAY" : \
    x == JSON_NODE_TOKEN_STRING ? "JSON_NODE_TOKEN_STRING" : \
    x == JSON_NODE_TOKEN_SEPARATOR ? "JSON_NODE_TOKEN_SEPARATOR" : \
    x == JSON_NODE_TOKEN_NUMBER ? "JSON_NODE_TOKEN_NUMBER" : \
    x == JSON_NODE_TOKEN_CONST ? "JSON_NODE_TOKEN_CONST" : \
    x == JSON_NODE_TOKEN_EOS ? "JSON_NODE_TOKEN_EOS" : "JSON_NODE_TOKEN_UNKNOWN" \
    ) \

#define JSON_STATE_STACK_MAX 1000
#define JSON_STATE_BEGIN 0
#define JSON_STATE_OBJECT_KEY 1
#define JSON_STATE_KEY_SEP 2
#define JSON_STATE_OBJECT_VALUE 3
#define JSON_STATE_OBJECT_SEP_CLOSE 4
#define JSON_STATE_ARRAY_VALUE 5
#define JSON_STATE_ARRAY_SEP_CLOSE 6
#define JSON_STATE_ERROR 7

#define JSON_NODE_STATE_TO_STRING(x) \
    (x == JSON_STATE_BEGIN ? "JSON_STATE_BEGIN" : \
    x == JSON_STATE_OBJECT_KEY ? "JSON_STATE_OBJECT_KEY" : \
    x == JSON_STATE_KEY_SEP ? "JSON_STATE_KEY_SEP" : \
    x == JSON_STATE_OBJECT_VALUE ? "JSON_STATE_OBJECT_VALUE" : \
    x == JSON_STATE_OBJECT_SEP_CLOSE ? "JSON_STATE_OBJECT_SEP_CLOSE" : \
    x == JSON_STATE_ARRAY_VALUE ? "JSON_STATE_ARRAY_VALUE" : \
    x == JSON_STATE_ARRAY_SEP_CLOSE ? "JSON_STATE_ARRAY_SEP_CLOSE" : "JSON_STATE_UNKNOWN" \
    ) \

#define JSON_TOKEN_IS_OPEN(x) ((x == '{' || x == '[') ? 1 : 0)
#define JSON_TOKEN_IS_WS(x) ((x == ' ' || x == '\t' || x == '\n') ? 1 : 0)
#define JSON_TOKEN_IS_ARRAY(x) ((x == '[' || x == ']') ? 1 : 0)
#define JSON_TOKEN_IS_OBJECT(x) ((x == '{' || x == '}') ? 1 : 0)
#define JSON_TOKEN_IS_STRING(x) (x == '"' ? 1 : 0)
#define JSON_TOKEN_IS_SEPARATOR(x) ((x == ',' || x == ':') ? 1 : 0)
#define JSON_TOKEN_IS_NUMBER(x) (((x >= '0' && x <= '9' ) \
|| x == '.' || x == '-' \
|| x == '+' || x == 'E' || x == 'e') ? 1 : 0)
#define JSON_TOKEN_IS_CONST(x) \
 ( x == 't' || x == 'r'  \
|| x == 'u' || x == 'e' \
|| x == 'f' || x == 'a' \
|| x == 'l' || x == 's' \
|| x == 'n' ? 1 : 0) 

#define JSON_VALUE_TYPE_P(x) x->value_type
#define JSON_VALUE_STRING_P(x) x->string_value
#define JSON_VALUE_NUMBER_P(x) x->number_value
#define JSON_VALUE_BOOLEAN_P(x) x->bool_value
#define JSON_VALUE_KEY_P(x) x->key_name
#define JSON_VALUE_NULL_P(x) 0
#define JSON_VALUE_PROPERTY_P(x, i) (i < x->array_count ? x->array[i] : 0)
#define JSON_VALUE_PROPERTY_KEY_P(x, i) (i < x->array_count ? x->array[i]->key_name : 0)
#define JSON_VALUE_PROPERTY_COUNT_P(x) x->array_count

#define JSON_PROPERTY_ITERATE(x, c) for (c = 0; c < x->array_count; c++)

#define JSON_NODE_ROOT(x) ((x && x->array_count > 0) ? x->array[0] : 0)

typedef struct json_node_t {
    char   value_type;
    char   bool_value;
    double number_value;
    char  *key_name;
    char  *string_value;
    int    array_count;
    int    last_state;
    struct json_node_t **array;
    struct json_node_t *parent;
} json_node_t, *json_node_p;

/**
 * create a new json node
 * @return json_node_p
 */
json_node_p json_node_create();
/*
 * json_node_array_add
 * @param json_node_p parent
 * @param json_node_p child
 */
void json_node_array_add(json_node_p, json_node_p);
/**
 * implicite destructor. 
 * Node gets freed
 * @param json_node_p
 */
void json_node_destroy(json_node_p);
/**
 * init the parser states
 */
void json_parser_init();
/**
 * parse json string
 * @param char *string
 * @return json_node_p data
 */
json_node_p json_parse_string(char*);
/**
 * Read a file and pass the content to json_parse_string
 * @see json_parse_string
 * @param char *filename
 * @return json_node_p data
 */
json_node_p json_parse_file(char*);
/**
 * Read a ressource and pass the content to json_parse_string
 * @see json_parse_string
 * @param 
 * @return 
 */
json_node_p json_parse_ressource(ressource_p);
/**
 * Gets the next useful json token
 * @param char *json_string_buffer
 * @param int *index
 * @param int json_string_buffer_length
 * @param int *token
 * @return string
 */
char *json_token_next(char*, int*, int, int*);
/**
 * dumps a json_node_p buffer to stdout
 * @param json_node_p data
 */
void json_node_dump(json_node_p);
/**
 *
 * @param root
 * @param depth
 * @param token
 */
void json_node_create_object(json_node_p root, int *depth, int token);
/**
 *
 * @param node
 * @param depth
 * @param token
 */
void json_node_close_object(json_node_p node, int *depth, int token);
/**
 *
 * @param root
 * @param path
 * @return * double
 */
double *json_node_get_number(json_node_p root, char *path);
/**
 *
 * @param root
 * @param path
 * @return
 */
char *json_node_get_bool(json_node_p root, char *path);
/**
 *
 * @param root
 * @param path
 * @return
 */
char *json_node_get_string(json_node_p root, char *path);
/**
 * find json_node by path
 * @param root
 * @param path
 * @return
 */
json_node_p json_node_get(json_node_p root, char *path);

#endif	/* JSON_H */

