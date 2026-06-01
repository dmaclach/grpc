// Copyright 2026 The gRPC Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef GRPC_SRC_CORE_LIB_EVENT_ENGINE_CF_ENGINE_DISPATCH_OBJECT_UNIQUE_REF_H
#define GRPC_SRC_CORE_LIB_EVENT_ENGINE_CF_ENGINE_DISPATCH_OBJECT_UNIQUE_REF_H
#include <grpc/support/port_platform.h>

#ifdef GPR_APPLE
#include <AvailabilityMacros.h>
#ifdef AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER

#include <dispatch/dispatch.h>

#include <type_traits>

namespace grpc_event_engine::experimental {

template <typename T>
class DispatchObjectUniqueRef {
  static_assert(std::is_convertible<T, dispatch_object_t>::value,
                "T should be `dispatch_object_t` type");

 public:
  /* implicit */
  DispatchObjectUniqueRef(T dispatch_object = nullptr)
      : dispatch_object_ref_(dispatch_object) {}
  ~DispatchObjectUniqueRef() { reset(); }

  DispatchObjectUniqueRef(DispatchObjectUniqueRef const&) = delete;
  DispatchObjectUniqueRef& operator=(DispatchObjectUniqueRef const&) = delete;

  DispatchObjectUniqueRef(DispatchObjectUniqueRef&& other)
      : dispatch_object_ref_(other.release()) {};
  DispatchObjectUniqueRef& operator=(DispatchObjectUniqueRef&& other) {
    reset(other.release());
    return *this;
  }

  operator T() { return dispatch_object_ref_; }

  // Note: this is for passing a CFTypeRef as output parameter to a CF API, the
  // current ref is released (if any) regardless of whether new value is set
  T* operator&() {
    reset();
    return &dispatch_object_ref_;
  }

  T release() {
    T old = dispatch_object_ref_;
    dispatch_object_ref_ = nullptr;
    return old;
  }

  void reset(T other = nullptr) {
    if (dispatch_object_ref_ == other) {
      return;
    }
    T old = dispatch_object_ref_;
    dispatch_object_ref_ = other;
    if (old) {
      dispatch_release(old);
    }
  }

 private:
  T dispatch_object_ref_;
};

}  // namespace grpc_event_engine::experimental

#endif  // AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER
#endif  // GPR_APPLE

#endif  // GRPC_SRC_CORE_LIB_EVENT_ENGINE_CF_ENGINE_DISPATCH_OBJECT_UNIQUE_REF_H
