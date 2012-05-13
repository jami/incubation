#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "json.h"
#include "log.h"

static int expected_token_state;
static json_node_p json_state_object_stack[JSON_STATE_STACK_MAX];
static int json_state_layer[] = {
    /* 0 state JSON_STATE_BEGIN */
    JSON_NODE_TOKEN_OBJECT | JSON_NODE_TOKEN_ARRAY,
    /* 1 0 + object JSON_STATE_OBJECT_KEY */
    JSON_NODE_TOKEN_STRING,
    /* 2 1 + key JSON_STATE_KEY_SEP */
    JSON_NODE_TOKEN_SEPARATOR,
    /* 3 2 + sep JSON_STATE_OBJECT_VALUE */
    JSON_NODE_TOKEN_ALL_TYPES,
    /* 4 3 + value JSON_STATE_OBJECT_SEP_CLOSE */
    JSON_NODE_TOKEN_SEPARATOR | JSON_NODE_TOKEN_OBJECT,
    /* 5 0 + array */
    JSON_NODE_TOKEN_ALL_TYPES | JSON_NODE_TOKEN_ARRAY,
    /* 6 5 + value */
    JSON_NODE_TOKEN_SEPARATOR | JSON_NODE_TOKEN_ARRAY
};

void _json_node_dump_indent(char *c, int repeat) {
    int i = 0;
    for (; i < repeat; i++) {
        printf("%s", c);
    }
}

void _json_node_dump(json_node_p node, int depth, int isArray) {
    _json_node_dump_indent("--", depth);

    if (!node->value_type) {
        int j = 0;
        for (; j < node->array_count; j++) {
            _json_node_dump(node->array[j], depth + 1, 0);
        }
        printf("root node\n");
        return;
    }

    switch (node->value_type) {
        case JSON_NODE_TYPE_VALUE_STRING:
            if (isArray)
                printf(" (string) %s", node->string_value);
            else
                printf(" %s => (string) %s", node->key_name, node->string_value);
            break;
        case JSON_NODE_TYPE_VALUE_NUMBER:
            if (isArray)
                printf(" (number) %f", node->number_value);
            else
                printf(" %s => (number) %f", node->key_name, node->number_value);
            break;
        case JSON_NODE_TYPE_VALUE_NULL:
            if (isArray)
                printf(" (null) NULL");
            else
                printf(" %s => (null) NULL", node->key_name);
            break;
        case JSON_NODE_TYPE_VALUE_BOOLEAN:
            if (isArray)
                printf(" (boolean) %s", node->bool_value ? "true" : "false");
            else
                printf(" %s => (boolean) %s", node->key_name, node->bool_value ? "true" : "false");
            break;
        case JSON_NODE_TYPE_VALUE_OBJECT:
            if (isArray)
                printf(" (object)\n");
            else
                printf(" %s => (object)\n", node->key_name);
            _json_node_dump_indent("--", depth);
            printf(" {\n");
            int j = 0;
            for (; j < node->array_count; j++) {
                _json_node_dump(node->array[j], depth + 1, 0);
            }
            _json_node_dump_indent("--", depth);
            printf(" }");
            break;
        case JSON_NODE_TYPE_VALUE_ARRAY:
            if (isArray)
                printf(" (array)\n");
            else
                printf(" %s => (array)\n", node->key_name);
            
            if (0 == node->array_count) {
                _json_node_dump_indent("--", depth);
                printf(" []\n");
            } else {
                _json_node_dump_indent("--", depth);
                printf(" [\n");
                int k = 0;
                for (; k < node->array_count; k++) {
                    _json_node_dump(node->array[k], depth + 1, 1);
                }
                _json_node_dump_indent("--", depth);
                printf(" ]");
            }
            break;

    }

    printf("\n");
}

void json_node_dump(json_node_p node) {
    if (0 == node)
        return;

    int depth = 0;
    _json_node_dump(node, depth, 0);
}

json_node_p json_node_create() {
    json_node_p json_node   = malloc(sizeof(json_node_t));
    json_node->array        = 0;
    json_node->array_count  = 0;
    json_node->bool_value   = 0;
    json_node->number_value = 0;
    json_node->key_name     = 0;
    json_node->string_value = 0;
    json_node->parent       = 0;
    json_node->value_type   = 0;
    json_node->last_state   = 0;
    return json_node;
}

void json_node_array_add(json_node_p root, json_node_p child) {
    child->parent = root;
    root->array = (json_node_p*)realloc(root->array, sizeof(json_node_p) * (root->array_count + 1));
    root->array[root->array_count] = child;
    root->array_count++;
}

