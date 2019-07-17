#include <eeros/control/MedianFilter.hpp>
#include <eeros/control/Constant.hpp>
#include <eeros/math/Matrix.hpp>

#include <gtest/gtest.h>

#include <sstream>
#include <string>

using namespace eeros;
using namespace eeros::control;


TEST(MedianFilterUnitTest, templateInstantiations) {
  MedianFilter<2> f1{};
  MedianFilter<5,int> f2{};
  MedianFilter<100,float> f3{};
  
  using namespace math; 
  MedianFilter<2,Matrix<2,2>> f4{};

  EXPECT_TRUE(true); // they would fail at compile time.
}


TEST(MedianFilterUnitTest, intMAFilter) {
  MedianFilter<5,int> mf{};
  
  Constant<int> c1{2};
  c1.run();
  mf.getIn().connect(c1.getOut());
  mf.run();

  // this test case also tests if enable is true by default.
  
  c1.setValue(5);
  c1.run();
  mf.run();
  c1.setValue(1);
  c1.run();
  mf.run();
  c1.setValue(4);
  c1.run();
  mf.run();
  c1.setValue(3);
  c1.run();
  mf.run();

  EXPECT_EQ (mf.getOut().getSignal().getValue(), 3);

  EXPECT_EQ (c1.getOut().getSignal().getTimestamp(), mf.getOut().getSignal().getTimestamp());
}


TEST(MedianFilterUnitTest, doubleMAFilter1) {
  MedianFilter<51> mf{};
  
  Constant<> c1{};
  mf.getIn().connect(c1.getOut());

  double values[51] = {7.67,-71.15,81.36,-38.97,4.05,35.45,-50.42,-83.72,13.17,-77.11,-38.02,-30.35,
    -77.1,52.63,-25.39,-41.61,5.47,40.19,-30.31,23.45,55.34,-59.2,-82.25,95.06,62.63,34.6,-17.78,
    -90.11,41.06,-29.87,-24.69,-2.42,-94,56.52,-44.9,91.15,-63.42,8.89,92.44,-79.48,-51.74,63.32,
    -22.6,-62.83,74.55,-57.16,-57.91,-23.36,76.63,-65.12,-84.94};
      
  for(int i = 0; i < 51; i++) {
	c1.setValue(values[i]);
	c1.run();
	mf.run();
  }

  EXPECT_DOUBLE_EQ (mf.getOut().getSignal().getValue(), -24.69);

  EXPECT_EQ (c1.getOut().getSignal().getTimestamp(), mf.getOut().getSignal().getTimestamp());
}


