#include <iostream>
#include <bitset>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "steg.h"

int main(int argc, char ** argv) {
  const int kLsb = 2;
  cv::Mat input = cv::imread(argv[2], CV_LOAD_IMAGE_COLOR);

  cv::Mat output = input.clone();
  cv::Mat output_str = input.clone();

  cv::namedWindow("Input", CV_WINDOW_NORMAL);
  cv::namedWindow("Output", CV_WINDOW_NORMAL);

  embedFile(output_str, argv[1] , kLsb);

  if(argc > 3) {
    showLsb(output_str, true, kLsb);
  }

  cv::imshow("Input", input);
  cv::imshow("Output", output_str);
  cv::waitKey(0);
  return 0;
}