int json_node_object_push_value(json_node_p root, char *buffer, int *depth, int token) {
    json_node_p node = root->array[root->array_count - 1];
    switch (token) {
        case JSON_NODE_TOKEN_ARRAY:
            if (JSON_TOKEN_IS_OPEN(buffer[0])) {
                node->value_type = JSON_NODE_TYPE_VALUE_ARRAY;
                node->last_state = JSON_STATE_ARRAY_SEP_CLOSE;
                (*depth)++;
                json_state_object_stack[*depth] = node;
                return JSON_STATE_ARRAY_VALUE;
            }
            break;
        case JSON_NODE_TOKEN_OBJECT:
            if (JSON_TOKEN_IS_OPEN(buffer[0])) {
                node->last_state = JSON_STATE_OBJECT_KEY;
                node->value_type = JSON_NODE_TYPE_VALUE_OBJECT;
                (*depth)++;
                json_state_object_stack[*depth] = node;
                return JSON_STATE_OBJECT_KEY;
            }
            break;
        case JSON_NODE_TOKEN_CONST:
            if (0 == strcmp(buffer, "true")
                || 0 == strcmp(buffer, "false")) {
                node->value_type = JSON_NODE_TYPE_VALUE_BOOLEAN;
                node->bool_value = 0 == strcmp(buffer, "false") ? 0 : 1;
            } else if (0 == strcmp(buffer, "null")) {
                node->value_type = JSON_NODE_TYPE_VALUE_NULL;
            } else {
                return JSON_STATE_ERROR;
            }
            return root->value_type == JSON_NODE_TYPE_VALUE_OBJECT ?
                JSON_STATE_OBJECT_SEP_CLOSE : JSON_STATE_ARRAY_SEP_CLOSE;
        case JSON_NODE_TOKEN_NUMBER:
            node->value_type   = JSON_NODE_TYPE_VALUE_NUMBER;
            node->number_value = strtod(buffer, 0);
            return root->value_type == JSON_NODE_TYPE_VALUE_OBJECT ?
                JSON_STATE_OBJECT_SEP_CLOSE : JSON_STATE_ARRAY_SEP_CLOSE;
        case JSON_NODE_TOKEN_STRING:
            node->value_type   = JSON_NODE_TYPE_VALUE_STRING;
            node->string_value = strdup(buffer);
            return root->value_type == JSON_NODE_TYPE_VALUE_OBJECT ?
                JSON_STATE_OBJECT_SEP_CLOSE : JSON_STATE_ARRAY_SEP_CLOSE;
    }

    return 0;
}

void json_node_prepare_keyvalue(json_node_p root, char *buffer) {
    json_node_p node = json_node_create();
    node->key_name   = strdup(buffer);
    node->value_type = JSON_NODE_TYPE_VALUE;
    json_node_array_add(root, node);
}

void json_node_close_object(json_node_p node, int *depth, int token) {
    //json_state_object_stack[*depth] = 0;
    (*depth)--;
}

int json_state_next(int token, char *buffer, int layer, int *depth) {
    int next_state = 0;
    switch (layer) {
        case JSON_STATE_BEGIN:
            if (JSON_TOKEN_IS_OPEN(buffer[0])) {
                json_node_p node = json_node_create();
                node->value_type = (token == JSON_NODE_TOKEN_OBJECT) ?
                    JSON_NODE_TYPE_VALUE_OBJECT : JSON_NODE_TYPE_VALUE_ARRAY;
                (*depth)++;
                json_state_object_stack[*depth] = node;
                json_node_array_add(json_state_object_stack[*depth - 1], node);
                next_state = JSON_NODE_TOKEN_OBJECT == token ? JSON_STATE_OBJECT_KEY : JSON_STATE_ARRAY_VALUE;
            }
            break;
        case JSON_STATE_OBJECT_KEY:
            json_node_prepare_keyvalue(json_state_object_stack[*depth], buffer);
            next_state = JSON_STATE_KEY_SEP;
            break;
        case JSON_STATE_KEY_SEP:
            next_state = JSON_STATE_OBJECT_VALUE;
            break;
        case JSON_STATE_OBJECT_VALUE:
            json_state_object_stack[*depth]->last_state = JSON_STATE_OBJECT_SEP_CLOSE;
            next_state = json_node_object_push_value(json_state_object_stack[*depth], buffer, depth, token);
            break;
        case JSON_STATE_OBJECT_SEP_CLOSE:
            if (JSON_NODE_TOKEN_SEPARATOR == token) {
                next_state = JSON_STATE_OBJECT_KEY;
            } else if (JSON_NODE_TOKEN_OBJECT == token && !JSON_TOKEN_IS_OPEN(buffer[0])) {
                json_node_close_object(json_state_object_stack[*depth], depth, token);
                next_state = json_state_object_stack[*depth]->last_state;
            }
            break;
        case JSON_STATE_ARRAY_SEP_CLOSE:
            if (JSON_NODE_TOKEN_SEPARATOR == token) {
                next_state = JSON_STATE_ARRAY_VALUE;
            } else if (JSON_NODE_TOKEN_ARRAY == token && !JSON_TOKEN_IS_OPEN(buffer[0])) {
                json_node_close_object(json_state_object_stack[*depth], depth, token);
                next_state = json_state_object_stack[*depth]->last_state;
            }
            break;
        case JSON_STATE_ARRAY_VALUE:
            if (JSON_NODE_TOKEN_ARRAY == token && !JSON_TOKEN_IS_OPEN(buffer[0])) {
                json_node_close_object(json_state_object_stack[*depth], depth, token);
                next_state = json_state_object_stack[*depth]->last_state;
            } else {
                json_node_p node = json_node_create();
                json_state_object_stack[*depth]->last_state = JSON_STATE_ARRAY_SEP_CLOSE;
                json_node_array_add(json_state_object_stack[*depth], node);
                next_state = json_node_object_push_value(json_state_object_stack[*depth], buffer, depth, token);
            }
            break;
    }

    if (JSON_STATE_ERROR == next_state) {
        LOGF("Error while processing token %s %s", buffer, JSON_NODE_TOKEN_TO_STRING(token));
    }

    return next_state;
}

