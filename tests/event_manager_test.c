#include "event_manager_test.h"
#include "event.h"

event_t test_event_receiver;

void event_manager_handler(event_p event) {
    test_event_receiver = *event;
}

void test_event_handler() {
    event_manager_init();

    event_t test_event;
    test_event.code = EVENT_TYPE_SYSTEM_TICK;
    test_event.data = (int*)malloc(sizeof(int));
    *((int*)(test_event.data)) = 1234;

    event_manager_add_handler(EVENT_TYPE_SYSTEM, &event_manager_handler);

    event_manager_trigger_handler(EVENT_TYPE_SYSTEM, &test_event);

    CU_ASSERT_EQUAL(test_event_receiver.code, test_event.code);
    CU_ASSERT_PTR_EQUAL(test_event_receiver.data, test_event.data);
    CU_ASSERT_EQUAL(*((int*)(test_event_receiver.data)), 1234);

    event_manager_delete_handler(EVENT_TYPE_SYSTEM, &event_manager_handler);

    test_event.code = EVENT_TYPE_SYSTEM_QUIT;
    event_manager_trigger_handler(EVENT_TYPE_SYSTEM, &test_event);

    CU_ASSERT_NOT_EQUAL(test_event_receiver.code, test_event.code);

    free(test_event.data);    
    event_manager_shutdown();
}

void test() {

}

CU_pSuite get_event_manager_testsuite() {
    /* add a suite to the registry */
    CU_pSuite pSuite = CU_add_suite("event manager test suite", NULL, NULL);
    /* add the tests to the suite */
    /* CU_add_test(pSuite, "Event Manager Initialization Test", test_initialization);*/
    CU_add_test(pSuite, "Event Manager Handler Test", test_event_handler);

    return pSuite;
}