TEST(MedianFilterUnitTest, doubleMAFilter2) {
  /*
   * Tests enable disable feature and its performance benefit. 
   */
  MedianFilter<1001> mf{};
  
  Constant<> c1{};
  mf.getIn().connect(c1.getOut());
  mf.disable(); // process values fast since no sorting is done.  
  
  double values[1001] = {-14.66,63.2,78.63,-31.66,-11.65,27.75,-55.56,46.04,-1.69,-21.94,60.6,-4.14,
    -25.33,70.89,-45.12,-96.66,71.12,-56.7,-30.1,-47.89,-30.96,23.73,-97.1,72.5,-88.7,-69.49,
    -9.76000000000001,11.45,83.6,4.93000000000001,-33.46,-90.86,-1.14999999999999,-45.34,-30.47,80.9,
    -25.97,41.13,1.73,12.6,98.64,-30.39,39.32,-69.98,78.05,88.82,-7.94,99.1,65.05,80.81,28.73,-80.48,
    -74.12,99.07,-81.64,38.24,74.36,-35.59,-99.1,81.89,-83.1,29.81,29.95,28.96,-39.56,-84.68,39.53,
    99.31,43.22,31.58,-38.64,-26.18,-80.42,38.41,-24.49,-55.15,-18.05,29.27,-78.83,-62.55,-40.53,25.53,
    -2.81999999999999,13.55,-77.64,-48.63,-74.5,-69.16,94,-47.61,-73.94,-53.59,-70.58,28.49,71.17,-99.58,
    -97.96,-51.96,69.35,-93.06,42.68,22.19,84.69,-16.12,-63.13,40.93,-40.85,-56.84,-23.19,-78.63,-52.8,
    -18.51,-5.53,20.75,43.62,20.73,11.29,-31.36,2.26000000000001,61.15,79.99,76.25,-84.4,71.04,88.68,
    43.14,-27.12,14.19,-37.73,26.67,5.11,-54.83,19.01,-68.48,37.19,63.84,-5.62,-16.02,-66.78,91.67,-39.7,
    -29.73,-47.38,8.55,33.92,-77.38,-4.75,63.51,99.68,42.59,33.78,0.0300000000000011,2.18000000000001,
    -16.74,-80.9,45.19,-89.34,88.53,-38.11,-3.00999999999999,-44.06,16.55,-15.63,20.6,51.19,-95.56,-6.88,
    -1.23,-35.79,-35.07,13.06,-34.6,-91.15,-27.64,-92.49,91.2,-83.18,61.37,-16.46,28.62,43.88,-33.34,
    9.35000000000001,-64.8,-96.41,57.71,-7.14999999999999,34.67,51.17,-47.53,83.82,-76.9,-44.09,-48.2,
    -54.34,-76.23,-48.73,37.86,-71.77,-81.66,-90.08,92.11,-47.21,-25.83,-73.19,52.53,-95.45,17.23,81.86,
    -71.63,65.27,-46.64,-87.33,13.21,71.79,28.04,-20,-94.03,13.72,-10.6,18.82,-44.94,62.28,32.94,-22.92,
    56.42,-58.13,-26.53,-92.53,36.74,67.68,-73.12,-23.05,-80.67,-99.3,-95.06,66.09,64.43,17.97,82.74,
    80.05,58.74,56.07,-74.29,-23.78,56.7,-6.5,-60.83,99.93,-98.85,-33.1,86.21,-5.14,36.66,15.58,69.1,
    9.04000000000001,74.64,61.03,-49.03,-52.4,51.31,45.95,-21.76,-7.84999999999999,8.11,-56.04,-68.08,
    88.26,7.60000000000001,-80.2,4.2,-5.03999999999999,7.08,75.59,6.03,-0.530000000000001,-42.8,66.38,
    91.7,36.51,75.13,-63.2,-20.47,40.69,-70.63,94.29,33,86.57,85.12,62.33,72.94,-73.95,2.96000000000001,
    94.86,-85.76,35.85,-54.02,61.17,6.94,42.3,1.5,10.68,0.0799999999999983,31.17,72.55,-7.97,1.12,-96.68,
    81.62,-57.89,93.65,15.02,-96.89,59.26,73.99,79.79,6.45,25.5,68.31,-72.87,98.34,18,-74.15,46.18,-51.05,
    27.12,93.29,-14.25,99.06,86.82,-47.1,-87.22,21.84,-25.82,59.82,80.08,58.9,-19.97,15.65,-38.1,-30.92,
    -60.56,22.65,-11.14,-44.01,-21.26,57.3,81.33,68.49,37.87,7.23,-48.48,10.27,30.22,-9.62,12,97.45,-73.34,
    -88.01,-0.739999999999995,8.12,35.86,-36.55,63.88,32.42,-65.71,19.6,-5.27,-89.79,26.95,-94.49,-58.92,
    -40.2,82.86,-64.59,64.58,9.45,58.93,-30.32,-83.11,87.52,-60.25,-69,50.83,-1.52,-89.47,-7.59,27.78,
    -59.56,87.29,3.03,-93.85,68.69,-66,-97.17,28.43,95.71,95.81,83.01,-2.03,80.85,-95.5,8.66,60.73,53.49,
    -61.89,70.54,-15.17,27.21,-29.96,-68.17,-59.46,-10.23,-7.81999999999999,58.21,-48.05,33.08,-58.59,-23.59,
    54.02,-28.56,60.42,-31.96,-60.22,48.03,86.01,36.4,97.65,2.67,-37.35,-84.27,10.1,62.78,-88.79,86.06,
    -55.61,11.94,84.35,73.24,9.82000000000001,-56.03,93.68,-9.81,-30.74,15.77,-89.63,-61.63,54.2,-45.25,
    53.42,64.54,-70.49,-7.78999999999999,-46.64,-12.95,-59.88,5.77,26.88,34.08,-10.93,-99.93,50.28,36.4,
    -3.95999999999999,80.63,93.76,23.93,83.91,-72.36,-47.7,68.41,92.93,-31.47,-29.99,16.67,63.51,-86.16,
    21.93,-71.54,85.45,91.23,83.18,-72.12,47.4,-42.25,54.85,-31.81,-52.06,59.09,-12.89,18.95,-91.47,34.93,
    37.86,26.11,-85.35,-87.66,46.45,98.21,40.32,71.29,16.17,68.21,-95.17,-99.59,48.58,-7.06,80.25,10.63,
    3.74000000000001,43.66,78.22,50.05,56.35,-19.92,51.74,45.89,24.93,-9.83,-48,-84.97,72.3,91.99,84.22,
    36.8,-25.29,-62.27,11.42,-19.12,88.27,-62.03,97.26,10.56,-57.53,2.26000000000001,9.7,25.24,-70.55,43.88,
    98.25,36.27,-57.35,-80.25,-12.97,-69.57,81.38,82.52,-76.23,7.25,15.9,81.9,10.43,62.04,35.14,59.62,67.31,
    -73.96,-0.579999999999998,0.680000000000007,-98.04,-11.74,-10.14,-73.68,78.27,98.76,30.66,-81.8,-5.53,
    -5.37,-50.43,-28.83,65.17,-17.93,84.86,13.87,66.72,-94.38,-54.29,-40.18,-67.12,-41.48,77.51,-94.51,
    28.63,96.96,5.52,-46.66,-15.6,21.17,71.27,25.13,30.1,58.46,-42.99,3.90000000000001,60.09,23.81,11.7,
    -22.05,78.16,-18.54,37.98,97.63,52.92,-16.66,97.13,-5.78,-50.23,-11,-4.03999999999999,-17.42,-4.59,
    17.32,-33.78,42.07,51.04,88.93,-3.41,33.85,41.53,0.700000000000003,-29.96,-61.77,-4.09999999999999,
    -65.48,-78.9,41.66,62.84,-70.25,43.88,-57.24,-92.65,43.15,-65.83,-62.88,-42.53,30.54,48.31,-61.57,1.41,
    -2.84,-75.87,-57.98,-51.39,69.95,-80.41,59.77,-3.87,74.29,-46.53,16.67,-78.98,98.42,-64.83,-92.71,
    -35.93,27.1,61.69,57.85,46.42,97.65,-54.38,-22.95,-1.59,-88.17,-33.56,-28.9,7.01000000000001,-44.82,
    34.08,43.73,91.48,-56.53,6.67,-80.08,-50.19,91.63,95.34,-55.48,-66.78,86.89,-79.22,71.04,-71.48,95.81,
    -63,-67.76,10.75,63.54,88.15,-52.56,-45.13,26.44,-80.2,66.6,-95.09,40.48,-17.61,38.96,-65.61,5.73,
    -24.41,-12.09,18.37,-27.37,74.81,97.37,14.01,-83.9,-80.1,5.69,68.46,-73.1,-7.03999999999999,-52.68,
    -50.82,18.09,46.88,-94.89,-47.79,-64.77,5.43000000000001,-10.47,-46.37,50.61,50.9,23.91,75.53,-89.41,
    -38.4,-67.54,2.87,88.41,61.06,-64.35,3.51000000000001,-99.83,83.47,-84.01,61.64,3.54000000000001,-18.35,
    96.75,-56.77,-75.26,-71.11,3.23,47.14,5.17,-51.7,85.21,55.06,61.84,-44.17,-64.98,-75.11,22.89,69.59,
    -86.42,60.07,-96.38,-75.07,-6.67999999999999,99.49,37.27,-19.92,40.53,-23.71,50.97,10.9,94.67,-37.34,
    0.719999999999999,-5.61,-30.68,54.53,78.4,-27.49,22.91,90.21,38.6,-24.64,-70.56,77.26,-41.55,33.8,
    -44.67,-53.18,-81.15,60.2,-78.22,-14.04,45.56,-85.64,93.02,-96.4,-63.48,-55.37,-86.12,-12.03,92.78,
    66.05,33.52,-81.69,-52.22,31.19,-6.03,69.89,16.65,73.03,-22.99,18.37,2.85000000000001,-31.56,63.55,
    -11.96,-38.21,98.44,29.11,95.71,-20.61,-30.14,88.49,-90.13,1.01000000000001,-64.56,-41.16,-84.36,
    6.65000000000001,-47.89,29.5,84.41,-32.35,59.17,21.85,-66.29,14.33,4.73,30.4,-22.72,22.31,-59.01,
    13.68,0.730000000000004,12.25,1.97,-88.24,-50.94,33.9,43.93,-94.69,49.54,28.13,91.71,81.9,-94.7,30.64,
    -24.51,99.97,86.62,-46.36,50.99,-0.950000000000003,-46.53,65.2,48.02,-69.19,-82.94,-64.11,14.26,
    -92.87,-69.24,88.71,-80.62,-20,5.56,76.25,-92.75,97.36,5.43000000000001,-7.17999999999999,-98.37,
    -85.74,6.71000000000001,49.03,24.96,53.29,-28.14,93.77,-54.91,-63.73,0.140000000000001,87.54,-91.73,
    9.55,76.95,-35.34,-92.75,-70.44,98.28,73.99,98.68,59.46,24.64,-71.57,-76.66,21.94,61.37,-67.09,-80.21,
    -33.16,94.26,-46.95,-18.38,15.48,-52.22,-56.99,5.86,-0.039999999999992,-73.2,87.41,88.7,70.59,-29.91,
    59.49,30.27,67.69,-87.36,42.86,42.52,-73.99,43.56,-40.66,-33.4,91.63,-83.16,-53.42,28.93,40.65,-84.05,
    37.4,86.29,23.96,-8.17,43.03,27.96,10.01,38.7,-62.54,75.91,-47.95,-94.83,56.26,-2.3,-86.15,5.64,80.48,
    81.61,0.469999999999999,94.06,-38.02,-24.13,5.09,48.05,80.61,-49.73,82.69,-93.2,60.11,58.68,-83.4,13.05,
    52.39,-45.06,-40.8,17.8,11.37,29.75,-51.48,-96.76,-78.57,-73.17,39.77,-1.02,-93.36,-78.83,-48.37,
    40.06,57.9,-89.99,-58.79,-68.25,24.91,-50.05,52.85,-79.97,-44.78,-83.32,-4.56999999999999,-38.5};
   
    
  for(int i = 0; i < 20; i++) {
	c1.setValue(values[i]);
	c1.run();
	mf.run();
	// MedianFilter outputs input when disabled.
	EXPECT_DOUBLE_EQ (mf.getOut().getSignal().getValue(), values[i]);
  }
  
  for(int i = 20; i < 1000; i++) {
	c1.setValue(values[i]);
	c1.run();
	mf.run();
  }
  
  mf.enable(); // sort.
  c1.setValue(values[1000]);
	c1.run();
	mf.run();
  
  EXPECT_DOUBLE_EQ (mf.getOut().getSignal().getValue(), 2.26000000000001);

  EXPECT_EQ (c1.getOut().getSignal().getTimestamp(), mf.getOut().getSignal().getTimestamp());
}


