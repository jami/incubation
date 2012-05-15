#include "configuration.h"
#include "gui_test.h"
#include "gui.h"
#include "config.h"
#include <stdio.h>

void test_gui_arguments() {
    static char *archive_path = TESTDATAPATH "/gui_fixture.tgz";
    static char *arguments[] = {
        "/path/to/execute",
        "--datapath=" TESTDATAPATH "/gui_fixture.tgz"
    };

    event_manager_init();
    configuration_manager_init(2, arguments);
    CU_ASSERT_TRUE(configuration.data_path != NULL);
    CU_ASSERT_STRING_EQUAL(configuration.data_path, archive_path);
    graphic_manager_init();

    gui_p gui = gui_create("./gui/menu.cnf");
    CU_ASSERT_PTR_NOT_NULL(gui);

    event_manager_poll();

    gui_destroy(gui);


    event_manager_shutdown();
    configuration_manager_shutdown();
    graphic_manager_shutdown();

    CU_ASSERT_TRUE(configuration.data_path == NULL);
}

CU_pSuite get_gui_testsuite() {
    /* add a suite to the registry */
    CU_pSuite pSuite = CU_add_suite("gui test suite", NULL, NULL);
    /* add the tests to the suite */
    CU_add_test(pSuite, "Gui Test 1", test_gui_arguments);

    return pSuite;
}