int json_token_get_type(char x) {
    if (JSON_TOKEN_IS_WS(x)) 
        return JSON_NODE_TOKEN_WS;
    else if (JSON_TOKEN_IS_ARRAY(x)) 
        return JSON_NODE_TOKEN_ARRAY; 
    else if (JSON_TOKEN_IS_OBJECT(x)) 
        return JSON_NODE_TOKEN_OBJECT; 
    else if (JSON_TOKEN_IS_STRING(x)) 
        return JSON_NODE_TOKEN_STRING; 
    else if (JSON_TOKEN_IS_SEPARATOR(x))
        return JSON_NODE_TOKEN_SEPARATOR;
    else if (JSON_TOKEN_IS_NUMBER(x))
        return JSON_NODE_TOKEN_NUMBER;
    else if (JSON_TOKEN_IS_CONST(x))
        return JSON_NODE_TOKEN_CONST;

    return JSON_NODE_TOKEN_ERROR; 
}

int json_token_get_ws_length(char *str, int index) {
    int len = 0;
    while (str[index] && JSON_TOKEN_IS_WS(str[index])) {
        len++;
        index++;
    }

    return len;
}

int json_token_get_number_length(char *str, int index) {
    int len = 0;
    while (str[index] && JSON_TOKEN_IS_NUMBER(str[index])) {
        len++;
        index++;
    }

    return len;
}

int json_token_get_const_length(char *str, int index) {
    int len = 0;
    while (str[index] && JSON_TOKEN_IS_CONST(str[index])) {
        len++;
        index++;
    }

    return len;
}

int json_token_get_string_length(char *str, int index) {
    int len = 0;
    while (str[index]) {
        if ('"' == str[index]
            && len > 0
            && '\\' != str[index - 1]) {
            break;
        }

        len++;
        index++;
    }

    if (str[index] == 0) {
        return -1;
    }

    return len;
}

char *json_token_next(char *string, int *index, int length, int *ret_token) {
    *ret_token = JSON_NODE_TOKEN_ERROR;

    if (*index >= length) {
        *ret_token = JSON_NODE_TOKEN_EOS;
        return 0;
    }

    char *buffer   = 0;
    int group_size = 0;
    int i          = *index;
    int offset     = 0;
    int token      = json_token_get_type(string[i]);

    if (JSON_NODE_TOKEN_ERROR == token) {
        LOGF("Unexpected token '%c'", string[i]);
        return 0;
    }

    if (JSON_NODE_TOKEN_WS == token) {
        group_size = json_token_get_ws_length(string, i);
    } else if (JSON_NODE_TOKEN_OBJECT == token) {
        group_size = 1;
    } else if (JSON_NODE_TOKEN_ARRAY == token) {
        group_size = 1;
    } else if (JSON_NODE_TOKEN_SEPARATOR == token) {
        group_size = 1;
    } else if (JSON_NODE_TOKEN_STRING == token) {
        group_size = json_token_get_string_length(string, i + 1);
        offset = 2;
        
        if (group_size == -1) {
           LOGF("Unable to detect closing string literal at %s", &string[i]);
           return 0;
        }
        i++;
    } else if (JSON_NODE_TOKEN_NUMBER == token) {
        group_size = json_token_get_number_length(string, i);        
    } else if (JSON_NODE_TOKEN_CONST == token) {
        group_size = json_token_get_const_length(string, i);
    } else {
        LOGF("Unable to process token %s", &string[i]);
        return 0;
    }

    buffer             = (char*)malloc(group_size + 1);
    buffer[group_size] = '\0';
    strncpy(buffer, (char*)&(string[i]), group_size);
    *index            += group_size + offset;
    *ret_token         = token;
    
    return buffer;
}