TEST(MedianFilterUnitTest, vector2MAFilter1) {
  using namespace math; 
  MedianFilter<5,Vector2> mf{};
  
  Constant<Vector2> c1{Matrix<2,1>::createVector2(1,6)};
  c1.run();
  mf.getIn().connect(c1.getOut());
  mf.run();
  
  c1.setValue(Matrix<2,1>::createVector2(5,10));
  c1.run();
  mf.run();
  c1.setValue(Matrix<2,1>::createVector2(4,9));
  c1.run();
  mf.run();
  c1.setValue(Matrix<2,1>::createVector2(2,7));
  c1.run();
  mf.run();
  c1.setValue(Matrix<2,1>::createVector2(3,8));
  c1.run();
  mf.run();

  EXPECT_DOUBLE_EQ (mf.getOut().getSignal().getValue()[0], 3);
  EXPECT_DOUBLE_EQ (mf.getOut().getSignal().getValue()[1], 8);

  EXPECT_EQ (c1.getOut().getSignal().getTimestamp(), mf.getOut().getSignal().getTimestamp());
}


TEST(MedianFilterUnitTest, vector2MAFilter2) {
  using namespace math; 
  MedianFilter<5,Vector2> mf{};
  
  Constant<Vector2> c1{Matrix<2,1>::createVector2(3,10)};
  c1.run();
  mf.getIn().connect(c1.getOut());
  mf.run();
  
  c1.setValue(Matrix<2,1>::createVector2(5,100));
  c1.run();
  mf.run();
  c1.setValue(Matrix<2,1>::createVector2(4,300));
  c1.run();
  mf.run();
  c1.setValue(Matrix<2,1>::createVector2(2,420));
  c1.run();
  mf.run();
  c1.setValue(Matrix<2,1>::createVector2(1,8));
  c1.run();
  mf.run();

  EXPECT_DOUBLE_EQ (mf.getOut().getSignal().getValue()[0], 5);
  EXPECT_DOUBLE_EQ (mf.getOut().getSignal().getValue()[1], 100);

  EXPECT_EQ (c1.getOut().getSignal().getTimestamp(), mf.getOut().getSignal().getTimestamp());
}


