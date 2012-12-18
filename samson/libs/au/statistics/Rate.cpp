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

#include "au/mutex/TokenTaker.h"
#include "au/string/StringUtilities.h"

#include "Rate.h"        // Own interface

namespace au {
Rate::Rate(int num_samples)
  : token_("Rate")
    , num_samples_(num_samples)
    , total_size_(0)
    , total_num_(0) {
  // Check valid num_samples
  if (num_samples_ <= 0) {
    num_samples_ = 1;                 // Init vectors
  }
  hits_ = (int *)malloc(num_samples_ * sizeof(int));
  size_ = (double *)malloc(num_samples_ * sizeof(double));

  for (int i = 0; i < num_samples_; i++) {
    hits_[i] = 0;
    size_[i] = 0;
  }

  last_time_correction = 0;
}

Rate::~Rate() {
  if (hits_ != NULL) {
    free(hits_);
  }
  if (size_ != NULL) {
    free(size_);
  }
}

void Rate::Push(size_t size) {
  au::TokenTaker tt(&token_);

  UpdateTime();

  // Global counters
  total_size_ += size;
  total_num_++;

  // Update only the first slot
  hits_[0]++;
  size_[0] += size;
}

void Rate::Clear() {
  au::TokenTaker tt(&token_);

  num_samples_ = 0;
  total_size_ = 0;
  total_num_ = 0;
  c.Reset();
  last_time_correction = 0;
}

std::string Rate::str() const {
  au::TokenTaker tt(&token_);

  // Update slots if necessary
  UpdateTime();

  // update_values();
  return au::str("[ Currently %s %s ] [ Accumulated in %s %s ]"
                 , au::str(hit_rate(), "hits/s").c_str()
                 , au::str(rate(), "B/s").c_str()
                 , au::str_detail(total_num_, "hits").c_str()
                 , au::str_detail(total_size_, "B").c_str()
                 );
}

size_t Rate::hits() const {
  return total_num_;
}

size_t Rate::size() const {
  return total_size_;
}

double Rate::rate() const {
  au::TokenTaker tt(&token_);

  UpdateTime();

  double total = 0;
  double alpha = 0.9;
  double current_weigth = 1;
  double total_weigth = 0;
  for (int i = 1; i < num_samples_; ++i) {
    total += current_weigth * size_[i];
    current_weigth *= alpha;
    total_weigth += current_weigth;
  }
  double tmp = total / total_weigth;
  return tmp;
}

double Rate::hit_rate() const {
  au::TokenTaker tt(&token_);

  UpdateTime();

  double total = 0;
  double alpha = 0.9;
  double current_weigth = 1;
  double total_weigth = 0;
  for (int i = 1; i < num_samples_; ++i) {
    total += current_weigth * (double)hits_[i];
    current_weigth *= alpha;
    total_weigth += current_weigth;
  }
  double tmp = total / total_weigth;
  return tmp;
}

void Rate::UpdateTime() const {
  // Take current time
  size_t time = c.seconds();

  if (time <= last_time_correction) {
    return;
  }

  // Compute difference with the last reference
  size_t diff = time - last_time_correction;

  // Move samples "diff" slots
  for (int i = 0; i < ((int)num_samples_ - (int)diff); ++i) {
    size_[num_samples_ - i - 1] = size_[num_samples_ - i - 2];
    hits_[num_samples_ - i - 1] = hits_[num_samples_ - i - 2];
  }

  // Init the new slots
  for (int i = 0; (i < (int)diff) && (i < num_samples_); i++) {
    size_[i] = 0;
    hits_[i] = 0;
  }

  // Set the new reference
  last_time_correction = time;
}

std::string Rate::str_debug() const {
  std::ostringstream output;

  for (int i = 0; i < num_samples_; i++) {
    output << "[" << size_[i] << "]";
  }
  return output.str();
}
}                   // end of namespace au::rate