#include <stdio.h>
#include <stdlib.h>
#include "state_test.h"
#include "state.h"
#include "event.h"

void test_state_lifecycle() {
    state_p state = state_create("teststate");

    CU_ASSERT_TRUE(state != 0);
    CU_ASSERT_STRING_EQUAL(state->name, "teststate");
    CU_ASSERT_TRUE(state->_constructor  == 0);
    CU_ASSERT_TRUE(state->_destructor   == 0);
    CU_ASSERT_TRUE(state->_state_switch == 0);
    CU_ASSERT_TRUE(state->_update       == 0);

    state_destroy(state);
    free(state);
}

void test_state_list_lifecycle() {
    state_list_p list = state_list_create();    

    CU_ASSERT_TRUE(list != 0);
    CU_ASSERT_EQUAL(list->count, 0);
    CU_ASSERT_TRUE(list->head == 0);
    CU_ASSERT_TRUE(list->tail == 0);
    CU_ASSERT_TRUE(list->active == 0);

    state_list_destroy(list);
    free(list);    
}

void null_function() {
}

static char switch_path[100] = "";
static int switch_count = 0;

void inc_function() {
    switch_count++;
}

int state_switch_callback(char *active_state_name) {
    switch (switch_count) {
        case 5:
            strcpy(active_state_name, "teststateB");
            return 1;
        case 10:
            strcpy(active_state_name, "teststateC");
            return 1;
        case 12:
            strcpy(active_state_name, "teststateD");
            return 1;
        default:
            return 0;
    }

    return 0;
}

void test_state_list_functions() {
    switch_count = 0;

    event_manager_init();

    state_p stateA = state_create("teststateA");
    state_p stateB = state_create("teststateB");
    state_p stateC = state_create("teststateC");
    state_p stateD = state_create("teststateD");

    stateA->_constructor = stateA->_destructor = stateA->_update = &null_function;
    stateB->_constructor = stateB->_destructor = stateB->_update = &null_function;
    stateC->_constructor = stateC->_destructor = stateC->_update = &null_function;
    stateD->_constructor = stateD->_destructor = stateD->_update = &null_function;
    stateA->_update = stateB->_update = stateC->_update = stateD->_update = &inc_function;

    stateA->_state_switch = &state_switch_callback;
    stateB->_state_switch = &state_switch_callback;
    stateC->_state_switch = &state_switch_callback;
    stateD->_state_switch = &state_switch_callback;

    state_list_p list = state_list_create();

    state_list_add(list, stateA);
    state_list_add(list, stateB);
    state_list_add(list, stateC);
    state_list_add(list, stateD);

    CU_ASSERT_EQUAL(list->count, 4);
    CU_ASSERT_TRUE(list->active == 0);
    CU_ASSERT_TRUE(list->head == stateA);
    CU_ASSERT_TRUE(list->tail == stateD);

    static event_t tick_event = {EVENT_TYPE_SYSTEM_TICK, NULL};

    state_list_active(list, "teststateA");
    int i = 0;
    char next_state[255];

    for (; i < 30; i++) {
        if (list->active
            && list->active->_state_switch(next_state)) {
            state_list_active(list, next_state);
        }

        if (i == 5) {
            CU_ASSERT_STRING_EQUAL(list->active->name, "teststateB");

        }

        if (i == 10) {
            CU_ASSERT_STRING_EQUAL(list->active->name, "teststateC");
        }

        if (i == 12) {
            CU_ASSERT_STRING_EQUAL(list->active->name, "teststateD");
        }

        event_manager_trigger_handler(EVENT_TYPE_UPDATE, &tick_event);
    }

    state_list_destroy(list);
    free(list);
 
    event_manager_shutdown();
}

CU_pSuite get_state_testsuite() {
    /* add a suite to the registry */
    CU_pSuite pSuite = CU_add_suite("state test suite", NULL, NULL);
    /* add the tests to the suite */
    CU_add_test(pSuite, "State list lifecycle Test", test_state_list_lifecycle);
    CU_add_test(pSuite, "State lifecycle test", test_state_lifecycle);
    CU_add_test(pSuite, "State function test", test_state_list_functions);

    return pSuite;
}
