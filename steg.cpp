#include "steg.h"
#include <iostream>

namespace {

// Hexdump raw bytes (used for debugging).
void hexdump(std::vector<uint8_t> data) {
  for (const auto & i : data) {
    printf("%.02X:", i);
  }
}

uint8_t makeLsb(uint8_t val, int start, int num) {
  uint8_t mask = (0xFF >> (8 - num)) << start;
  return (val & mask) >> start;
}

uint8_t fillLsb(uint8_t val, uint8_t mask, int num) {
  int n = 0xFF >> (8 - num);
  uint8_t clipped = val&~n;
  return clipped | mask;
}

void embedAsLsb(uint8_t* data, uint8_t value, int num) {
  int num_chunks = 8  / num;
  for (int i = 0; i < num_chunks; ++i) {
    uint8_t lsb = makeLsb(value, i*num, num);
    uint8_t newval = fillLsb(*(data + i), lsb, num);
    *(data + i) = newval;
  }
}

std::vector<uint8_t> decodeSection(std::vector<uint8_t> data, int length, const int& kLsb) {
  const int kMult = 8 / kLsb;
  uint8_t mask = 0xFF >> (8 - kLsb);
  std::vector<uint8_t> buffer;
  for (int i = 0; i < length * kMult; i+=kMult) {
    uint8_t byte = 0;
    for (int j = 0; j < kMult; ++j) {
      uint8_t val = data[i + j] & mask;
      byte |= (val << (j * kLsb));
    }
    buffer.emplace_back(byte);
  }
  return buffer;
}

} // namespace

Data decodeBytes(cv::Mat input, const int& kLsb) {
  uint8_t* raw_data = reinterpret_cast<uint8_t*>(input.data);
  int image_size = input.channels() * input.cols *  input.rows;
  std::vector<uint8_t> data(raw_data, raw_data + image_size);

  auto header_buf = decodeSection(data, sizeof(Header), kLsb);
  Header * header = reinterpret_cast<Header*>(header_buf.data());

  auto payload_buf = decodeSection(data, header->size + sizeof(Header), kLsb);

  Data output;
  output.header = *header;
  output.payload = std::vector<uint8_t>(payload_buf.begin() + sizeof(Header), payload_buf.end());
  return output;
}

cv::Mat decodeImage(cv::Mat input, const int& kLsb) {
  auto data = decodeBytes(input, kLsb);
  cv::Mat out_mat(data.header.height, data.header.width, CV_8UC3, data.payload.data());
  return out_mat.clone();
};

bool embedBytes(cv::Mat& input, std::vector<uint8_t> buffer, const int& kLsb) {
  const int kMult  = 8 / kLsb;
  if (input.cols *input.rows * input.channels() < (buffer.size() * kMult)) {
    return false;
  }
  int index = 0;
  for (int i =  0; i < buffer.size(); ++i) {
    embedAsLsb(input.data + index, buffer[i], kLsb);
    index+=kMult;
  }
  return true;
}

bool embedImage(cv::Mat& input, cv::Mat embed, const int& kLsb) {
  int size = embed.rows * embed.cols * embed.channels();
  std::vector<uint8_t> data(embed.data, embed.data + size);

  // Create header.
  Header header;
  header.type = EmbedType::TYPE_IMAGE_BGR;
  header.size = size;
  header.width = embed.cols;
  header.height = embed.rows;

  uint8_t* header_buf = reinterpret_cast<uint8_t*>(&header);
  std::vector<uint8_t> header_vec(header_buf, header_buf + sizeof(header));
  // insert header data into first portion of the data.
  data.insert(data.begin(), header_vec.begin(), header_vec.end());

  return embedBytes(input, data, kLsb);
}

bool embedString(cv::Mat& input, const std::string& embed, const int& kLsb) {
  std::vector<uint8_t> data(embed.begin(), embed.end());

  // Create header.
  Header header;
  header.type = EmbedType::TYPE_STRING;
  header.size = embed.size();

  uint8_t* header_buf = reinterpret_cast<uint8_t*>(&header);
  std::vector<uint8_t> header_vec(header_buf, header_buf + sizeof(header));
  // insert header data into first portion of the data.
  data.insert(data.begin(), header_vec.begin(), header_vec.end());

  return embedBytes(input, data, kLsb);
}

bool embedFile(cv::Mat& input, const std::string& filename, const int& kLsb) {
  FILE * fp;
  fp = fopen(filename.c_str(), "rb");
  if(!fp) { return false; }

  fseek(fp, 0, SEEK_END);
  size_t length = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  size_t total = 0;
  std::vector<uint8_t> buffer;
  buffer.resize(length);
  while(total < length) {
    int bytes_read = fread(&buffer.front() + total, sizeof(uint8_t), length - total, fp);
    total += bytes_read;
  }
  fclose(fp);


  Header header;
  header.type = EmbedType::TYPE_FILE;
  header.size = length;
  memcpy(header.ext, filename.substr(filename.length() - 4).c_str(), 4);

  uint8_t* header_buf = reinterpret_cast<uint8_t*>(&header);
  std::vector<uint8_t> header_vec(header_buf, header_buf + sizeof(header));
  buffer.insert(buffer.begin(), header_vec.begin(), header_vec.end());
  return embedBytes(input, buffer, kLsb);
}

std::string decodeString(cv::Mat input, const int& kLsb) {
  auto data = decodeBytes(input, kLsb);
  return std::string(data.payload.begin(), data.payload.end());
}

void showLsb(cv::Mat& input, bool stretch, const int& kLsb) {
  int size = input.rows * input.cols * input.channels();
  uint8_t mask = 0xFF >> (8 - kLsb);
  for (int i = 0; i < size; ++i) {
    uint8_t val = input.data[i] & mask;
    if(stretch) {
      val = val << (8 -kLsb);
    }
    input.data[i] = val;
  }
}
