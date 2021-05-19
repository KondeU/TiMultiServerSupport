#ifdef TI_MSS_IMPL
// From tmss/MultiServerSupportBase.hpp
ti::communicator::Communicator ti::MultiServerSupportBase::communicator;
// From tmss/StatusSyncDataSet.hpp
std::mutex ti::dataset::StatusSyncDataHash::hashMutex;
ti::dataset::StatusSyncDataHash::HashType ti::dataset::StatusSyncDataHash::hashSeed;
#endif
