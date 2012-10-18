#ifndef _H_SAMSON_KVRANGEDIVISION
#define _H_SAMSON_KVRANGEDIVISION

#include <algorithm>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "logMsg/logMsg.h"

#include "samson/common/common.h"
#include "samson/common/KVRange.h"
#include "samson/common/samson.pb.h"

namespace samson {

  // Complete set of ranges to cover the entire map of hgs
  class KVRangeDivision {
    
  public:
    
    KVRangeDivision();
    explicit KVRangeDivision(int num_divisions);
    ~KVRangeDivision(){};

    // Add break points to delimiter full range of hash-groups
    void AddDivision(int hg);
    void AddDivision(const KVRange& range);
    void AddDivision(const std::vector<KVRange>& ranges);
    
    // Get ranges with current divisions
    std::vector<KVRange> ranges() const;

  private:
    
    std::set<int> divisions_; // Pointer where a new KVRange will be defined
    

    
  };
  
}

#endif  // ifndef _H_SAMSON_KVRANGE
