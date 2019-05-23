#include "opencv2/core/core.hpp"

// Supported data types to embed in an image.
enum class EmbedType : uint16_t {
  TYPE_STRING = 0,
  TYPE_IMAGE_BGR = 1,
  TYPE_FILE = 2,
};

// Custom header which is the first thing placed in an image.
struct Header {
  EmbedType type = EmbedType::TYPE_STRING;
  uint32_t size = 0;
  uint16_t width = 0;
  uint16_t height = 0;
  char ext[4];
} __attribute__((packed));

struct Data {
  Header header;
  std::vector<uint8_t> payload;
};

// Embed raw bytes.
bool embedBytes(cv::Mat& input, std::vector<uint8_t> buffer, const int& kLsb = 2);

// Embed an image in another.
bool embedImage(cv::Mat& input, cv::Mat embed, const int& kLsb = 2);

// Embed a string in an image.
bool embedString(cv::Mat& input, const std::string& embed, const int& kLsb = 2);

// Embed any file as raw bytes.
bool embedFile(cv::Mat& input, const std::string& filename, const int& kLsb = 2);

// Decode embedded image.
cv::Mat decodeImage(cv::Mat input, const int& kLsb = 2);

// Decode a string from an image.
std::string decodeString(cv::Mat input, const int& kLsb = 2);

// Decode raw bytes from an image.
Data decodeBytes(cv::Mat input, const int& kLsb = 2);

// Show only the lsb that are embedded in an image.
void showLsb(cv::Mat& input, bool stretch, const int& kLsb = 2);
