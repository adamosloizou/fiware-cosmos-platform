#include "au/containers/Box.h"
#include "au/containers/SharedPointer.h"
#include "gtest/gtest.h"
#include "unitTest/TestClasses.h"

TEST(au_containers_Box, assignation) {
  EXPECT_EQ(TestBase::num_instances(),
            0) << "Error in number of instances of test class";

  {
    au::Box<TestBase> box;


    au::SharedPointer<TestBase> t(new TestBase(10));
    au::SharedPointer<TestBase> t2(new TestBase(10));
    au::SharedPointer<TestBase> t3(new TestBase(10));

    box.Insert(t);
    box.Insert(t2);

    EXPECT_TRUE(TestBase::num_instances() > 0) << "Something wrong with the instance counter for test class";

    EXPECT_EQ(box.Contains(t), true) << "Error in au::Box basic operations";
    EXPECT_EQ(box.Contains(t3), false) << "Error in au::Box basic operations";

    EXPECT_EQ((size_t)2, box.size()) << "Error in au::Box basic operations";
  }

  EXPECT_EQ(0, TestBase::num_instances()) << "Error in number of instances of test class";
}

