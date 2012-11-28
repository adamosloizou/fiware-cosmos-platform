/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#ifndef _H_SAMSON_ZOO_NODE_COMITTER
#define _H_SAMSON_ZOO_NODE_COMITTER

#include <list>
#include <string>
#include <vector>

#include "au/containers/SharedPointer.h"
#include "au/containers/Uint64Vector.h"

#include "engine/Engine.h"
#include "engine/Notification.h"
#include "engine/NotificationListener.h"

#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/gpb_operations.h"

#include "samson/worker/CommitCommand.h"
#include "zoo/Connection.h"
#include "zoo/common.h"

namespace au {
const int node_commiter_num_commits_to_save_ = 10;

/**
 * \brief StringCommitRecord is a record-log for StringDataModel class
 */


class StringCommitRecord {
public:

  StringCommitRecord(const std::string& caller
                     , const std::string& commit_command
                     , const std::string& result
                     , double commit_time) {
    caller_ = caller;
    commit_command_ = commit_command;
    result_ = result;
    commit_time_ = commit_time;
  }

  std::string caller() const {
    return caller_;
  }

  std::string commit_command() const {
    return commit_command_;
  }

  std::string result() const {
    return result_;
  }

  double commit_time() const {
    return commit_time_;
  }

private:

  std::string caller_;
  std::string commit_command_;
  std::string result_;
  double commit_time_;
};

/**
 * \brief StringDataModel is a distributed synchronous efficient data-model based on zookeeper where commits are strings
 */

template<class C>     // C is suppoused to be a gpb message class
class StringDataModel : public engine::NotificationListener {
public:

  StringDataModel(zoo::Connection *zoo_connection, const std::string& path) : token_("ZooNodeCommiter") {
    zoo_connection_ = zoo_connection;
    path_ = path;

    // Create sub-directories for commits and versions
    zoo_connection_->Create(path);
    zoo_connection_->Create(path + "/versions");
    zoo_connection_->Create(path + "/commits");

    version_ = -1;           // No current version
  }

  /*
   * \brief Virtual method to init data model "C c_"
   */

  virtual void Init(au::SharedPointer<C> c) = 0;

  /*
   * \brief Virtual method to perform a commit over data model
   */

  virtual void PerformCommit(au::SharedPointer<C> c
                             , std::string commit_command
                             , int version
                             , au::ErrorManager& error) = 0;


  /*
   * \brief Virtual method to be notified from engine system
   * virtual method of engine::NotificationListener
   */

  virtual void notify(engine::Notification *notification) {
    UpdateToLastVersion();   // Update to the last version
  }

  /*
   * \brief Get current data model
   */
  au::SharedPointer<C> getCurrentModel() {
    au::TokenTaker tt(&token_);

    return c_;
  }

  /*
   * \brief Get a copy of current data model
   */
  au::SharedPointer<C> getDuplicatedCurrentModel() {
    au::TokenTaker tt(&token_);

    au::SharedPointer<C> c(new C());
    c->CopyFrom(*c_.shared_object());
    return c;
  }

  /*
   * \brief Perform a new commit over data model
   */
  void Commit(const std::string& caller, const std::string& commit_command, au::ErrorManager& error) {
    au::TokenTaker tt(&token_);

    // Perform intern commit ( counting time )
    au::Cronometer cronometer;

    InternCommit(commit_command, error);
    double time = cronometer.seconds();

    // Log activity for debugging
    if (error.IsActivated()) {
      last_commits_.push_front(StringCommitRecord(caller, commit_command, error.GetMessage(), time));
    } else {
      last_commits_.push_front(StringCommitRecord(caller, commit_command, "OK", time));
    }
    while (last_commits_.size() > 100) {
      last_commits_.pop_back();
    }
  }

  /*
   * \brief Update to the last available version taking commits from other comitters
   */

  int UpdateToLastVersion() {
    au::TokenTaker tt(&token_);

    // If version == -1 --> Recover the last version from /versions subdirectory
    if (version_ == -1) {
      au::StringVector str_versions;
      int rc = zoo_connection_->GetChildrens(GetVersionsPath(), str_versions);
      if (rc) {
        Reset();
        return rc;
      }

      if (str_versions.size() == 0) {
        c_.Reset(new C());        // Create a new data-model instance
        Init(c_);
        version_ = 0;
      } else {
        std::vector<int> int_versions = GetVectorOfInts(str_versions);  // Get the int version of the same vector

        // Get the last version we have here
        version_ = au::max_element(int_versions, 0);
        c_.Reset(new C());
        rc = zoo_connection_->Get(GetVersionPath(version_),  c_.shared_object());
        if (rc) {
          Reset();
          return rc;
        }
      }
    }

    // Update to the last commit...
    while (true) {
      int rc = zoo_connection_->Exists(GetCommitPath(version_), engine_id());
      if (rc == ZNONODE) {
        return 0;   // This node still do not exist, so we are in the last available version
      }
      if (rc) {
        return rc;   // Any other error is not accepted
      }

      // Read the commit
      std::string commit_command;
      rc = zoo_connection_->Get(GetCommitPath(version_), commit_command);
      if (rc) {
        return rc;
      }

      // Apply this commit to our data model ( errors are not expected since they are supposued to be tested by committer )
      au::ErrorManager error;
      PerformCommit(c_, commit_command, version_, error);
      if (error.IsActivated()) {
        LOG_E(logs.zoo, ("Error in commit %s", commit_command.c_str()));
      }
      version_++;
    }
  }

