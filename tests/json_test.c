#include <stdio.h>
#include <stdlib.h>
#include "json_test.h"
#include "json.h"
#include "config.h"

void test_json_node() {
    json_node_p node = json_node_create();
    CU_ASSERT_PTR_NOT_NULL(node);

    CU_ASSERT_EQUAL(node->array_count, 0);
    CU_ASSERT_PTR_NULL(node->array);
    CU_ASSERT_PTR_NULL(node->parent);
    CU_ASSERT_PTR_NULL(node->key_name);
    CU_ASSERT_PTR_NULL(node->string_value);

    json_node_destroy(node);
}

void test_json_token_to_string() {
    CU_ASSERT_STRING_EQUAL(JSON_NODE_TOKEN_TO_STRING(JSON_NODE_TOKEN_ERROR), "JSON_NODE_TOKEN_ERROR");
    CU_ASSERT_STRING_EQUAL(JSON_NODE_TOKEN_TO_STRING(JSON_NODE_TOKEN_WS), "JSON_NODE_TOKEN_WS");
    CU_ASSERT_STRING_EQUAL(JSON_NODE_TOKEN_TO_STRING(JSON_NODE_TOKEN_OBJECT), "JSON_NODE_TOKEN_OBJECT");
    CU_ASSERT_STRING_EQUAL(JSON_NODE_TOKEN_TO_STRING(JSON_NODE_TOKEN_ARRAY), "JSON_NODE_TOKEN_ARRAY");
    CU_ASSERT_STRING_EQUAL(JSON_NODE_TOKEN_TO_STRING(JSON_NODE_TOKEN_STRING), "JSON_NODE_TOKEN_STRING");
    CU_ASSERT_STRING_EQUAL(JSON_NODE_TOKEN_TO_STRING(JSON_NODE_TOKEN_SEPARATOR), "JSON_NODE_TOKEN_SEPARATOR");
    CU_ASSERT_STRING_EQUAL(JSON_NODE_TOKEN_TO_STRING(JSON_NODE_TOKEN_NUMBER), "JSON_NODE_TOKEN_NUMBER");
    CU_ASSERT_STRING_EQUAL(JSON_NODE_TOKEN_TO_STRING(JSON_NODE_TOKEN_EOS), "JSON_NODE_TOKEN_EOS");
    CU_ASSERT_STRING_EQUAL(JSON_NODE_TOKEN_TO_STRING(-1), "JSON_NODE_TOKEN_UNKNOWN");
}

void test_json_node_array() {
    json_node_p parent = json_node_create();
    json_node_p node = json_node_create();
    
    CU_ASSERT_PTR_NOT_NULL(parent);
    CU_ASSERT_PTR_NOT_NULL(node);

    json_node_array_add(parent, node);
    CU_ASSERT_EQUAL(parent->array[0], node);
    CU_ASSERT_EQUAL(node->parent, parent);
    CU_ASSERT_EQUAL(parent->array_count, 1);
    CU_ASSERT_PTR_NULL(node->array);
    CU_ASSERT_PTR_NOT_NULL(node->parent);

    json_node_p node2 = json_node_create();
    json_node_array_add(parent, node2);
    CU_ASSERT_EQUAL(parent->array[1], node2);
    CU_ASSERT_PTR_NOT_NULL(node2->parent);
    CU_ASSERT_EQUAL(node2->parent, parent);
    CU_ASSERT_EQUAL(parent->array_count, 2);
    CU_ASSERT_PTR_NULL(node2->array);

    json_node_destroy(parent);
}

void test_json_state() {
    char *string  = "{ \"test\" : 123.34, \"bool\" : true, \"ref\" : null,"
        "\"array\" : [12.3,\"abc\", null, null, false, [\"abcdef\",false,null]]}";
    json_node_p node = json_parse_string(string);

    CU_ASSERT_PTR_NOT_NULL(node);
    CU_ASSERT_EQUAL(JSON_VALUE_TYPE_P(node), 0);
    json_node_p main_node = JSON_VALUE_PROPERTY_P(node, 0);
    CU_ASSERT_PTR_NOT_NULL(main_node);
    CU_ASSERT_EQUAL(JSON_NODE_TYPE_VALUE_OBJECT, JSON_VALUE_TYPE_P(main_node));
    json_node_p child_node = JSON_VALUE_PROPERTY_P(main_node, 0);
    CU_ASSERT_PTR_NOT_NULL(child_node);
    CU_ASSERT_EQUAL(JSON_NODE_TYPE_VALUE_NUMBER, JSON_VALUE_TYPE_P(child_node));
    CU_ASSERT_DOUBLE_EQUAL(123.340, JSON_VALUE_NUMBER_P(child_node), 0.001);
    CU_ASSERT_STRING_EQUAL("test", JSON_VALUE_KEY_P(child_node));
    child_node = JSON_VALUE_PROPERTY_P(main_node, 1);
    CU_ASSERT_PTR_NOT_NULL(child_node);
    CU_ASSERT_STRING_EQUAL("bool", JSON_VALUE_KEY_P(child_node));
    CU_ASSERT_EQUAL(JSON_NODE_TYPE_VALUE_BOOLEAN, JSON_VALUE_TYPE_P(child_node));
    CU_ASSERT_EQUAL(1, JSON_VALUE_BOOLEAN_P(child_node));
    child_node = JSON_VALUE_PROPERTY_P(main_node, 2);
    CU_ASSERT_PTR_NOT_NULL(child_node);
    CU_ASSERT_STRING_EQUAL("ref", JSON_VALUE_KEY_P(child_node));
    CU_ASSERT_EQUAL(JSON_NODE_TYPE_VALUE_NULL, JSON_VALUE_TYPE_P(child_node));
    CU_ASSERT_EQUAL(0, JSON_VALUE_NULL_P(child_node));
    child_node = JSON_VALUE_PROPERTY_P(main_node, 3);
    CU_ASSERT_PTR_NOT_NULL(child_node);
    CU_ASSERT_STRING_EQUAL("array", JSON_VALUE_KEY_P(child_node));
    CU_ASSERT_EQUAL(JSON_NODE_TYPE_VALUE_ARRAY, JSON_VALUE_TYPE_P(child_node));
    CU_ASSERT_EQUAL(6, JSON_VALUE_PROPERTY_COUNT_P(child_node));

    json_node_destroy(node);
}

