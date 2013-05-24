


int init_Basic(void)
{
   if (NULL == (temp_file = fopen("temp.txt", "w+"))) {
      return -1;
   }
   else {
      return 0;
   }
}


int clean_Basic(void)
{
   if (0 != fclose(temp_file)) {
      return -1;
   }
   else {
      temp_file = NULL;
      return 0;
   }
}

void testVAL(void)
{
  FILE * in;
  in = fopen("./testULTIMATE", "r");
  SymbolIdent* it = parse(in, NULL);
  CU_ASSERT(!strcmp(it->name, "sumlist")); //name of function
  CU_ASSERT(!strcmp(it->argnames->name, "x")); //name of arg to function
  CU_ASSERT(!strcmp(getCharVal(it->parseeTree->value), "ite")); // name of first expression in function
  CU_ASSERT(!strcmp(getCharVal(it->parseeTree->argList->target->value), "length")); // name of first expression in if-then-else
  CU_ASSERT(!strcmp(getCharVal(it->parseeTree->argList->target->argList->target->value), "x"));
  CU_ASSERT(!strcmp(getCharVal(it->parseeTree->argList->next->target->value), "plus"));
  CU_ASSERT(!strcmp(getCharVal(it->parseeTree->argList->next->target->argList->target->value), "hd"));
  CU_ASSERT(!strcmp(getCharVal(it->parseeTree->argList->next->target->argList->target->argList->value), "x"));
  CU_ASSERT(!strcmp(getCharVal(it->parseeTree->argList->next->target->argList->next->target->value), "sumlist"));
  CU_ASSERT(!strcmp(getCharVal(it->parseeTree->argList->next->target->argList->next->target->argList->target->value), "tl"));
  CU_ASSERT(!strcmp(getCharVal(it->parseeTree->argList->next->target->argList->next->target->argList->target->argList->target->value), "x"));
}
int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Basic", init_Basic, clean_Basic);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
   if ((NULL == CU_add_test(pSuite, "test of sub", testSUB)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}