  std::string GetCommitPath(int version) const {
    return path_ + "/commits/" + au::str("%d", version);
  }

  std::string GetVersionPath(int version) const {
    return path_ + "/versions/" + au::str("%d", version);
  }

  std::string GetCommitsPath() const {
    return path_ + "/commits";
  }

  std::string GetVersionsPath() const {
    return path_ + "/versions";
  }

  int version() const {
    return version_;
  }

  std::vector<StringCommitRecord> last_commits() const {
    std::vector<StringCommitRecord> copy_last_commits;
    std::list<StringCommitRecord>::const_iterator it;
    for (it = last_commits_.begin(); it != last_commits_.end(); it++) {
      copy_last_commits.push_back(*it);
    }
    return copy_last_commits;
  }

private:

  void InternCommit(const std::string& commit_command, au::ErrorManager& error) {
    // Mutex protection
    au::TokenTaker tt(&token_);

    int trial = 0;

    while (true) {
      trial++;

      // Load version if still not loaded
      if (version_ == -1) {
        int rc = UpdateToLastVersion();
        if (rc) {
          error.set(au::str("Internal error in DataModelCommiter with ZK: %s", zoo::str_error(rc).c_str()));
          return;
        }
      }

      // Get a copy of the data model
      au::SharedPointer<C> c = getDuplicatedCurrentModel();

      LOG_M(logs.zoo, ("trying to performing commit %s over path %s", commit_command.c_str(), path_.c_str()));
      PerformCommit(c, commit_command, version_, error);     // Change on a duplicated data model
      if (error.IsActivated()) {          // If error in the operation itself, No commit is done at all
        return;
      }

      // Try to commit
      std::string path = GetCommitPath(version_);
      int rc = zoo_connection_->Create(path, 0, commit_command);

      if (rc == ZNODEEXISTS) {
        LOG_M(logs.zoo, ("Not possible to commit version %d (path %s).This mean another worker has commited first"
                         , version_
                         , path_.c_str()));

        int rc = UpdateToLastVersion();
        if (rc) {
          error.set(au::str("Error with ZK: %s", zoo::str_error(rc).c_str()));
          return;
        }
        continue;     // Loop to load again data model and commit
      }

      if (rc) {
        error.set(au::str("Error with ZK: %s ", zoo::str_error(rc).c_str()));  // Any other error cancel this operation
        return;
      }

      // Operation is commited correctly
      PerformCommit(c_, commit_command, version_, error);     // Real changes on data model
      version_++;
      ReviewInternalCommit();                                                 // Interal review to consolidate data model and remove paths
      return;
    }
  }

  void ReviewInternalCommit() {
    if (( version_ > 0 ) && (version_ % node_commiter_num_commits_to_save_ == 0)) {
      // Save this version to stable
      LOG_M(logs.zoo, ("Review of version %d for path %s... saving data to do %s"
                       , version_
                       , path_.c_str()
                       , GetVersionPath(version_).c_str()));

      int rc = zoo_connection_->Create(GetVersionPath(version_), 0, c_.shared_object());
      if (rc) {
        LOG_E(logs.zoo,
              ( "Not possible to create path %s: %s ", GetVersionPath(version_).c_str(), zoo::str_error(rc).c_str()));
        return;
      }

      // Remove old versions
      au::StringVector str_versions;
      rc = zoo_connection_->GetChildrens(GetVersionsPath(), str_versions);
      if (rc) {
        LOG_E(logs.zoo,
              ( "Not possible to get childrens of %s: %s ", GetVersionsPath().c_str(), zoo::str_error(rc).c_str()));
        return;
      }

      if (str_versions.size() == 0) {
        LOG_W(1, ("Internal error"));
        return;
      }

      // Remove old versions of data model
      std::vector<int> int_versions = au::GetVectorOfInts(str_versions);
      int last_version = au::max_element(int_versions, 0);
      std::vector<int> int_old_versions = au::GetVectorOfElementsLowerThan(int_versions, last_version);

      LOG_M(logs.zoo, ( "Last version of data model in %sis %d ", path_.c_str(), last_version ));
      LOG_M(logs.zoo, ( "Removing %lu old versions of data model in %s ", int_old_versions.size(), path_.c_str()));

      for (size_t i = 0; i < int_old_versions.size(); i++) {
        zoo_connection_->Remove(GetVersionPath(int_old_versions[i]));    // We are not interested in errores
      }

      // Remove really old commits
      au::StringVector str_commits;
      rc = zoo_connection_->GetChildrens(GetCommitsPath(), str_commits);
      if (rc) {
        LOG_E(logs.zoo,
              ( "Not possible to get childrens of %s: %s ", GetCommitsPath().c_str(), zoo::str_error(rc).c_str()));
        return;
      }

      std::vector<int> int_commits = GetVectorOfInts(str_commits);
      std::vector<int> int_old_commits = au::GetVectorOfElementsLowerThan(
        int_commits, last_version - 5 * node_commiter_num_commits_to_save_);
      for (size_t i = 0; i < int_old_commits.size(); i++) {
        zoo_connection_->Remove(GetCommitPath(int_old_commits[i]));    // We are not interested in errores
      }
    }
  }

  void Reset() {
    c_.Reset(new C());
    version_ = -1;
  }

  zoo::Connection *zoo_connection_;
  au::Token token_;
  std::string path_;
  au::SharedPointer<C> c_;
  int version_;

  // List of last commits for debugging
  std::list<StringCommitRecord> last_commits_;
};
}

#endif  // ifndef _H_SAMSON_ZOO_NODE_COMITTER