void test_json_state2() {
    static char *string  = "{ \"test\" : 123.34, \"bool\" : true, \"texture\" : {"
        "\"ressource\" : \"texture.png\",\"areamap\" : ["
        "{\"name\":\"area1\", \"x1\":0.5,\"x2\":1.0,\"y1\":0.5,\"y2\":1.0},"
        "{\"name\":\"area2\", \"x1\":0.0,\"x2\":0.5,\"y1\":0.0,\"y2\":0.5}"
        "]}}";
    json_node_p document = json_parse_string(string);
    CU_ASSERT_PTR_NOT_NULL(document);

    json_node_p root = JSON_NODE_ROOT(document);
    CU_ASSERT_PTR_NOT_NULL(JSON_VALUE_PROPERTY_P(root, 0));
    CU_ASSERT_STRING_EQUAL("test", JSON_VALUE_KEY_P(JSON_VALUE_PROPERTY_P(root, 0)));
    CU_ASSERT_DOUBLE_EQUAL(123.34, JSON_VALUE_NUMBER_P(JSON_VALUE_PROPERTY_P(root, 0)), 0.001);
    CU_ASSERT_PTR_NOT_NULL(JSON_VALUE_PROPERTY_P(root, 1));
    CU_ASSERT_STRING_EQUAL("bool", JSON_VALUE_KEY_P(JSON_VALUE_PROPERTY_P(root, 1)));
    CU_ASSERT_EQUAL(1, JSON_VALUE_BOOLEAN_P(JSON_VALUE_PROPERTY_P(root, 1)));
    CU_ASSERT_PTR_NOT_NULL(JSON_VALUE_PROPERTY_P(root, 2));
    CU_ASSERT_STRING_EQUAL("texture", JSON_VALUE_KEY_P(JSON_VALUE_PROPERTY_P(root, 2)));
    CU_ASSERT_EQUAL(2, JSON_VALUE_PROPERTY_COUNT_P(JSON_VALUE_PROPERTY_P(root, 2)));
    
    json_node_destroy(document);
}


void test_json_from_file() {
    json_node_p document = json_parse_file(TESTDATAPATH "/json_fixture.json");
    CU_ASSERT_PTR_NOT_NULL(document);

    json_node_p root      = JSON_NODE_ROOT(document);

    char *texture_name    = json_node_get_string(root, ".texture");
    json_node_p area      = json_node_get(root, ".area");
    json_node_p animation = json_node_get(root, ".animation");

    CU_ASSERT_PTR_NOT_NULL(texture_name);
    CU_ASSERT_PTR_NOT_NULL(area);
    CU_ASSERT_PTR_NULL(animation);

    CU_ASSERT_STRING_EQUAL("./intro/logo.png", texture_name);
    CU_ASSERT_EQUAL(JSON_VALUE_PROPERTY_COUNT_P(area), 3);

    char *boolean = json_node_get_bool(root, ".setting.filter.mipmap");
    CU_ASSERT_PTR_NOT_NULL(boolean);
    CU_ASSERT_EQUAL(*boolean, 1);

    double *number = json_node_get_number(root, ".setting.filter.alpha");
    CU_ASSERT_PTR_NOT_NULL(number);
    CU_ASSERT_DOUBLE_EQUAL(*number, 0.5, 0.0001);

    json_node_destroy(document);
}

CU_pSuite get_json_testsuite() {
    /* add a suite to the registry */
    CU_pSuite pSuite = CU_add_suite("json test suite", NULL, NULL);
    /* add the tests to the suite */
    CU_add_test(pSuite, "json node test", test_json_node);
    CU_add_test(pSuite, "json node array test", test_json_node_array);
    CU_add_test(pSuite, "json node token to string test", test_json_token_to_string);
    CU_add_test(pSuite, "json node from file test", test_json_from_file);
    CU_add_test(pSuite, "json node from string", test_json_state);
    CU_add_test(pSuite, "json node from string (more complex)", test_json_state2);

    return pSuite;
}