void json_node_destroy(json_node_p node) {
    if (node->array_count > 0) {
        int i = 0;
        for (;i < node->array_count; i++) {
            json_node_destroy(node->array[i]);
        }
        free(node->array);
    }

    if (node->key_name) {
        free(node->key_name);
    }

    if (node->string_value) {
        free(node->string_value);
    }
    
    free(node);
}

void json_parser_init() {
    expected_token_state = JSON_NODE_TOKEN_WS | JSON_NODE_TOKEN_OBJECT | JSON_NODE_TOKEN_ARRAY;
}

char *json_node_get_bool(json_node_p root, char *path) {
    json_node_p node = json_node_get(root, path);

    if (node && JSON_NODE_TYPE_VALUE_BOOLEAN  == node->value_type) {
        return &node->bool_value;
    }

    return 0;
}

double *json_node_get_number(json_node_p root, char *path) {
    json_node_p node = json_node_get(root, path);

    if (node && JSON_NODE_TYPE_VALUE_NUMBER  == node->value_type) {
        return &node->number_value;
    }

    return 0;
}

char *json_node_get_string(json_node_p root, char *path) {
    json_node_p node = json_node_get(root, path);

    if (node && JSON_NODE_TYPE_VALUE_STRING  == node->value_type) {
        return node->string_value;
    }

    return 0;
}

json_node_p json_node_get(json_node_p root, char *path) {
    static char pathname[500];

    if (0 == root
        || JSON_NODE_TYPE_VALUE_OBJECT != root->value_type) {
        return 0;
    }

    if ('.' != path[0]) {
        LOGF("json path contains no object '%s'", path);
        return 0;
    }

    int l = 0;
    int i = 1;
    for (; i < strlen(path); i++) {
        if ('.' == path[i])
            break;
    }

    strncpy(pathname, &path[1], i - 1);
    pathname[i - 1] = '\0';

    if (i >= strlen(path)) {
        l = 1;
    }

    int it;
    JSON_PROPERTY_ITERATE(root, it) {
        if (JSON_VALUE_PROPERTY_KEY_P(root, it)
            && 0 == strcmp(JSON_VALUE_PROPERTY_KEY_P(root, it), pathname)) {
            if (l) {
                return root->array[it];
            } else {
                return json_node_get(root->array[it], &path[i]);
            }
        }
    }
    
    return 0;
}

json_node_p json_parse_ressource(ressource_p ressource) {
    char *buffer = (char*)malloc(ressource->buffer_length + 1);
    memcpy(buffer, ressource->stream, ressource->buffer_length);
    buffer[ressource->buffer_length] = '\0';
    json_node_p doc = json_parse_string(buffer);
    free(buffer);
    return doc;
}

json_node_p json_parse_string(char *string) {
    char *buffer     = 0;
    int token        = JSON_NODE_TOKEN_ERROR;
    int index        = 0;
    int layer_index  = 0;
    int depth        = 0;
    int length       = strlen(string);
    json_node_p root = json_node_create();

    json_parser_init();
    json_state_object_stack[0] = root;

    while (0 != (buffer = json_token_next(string, &index, length, &token))) {
        if (JSON_NODE_TOKEN_EOS == token) {
            break;
        }

        if (JSON_NODE_TOKEN_WS == token) {
            free(buffer);
            continue;
        }

        if (!(token & json_state_layer[layer_index])) {
            LOGF("Parser did not expect token %s", buffer);
            free(buffer);
            free(root);
            return 0;
        }

        layer_index = json_state_next(token, buffer, layer_index, &depth);
        
        if (JSON_STATE_ERROR == layer_index) {
            LOGF("Error while processing token %s %s", buffer, JSON_NODE_TOKEN_TO_STRING(token));
            free(buffer);
            json_node_destroy(root);
            return 0;
        }

        free(buffer);
    }

    return root;
}

json_node_p json_parse_file(char *filename) {
    FILE *fp = fopen(filename, "r");

    if (!fp) {
        LOGF("Can't open json file %s", filename);
        return 0;
    }

    struct stat file_stat;
    if (fstat(fileno(fp), &file_stat) < 0) {
        LOGF("Can't get file stat %s", filename);
        fclose(fp);
        return 0;
    }

    long file_size = file_stat.st_size;
    char *buffer = (char*)malloc(file_size + 1);
    fread(buffer, file_size, 1, fp);
    buffer[file_size] = '\0';

    json_node_p document =  json_parse_string(buffer);
    fclose(fp);
    free(buffer);
    
    return document;
}

