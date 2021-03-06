/*
Copyright 2020 The Magma Authors.
This source code is licensed under the BSD-style license found in the
LICENSE file in the root directory of this source tree.
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/*****************************************************************************
  Source      	SessionStateEnforcer.h
  Version     	0.1
  Date       	2020/08/08
  Product     	SessionD
  Subsystem   	5G managing & maintaining state & store of session of SessionD
                Fanout message to Access and UPF through respective client obj
  Author/Editor Sanjay Kumar Ojha
  Description 	Objects run in main thread context invoked by folly event
*****************************************************************************/

#pragma once

#include <unordered_map>
#include <map>
#include <unordered_set>
#include <vector>

#include <folly/io/async/EventBaseManager.h>
#include <lte/protos/mconfig/mconfigs.pb.h>
#include <lte/protos/policydb.pb.h>
#include "PipelinedClient.h"
#include "RuleStore.h"
#include "SessionState.h"
#include "SessionStore.h"
#include "AmfServiceClient.h"

namespace magma {

class SessionStateEnforcer {
 public:
  SessionStateEnforcer(
      std::shared_ptr<StaticRuleStore> rule_store, SessionStore& session_store,
      /*M5G specific parameter new objects to communicate UPF and response to
         AMF*/
      std::shared_ptr<PipelinedClient> pipelined_client,
      std::shared_ptr<AmfServiceClient> amf_srv_client,
      magma::mconfig::SessionD mconfig);

  ~SessionStateEnforcer() {}

  void attachEventBase(folly::EventBase* evb);

  void stop();

  folly::EventBase& get_event_base();

  /*Member functions*/
  bool m5g_init_session_credit(
      SessionMap& session_map, const std::string& imsi,
      const std::string& session_id, const SessionConfig& cfg);
  /*Charging & rule related*/
  void handle_session_init_rule_updates(
      SessionMap& session_map, const std::string& imsi,
      SessionState& session_state);

 private:
  std::vector<std::string> static_rules;

  ConvergedRuleStore GlobalRuleList;
  std::unordered_multimap<std::string, uint32_t> pdr_map_;
  std::unordered_multimap<std::string, uint32_t> far_map_;

  std::shared_ptr<StaticRuleStore> rule_store_;
  SessionStore& session_store_;
  std::shared_ptr<PipelinedClient> pipelined_client_;
  std::shared_ptr<AmfServiceClient> amf_srv_client_;
  folly::EventBase* evb_;
  std::chrono::seconds retry_timeout_;
  magma::mconfig::SessionD mconfig_;
  bool static_rule_init();
  /* To send response back to AMF
   * Fill the response structure and call rpc of AmfServiceClient
   */
  void prepare_response_to_access(
      const std::string& imsi, SessionState& session_state,
      const magma::lte::M5GSMCause m5gsmcause);

};  // End of class SessionStateEnforcer

}  // end namespace magma
