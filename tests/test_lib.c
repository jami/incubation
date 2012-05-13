#include "CUnit/Basic.h"
#include "CUnit/Automated.h"
#include "event_manager_test.h"
#include "configuration_test.h"
#include "state_test.h"
#include "json_test.h"
#include "ressource_test.h"
#include "image_test.h"

int main (int argc, char** argv) {
   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry()) {
      return CU_get_error();
   }

   /* event manager test suite */
   get_event_manager_testsuite();
   /* configuration test suite */
   get_configuration_testsuite();
   /* state test suite */
   get_state_testsuite();
   /* json tests */
   get_json_testsuite();
   /* ressource tests */
   get_ressource_testsuite();
   /* image testsuite  */
   get_image_testsuite();
   /* Run all tests using the CUnit Basic interface */
   /*CU_set_output_filename("cunit_testall");
   CU_list_tests_to_file();
   CU_automated_run_tests();*/

   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}