TEST(MedianFilterUnitTest, vector2MAFilter3) {
  /*
   * Note: If the value pair of a Vector2 must be filtered without keeping them
   *       together, two MedianFilter instances must be used as shown in this test case.
   */
  
  using namespace math; 
  MedianFilter<5> mf1{};
  MedianFilter<5> mf2{};
  
  Vector2 v1{Matrix<2,1>::createVector2(1,10)};
  Vector2 v2{Matrix<2,1>::createVector2(60,99999)};
  Vector2 v3{Matrix<2,1>::createVector2(37,1252)};
  Vector2 v4{Matrix<2,1>::createVector2(49332,5)};
  Vector2 v5{Matrix<2,1>::createVector2(284,342)};
  
  Constant<> c1{v1[0]};
  Constant<> c2{v1[1]};
  c1.run();
  c2.run();
  mf1.getIn().connect(c1.getOut());
  mf2.getIn().connect(c2.getOut());
  mf1.run();
  mf2.run();
  
  c1.setValue(v2[0]);
  c2.setValue(v2[1]);
  c1.run();
  c2.run();
  mf1.run();
  mf2.run();
  
  c1.setValue(v3[0]);
  c2.setValue(v3[1]);
  c1.run();
  c2.run();
  mf1.run();
  mf2.run();

  c1.setValue(v4[0]);
  c2.setValue(v4[1]);
  c1.run();
  c2.run();
  mf1.run();
  mf2.run();
  
  c1.setValue(v5[0]);
  c2.setValue(v5[1]);
  c1.run();
  c2.run();
  mf1.run();
  mf2.run();

  Vector2 vMedian{Matrix<2,1>::createVector2(mf1.getOut().getSignal().getValue(), mf2.getOut().getSignal().getValue())};

  EXPECT_DOUBLE_EQ (vMedian[0], 60);
  EXPECT_DOUBLE_EQ (vMedian[1], 342);
}


TEST(MedianFilterUnitTest, printMAFilter) {
  MedianFilter<3> mf{};
  mf.setName("myMedianFilter");
  
  Constant<> c1{2};
  c1.run();
  mf.getIn().connect(c1.getOut());
  mf.run();
  
  c1.setValue(3);
  c1.run();
  mf.run();
  c1.setValue(1);
  c1.run();
  mf.run();
  
  std::stringstream sstream{};
  sstream << mf;
  std::string str1 = "Block MedianFilter: 'myMedianFilter' is enabled=1, current median=2, medianIndex=1, current values:[2,3,1]";
  std::string str2 = sstream.str();
  EXPECT_STREQ (str1.c_str(), str2.c_str());
}
