#ifndef EXERCISE_01_SPLIT_IMPL
#define EXERCISE_01_SPLIT_IMPL

double get_normal_global();

void reinitialize_generators(unsigned int n_slots);

double get_normal_per_slot(unsigned int slot);

double get_normal_per_slot_and_entry(unsigned int slot,
                                     unsigned long long entry);

#endif
