#include "configuration_test.h"
#include "configuration.h"
#include "config.h"
#include <stdio.h>

void test_configuration_arguments() {
    static char *archive_path = TESTDATAPATH "/archive_fixture.tgz";
    static char *arguments[] = {
        "/path/to/execute",
        "--datapath=" TESTDATAPATH "/archive_fixture.tgz"
    };

    configuration_manager_init(2, arguments);
    CU_ASSERT_TRUE(configuration.data_path != NULL);
    CU_ASSERT_STRING_EQUAL(configuration.data_path, archive_path);

    configuration_manager_shutdown();
    CU_ASSERT_TRUE(configuration.data_path == NULL);
}

CU_pSuite get_configuration_testsuite() {
    /* add a suite to the registry */
    CU_pSuite pSuite = CU_add_suite("configuration test suite", NULL, NULL);
    /* add the tests to the suite */
    CU_add_test(pSuite, "Configuration Test 1", test_configuration_arguments);

    return pSuite;
}
