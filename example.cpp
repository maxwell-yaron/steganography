#include <iostream>
#include <bitset>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "steg.h"

int main(int argc, char ** argv) {
  const int kLsb = 2;
  cv::Mat input = cv::imread(argv[2], CV_LOAD_IMAGE_COLOR);

  cv::Mat output = input.clone();

  cv::namedWindow("Input", CV_WINDOW_NORMAL);
  cv::namedWindow("Output", CV_WINDOW_NORMAL);

  if(!embedFile(output, argv[1] , kLsb)) {
    std::cerr << "Data does not fit in the supplied image" << std::endl;
    return EXIT_FAILURE;
  }

  auto dec = decodeBytes(output, kLsb);
  std::string data = std::string(dec.payload.begin(), dec.payload.end());
  std::cout << data << std::endl;

  if(argc > 3) {
    showLsb(output, true, kLsb);
  }
  cv::imshow("Input", input);
  cv::imshow("Output", output);

  cv::waitKey(0);
  return 0;
}
