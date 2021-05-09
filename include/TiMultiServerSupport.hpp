#pragma once

#include "tmss/RpcProcessRequest.hpp"
#include "tmss/RpcProcessResponse.hpp"

#include "tmss/StatusSyncPrimary.hpp"
#include "tmss/StatusSyncSecondary.hpp"

#ifdef TI_MSS_IMPL
ti::communicator::Communicator ti::MultiServerSupportBase::communicator;
#endif
