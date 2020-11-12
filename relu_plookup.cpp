TEST(plookup_composer, Relu_lookup_proof)
{
   waffle::PLookupComposer composer = waffle::PLookupComposer();
   composer.lookup_tables.emplace_back(std::move(generate_xor_table()));

   size_t num_bits = 30;

   uint32_t random_value = engine.get_random_uint32();
   // uint32_t random_value = 1073741824-1;//2**30-1
   fr witness_value = fr{ random_value, 0, 0, 0 }.to_montgomery_form();
   uint32_t witness_index = composer.add_variable(witness_value);

   std::vector<uint32_t> accumulators = composer.create_range_constraint(witness_index, num_bits);

   for (uint32_t j = 0; j < num_bits/2; ++j) {
       uint32_t result = (random_value >> (30U - (2 * (j+(32-num_bits)/2))));
       fr source = composer.get_variable(accumulators[j]).from_montgomery_form();
       uint32_t expected = static_cast<uint32_t>(source.data[0]);
       EXPECT_EQ(result, expected);
   }
   for (uint32_t j = 1; j < num_bits/2; ++j) {
       uint32_t left = (random_value >> (30U - (2 * j)));
       uint32_t right = (random_value >> (30U - (2 * (j - 1))));
       EXPECT_EQ(left - 4 * right < 4, true);
   }

   for(size_t i = 0; i < num_bits; i++){
       uint32_t quotient = random_value/16;
       uint32_t value = random_value - quotient * 16;
       random_value = quotient;

       uint64_t left = static_cast<uint64_t>(value);
       uint64_t right = left;
       uint32_t left_idx = composer.add_variable(fr(left));
       uint32_t right_idx = composer.add_variable(fr(right));

       uint32_t result_idx = composer.read_from_table(waffle::PLookupTableId::XOR, left_idx, right_idx);
       uint32_t add_idx = composer.add_variable(fr(left) + fr(right));

       composer.create_big_add_gate({ left_idx, right_idx, result_idx, add_idx, fr(1), fr(1), fr(1), fr(-1), fr(0) });
   }

   auto prover = composer.create_prover();

   auto verifier = composer.create_verifier();

   auto proof = prover.construct_proof();

   bool result = verifier.verify_proof(proof); // instance, prover.reference_string.SRS_T2);
   EXPECT_EQ(result, true);
}

TEST(plookup_composer, Relu_lookup_proof)
{
   waffle::PLookupComposer composer = waffle::PLookupComposer();
   composer.lookup_tables.emplace_back(std::move(generate_xor_table()));
 
   size_t num_bits = 30;
 
   // uint32_t random_value = engine.get_random_uint32();
   uint32_t random_value = 1073741824-1;//2**30-1
      
   std::cout<<random_value<<std::endl;
 
   fr witness_value = fr{ random_value, 0, 0, 0 }.to_montgomery_form();
   uint32_t witness_index = composer.add_variable(witness_value);
 
   std::vector<uint32_t> accumulators = composer.create_range_constraint(witness_index, num_bits);
 
   uint32_t num_variables = (uint32_t)composer.get_num_variables()-1;
 
   fr source = composer.get_variable(num_variables).from_montgomery_form();
   uint32_t expected = static_cast<uint32_t>(source.data[0]);
 
   std::cout<<expected<<std::endl;
 
   auto prover = composer.create_prover();
 
   auto verifier = composer.create_verifier();
 
   auto proof = prover.construct_proof();
 
   bool result = verifier.verify_proof(proof); // instance, prover.reference_string.SRS_T2);
   EXPECT_EQ(result, true);
}
